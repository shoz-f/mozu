/***  File Header  ************************************************************/
/**
* mozu_nif.cpp
*
* Elixir/Erlang Port ext. of Post-processing for DNN.
* @author   Shozo Fukuda
* @date     create 2024-04-05 21:51:01
* System    Windows10, WSL2/Ubuntu 20.04.2<br>
*
**/
/**************************************************************************{{{*/

#include "my_erl_nif.h"

/**************************************************************************}}}*/
/* enif resource setup                                                        */
/**************************************************************************{{{*/
int load(ErlNifEnv *env, void **priv_data, ERL_NIF_TERM load_info)
{
    return 0;
}

/**************************************************************************}}}*/
/* enif function dispach table                                                */
/**************************************************************************{{{*/
#include "mozu_nif.inc"

ERL_NIF_INIT(Elixir.Mozu.NIF, nif_funcs, load, NULL, NULL, NULL)

/*** mozu_nif.cpp *****************************************************}}}*/
