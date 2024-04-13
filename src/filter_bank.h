/***  File Header  ************************************************************/
/**
* filter_bank.h
*
* Elixir/Erlang Port ext. of Post-processing for DNN.
* @author   Shozo Fukuda
* @date     create 2024-04-07 22:52:35
* System    Windows10, WSL2/Ubuntu 20.04.2<br>
*
**/
/**************************************************************************{{{*/

#include "my_erl_nif.h"

_DECL_NIF(hz2mel);
_DECL_NIF(mel2hz);
_DECL_NIF(linspace);
_DECL_NIF(mel_filter_bank);
_DECL_NIF(create_triangular_filter_bank);

/*** filter_bank.h ********************************************************}}}*/
