#ifndef AUDIOIO_H
#define AUDIOIO_H

#include <string>
#include <cstdint>

#include <alsa/asoundlib.h>
#include <fftw3.h>

class AudioIO
{
public:
    AudioIO(std::string device_, unsigned num_samples_, unsigned sample_rate_);
    ~AudioIO();

    int input_read(int16_t* buf);
    int output_write(int16_t* buf);

private:
    std::string device;
    unsigned int num_samples;
    unsigned int sample_rate;

    snd_pcm_t* capture_handle_in;
    snd_pcm_t* capture_handle_out;

    void input_init();
    [[noreturn]] void error(std::string mess, int err);

};

#endif // AUDIOIO_H
