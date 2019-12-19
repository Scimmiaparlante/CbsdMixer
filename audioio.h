#ifndef AUDIOIO_H
#define AUDIOIO_H

#include <cstdint>

#include <alsa/asoundlib.h>

class AudioIO
{
public:
    AudioIO(const char* deviceIn_, const char* deviceOut_, unsigned num_samples_, unsigned sample_rate_);
    ~AudioIO();

    int input_read(int16_t* buf);
    int output_write(int16_t* buf);

private:
    unsigned int num_samples;
    unsigned int sample_rate;

    snd_pcm_t* handle_in;
    snd_pcm_t* handle_out;

    void input_init(const char* dev);
    void output_init(const char* dev);

    [[noreturn]] void error(const char* mess, int err);

};

#endif // AUDIOIO_H
