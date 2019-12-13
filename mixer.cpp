#include "mixer.h"


Mixer::Mixer(std::vector<double> frequencies_, FilteringShape shape_, WindowingFucntion wind_)
{
    //copy construction data
    frequencies = frequencies_;
    shape = shape_;
    wind = wind_;

    //allocate buffers
    rawData_i = new int16_t[NUM_SAMPLES]();
    processedData_i = new int16_t[NUM_SAMPLES]();
    rawFrequencies_c = new fftw_complex[COMP_SAMPLES]();
    processedFrequencies_c = new fftw_complex[COMP_SAMPLES]();

    rawData_d = new double[NUM_SAMPLES];
    processedData_d = new double[NUM_SAMPLES];
    rawFrequencies_d = new double[COMP_SAMPLES];
    processedFrequencies_d = new double[COMP_SAMPLES];

    //allocate device
    device = new AudioIO(DEF_DEVICE_IN, DEF_DEVICE_OUT, NUM_SAMPLES, SAMPLE_RATE);

    //prepare fft
    direct_plan = fftw_plan_dft_r2c_1d(NUM_SAMPLES, rawData_d, rawFrequencies_c, 0);
    inverse_plan = fftw_plan_dft_c2r_1d(NUM_SAMPLES, processedFrequencies_c, processedData_d, FFTW_PRESERVE_INPUT);

    //reset filter factors and volume to 1
    for(unsigned long i = 0; i < frequencies.size(); ++i)
        filter_factors.push_back(1);
    volume = 1;

    //allocate and initialize filter
    filter = new double[COMP_SAMPLES];
    compute_filter();
}

void Mixer::start()
{
    while(true)
    {
        //memcpy(rawData_i + NUM_SAMPLES - WINDOW_SIZE, rawData_i, WINDOW_SIZE);
        //read data from the input
        device->input_read(rawData_i);

        //conversion of the array to double
        for(int i = 0; i < NUM_SAMPLES; ++i)
            rawData_d[i] = static_cast<double>(rawData_i[i]);

        //go to frequenct domain
        fftw_execute(direct_plan);

        //filter
        apply_filter();

        //return to time
        fftw_execute(inverse_plan);

        //normalization and integer conversion
        for(int i = 0; i < NUM_SAMPLES; ++i) {
            processedData_d[i] /= static_cast<double>(NUM_SAMPLES);
            processedData_i[i] = static_cast<int16_t>(processedData_d[i]);
        }

        //output
        device->output_write(processedData_i);
    }
}



void Mixer::apply_filter()
{
    for(unsigned int i = 0; i < COMP_SAMPLES; ++i)
    {
        processedFrequencies_c[i][0] = volume*filter[i]*rawFrequencies_c[i][0];
        processedFrequencies_c[i][1] = volume*filter[i]*rawFrequencies_c[i][1];
    }
}

void Mixer::compute_filter()
{
    switch(shape)//choose the correct type of filter
    {
    case RECTANGULAR_FILTERING:
        compute_rectangular_filter();
        break;
    case TRIANGULAR_FILTERING:
        compute_triangular_filter();
        break;
    case COSINE_FILTERING:
        compute_cosine_filter();
        break;
    }
}

void Mixer::compute_rectangular_filter()
{
    unsigned long part = 0;
    unsigned long len = frequencies.size();

    for(unsigned long i = 0; i < COMP_SAMPLES; ++i)
    {
        double f = i*SAMPLE_RATE/NUM_SAMPLES;

        if(f < (frequencies[0] + frequencies[1])/2)
            filter[i] = filter_factors[0];
        else if(f > (frequencies[len-1] + frequencies[len-2])/2)
            filter[i] = filter_factors[len - 1];
        else if(part > 0 && part < (len-1) && f > (frequencies[part] + frequencies[part-1])/2 && f < (frequencies[part]+frequencies[part+1])/2)
            filter[i] = filter_factors[part];
        else {
            part++; i--;
        }
    }
}

void Mixer::compute_triangular_filter()
{
    unsigned long part = 0;
    unsigned long len = frequencies.size();

    for(unsigned long i = 0; i < COMP_SAMPLES; ++i)
    {
        double f = i*SAMPLE_RATE/NUM_SAMPLES;

        if(f < frequencies[0])
            filter[i] = 1 + (filter_factors[0]-1)*f/frequencies[0];
        else if(f > (frequencies[len-1]))
        {
            double n = (f - frequencies[len-1]) / (SAMPLE_RATE/2 - frequencies[len-1]);
            filter[i] = 1 * n +  filter_factors[len-1] * (1-n);
        }
        else if(part > 0 && part < (len) && f >= frequencies[part-1] && f <= frequencies[part])
        {
            double n = (f - frequencies[part-1]) / (frequencies[part] - frequencies[part-1]);
            filter[i] = filter_factors[part] * n +  filter_factors[part-1] * (1-n);
        }
        else {
            part++; i--;
        }
    }
}


void Mixer::compute_cosine_filter()
{
    unsigned long part = 0;
    unsigned long len = frequencies.size();

    for(unsigned long i = 0; i < COMP_SAMPLES; ++i)
    {
        double f = i*SAMPLE_RATE/NUM_SAMPLES;

        if(f < frequencies[0])
        {
            double n = f / frequencies[0];
            double n2 = (1 - cos(n * M_PI)) / 2;
            filter[i] = filter_factors[0] * n2 + 1 * (1-n2);
        }
        else if(f > (frequencies[len-1]))
        {
            double n = (f - frequencies[len-1]) / (SAMPLE_RATE/2 - frequencies[len-1]);
            double n2 = (1 - cos(n * M_PI)) / 2;
            filter[i] = 1 * n2 + filter_factors[len-1] * (1-n2);
        }
        else if(part > 0 && part < (len) && f >= frequencies[part-1] && f <= frequencies[part])
        {
            double n = (f - frequencies[part-1]) / (frequencies[part] - frequencies[part-1]);
            double n2 = (1 - cos(n * M_PI)) / 2;
            filter[i] = filter_factors[part] * n2 + filter_factors[part-1] * (1-n2);
        }
        else {
            part++; i--;
        }
    }
}


int Mixer::set_filterValue(int n_filter, double value)
{
    unsigned long len = frequencies.size();
    if (n_filter >= static_cast<int>(len))
        return -1;

    filter_factors[static_cast<unsigned long>(n_filter)] = value;

    //recompute the filter's values
    compute_filter();

    return 0;
}


double* Mixer::get_rawFrequencies()
{
    for(int i = 0; i < COMP_SAMPLES; ++i)
        rawFrequencies_d[i] = fftw_complex_mod(rawFrequencies_c[i]);

    return rawFrequencies_d;

}

double* Mixer::get_processedFrequencies()
{
    for(int i = 0; i < COMP_SAMPLES; ++i)
        processedFrequencies_d[i] = fftw_complex_mod(processedFrequencies_c[i]);

    return processedFrequencies_d;
}


Mixer::~Mixer()
{
    delete[] rawData_d;
    delete[] processedData_d;
    delete[] rawFrequencies_d;
    delete[] processedFrequencies_d;

    delete[] rawData_i;
    delete[] processedData_i;
    delete[] rawFrequencies_c;
    delete[] processedFrequencies_c;

    delete device;

    fftw_destroy_plan(direct_plan);
    fftw_destroy_plan(inverse_plan);
}

