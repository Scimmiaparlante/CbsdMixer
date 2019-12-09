#include "mixer.h"

#include <math.h>


Mixer::Mixer(std::vector<float> frequencies_, FilteringShape shape_, WindowingFucntion wind_)
{
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

}

void Mixer::start()
{
    while (true)
    {
        //read data from the input
        device->input_read(rawData_i);

        //conversion of the array to double
        for (int i = 0; i < NUM_SAMPLES; ++i)
            rawData_d[i] = static_cast<double>(rawData_i[i]);

        //go to frequenct domain
        fftw_execute(direct_plan);

        //double output for the users  (and the filter??)
        for (int i = 0; i < COMP_SAMPLES; ++i)
            rawFrequencies_d[i] = sqrt((rawFrequencies_c[i][0] * rawFrequencies_c[i][0]) + (rawFrequencies_c[i][1] * rawFrequencies_c[i][1]));

        //filter
        //return to time
        fftw_execute(inverse_plan);

        //normalization and integer conversion
        for (int i = 0; i < NUM_SAMPLES; ++i) {
            processedData_d[i] /= static_cast<double>(NUM_SAMPLES);
            processedData_i[i] = static_cast<int16_t>(processedData_d[i]);
        }

        //output
    }
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
    //fftw_destroy_plan(inverse_plan);

}
