//generalize program to work for any image
//make separate program for when its just a single word (or use old one)
//same math and same scaling as this one
//try diff words

//think abt: program to trim an audio file with only one spoken word
//look at energy-- square audio samples
//find a way to trim that


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
    double *norms = NULL;
    int num_pts;
    double Fs;
    double T;
    double deltaf;
    FILE *indes;
    int i, j, k;
    short temp;
    double max = 0;
    unsigned char *picdat;
    double *fft_dat;
    int rows;
    int retval;
    int cols = FFTSIZE/2;
    double sum;
    double global_max = 0;
    int grouping = 90;
    double calcval;
    
    if (argc != 5){
        fprintf (stderr, "error usage is: %s num_pts Fs filename.raw out_img.bmp\n", argv[0]);
        exit (0);
    }
	
	num_pts = atoi (argv[1]);
    Fs = atof (argv[2]);
    T = 1 / Fs;
	deltaf = Fs / num_pts;
    
    indes = fopen (argv[3], "rb");
    if (indes == NULL) {
        fprintf (stderr, "failed to open file\n");
        exit (1);
    }
    //fprintf (stderr, "loc 1\n");

	bufin = (cmplx_fp_t*) malloc(sizeof(cmplx_fp_t) * FFTSIZE);
	bufout = (cmplx_fp_t*) malloc(sizeof(cmplx_fp_t) * FFTSIZE);
    norms = (double*) malloc(sizeof(double) * FFTSIZE);
    
    rows = num_pts/FFTSIZE;
    fprintf (stderr, "rows = %d  cols = %d\n", rows, cols);
    picdat = (unsigned char*) malloc (rows * cols * sizeof (unsigned char));
    fft_dat = (double*) malloc (rows * cols * sizeof (double));
    
    if (picdat == NULL){
        fprintf (stderr, "ERROR: allocating memory picdat\n");
        exit (1);
    }
    
    //fprintf (stderr, "loc 2\n");

	if( bufin == NULL || bufout == NULL ){
		fprintf(stderr, "ERROR: allocating memory\n");
		exit(1);
	}
    
    //fprintf (stderr, "loc 3\n");

	for(i = 0; i < rows; i++){
        max = 0;
        sum = 0;
        
        for (j = 0; j < FFTSIZE; j++){
            retval = fread(&temp, sizeof(short), 1, indes);
            if (retval != 1) temp = 0;
            bufin[j].re = temp;
            bufin[j].im = 0;
        }

        my_fft.fft1d(bufin, bufout, FFTSIZE);
        
        for (j = 0; j < cols; j++){
            fft_dat[i*cols + j] = sqrt(bufout[j].re * bufout[j].re + bufout[j].im * bufout[j].im);
        }
    }
    
    for (k = 0; k < 5; k++){
        max = 0;
        for (i = k*grouping; i < k*grouping + grouping; i++){
            for (j = 0; j < cols; j++){
                if (fft_dat [i * cols + j] > max) max = fft_dat [i * cols + j];
            }
        }
        
        for (i = k*grouping; i < k*grouping + grouping; i++){
            for (j = 0; j < cols; j++){
                calcval = fft_dat [i * cols+j]/max;
                calcval *= 255;
                calcval = log10(calcval) * (255/log10(255));
                if (calcval > 255) picdat [mi (i, j, cols)] = 255;
                else if (calcval < 0) picdat [mi (i, j, cols)] = 0;
                else picdat [mi (i, j, cols)] = floor (calcval + 0.5);
            }
        }
    }
    
    
    for (i = 0; i < rows; i++){
        sum = 0;
        for (j = 0; j < cols; j++){
            sum += fft_dat [i * cols + j];
        }
        fprintf (stderr, "%d     %lf\n", i, sum);
	}
    
    output_bmp (argv[4], cols, rows, picdat, MONO);

	if( bufin != NULL ) free( bufin );
	if( bufout != NULL ) free( bufout );
	if( indes != NULL ) fclose(indes);
    if( bufout != NULL ) free( norms );
    free (picdat);

}


