/***  File Header  ************************************************************/
/**
* audio.cc
*
* Elixir/Erlang Port ext. of Post-processing for DNN.
* @author   Shozo Fukuda
* @date     create 2024-04-05 21:51:01
* System    Windows10, WSL2/Ubuntu 20.04.2<br>
*
**/
/**************************************************************************{{{*/

#include "my_erl_nif.h"
#include <vector>
#include <cstdint>
#include <cmath>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include "npy_utils.h"

/***  Module Header  ******************************************************}}}*/
/**
* Load WAV file
* @par DESCRIPTION
*   Load audio data from specific WAV file.
*
* @retval binary
**/
/**************************************************************************{{{*/
DECL_NIF(wav_load) {
    std::string fname;

    if (ality != 1
    || !enif_get_str(env, term[0], &fname)) {
        return enif_make_badarg(env);
    }

    drwav wav;
    if (!drwav_init_file(&wav, fname.c_str(), NULL)) {
        return enif_make_badarg(env);
    }

    uint16_t channels              = wav.channels;
    uint32_t sample_rate           = wav.sampleRate;
    uint64_t total_PCM_frame_count = wav.totalPCMFrameCount;

    ErlNifBinary pcm_f32;
    enif_alloc_binary(total_PCM_frame_count * channels * sizeof(float), &pcm_f32);

    drwav_read_pcm_frames_f32(&wav, total_PCM_frame_count, (float*)pcm_f32.data);

    drwav_uninit(&wav);

    return enif_make_ok(env,
             enif_make_tuple3(env,
                enif_make_uint(env, channels),
                enif_make_uint(env, sample_rate),
                enif_make_binary(env, &pcm_f32)));
}

/***  Module Header  ******************************************************}}}*/
/**
* Save WAV file
* @par DESCRIPTION
*   Save audio data to specific WAV file.
*
* @retval :ok
**/
/**************************************************************************{{{*/
DECL_NIF(wav_save) {
    std::string  fname;
    unsigned int channels;
    unsigned int sample_rate;
    ErlNifBinary pcm_f32;

    if (ality != 4
    || !enif_get_str(env, term[0], &fname)
    || !enif_get_uint(env, term[1], &channels)
    || !enif_get_uint(env, term[2], &sample_rate)
    || !enif_inspect_binary(env, term[3], &pcm_f32)) {
        return enif_make_badarg(env);
    }

    drwav_data_format format;
    format.container     = drwav_container_riff;
    format.format        = DR_WAVE_FORMAT_PCM;
    format.channels      = channels;
    format.sampleRate    = sample_rate;
    format.bitsPerSample = 16;

    drwav wav;
    if (!drwav_init_file_write(&wav, fname.c_str(), &format, NULL)) {
        return enif_make_badarg(env);
    }

    // convert f32 to s16
    size_t total_count = pcm_f32.size/sizeof(float);
    int16_t* pcm_s16 = (int16_t*)calloc(total_count, sizeof(int16_t));
    if (pcm_s16 == NULL) {
        return enif_make_error(env);
    }
    drwav_f32_to_s16(pcm_s16, (const float*)pcm_f32.data, total_count);

    // save as 16-bit PCM.
    drwav_write_pcm_frames(&wav, total_count/channels, pcm_s16);

    free(pcm_s16);
    drwav_uninit(&wav);

    return enif_make_ok(env);
}

/***  Module Header  ******************************************************}}}*/
/**
* Load WAV file
* @par DESCRIPTION
*   Load audio data from specific WAV file.
*
* @retval binary
**/
/**************************************************************************{{{*/
DECL_NIF(to_frames) {
    std::vector<float> wave;
    int hop;
    int window;
    bool center;

    if (ality != 4
    || !enif_get_vector(env, term[0], wave)
    || !enif_get_int(env, term[1], &hop)
    || !enif_get_int(env, term[2], &window)
    || !enif_get_bool(env, term[3], &center)) {
        return enif_make_badarg(env);
    }

    if (center) {
        const int half_window = int(window / 2);
        _pad(wave, half_window, half_window, PAD_REFLECT);
    }

    std::vector<double> frames;
    for (auto src = wave.begin(); src < wave.end() - window; src += hop) {
        std::vector<double> frame(src, src+window);
   
        std::copy(frame.begin(), frame.end(), std::back_inserter(frames));
    }

    return enif_make_ok(env, enif_make_vector(env, std::move(frames)));
}

/***  Module Header  ******************************************************}}}*/
/**
* dtype conversion
* @par DESCRIPTION
*   
*
* @retval 
**/
/**************************************************************************{{{*/
DECL_NIF(pad) {
    std::vector<float> array;
    unsigned int front_size;
    unsigned int rear_size;
    unsigned int mode;

    if (ality != 4
    || !enif_get_vector(env, term[0], array)
    || !enif_get_uint(env, term[1], &front_size)
    || !enif_get_uint(env, term[2], &rear_size)
    || !enif_get_uint(env, term[3], &mode)) {
        return enif_make_badarg(env);
    }

    _pad(array, front_size, rear_size, mode);

    return enif_make_ok(env, enif_make_vector(env, move(array)));
}


/***  Module Header  ******************************************************}}}*/
/**
* dtype conversion
* @par DESCRIPTION
*   
*
* @retval 
**/
/**************************************************************************{{{*/
template <typename T>
std::vector<T> _hanning(int N)
{
    std::vector<T> w(N);
    for (int i = 0; i < N; i++) {
        w[i] = 0.5*(1 - cos(2*M_PI*i/(N-1))); 
    }

    return w;
}

DECL_NIF(hanning) {
    unsigned int N;

    if (ality != 1
    || !enif_get_uint(env, term[0], &N)) {
        return enif_make_badarg(env);
    }

    return enif_make_ok(env, enif_make_vector(env, _hanning<double>(N)));
}

template <typename T>
std::vector<T> _hamming(int N)
{
    std::vector<T> w(N);
    for (int i = 0; i < N; i++) {
        w[i] = 0.54 - 0.46*cos(2*M_PI*i/(N-1)); 
    }

    return w;
}

DECL_NIF(hamming) {
    unsigned int N;

    if (ality != 1
    || !enif_get_uint(env, term[0], &N)) {
        return enif_make_badarg(env);
    }

    return enif_make_ok(env, enif_make_vector(env, _hamming<double>(N)));
}

/*** audio.cpp *****************************************************}}}*/
