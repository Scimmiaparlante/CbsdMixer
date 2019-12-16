#include "mixer.h"

#include <sched.h>

#define NUM_BUFFERS 2 //DO NOT MODIFY! The implementation of get_inactive_buffer() requires it to be 2


Mixer::Mixer(std::vector<double> frequencies_, FilteringShape shape_, WindowingFucntion wind_)
{
    //copy construction data
    frequencies = frequencies_;
    shape = shape_;
    wind = wind_;

    init_buffers();

    //fft plans arrays
    direct_plan = new fftw_plan[2];
    inverse_plan = new fftw_plan[2];

    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        //prepare fft plans
        direct_plan[i] = fftw_plan_dft_r2c_1d(NUM_SAMPLES, rawData_d[i], rawFrequencies_c[i], 0);
        inverse_plan[i] = fftw_plan_dft_c2r_1d(NUM_SAMPLES, processedFrequencies_c[i], processedData_d[i], FFTW_PRESERVE_INPUT);
    }

    //allocate device
    device = new AudioIO(DEF_DEVICE_IN, DEF_DEVICE_OUT, NUM_SAMPLES, SAMPLE_RATE);

    //reset filter factors and volume to 1
    for(unsigned long i = 0; i < frequencies.size(); ++i)
        filter_factors.push_back(1);
    volume = 1;

    //allocate and initialize filter
    filter = new double[COMP_SAMPLES];
    compute_filter();
}

void Mixer::init_buffers()
{
    //array of buffers creation
    rawData_i = new int16_t*[NUM_BUFFERS];
    processedData_i = new int16_t*[NUM_BUFFERS];
    rawFrequencies_c = new fftw_complex*[NUM_BUFFERS];
    processedFrequencies_c = new fftw_complex*[NUM_BUFFERS];

    rawData_d = new double*[NUM_BUFFERS];
    processedData_d = new double*[NUM_BUFFERS];
    rawFrequencies_d = new double*[NUM_BUFFERS];
    processedFrequencies_d = new double*[NUM_BUFFERS];

    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        //allocate buffers
        rawData_i[i] = new int16_t[NUM_SAMPLES];
        processedData_i[i] = new int16_t[NUM_SAMPLES];
        rawFrequencies_c[i] = new fftw_complex[COMP_SAMPLES];
        processedFrequencies_c[i] = new fftw_complex[COMP_SAMPLES];

        rawData_d[i] = new double[NUM_SAMPLES];
        processedData_d[i] = new double[NUM_SAMPLES];
        rawFrequencies_d[i] = new double[COMP_SAMPLES];
        processedFrequencies_d[i] = new double[COMP_SAMPLES];
    }

    active_buffer = 0;

    //cleaning the inactive buffers for the first run
    for(int i = 0; i < NUM_SAMPLES; ++i) {
        rawData_i[active_buffer][i] = 100;
        rawData_d[active_buffer][i] = 100;
    }

    for(int i = 0; i < COMP_SAMPLES; ++i) {
        rawFrequencies_c[active_buffer][i][0] = 100;
        rawFrequencies_c[active_buffer][i][1] = 100;
        processedFrequencies_c[active_buffer][i][0] = 100;
        processedFrequencies_c[active_buffer][i][1] = 100;
    }
}



void Mixer::start()
{
    //make this a real-time thread
    sched_param p;
    sched_getparam(0, &p);
    p.sched_priority = sched_get_priority_max(SCHED_RR);
    sched_setscheduler(0, SCHED_RR, &p);

    unsigned int inactive_buf;
    device->input_read(rawData_i[active_buffer]);

    while(true)
    {
        //read data from the input to the active buffer
        device->input_read(rawData_i[active_buffer]);

        //conversion of the array to double
        for(int i = 0; i < NUM_SAMPLES; ++i)
            rawData_d[inactive_buf][i] = static_cast<double>(rawData_i[inactive_buf][i]);

        //go to frequenct domain
        fftw_execute(direct_plan[inactive_buf]);

        //filter
        apply_filter(inactive_buf);

        //return to time
        fftw_execute(inverse_plan[inactive_buf]);

        //normalization and integer conversion
        for(int i = 0; i < NUM_SAMPLES; ++i) {
            processedData_d[inactive_buf][i] /= static_cast<double>(NUM_SAMPLES);
            processedData_i[inactive_buf][i] = static_cast<int16_t>(processedData_d[inactive_buf][i]);
        }

        //output
        device->output_write(processedData_i[inactive_buf]);

        //buffer switch
        inactive_buf = active_buffer;
        active_buffer = get_inactive_buffer();
    }
}



void Mixer::apply_filter(unsigned int num_buf)
{
    for(unsigned int i = 0; i < COMP_SAMPLES; ++i)
    {
        processedFrequencies_c[num_buf][i][0] = volume*filter[i]*rawFrequencies_c[num_buf][i][0];
        processedFrequencies_c[num_buf][i][1] = volume*filter[i]*rawFrequencies_c[num_buf][i][1];
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
    unsigned int buf_n = get_active_buffer();

    for(int i = 0; i < COMP_SAMPLES; ++i)
        rawFrequencies_d[buf_n][i] = fftw_complex_mod(rawFrequencies_c[buf_n][i]);

    return rawFrequencies_d[buf_n];

}

double* Mixer::get_processedFrequencies()
{
    unsigned int buf_n = get_active_buffer();

    for(int i = 0; i < COMP_SAMPLES; ++i)
        processedFrequencies_d[buf_n][i] = fftw_complex_mod(processedFrequencies_c[buf_n][i]);

    return processedFrequencies_d[buf_n];
}


Mixer::~Mixer()
{
    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        delete[] rawData_d[i];
        delete[] processedData_d[i];
        delete[] rawFrequencies_d[i];
        delete[] processedFrequencies_d[i];

        delete[] rawData_i[i];
        delete[] processedData_i[i];
        delete[] rawFrequencies_c[i];
        delete[] processedFrequencies_c[i];

        fftw_destroy_plan(direct_plan[i]);
        fftw_destroy_plan(inverse_plan[i]);
    }


    delete[] rawData_d;
    delete[] processedData_d;
    delete[] rawFrequencies_d;
    delete[] processedFrequencies_d;

    delete[] rawData_i;
    delete[] processedData_i;
    delete[] rawFrequencies_c;
    delete[] processedFrequencies_c;

    delete[] direct_plan;
    delete[] inverse_plan;

    delete device;
}

