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
#include <algorithm>
#include <iostream>

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

/***  Module Header  ******************************************************}}}*/
/**
* 
* @par DESCRIPTION
*   
*
* @retval 
**/
/**************************************************************************{{{*/
template <typename T, typename U>
std::vector<U> _astype(const std::vector<T>& input)
{
    return std::vector<U>(input.begin(), input.end());
}

/***  Module Header  ******************************************************}}}*/
/**
* 
* @par DESCRIPTION
*   
*
* @retval 
**/
/**************************************************************************{{{*/
enum PAD_MODE {
    PAD_ZERO = 0,
    PAD_EDGE,
    PAD_REFLECT
};

template <typename T>
void _pad(std::vector<T>& array, size_t front_size, size_t rear_size, int mode=PAD_ZERO)
{
    std::cout << "front:" << front_size << ", rear:" << rear_size << ", mode:" << mode << std::endl;

    if (front_size > 0) {
        // padding front
        std::vector<T> front_pad(front_size);
        switch (mode) {
        case PAD_EDGE:
            // edge
            front_pad.assign(front_size, array.front());
            break;
        case PAD_REFLECT:
            // refrect
            std::reverse_copy(array.begin()+1, array.begin()+front_size+1, front_pad.begin());
            break;
        case PAD_ZERO:
        default:
            // zero fill
            front_pad.assign(front_size, 0);
            break;
        }

        array.insert(array.begin(), front_pad.begin(), front_pad.end());
    }

    if (rear_size > 0) {
        // padding rear
        std::vector<T> rear_pad(rear_size);
        switch (mode) {
        case PAD_EDGE:
            // edge
            rear_pad.assign(rear_size, array.back());
            break;
        case PAD_REFLECT:
            // refrect
            std::reverse_copy(array.end()-rear_size-1, array.end()-1, rear_pad.begin());
            break;
        case PAD_ZERO:
        default:
            // zero fill
            rear_pad.assign(rear_size, 0);
            break;
        }

        array.insert(array.end(), rear_pad.begin(), rear_pad.end());
    }
}

#endif
/*** npy_utils.h *********************************************************}}}*/
