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

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

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

    return enif_make_tuple2(env, enif_make_ok(env),
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
    || !enif_get_binary_as_vector(env, term[0], wave)
    || !enif_get_int(env, term[1], &hop)
    || !enif_get_int(env, term[2], &window)
    || !enif_get_bool(env, term[3], &center)) {
        return enif_make_badarg(env);
    }

    std::vector<float> frames;
    size_t num_frames = 0;

    if (center) {
        const int half_window = int((window - 1) / 2) + 1;
    
        for (auto src = wave.begin(); src < wave.end(); src += hop) {
            auto frame = src - half_window;
            for (int i = 0; i < window; i++) {
                // add refrect padding.
                if (frame < wave.begin()) {
                    int refrect = wave.begin() - frame;
                    frames.push_back(*(wave.begin() + refrect));
                }
                else if (frame >= wave.end()) {
                    int refrect = frame - wave.end() + 1;
                    frames.push_back(*(wave.end() - refrect));
                }
                else {
                    frames.push_back(*frame);
                }
                frame++;
            }
            num_frames++;
        }
    }
    else {
        for (auto src = wave.begin(); (src + window) <= wave.end(); src += hop) {
            auto frame = src;
            for (int i = 0; i < window; i++) {
                frames.push_back(*frame);
                frame++;
            }
            num_frames++;
        }

    }

    return enif_make_tuple2(env, enif_make_ok(env),
             enif_make_tuple2(env,
               enif_make_uint(env, num_frames),
               enif_make_binary_from_vector(env, frames)));
}

/*** audio.cpp *****************************************************}}}*/