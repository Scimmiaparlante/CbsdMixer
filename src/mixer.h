#ifndef MIXER_H
#define MIXER_H

#include <vector>
#include <math.h>
#include <mutex>
#include <atomic>
#include <fftw3.h>

#include "audioio.h"
#include "Filters/filter_types.h"

#define SAMPLE_RATE     44100
#define NUM_SAMPLES     4704
#define DEF_DEVICE_IN   "sysdefault"
#define DEF_DEVICE_OUT  "sysdefault"

//for this computation refer to:
//http://www.fftw.org/fftw3_doc/One_002dDimensional-DFTs-of-Real-Data.html
#define COMP_SAMPLES    ((NUM_SAMPLES/2) + 1)

enum FilteringShape {
    RECTANGULAR_FILTERING,
    TRIANGULAR_FILTERING,
    COSINE_FILTERING
};

enum WindowingFucntion {
    DISABLE_WINDOWING,
    OVERLAP_WINDOWING
};


class Mixer
{
private:
    //configuration data
    WindowingFucntion wind;
    double volume;
    unsigned int overlap_size;

    //filter value for avery frequency
    Filter* filter;

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
    void init_filter(FilteringShape shape, std::vector<double> freq);

    //utility functions
    double inline fftw_complex_mod(fftw_complex c) { return sqrt(c[0]*c[0] + c[1]*c[1]);}
    void set_rt_priority();

    //functions to get the correct buffer index
    inline unsigned int get_active_buffer() {return active_buffer;}
    inline unsigned int get_inactive_buffer() {return ((active_buffer == 0) ? 1 : 0);} //only because we have two buffers

public:
    Mixer(std::vector<double> frequencies_, FilteringShape shape_ = RECTANGULAR_FILTERING, WindowingFucntion wind_ = DISABLE_WINDOWING);
    ~Mixer();

    [[noreturn]] void startAcquisition();
    [[noreturn]] void startReproduction();

    inline double* get_filter()                 {return filter->get_filter();}
    inline double* get_rawData()                {return rawData_d[get_inactive_buffer()];}
    inline double*  get_processedData()         {return processedData_d[get_inactive_buffer()];}
    double* get_rawFrequencies();
    double* get_processedFrequencies();

    int set_filterValue(int n_filter, double value) { return filter->set_filterValue(n_filter, value); }
    void set_volume(double value)       {volume = value;}
};

#endif // MIXER_H
