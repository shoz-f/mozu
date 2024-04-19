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
#include "fft.h"
#include <vector>

/***  Module Header  ******************************************************}}}*/
/**
* 
* @par DESCRIPTION
*   
*
* @return
**/
/**************************************************************************{{{*/
DECL_NIF(fft) {
    std::vector<float> wave;

    if (ality != 1
    || !enif_get_binary_as_vector(env, term[0], wave)) {
        return enif_make_badarg(env);
    }

    std::vector<double> spectrum = fft(std::vector<double>(wave.begin(), wave.end()));
    int len = spectrum.size()/2;

    return enif_make_tuple2(env,
        enif_make_ok(env),
        enif_make_tuple2(env,
            enif_make_int(env, len),
            enif_make_binary_from_vector(env, spectrum)));
}

/*** fft.cc ***************************************************************}}}*/