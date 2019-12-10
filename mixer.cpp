#include "mixer.h"

#include <iostream>

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
    device = new AudioIO(DEF_DEVICE, NUM_SAMPLES, SAMPLE_RATE);

    //prepare fft
    direct_plan = fftw_plan_dft_r2c_1d(NUM_SAMPLES, rawData_d, rawFrequencies_c, 0);
    inverse_plan = fftw_plan_dft_c2r_1d(NUM_SAMPLES, rawFrequencies_c/*processedFrequencies_c*/, processedData_d, FFTW_PRESERVE_INPUT);

    //reset filter factors and volume to 1
    for(unsigned long i = 0; i < frequencies.size(); ++i)
        filter_factors.push_back(1);
    volume = 1;
}

void Mixer::start()
{
    while(true)
    {
        //read data from the input
        device->input_read(rawData_i);

        //conversion of the array to double
        for(int i = 0; i < NUM_SAMPLES; ++i)
            rawData_d[i] = static_cast<double>(rawData_i[i]);

        //go to frequenct domain
        fftw_execute(direct_plan);

        //double output for the users  (and the filter??)
        for(int i = 0; i < COMP_SAMPLES; ++i)
            rawFrequencies_d[i] = fftw_complex_mod(rawFrequencies_c[i]);

        //filter
        apply_filter();

        //return to time
        //fftw_execute(inverse_plan);

        //normalization and integer conversion
        for(int i = 0; i < NUM_SAMPLES; ++i) {
            processedData_d[i] /= static_cast<double>(NUM_SAMPLES);
            processedData_i[i] = static_cast<int16_t>(processedData_d[i]);
        }

        //output
        //device->output_write(processed_data);
    }
}


void Mixer::apply_filter()
{
    //choose the correct type of filter
    if(shape == RECTANGULAR_FILTERING)
        apply_rectangular_filter();
    else if(shape == TRIANGULAR_FILTERING)
        apply_triangular_filter();
}

void Mixer::apply_rectangular_filter()
{
    unsigned long part = 0;
    unsigned long len = frequencies.size();

    for(unsigned long i = 0; i < COMP_SAMPLES; ++i)
    {
        double f = i*SAMPLE_RATE/NUM_SAMPLES;

        if(f < (frequencies[0] + frequencies[1])/2)
            processedFrequencies_d[i] = rawFrequencies_d[i] * filter_factors[0];
        else if(f > (frequencies[len-1] + frequencies[len-2])/2)
            processedFrequencies_d[i] = rawFrequencies_d[i] * filter_factors[len - 1];
        else if(part > 0 && part < (len-1) && f > (frequencies[part] + frequencies[part-1])/2 && f < (frequencies[part]+frequencies[part+1])/2)
            processedFrequencies_d[i] = rawFrequencies_d[i] * filter_factors[part];
        else {
            part++; i--;
        }

        //apply volume
        processedFrequencies_d[i] *= volume;
    }
}

void Mixer::apply_triangular_filter()
{

}


int Mixer::set_filterValue(int n_filter, double value)
{
    unsigned long len = frequencies.size();
    if (n_filter >= static_cast<int>(len))
        return -1;

    filter_factors[static_cast<unsigned long>(n_filter)] = value;

    return 0;
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

