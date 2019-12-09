#ifndef MIXER_H
#define MIXER_H

#include <vector>

#include "audioio.h"

#define SAMPLE_RATE     44100
#define NUM_SAMPLES     2048
#define DEF_DEVICE      "default"

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
public:
    //configuration data
    std::vector<float> frequencies;
    FilteringShape shape;
    WindowingFucntion wind;

    //buffers
    int16_t* rawData;
    double* processedData;
    double* rawFrequencies;
    double* processedFrequecies;

    //AudioIO object to retrieve data
    AudioIO* device;

    Mixer(std::vector<float> frequencies_, FilteringShape shape_ = RECTANGULAR_FILTERING, WindowingFucntion wind_ = DISABLE_WINDOWING);

    [[noreturn]] void start();

    int16_t* get_rawData();




};

#endif // MIXER_H
