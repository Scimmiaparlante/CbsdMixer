#include "audioio.h"

#include <iostream>

AudioIO::AudioIO(std::string device_, unsigned num_samples_, unsigned sample_rate_)
{
    device = device_;
    num_samples = num_samples_;
    sample_rate = sample_rate_;

    input_init();
}

AudioIO::~AudioIO()
{
    snd_pcm_close(capture_handle);
}


void AudioIO::input_init()
{
    int ret;
    snd_pcm_hw_params_t *hw_params;

    ret = snd_pcm_open(&capture_handle, device.c_str(), SND_PCM_STREAM_CAPTURE, 0);
    if (ret < 0)
        error("cannot open audio device", ret);

    ret = snd_pcm_hw_params_malloc(&hw_params);
    if (ret < 0)
        error("cannot allocate hardware parameter structure", ret);

    ret = snd_pcm_hw_params_any(capture_handle, hw_params);
    if (ret < 0)
        error("cannot initialize hardware parameter structure", ret);

    ret = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (ret < 0)
        error("cannot set access type", ret);

    ret = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16);
    if (ret < 0)
        error("cannot set sample format", ret);

    ret = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &sample_rate, nullptr);
    if (ret < 0)
        error("cannot set sample rate", ret);

    ret = snd_pcm_hw_params_set_channels(capture_handle, hw_params, 1);
    if (ret < 0)
        error("cannot set channel count", ret);

    ret = snd_pcm_hw_params(capture_handle, hw_params);
    if(ret < 0)
        error("cannot set parameters", ret);

    snd_pcm_hw_params_free(hw_params);

    ret = snd_pcm_prepare(capture_handle);
    if(ret < 0)
        error("cannot prepare audio interface for use", ret);
}


int AudioIO::input_read(int16_t* buf)
{
    snd_pcm_sframes_t ret;
    unsigned int to_read = num_samples;

    while(to_read > 0)
    {
        ret = snd_pcm_readi(capture_handle, buf, to_read);
        if (ret < 0) {
            std::cerr << "read from audio interface failed: " << snd_strerror(ret) << std::endl;
            return 1;
        }
        to_read -= ret;
        buf += ret;
    }
    return 0;
}



void AudioIO::error(std::string mess, int err)
{
    std::cerr << "Input device error: " << mess << " (" << snd_strerror(err) << ")" << std::endl;
    exit(1);
}
