#ifndef MIXER_H
#define MIXER_H

#include <vector>
#include <math.h>
#include <mutex>
#include <atomic>

#include "audioio.h"

#define SAMPLE_RATE     44100
#define NUM_SAMPLES     4096
#define DEF_DEVICE_IN   "default"
#define DEF_DEVICE_OUT  "default"


#define COMP_SAMPLES    ((NUM_SAMPLES/2) + 1)




enum FilteringShape {
    RECTANGULAR_FILTERING,
    TRIANGULAR_FILTERING,
    COSINE_FILTERING
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

    //variable to state which is the active buffer
    std::atomic<unsigned int> active_buffer;

    //buffers couples
    double** rawData_d;
    double** processedData_d;
    double** rawFrequencies_d;
    double** processedFrequencies_d;
    int16_t** rawData_i;
    int16_t** processedData_i;
    fftw_complex** rawFrequencies_c;
    fftw_complex** processedFrequencies_c;

    //AudioIO object to retrieve data
    AudioIO* device;

    //fft plans couples
    fftw_plan* direct_plan;
    fftw_plan* inverse_plan;

    void init_buffers();

    //utility functions
    double inline fftw_complex_mod(fftw_complex c) { return sqrt(c[0]*c[0] + c[1]*c[1]);}

    //filters functions
    void apply_filter(unsigned int num_buf);

    void compute_filter();
    void compute_rectangular_filter();
    void compute_triangular_filter();
    void compute_cosine_filter();

    //functions to get the correct buffer index
    inline unsigned int get_active_buffer() {return active_buffer;}
    inline unsigned int get_inactive_buffer() {return ((active_buffer == 0) ? 1 : 0);} //only because we have two buffers



public:
    Mixer(std::vector<double> frequencies_, FilteringShape shape_ = RECTANGULAR_FILTERING, WindowingFucntion wind_ = DISABLE_WINDOWING);
    ~Mixer();

    [[noreturn]] void start();

    inline double* get_rawData()                {return rawData_d[get_inactive_buffer()];}
    inline double*  get_processedData()         {return processedData_d[get_inactive_buffer()];}
    double* get_rawFrequencies();
    double* get_processedFrequencies();

    int set_filterValue(int n_filter, double value);
    void set_volume(double value)       {volume = value;}


};

#endif // MIXER_H
