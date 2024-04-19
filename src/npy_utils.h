/***  File Header  ************************************************************/
/**
* npy_utils.h
*
* Elixir/Erlang Port ext. of Post-processing for DNN.
* @author   Shozo Fukuda
* @date     create 2024-04-07 22:52:35
* System    Windows10, WSL2/Ubuntu 20.04.2<br>
*
**/
/**************************************************************************{{{*/
#ifndef _NPY_UTILS_H
#define _NPY_UTILS_H

#include "my_erl_nif.h"
#include <vector>

/***  Module Header  ******************************************************}}}*/
/**
* linearly spaced points
* @par DESCRIPTION
*   Generates linearly spaced points between a specified start and stop value.
*
* @retval spaced points
**/
/**************************************************************************{{{*/
template <typename T=double>
std::vector<T> _linspace(
double start,
double stop,
int    num,
bool   endpoint=true)
{
    int section = (endpoint) ? (num - 1) : num;

    std::vector<T> result;
    for (int i = 0; i < num; i++) {
        result.push_back(start + i*(stop - start)/section);
    }

    return result;
}

#endif
/*** npy_utils.h *********************************************************}}}*/
