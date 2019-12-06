#ifndef MIXER_H
#define MIXER_H

#include <vector>

#include "audioio.h"

#define NUM_SAMPLES 1024

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
    double* rawData;
    double* processedData;
    double* rawFrequencies;
    double* processedFrequecies;

    //AudioIO object to retrieve data
    AudioIO* device;

    Mixer(std::vector<float> frequencies_, FilteringShape shape_, WindowingFucntion wind_);

    void start();





};

#endif // MIXER_H
