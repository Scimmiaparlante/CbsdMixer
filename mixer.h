#ifndef MIXER_H
#define MIXER_H

#include <vector>

#include "audioio.h"

#define SAMPLE_RATE     44100
#define NUM_SAMPLES     2048
#define DEF_DEVICE      "default"

#define COMP_SAMPLES    ((NUM_SAMPLES/2) + 1)

enum FilteringShape {
    RECTANGULAR_FILTERING,
    TRIANGULAR_FILTERING
};

enum WindowingFucntion {
    DISABLE_WINDOWING,
    LINEAR_WINDOWING
};


class Mixer
{
private:
    //configuration data
    std::vector<float> frequencies;
    FilteringShape shape;
    WindowingFucntion wind;

    //buffers
    double* rawData_d;
    double* processedData_d;
    double* rawFrequencies_d;
    double* processedFrequencies_d;
    int16_t* rawData_i;
    int16_t* processedData_i;
    fftw_complex* rawFrequencies_c;
    fftw_complex* processedFrequencies_c;

    //AudioIO object to retrieve data
    AudioIO* device;

    //fft plans
    fftw_plan direct_plan;
    fftw_plan inverse_plan;

    double fftw_complex_mod(fftw_complex c);

public:
    Mixer(std::vector<float> frequencies_, FilteringShape shape_ = RECTANGULAR_FILTERING, WindowingFucntion wind_ = DISABLE_WINDOWING);
    ~Mixer();

    [[noreturn]] void start();

    double* get_rawData()           {return rawData_d;}
    double* get_rawFrequencies()    {return rawFrequencies_d;}




};

#endif // MIXER_H
