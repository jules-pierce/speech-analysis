

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "imgstuff.h"
#include "bmp_io.h"
#include "spectrum_compare_recent.h"
//#define DEBUG

feat_vector_t spectrum_compare_recent(double *fft_dat, int start, int end, int cols)
{
    int i, j;
    feat_vector_t word;
    double moment_total;
    double weight_total;
    double sum = 0;
    int max = 0;
    double place = 0;
    int peaks = 0;
    double thresh = 100; //i have no idea what the threshold should be
    int in_peak = 0;
    
    //calculate y center of gravity for the word
    moment_total = 0;
    weight_total = 0;
    for (i = start; i < end; i++){
        sum = 0;
        for (j = 0; j < cols; j++){
            sum += fft_dat [i*cols + j];
        }
        weight_total += sum;
        moment_total += sum * (i-start);
    }
    
    word.y_cog = moment_total/weight_total;
    word.y_cog /= (end-start);
    
    //calculate x center of gravity for the word
    moment_total = 0;
    weight_total = 0;
    for (j = 0; j < cols; j++){
        sum = 0;
        for (i = start; i < end; i++){
            sum += fft_dat [i*cols + j];
        }
        weight_total += sum;
        moment_total += sum * j;
    }
    
    word.x_cog = moment_total/weight_total;
    
    //calculate sum of pixels in each row
    for (i = start; i < end; i++){
        sum = 0;
        for (j = 0; j < cols; j++){
            sum += fft_dat [i*cols + j];
        }
        if (sum > max) place = i - start;
    }
    word.max_row = place;
    word.max_row /= (end-start);
    
    //find number of peaks
    for (i = start; i < end; i++){
        sum = 0;
        for (j = 0; j < cols; j++){
            sum += fft_dat [i*cols + j];
        }
        if (sum > thresh && in_peak == 0){
            peaks++;
            in_peak = 1;
        }
        else if (sum < thresh && in_peak == 1){
            in_peak = 0;
        }
    }
    word.peaks = peaks;

    return word;
}

