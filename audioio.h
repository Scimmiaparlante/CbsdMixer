#ifndef AUDIOIO_H
#define AUDIOIO_H

#include <string>
#include <cstdint>

#include <alsa/asoundlib.h>


#define SAMPLERATE

class AudioIO
{
public:
    AudioIO(std::string device_, unsigned num_samples_, unsigned sample_rate_);
    ~AudioIO();

    int input_read(int16_t* buf);

private:
    std::string device;
    unsigned num_samples;
    unsigned sample_rate;

    snd_pcm_t *capture_handle;

    void input_init();
    [[noreturn]] void error(std::string mess, int err);

};

#endif // AUDIOIO_H
