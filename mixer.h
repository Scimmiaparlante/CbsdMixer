#ifndef MIXER_H
#define MIXER_H

#include <vector>

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
    std::vector<float> frequencies;
    FilteringShape shape;
    WindowingFucntion wind;

    Mixer(std::vector<float> frequencies_, FilteringShape shape_, WindowingFucntion wind_);








};

#endif // MIXER_H
