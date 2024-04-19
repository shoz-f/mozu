/***  File Header  ************************************************************/
/**
* @file fft.h
*   Fast Fourier transform
* 
* @author	Shozo Fukuda
* @date	    create 2024-04-16 10:05:17
* System	Windows10<br>
*
**/
/**************************************************************************{{{*/
#ifndef _FFT_H
#define _FFT_H

#include <vector>
#include <cmath>

/***  Module Header  ******************************************************}}}*/
/**
* Discrete Fourier transform
* @par DESCRIPTION
*   Convert signal to spectrum.
*
* @retval spectrum (complex)
**/
/**************************************************************************{{{*/
template <typename T>
std::vector<T> dft(const std::vector<T> &input) {
    std::vector<T> output;

    int N = input.size();

    for (int k = 0; k < N; k++) {
        T re = 0;
        T im = 0;

        for (int n = 0; n < N; n++) {
            T theta = 2*M_PI * k/N * n;
            re += input[n] * cos(theta);
            im -= input[n] * sin(theta);
        }

        output.push_back(re);
        output.push_back(im);
    }

    return output;
}

/***  Module Header  ******************************************************}}}*/
/**
* Fast Fourier transform
* @par DESCRIPTION
*   Convert signal to spectrum.
*
* @retval spectrum (complex)
**/
/**************************************************************************{{{*/
template <typename T>
std::vector<T> fft(const std::vector<T> &input) {
    int N = input.size();

    if (N == 1) {
        std::vector<T> output = {input[0], 0};
        return output;
    }
    else if (N % 2 == 1) {
        return dft(input);
    }
    else {
        std::vector<T> even;
        std::vector<T> odd;
        for (int i = 0; i < N; i++) {
            if (i % 2 == 0) {
                even.push_back(input[i]);
            } else {
                odd.push_back(input[i]);
            }
        }

        std::vector<T> evenFFT = fft(even);
        std::vector<T> oddFFT  = fft(odd);

        std::vector<T> output(2*N);
        for (int k = 0; k < N/2; k++) {
            T theta = 2*M_PI * k/N;
            T re =  cos(theta);
            T im = -sin(theta);

            T reOdd = oddFFT[2*k + 0];
            T imOdd = oddFFT[2*k + 1];

            output[0 + 2*k + 0] = evenFFT[2*k + 0] + re*reOdd - im*imOdd;
            output[0 + 2*k + 1] = evenFFT[2*k + 1] + re*imOdd + im*reOdd;

            output[N + 2*k + 0] = evenFFT[2*k + 0] - re*reOdd + im*imOdd;
            output[N + 2*k + 1] = evenFFT[2*k + 1] - re*imOdd - im*reOdd;
        }

        return output;
    }
}

#endif
/*** fft.h ***************************************************************}}}*/
