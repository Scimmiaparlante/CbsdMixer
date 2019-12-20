#include "mixer.h"

#include <sched.h>
#include <unistd.h>
#include <iostream>

#define DEF_OVERLAP_SIZE 300        //default size of the overlapping window

//number of buffers
#define NUM_BUFFERS 2 //DO NOT MODIFY! The implementation of get_inactive_buffer() requires it to be 2


Mixer::Mixer(std::vector<double> frequencies_, FilteringShape shape_, WindowingFucntion wind_)
{
    //copy construction data
    wind = wind_;
    overlap_size = (wind == OVERLAP_WINDOWING) ? DEF_OVERLAP_SIZE : 0;

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
    device = new AudioIO(DEF_DEVICE_IN, DEF_DEVICE_OUT, NUM_SAMPLES - overlap_size, SAMPLE_RATE);

    //initialize filter and volume
    init_filter(shape_, frequencies_);
    volume = 1;
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



void Mixer::init_filter(FilteringShape shape, std::vector<double> freq)
{
    switch(shape)//choose the correct type of filter
    {
    case RECTANGULAR_FILTERING:
        filter = new RectangularFilter(COMP_SAMPLES, SAMPLE_RATE, freq);
        break;
    case TRIANGULAR_FILTERING:
        filter = new TriangularFilter(COMP_SAMPLES, SAMPLE_RATE, freq);
        break;
    case COSINE_FILTERING:
        filter = new CosineFilter(COMP_SAMPLES, SAMPLE_RATE, freq);
        break;
    }
}

void Mixer::set_rt_priority()
{
    //make this a real-time thread
    sched_param p;
    sched_getparam(0, &p);
    p.sched_priority = sched_get_priority_max(SCHED_RR);
    int ret = sched_setscheduler(0, SCHED_RR, &p);
    if (ret < 0)
        std::cerr << "Errore nella selezione della priorità" << std::endl;
}



void Mixer::startAcquisition()
{
    set_rt_priority();

    while(true)
    {
        //read data from the input to the active buffer
        device->input_read(rawData_i[active_buffer] + overlap_size);

        //buffer switch
        active_buffer = get_inactive_buffer();
    }
}



void Mixer::startReproduction()
{
    set_rt_priority();

    unsigned int inactive_buf, active_buf;

    while(true)
    {
        inactive_buf = get_inactive_buffer();
        active_buf = get_active_buffer();

        //conversion of the array to double
        for(unsigned int i = 0; i < NUM_SAMPLES; ++i)
        {
            if (i < overlap_size)   //copy the overlapping part from the previous run
                rawData_d[inactive_buf][i] =  rawData_d[active_buf][i + NUM_SAMPLES - overlap_size];
            else                    //copy the second part from the input buffer
                rawData_d[inactive_buf][i] = static_cast<double>(rawData_i[inactive_buf][i]);
        }

        //go to frequenct domain
        fftw_execute(direct_plan[inactive_buf]);

        //filter
        filter->apply(processedFrequencies_c[inactive_buf], rawFrequencies_c[inactive_buf], volume);

        //return to time
        fftw_execute(inverse_plan[inactive_buf]);

        //normalization and integer conversion
        for(unsigned int i = 0; i < NUM_SAMPLES; ++i)
        {
            double i_d = static_cast<double>(i);

            //create alias for the buffer element
            double* pd_d = &processedData_d[inactive_buf][i];

            //normalize
            *pd_d /= static_cast<double>(NUM_SAMPLES);

            //integrate with the past step
            if(i < overlap_size)
                *pd_d = (*pd_d)*(i_d/overlap_size) + processedData_d[active_buf][i + NUM_SAMPLES - overlap_size]*(1 - i_d/overlap_size);

            processedData_i[inactive_buf][i] = static_cast<int16_t>(processedData_d[inactive_buf][i]);
        }

        //output
        device->output_write(processedData_i[inactive_buf]);

        //usleep(90000);
    }
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

    delete filter;
    delete device;
}

