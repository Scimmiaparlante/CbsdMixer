#include "mixer.h"

#include <fftw3.h>


Mixer::Mixer(std::vector<float> frequencies_, FilteringShape shape_, WindowingFucntion wind_)
{
    frequencies = frequencies_;
    shape = shape_;
    wind = wind_;

    rawData = new int16_t[NUM_SAMPLES]();

    device = new AudioIO(DEF_DEVICE, NUM_SAMPLES, SAMPLE_RATE);
}


void Mixer::start()
{
    while (true)
    {
        device->input_read(rawData);
    }
}


int16_t* Mixer::get_rawData()
{
    return rawData;
}
