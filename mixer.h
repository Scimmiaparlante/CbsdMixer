#ifndef MIXER_H
#define MIXER_H

#include <vector>
#include <math.h>

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
    std::vector<double> frequencies;
    FilteringShape shape;
    WindowingFucntion wind;
    std::vector<double> filter_factors;
    double volume;

    //filter value for avery frequency
    double* filter;

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

    //utility functions
    double inline fftw_complex_mod(fftw_complex c) { return sqrt(c[0]*c[0] + c[1]*c[1]);}

    //filters functions
    void apply_filter();

    void compute_filter();
    void compute_rectangular_filter();
    void compute_triangular_filter();

public:
    Mixer(std::vector<double> frequencies_, FilteringShape shape_ = RECTANGULAR_FILTERING, WindowingFucntion wind_ = DISABLE_WINDOWING);
    ~Mixer();

    [[noreturn]] void start();

    double* get_rawData()               {return rawData_d;}
    double* get_rawFrequencies()        {return rawFrequencies_d;}
    double* get_processedFrequencies()  {return processedFrequencies_d;}

    int set_filterValue(int n_filter, double value);
    void set_volume(double value)       {volume = value;}


};

#endif // MIXER_H
