// this routine performs the FFT and computes the spectrum
// of a real-valued input signal.  Power of non-DC coefficients
// is doubled to account for negative frequencies

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "fftRoutines.h"

int main(int argc, char *argv[] )
{
	fftRoutines my_fft;
	cmplx_fp_t *bufin = NULL;
	cmplx_fp_t *bufout = NULL;
    int num_pts;
    double Fs;
    double T;
    double deltaf;
    FILE *indes;
    int i;
    
    if (argc != 4){
        fprintf (stderr, "error usage is: %s num_pts Fs filename.txt\n", argv[0]);
        exit (0);
    }
	
	num_pts = atoi (argv[1]);
    Fs = atof (argv[2]);
    T = 1 / Fs;
	deltaf = Fs / num_pts;
    
    indes = fopen (argv[3], "r");

	bufin = (cmplx_fp_t*) malloc(sizeof(cmplx_fp_t) * num_pts);
	bufout = (cmplx_fp_t*) malloc(sizeof(cmplx_fp_t) * num_pts);

	if( bufin == NULL || bufout == NULL ){
		fprintf(stderr, "ERROR: allocating memory\n");
		exit(1);
	}

	for(i = 0; i < num_pts; i++){
		fscanf(indes, "%lf",&(bufin[i].re) );
		bufin[i].im = 0;
	}

    my_fft.fft1d(bufin, bufout, num_pts);
    
    for (i = 0; i < num_pts; i++){
        fprintf (stderr, "bufout[%d].re = %lf   im = %lf   norm = %lf\n", i, bufout[i].re, bufout[i].im, sqrt(bufout[i].re * bufout[i].re + bufout[i].im * bufout[i].im));
    }

	if( bufin != NULL ) free( bufin );
	if( bufout != NULL ) free( bufout );
	if( indes != NULL ) fclose(indes);

}
