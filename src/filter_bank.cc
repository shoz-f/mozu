/***  File Header  ************************************************************/
/**
* filter_bank.cc
*
* Elixir/Erlang Port ext. of Post-processing for DNN.
* @author   Shozo Fukuda
* @date     create 2024-04-07 22:52:35
* System    Windows10, WSL2/Ubuntu 20.04.2<br>
*
**/
/**************************************************************************{{{*/

#include "my_erl_nif.h"
#include "npy_utils.h"
#include <cmath>
#include <functional>
#include <vector>
#include <cassert>

enum MelScale {
    NONE = 0,
    HTK,
    KALDI,
    SLANEY
};

bool enif_get_mel_scale(ErlNifEnv* env, ERL_NIF_TERM term, int* mel_scale)
{
    char mel_scale_name[16];
    int len;

    if ((len = enif_get_atom(env, term, mel_scale_name, sizeof(mel_scale_name), ERL_NIF_LATIN1)) == 0) {
        return false;
    }

    *mel_scale = (std::strcmp(mel_scale_name, "htk"   ) == 0) ? HTK
               : (std::strcmp(mel_scale_name, "kaldi" ) == 0) ? KALDI
               : (std::strcmp(mel_scale_name, "slaney") == 0) ? SLANEY
               : NONE;

    return (*mel_scale != NONE);
}

typedef double DType;
typedef std::vector<DType> Array;

/***  Module Header  ******************************************************}}}*/
/**
* Convert frequency(hertz) to mel
* @par DESCRIPTION
*   Convert frequency to mel in each method.
*
* @retval mel
**/
/**************************************************************************{{{*/
std::function<DType(DType)> _fn_hz2mel(
int mel_scale=HTK)
{
    switch (mel_scale) {
    case HTK:
        return [](DType x) { return 2595.0*log10(1.0 + x/700.0); };
    case KALDI:
        return [](DType x) { return 1127.0*log(1.0 + x/700.0); };
    case SLANEY:
        return [](DType x) { return (x >= 1000.0) ? (15.0 + log(x/1000.0)*(27.0/log(6.4)))
                                                  : (3.0*x/200.0); };
    }

    // never come here.
    return nullptr;
}

DType _hz2mel(
DType freq,
int mel_scale=HTK)
{
    return _fn_hz2mel(mel_scale)(freq);
}

Array _hz2mel(
Array& freq,
int mel_scale=HTK)
{
    Array result;
    std::transform(freq.begin(), freq.end(), std::back_inserter(result), _fn_hz2mel(mel_scale));

    return result;
}

DECL_NIF(hz2mel) {
    Array freq_array;
    int mel_scale;

    if (ality != 2
    || !enif_get_binary_as_vector(env, term[0], freq_array)
    || !enif_get_mel_scale(env, term[1], &mel_scale)) {
        return enif_make_badarg(env);
    }

    Array mel_array = _hz2mel(freq_array, mel_scale);

    return enif_make_tuple2(env, enif_make_ok(env),
                                 enif_make_binary_from_vector(env, mel_array));
}

/***  Module Header  ******************************************************}}}*/
/**
* Reverse mel to frequency(hertz)
* @par DESCRIPTION
*   Reverse mel to frequency in each method.
*
* @retval frequency(hertz)
**/
/**************************************************************************{{{*/
std::function<DType(DType)> _fn_mel2hz(
int mel_scale=HTK)
{
    switch (mel_scale) {
    case HTK:
        return [](DType x) { return 700.0*(pow(10.0, x/2595.0) - 1.0); };
    case KALDI:
        return [](DType x) { return 700.0*(exp(x/1127.0) - 1.0); };
    case SLANEY:
        return [](DType x) { return (x >= 15.0) ? (1000.0*exp((log(6.4)/27.0)*(x - 15.0)))
                                                : (200.0*x/3.0); };
    }

    // never come here.
    return nullptr;
}

DType _mel2hz(
DType mel,
int mel_scale=HTK)
{
    return _fn_mel2hz(mel_scale)(mel);
}

Array _mel2hz(
Array& mel,
int mel_scale=HTK)
{
    Array result;
    std::transform(mel.begin(), mel.end(), std::back_inserter(result), _fn_mel2hz(mel_scale));

    return result;
}

DECL_NIF(mel2hz) {
    Array mel_array;
    int mel_scale;

    if (ality != 2
    || !enif_get_binary_as_vector(env, term[0], mel_array)
    || !enif_get_mel_scale(env, term[1], &mel_scale)) {
        return enif_make_badarg(env);
    }

    Array freq_array = _mel2hz(mel_array, mel_scale);

    return enif_make_tuple2(env, enif_make_ok(env),
                                 enif_make_binary_from_vector(env, freq_array));
}

/***  Module Header  ******************************************************}}}*/
/**
* linearly spaced points
* @par DESCRIPTION
*   Generates linearly spaced points between a specified start and stop value.
*
* @retval spaced points
**/
/**************************************************************************{{{*/
DECL_NIF(linspace) {
    double start, stop;
    int num;
    bool endpoint;

    if (ality != 4
    || !enif_get_number(env, term[0], &start)
    || !enif_get_number(env, term[1], &stop)
    || !enif_get_int(env, term[2], &num)
    || !enif_get_bool(env, term[3], &endpoint)) {
        return enif_make_badarg(env);
    }

    Array array = _linspace(start, stop, num, endpoint);

    return enif_make_tuple2(env, enif_make_ok(env),
                                 enif_make_binary_from_vector(env, array));
}

/***  Module Header  ******************************************************}}}*/
/**
* create triangle filter bank
* @par DESCRIPTION
*   Create triangular filter bank from two vectors.
*
* @retval filter bank.
**/
/**************************************************************************{{{*/
Array _create_triangular_filter_bank(
Array& fft_freqs,
Array& filter_freqs)
{
    const int max_row = fft_freqs.size();
    const int max_col = filter_freqs.size();

    Array diff;    // vector[max_col - 1]
    for (int i = 0; i < (max_col - 1); i++) {
        diff.push_back(filter_freqs[i+1] - filter_freqs[i]); 
    }

    Array slopes;  // matrix[max_row, max_col]
    for (int i = 0; i < max_row; i++) {
    for (int j = 0; j < max_col; j++) {
        slopes.push_back(filter_freqs[j] - fft_freqs[i]);
    }}

    Array result;  // matrix[max_row, max_col - 2]
    auto slopes_row = slopes.begin();
    for (int i = 0; i < max_row; i++) {
        for (int j = 0; j < (max_col - 2); j++) {
            DType down_slope = - slopes_row[j] / diff[j];
            DType up_slope   =   slopes_row[j+2] / diff[j+1];
            result.push_back(std::max(std::min(down_slope, up_slope), (DType)0.0));
        }
        slopes_row += max_col;
    }

    return result;
}

/***  Module Header  ******************************************************}}}*/
/**
* create mel filter bank
* @par DESCRIPTION
*   Create mel filter bank with specified parameters.
*
* @retval 
**/
/**************************************************************************{{{*/
DECL_NIF(mel_filter_bank) {
    int num_frequency_bins;
    int num_mel_filters;
    double min_frequency;
    double max_frequency;
    int sampling_rate;
    bool norm;
    int mel_scale;
    bool triangularize_in_mel_space;

    if (ality != 8
    || !enif_get_int(env, term[0], &num_frequency_bins)
    || !enif_get_int(env, term[1], &num_mel_filters)
    || !enif_get_number(env, term[2], &min_frequency)
    || !enif_get_number(env, term[3], &max_frequency)
    || !enif_get_int(env, term[4], &sampling_rate)
    || !enif_get_mel_scale(env, term[5], &mel_scale)
    || !enif_get_bool(env, term[6], &norm)
    || !enif_get_bool(env, term[7], &triangularize_in_mel_space)) {
        return enif_make_badarg(env);
    }

    Array filter_freqs = _linspace(_hz2mel(min_frequency, mel_scale), _hz2mel(max_frequency, mel_scale), num_mel_filters+2);
    Array fft_freqs    = _linspace(0, int(sampling_rate / 2), num_frequency_bins);
    if (triangularize_in_mel_space) {
        fft_freqs = _hz2mel(fft_freqs, mel_scale);
    }
    else {
        filter_freqs = _mel2hz(filter_freqs, mel_scale);
    }

    Array mel_filters = _create_triangular_filter_bank(fft_freqs, filter_freqs);

    if (norm && mel_scale == SLANEY) {
        Array enorm;
        for (int j = 0; j < num_mel_filters; j++) {
            enorm.push_back(2.0/(filter_freqs[j+2] - filter_freqs[j]));
        }

        auto mel_filters_row = mel_filters.begin();
        for (int i = 0; i < num_frequency_bins; i++) {
            for (int j = 0; j < num_mel_filters; j++) {
                mel_filters_row[j] *= enorm[j];
            }

            mel_filters_row += num_mel_filters;
        }
    }

    return enif_make_tuple2(env, enif_make_ok(env),
                                 enif_make_binary_from_vector(env, mel_filters));
}

/*** filter_bank.cc ******************************************************}}}*/
