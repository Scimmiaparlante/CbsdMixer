#include "mixer.h"

Mixer::Mixer(std::vector<float> frequencies_, FilteringShape shape_, WindowingFucntion wind_)
{
    frequencies = frequencies_;
    shape = shape_;
    wind = wind_;
}
