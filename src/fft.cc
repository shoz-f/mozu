/***  File Header  ************************************************************/
/**
* @file fft.cc
*
* 
* @author	Shozo Fukuda
* @date	    create 2024-04-16 17:17:19
* System	Windows10 <br>
*
**/
/**************************************************************************{{{*/

#include "my_erl_nif.h"
#include <vector>
#include <cstring>

#include "pocketfft_hdronly.h"

template <typename T>
std::vector<std::complex<T>> _rfft_1D(const std::vector<T>& input)
{
    size_t N = input.size();
    pocketfft::shape_t shape{N};
    pocketfft::shape_t axes = {0};
    pocketfft::stride_t stride_in  = {sizeof(T)};
    pocketfft::stride_t stride_out = {sizeof(std::complex<T>)};
    std::vector<std::complex<T>> output(int(N/2)+1);
    pocketfft::r2c(shape, stride_in, stride_out, axes, pocketfft::FORWARD, input.data(), output.data(), 1.0);

    return output;
}

template <typename T>
std::vector<T> _abs(const std::vector<std::complex<T>>& input)
{
    std::vector<T> absolute;
    for (const auto& c : input) {
        absolute.push_back(std::abs(c));
    }

    return absolute;
}

template <typename T>
std::vector<T> _norm(const std::vector<std::complex<T>>& input)
{
    std::vector<T> norm;
    for (const auto& c : input) {
        norm.push_back(std::norm(c));
    }

    return norm;
}

/***  Module Header  ******************************************************}}}*/
/**
* 
* @par DESCRIPTION
*   
*
* @return
**/
/**************************************************************************{{{*/
DECL_NIF(rfft_1D) {
    std::vector<double> wave_as_double;
    char power[8];
    bool oneside;

    if (ality != 3
    || !enif_get_vector_as<float, double>(env, term[0], wave_as_double)
    || !enif_get_atom(env, term[1], power, sizeof(power), ERL_NIF_LATIN1)
    || !enif_get_bool(env, term[2], &oneside)) {
        return enif_make_badarg(env);
    }

    auto spectrum = _rfft_1D(wave_as_double);

    if (!oneside) {
        // copy positive part to negative part.
        size_t N = wave_as_double.size();
        spectrum.resize(N);     // make room for negative part.

        auto src = spectrum.begin() + (int(N/2)-1);
        auto dst = spectrum.begin() + (int(N/2)+1);
        while (dst < spectrum.end()) {
            *dst++ = std::conj(*src--);
        }
    }

    ERL_NIF_TERM output = (strcmp(power, "abs")  == 0) ? enif_make_vector(env, _abs(spectrum))  // absolute
                        : (strcmp(power, "norm") == 0) ? enif_make_vector(env, _norm(spectrum)) // norm
                        : enif_make_vector(env, std::move(spectrum));

    return enif_make_ok(env, output);
}

/***  Module Header  ******************************************************}}}*/
/**
* 
* @par DESCRIPTION
*   
*
* @return
**/
/**************************************************************************{{{*/
DECL_NIF(power) {
    std::vector<std::complex<double>> spectrum;
    char power[8];

    if (ality != 2
    || !enif_get_vector(env, term[0], spectrum)
    || !enif_get_atom(env, term[1], power, sizeof(power), ERL_NIF_LATIN1)) {
        return enif_make_badarg(env);
    }

    return (strcmp(power, "abs")  == 0) ? enif_make_ok(env, enif_make_vector(env, _abs(spectrum)))
         : (strcmp(power, "norm") == 0) ? enif_make_ok(env, enif_make_vector(env, _norm(spectrum)))
         : enif_make_badarg(env);
}

/*** fft.cc ***************************************************************}}}*/