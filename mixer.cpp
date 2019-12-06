#include "mixer.h"

//TODO: check this values
#define SAMPLE_RATE     44100
#define NUM_SAMPLES     1024
#define DEVICE          "default"

Mixer::Mixer(std::vector<float> frequencies_, FilteringShape shape_, WindowingFucntion wind_)
{
    frequencies = frequencies_;
    shape = shape_;
    wind = wind_;

    device = new AudioIO(DEVICE, NUM_SAMPLES, SAMPLE_RATE);
}


void Mixer::start()
{
    //ogni tot, leggi






}
