#include "audioio.h"

#include <iostream>

AudioIO::AudioIO(const char* deviceIn_, const char* deviceOut_, unsigned int num_samples_, unsigned int sample_rate_)
{
    num_samples = num_samples_;
    sample_rate = sample_rate_;

    input_init(deviceIn_);
    output_init(deviceOut_);
}

AudioIO::~AudioIO()
{
    snd_pcm_drain(capture_handle_out);

    snd_pcm_close(capture_handle_in);
    snd_pcm_close(capture_handle_out);
}


void AudioIO::input_init(const char* dev)
{
    int ret;
    snd_pcm_hw_params_t *hw_params;

    //---------------------INPUT SETUP------------------------------------------------*/

    ret = snd_pcm_open(&capture_handle_in, dev, SND_PCM_STREAM_CAPTURE, SND_PCM_ASYNC);
    if (ret < 0)
        error("cannot open audio device", ret);

    ret = snd_pcm_hw_params_malloc(&hw_params);
    if (ret < 0)
        error("cannot allocate hardware parameter structure", ret);

    ret = snd_pcm_hw_params_any(capture_handle_in, hw_params);
    if (ret < 0)
        error("cannot initialize hardware parameter structure", ret);

    ret = snd_pcm_hw_params_set_access(capture_handle_in, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (ret < 0)
        error("cannot set access type", ret);

    ret = snd_pcm_hw_params_set_format(capture_handle_in, hw_params, SND_PCM_FORMAT_S16);
    if (ret < 0)
        error("cannot set sample format", ret);

    ret = snd_pcm_hw_params_set_rate_near(capture_handle_in, hw_params, &sample_rate, nullptr);
    if (ret < 0)
        error("cannot set sample rate", ret);

    ret = snd_pcm_hw_params_set_channels(capture_handle_in, hw_params, 1);
    if (ret < 0)
        error("cannot set channel count", ret);

    ret = snd_pcm_hw_params(capture_handle_in, hw_params);
    if(ret < 0)
        error("cannot set parameters", ret);

    snd_pcm_hw_params_free(hw_params);

    ret = snd_pcm_prepare(capture_handle_in);
    if(ret < 0)
        error("cannot prepare audio interface for use", ret);
}

void AudioIO::output_init(const char *dev)
{
    int ret;
    snd_pcm_hw_params_t *hw_params;

    //---------------------OUTPUT SETUP------------------------------------------------*/

    ret = snd_pcm_open(&capture_handle_out, dev, SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC);
    if (ret  < 0)
        error("cannot open audio device", ret);

    ret = snd_pcm_hw_params_malloc(&hw_params);
    if (ret < 0)
        error("cannot allocate hardware parameter structure", ret);

    ret = snd_pcm_hw_params_any(capture_handle_out, hw_params);
    if (ret < 0)
        error("cannot initialize hardware parameter structure", ret);

    ret = snd_pcm_hw_params_set_access(capture_handle_out, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (ret < 0)
        error("cannot set access type", ret);

    ret = snd_pcm_hw_params_set_format(capture_handle_out, hw_params, SND_PCM_FORMAT_S16_LE);
    if (ret < 0)
        error("cannot set sample format", ret);

    unsigned int sr = sample_rate;
    ret = snd_pcm_hw_params_set_rate_near(capture_handle_out, hw_params, &sr, nullptr);
    if (ret < 0)
        error("cannot set sample rate", ret);

    ret = snd_pcm_hw_params_set_channels(capture_handle_out, hw_params, 1);
    if (ret < 0)
        error("cannot set channel count", ret);

    ret = snd_pcm_hw_params_set_buffer_size(capture_handle_out, hw_params, num_samples);
    if(ret < 0)
        error("cannot set buffer size near", ret);

    ret = snd_pcm_hw_params(capture_handle_out, hw_params);
    if (ret < 0)
        error("cannot set parameters", ret);

    snd_pcm_hw_params_free(hw_params);

    ret = snd_pcm_prepare(capture_handle_out);
    if (ret < 0)
        error("cannot prepare audio interface for use", ret);

    ret = snd_pcm_start(capture_handle_out);
    if (ret < 0)
        error("cannot start audio stream", ret);
}


int AudioIO::input_read(int16_t* buf)
{
    snd_pcm_sframes_t ret;
    unsigned int to_read = num_samples;

    while(to_read > 0)
    {
        ret = snd_pcm_readi(capture_handle_in, buf, to_read);
        if (ret < 0) {
            std::cerr << "read from audio interface failed: " << snd_strerror(static_cast<int>(ret)) << std::endl;
            return 1;
        }
        to_read -= ret;
        buf += ret;
    }
    return 0;
}



int AudioIO::output_write(int16_t* buf)
{
    snd_pcm_sframes_t ret;
    unsigned int to_write = num_samples;

    while(to_write > 0)
    {
        snd_pcm_prepare(capture_handle_out);

        ret = snd_pcm_writei(capture_handle_out, buf, to_write);
        if (ret < 0) {
            std::cerr << "write to audio interface failed: " << snd_strerror(static_cast<int>(ret)) << std::endl;
            return 1;
        }

        buf += ret;
        to_write -= ret;
    }

    return 0;
}


void AudioIO::error(const char* mess, int err)
{
    std::cerr << "Input device error: " << mess << " (" << snd_strerror(err) << ")" << std::endl;
    exit(1);
}
