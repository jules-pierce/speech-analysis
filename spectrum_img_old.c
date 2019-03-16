
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
    int i, j;
    short temp;
    double max = 0;
    unsigned char *picdat;
    double *fft_dat;
    int rows;
    int retval;
    int cols = FFTSIZE/2;
    double sum;
    double global_max = 0;
    
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
#ifdef DEBUG
        fprintf (stderr, "loop 0, i = %d\n", i);
#endif
        
        for (j = 0; j < FFTSIZE; j++){
#ifdef DEBUG
            fprintf (stderr, "loop 20, j = %d\n", j);
#endif
            retval = fread(&temp, sizeof(short), 1, indes);
            if (retval != 1) temp = 0;
            bufin[j].re = temp;
#ifdef DEBUG
            fprintf (stderr, "bufin[j] = %f\n", bufin[j].re);
            fprintf (stderr, "loop 22, j = %d\n", j);
#endif
            bufin[j].im = 0;
        }
#ifdef DEBUG
        fprintf (stderr, "loop 1, i = %d\n", i);
#endif
        my_fft.fft1d(bufin, bufout, FFTSIZE);
        
        for (j = 0; j < cols; j++){
            fft_dat[i*cols + j] = sqrt(bufout[j].re * bufout[j].re + bufout[j].im * bufout[j].im);
        }
        //fprintf (stderr, "sum = %lf\n", sum);
#ifdef DEBUG
        fprintf (stderr, "max = %f\n", max);
#endif
        
        for (j = 0; j < cols; j++){
            norms [j] /= 247180; //used to divide by max
            norms[j] *= 255;
            norms[j] = floor (norms[j] + 0.5);
            if (norms[j] > 255) norms[j] = 255;
            if (norms[j] <= 1) norms [j] = 1;
        }
#ifdef DEBUG
        fprintf (stderr, "loop 2, i = %d\n", i);
#endif
        
        for (j = 0; j < cols; j++){
            if (sum > 50000){
                picdat [mi (i, j, cols)] = floor(log10 (norms [j]) * (254/log10(255))+0.5);
            }
            else {
                picdat [mi (i, j, cols)] = 0;
            }
        }
#ifdef DEBUG
        fprintf (stderr, "loop 3, i = %d\n", i);
        fprintf (stderr, "loop 4, i = %d\n", i);
#endif
	}
#ifdef DEBUG
    fprintf (stderr, "loc 4\n");
#endif
    fprintf (stderr, "global max = %lf\n", global_max);
    output_bmp (argv[4], cols, rows, picdat, MONO);

	if( bufin != NULL ) free( bufin );
	if( bufout != NULL ) free( bufout );
	if( indes != NULL ) fclose(indes);
    if( bufout != NULL ) free( norms );
    free (picdat);

}
