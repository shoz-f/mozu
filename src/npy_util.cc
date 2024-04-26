/***  File Header  ************************************************************/
/**
* npy_util.cc
*
* Elixir/Erlang Port ext. of Post-processing for DNN.
* @author   Shozo Fukuda
* @date     create 2024-04-24 11:08:21
* System    Windows10, WSL2/Ubuntu 20.04.2<br>
*
**/
/**************************************************************************{{{*/

#include "my_erl_nif.h"
#include <complex>
#include "npy_utils.h"

/***  Module Header  ******************************************************}}}*/
/**
* dtype conversion
* @par DESCRIPTION
*   
*
* @retval 
**/
/**************************************************************************{{{*/
DECL_NIF(f4_astype) {
    std::vector<float> input;
    std::string type;

    if (ality != 2
    || !enif_get_vector(env, term[0], input)
    || !enif_get_str(env, term[1], type)) {
        return enif_make_badarg(env);
    }

    return (type == "<f8") ? enif_make_ok(env, enif_make_vector(env, _astype<float,double>(input)))
         : (type == "<i4") ? enif_make_ok(env, enif_make_vector(env, _astype<float,int>(input)))
         : (type == "<u4") ? enif_make_ok(env, enif_make_vector(env, _astype<float,unsigned int>(input)))
         : enif_make_badarg(env);
}

DECL_NIF(f8_astype) {
    std::vector<double> input;
    std::string type;

    if (ality != 2
    || !enif_get_vector(env, term[0], input)
    || !enif_get_str(env, term[1], type)) {
        return enif_make_badarg(env);
    }

    return (type == "<f4") ? enif_make_ok(env, enif_make_vector(env, _astype<double,float>(input)))
         : (type == "<i4") ? enif_make_ok(env, enif_make_vector(env, _astype<double,int>(input)))
         : (type == "<u4") ? enif_make_ok(env, enif_make_vector(env, _astype<double,unsigned int>(input)))
         : enif_make_badarg(env);
}

DECL_NIF(c16_astype) {
    std::vector<std::complex<double>> input;
    std::string type;

    if (ality != 2
    || !enif_get_vector(env, term[0], input)
    || !enif_get_str(env, term[1], type)) {
        return enif_make_badarg(env);
    }

    return (type == "<c8") ? enif_make_ok(env, enif_make_vector(env, _astype<std::complex<double>,std::complex<float>>(input)))
         : enif_make_badarg(env);
}

DECL_NIF(c8_astype) {
    std::vector<std::complex<float>> input;
    std::string type;

    if (ality != 2
    || !enif_get_vector(env, term[0], input)
    || !enif_get_str(env, term[1], type)) {
        return enif_make_badarg(env);
    }

    return (type == "<c8") ? enif_make_ok(env, enif_make_vector(env, _astype<std::complex<float>,std::complex<double>>(input)))
         : enif_make_badarg(env);
}

/*** npy_util.cc*** *******************************************************}}}*/
