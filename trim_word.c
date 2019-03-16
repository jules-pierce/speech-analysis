//new program: take a recording of one word and trims it to the right size

//new program: take a picture of one word (to be matched) and find the place where it matches up with the bank, using mean square error
//find peak that it belongs to, and that will line up with which word it matches
//use old program to make pic of one word


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "fftRoutines.h"
#include "imgstuff.h"
#include "bmp_io.h"
//#define DEBUG
#define FFTSIZE 256

int main(int argc, char *argv[] )
{
	fftRoutines my_fft;
	cmplx_fp_t *bufin = NULL;
	cmplx_fp_t *bufout = NULL;
    int *peak_start = NULL;
    int *peak_end = NULL;
    double *energy = NULL;
    int num_pts;
    double Fs;
    double T;
    FILE *indes;
    int i, j;
    short temp;
    double max = 0;
    unsigned char *picdat;
    double *fft_dat;
    int rows;
    int retval;
    int cols = FFTSIZE/2;
    int peaks;
    int sum = 0;
    int threshold = 100;
    
    if (argc != 5){
        fprintf (stderr, "error usage is: %s num_pts Fs filename.raw out_img.bmp\n", argv[0]);
        exit (0);
    }
	
	num_pts = atoi (argv[1]);
    Fs = atof (argv[2]);
    T = 1 / Fs;
    
    indes = fopen (argv[3], "rb");
    
    if (indes == NULL) {
        fprintf (stderr, "failed to open file\n");
        exit (1);
    }

	bufin = (cmplx_fp_t*) malloc(sizeof(cmplx_fp_t) * FFTSIZE);
	bufout = (cmplx_fp_t*) malloc(sizeof(cmplx_fp_t) * FFTSIZE);
    
    rows = num_pts/FFTSIZE;
    fprintf (stderr, "rows = %d  cols = %d\n", rows, cols);
    
    picdat = (unsigned char*) malloc (rows * cols * sizeof (unsigned char));
    fft_dat = (double*) malloc (rows * cols * sizeof (double));
    energy = (double*) malloc (rows * sizeof (double));
    
    peak_start = (int*) malloc (rows * sizeof (int));
    peak_end = (int*) malloc (rows * sizeof (int));
    
    if (picdat == NULL){
        fprintf (stderr, "ERROR: allocating memory picdat\n");
        exit (1);
    }
    
    if (fft_dat == NULL){
        fprintf (stderr, "ERROR: allocating memory fft_dat\n");
        exit (1);
    }
    
    if (peak_start == NULL || peak_end == NULL){
        fprintf (stderr, "ERROR: allocating memory peak_start or peak_end\n");
        exit (1);
    }

	if( bufin == NULL || bufout == NULL ){
		fprintf(stderr, "ERROR: allocating memory\n");
		exit(1);
	}
    
    for (i = 0; i < rows; i++){
        peak_start [i] = -1;
        peak_end [i] = -1;
    }

	for(i = 0; i < rows; i++){
        max = 0;
        
        for (j = 0; j < FFTSIZE; j++){
            retval = fread(&temp, sizeof(short), 1, indes);
            if (retval != 1) temp = 0;
            bufin[j].re = temp;
            bufin[j].im = 0;
        }

        my_fft.fft1d(bufin, bufout, FFTSIZE);
        
        for (j = 0; j < cols; j++){
            fft_dat[i*cols + j] = sqrt(bufout[j].re * bufout[j].re + bufout[j].im * bufout[j].im);
            if (fft_dat[i*cols + j] > max) max = fft_dat[i*cols + j];
        }
    }
    
    //picdat [mi (i, j, cols)]: i is rows, j is cols
    
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            if (fft_dat [i * cols + j] > 255) picdat [mi (i, j, cols)] = 255;
            else if (fft_dat [i * cols + j] < 0) picdat [mi (i, j, cols)] = 0;
            else picdat [mi (i, j, cols)] = fft_dat [i * cols + j];
        }
    }
    
    output_bmp (argv[4], cols, rows, picdat, MONO);

	if( bufin != NULL ) free( bufin );
	if( bufout != NULL ) free( bufout );
	if( indes != NULL ) fclose(indes);
    if( fft_dat != NULL ) free( fft_dat );
    if( peak_start != NULL ) free( peak_start );
    if( peak_end != NULL ) free( peak_end );
    if (picdat != NULL) free (picdat);

}

int find_peaks (double* DA, int num_pts, int* peak_start, int* peak_end){
    double max = 0;
    double min = 1E50;
    double avg = 0;
    int i;
    int *flag;
    int peak = 0;
    double sum = 0;
    int seek_val = 1;
    
    flag = (int*) malloc (num_pts * sizeof(int));
    
    for (i = 0; i < num_pts; i++){
        if (DA[i] > max) max = DA[i];
        if (DA [i] < min) min = DA[i];
        avg += DA[i];
    }
    
    avg /= num_pts;
    
    for (i = 0; i < num_pts; i++){
        if (DA[i] >= avg) flag[i] = 1;
        else flag[i] = 0;
        if (i < 10) flag[i] = 0; //get rid of noise at the beginning of the recording
    }
    
    for (i = 1; i < num_pts - 1; i++){
        if (flag[i] == 1 && flag [i-1] == 0 && flag [i+1] == 0) flag [i] = 0;
        else if (flag [i] == 0 && flag [i-1] == 1 && flag [i+1] == 1) flag [i] = 1;
    }
    
    for (i = 0; i < num_pts; i++){
        fprintf (stderr, "%d", flag[i]);
    }
    
    i = 0;
    while (i < num_pts){
        //fprintf (stderr, "i = %d\n", i);
        while (flag [i] != seek_val){
            i++;
        }
        
        if (seek_val == 1){
            peak_start [peak] = i;
            seek_val = 0;
        }
        
        else {
            peak_end [peak] = i;
            peak ++;
            seek_val = 1;
        }
    }
    
    fprintf (stderr, "\n%d peaks\n", peak);
    for (i = 0; i < peak; i++){
        fprintf (stderr, "start = %d  end = %d\n", peak_start[i], peak_end[i]);
    }
    
    for (i = 0; i < peak; i++){
        if (i == 0) {
            peak_start[0] = 0;
            peak_end [0] += (peak_start[1] - peak_end[0])/2;
        }
        else if (i == peak-1){
            peak_start [i] = peak_end [i-1] + 1;
            peak_end[i] = num_pts - 1;
        }
        else {
            peak_start[i] = peak_end[i-1] + 1;
            peak_end [i] += (peak_start[i+1] - peak_end[i])/2;
        }
    }
    
    for (i = 0; i < peak; i++){
        fprintf (stderr, "start = %d  end = %d\n", peak_start[i], peak_end[i]);
    }
    
    return peak;
    
    free (flag);
    
}
