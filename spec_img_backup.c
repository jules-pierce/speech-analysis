
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

int find_peaks (double* DA, int num_pts, int* peak_start, int* peak_end, int *flag);
int check_bounds (int index, int array_size, char* func_name, int loc);

int bufin_size;
int bufout_size;
int peak_start_size;
int peak_end_size;
int energy_size;
int picdat_size;
int fft_dat_size;

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
    int i, j, k;
    short temp;
    double max = 0;
    unsigned char *picdat = NULL;
    double *fft_dat = NULL;
    int rows;
    int retval;
    int cols = FFTSIZE/2;
    int peaks;
    int sum = 0;
    double temp_f;
    int db_clip = -25;
    FILE *outdes;
    int *flag;
    
    if (argc != 6){
        fprintf (stderr, "error usage is: %s num_pts Fs filename.raw out_img.bmp out.txt\n", argv[0]);
        exit (0);
    }
	
	num_pts = atoi (argv[1]);
    Fs = atof (argv[2]);
    T = 1 / Fs;
    
    indes = fopen (argv[3], "rb");
    outdes = fopen (argv[5], "w");
    
    if (indes == NULL) {
        fprintf (stderr, "failed to open file\n");
        exit (1);
    }
    
    bufin_size = bufout_size = FFTSIZE;
	bufin = (cmplx_fp_t*) malloc(sizeof(cmplx_fp_t) * bufin_size);
	bufout = (cmplx_fp_t*) malloc(sizeof(cmplx_fp_t) * bufout_size);
    flag = (int*) malloc (num_pts * sizeof(int));

    
    rows = num_pts/FFTSIZE;
    fprintf (stderr, "rows = %d  cols = %d\n", rows, cols);
    
    picdat = (unsigned char*) malloc (rows * cols * sizeof (unsigned char));
    picdat_size = rows * cols;
    fft_dat = (double*) malloc (rows * cols * sizeof (double));
    fft_dat_size = rows * cols;
    energy = (double*) malloc (rows * sizeof (double));
    energy_size = rows;
    
    peak_start = (int*) malloc (rows * sizeof (int));
    peak_start_size = rows;
    peak_end = (int*) malloc (rows * sizeof (int));
    peak_end_size = rows;
    
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
        check_bounds (i, peak_start_size, "peak_start", 0);
        peak_start [i] = -1;
        check_bounds (i, peak_end_size, "peak_end", 1);
        peak_end [i] = -1;
    }

	for(i = 0; i < rows; i++){
        max = 0;
        
        for (j = 0; j < FFTSIZE; j++){
            retval = fread(&temp, sizeof(short), 1, indes);
            if (retval != 1) {
                temp = 0;
            }
            check_bounds (j, bufin_size, "bufin", 0);
            bufin[j].re = temp;
            bufin[j].im = 0;
        }

        //fprintf (stderr, "starting transform..."); fflush (stderr);
        my_fft.fft1d(bufin, bufout, FFTSIZE);
        //fprintf (stderr, "done\n"); fflush (stderr);
        
        for (j = 0; j < cols; j++){
            check_bounds (j, bufout_size, "bufout", 0);
            check_bounds (i * cols + j, fft_dat_size, "fft_dat", 0);
            fft_dat[i*cols + j] = sqrt(bufout[j].re * bufout[j].re + bufout[j].im * bufout[j].im);
        }
    }
    
    for (i = 0; i < rows; i++){
        sum = 0;
        for (j = 0; j < cols; j++){
            check_bounds (i*cols + j, fft_dat_size, "fft_dat", 0);
            sum += fft_dat [i*cols +j];
        }
        check_bounds (i, energy_size, "energy", 0);
        energy [i] = sum;
        fprintf (stderr, "%3d, %lf\n", i, energy[i]);
    }
    
    peaks = find_peaks (energy, rows, peak_start, peak_end, flag);
    
    for (i = 0; i < peaks; i++){
        fprintf (stderr, "peak = %4d  start = %4d   end = %4d\n", i, peak_start[i], peak_end[i]);
    }
    
    
    //normalize each peak, store those values in picdat
    for (i = 0; i < peaks; i++){
        max = 0;
        check_bounds (i, peak_start_size, "peak_start", 0);
        check_bounds (i, peak_end_size, "peak_end", 2);
        for (j = peak_start[i]; j <= peak_end [i]; j++){
            for (k = 0; k < cols; k++){
                check_bounds (j*cols+k, fft_dat_size, "fft_dat", 0);
                if (fft_dat[j * cols + k] > max) max = fft_dat[j * cols + k];
            }
        }
        
        check_bounds (i, peak_start_size, "peak_start", 0);
        check_bounds (i, peak_end_size, "peak_end", 3);
        
        for (j = peak_start[i]; j <= peak_end [i]; j++){
            for (k = 0; k < cols; k++){
                //fft_dat [j * cols + k] *= 255/max;
                //temp_f = 1 + (fft_dat [j*cols + k]/max);
                //fft_dat [j * cols + k] = 255 * log10 (temp_f)/log10(2);
                check_bounds (j*cols + k, fft_dat_size, "fft_dat", 0);
                if (fft_dat [j*cols+k] > 0){
                    temp_f = 10 * log10 (fft_dat[j*cols +k]/max);
                    
                }
                else temp_f = db_clip;
                if (temp_f < db_clip) temp_f = db_clip;
                fft_dat [j * cols + k] = (-255.0/db_clip) * temp_f + 255;
            }
        }
    }
    
    //picdat [mi (i, j, cols)]: i is rows, j is cols
    
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            check_bounds (i * cols + j, fft_dat_size, "fft_dat", 0);
            check_bounds (mi (i, j, cols), picdat_size, "picdat", 0);
            if (flag [i] == 0) picdat [mi (i, j, cols)] = 0;
            else if (i > peak_end[peaks-1]) picdat [mi (i, j, cols)] = 0;
            else if (fft_dat [i * cols + j] > 255) picdat [mi (i, j, cols)] = 255;
            else if (fft_dat [i * cols + j] < 0) picdat [mi (i, j, cols)] = 0;
            else picdat [mi (i, j, cols)] = fft_dat [i * cols + j];        }
    }
    
    //fprintf (stderr, "calling output_bmp..."); fflush (stderr);
    output_bmp (argv[4], cols, rows, picdat, MONO);
    //fprintf (stderr, "done\n"); fflush (stderr);
    
    fprintf (outdes, "%d\n", peaks);
    for (i = 0; i < peaks; i++){
        fprintf (outdes, "%d     %d\n", peak_start[i], peak_end[i]);
    }

	if( bufin != NULL ) free( bufin );
	if( bufout != NULL ) free( bufout );
	if( indes != NULL ) fclose(indes);
    if( fft_dat != NULL ) free( fft_dat );
    if( peak_start != NULL ) free( peak_start );
    if( peak_end != NULL ) free( peak_end );
    if (picdat != NULL) free (picdat);
    fclose (outdes);

}

int find_peaks (double* DA, int num_pts, int* peak_start, int* peak_end, int *flag){
    double max = 0;
    double min = 1E50;
    double avg = 0;
    int i;
    int peak = 0;
    double sum = 0;
    int seek_val = 1;
    int flag_size;
    
    //fprintf (stderr, "+++++++starting find_peaks\n"); fflush (stderr);
    
    flag_size = num_pts;
    
    for (i = 0; i < num_pts; i++){
        check_bounds (i, energy_size, "DA", 0);
        if (DA[i] > max) max = DA[i];
        if (DA [i] < min) min = DA[i];
        avg += DA[i];
    }
    
    avg /= num_pts;
    
    for (i = 0; i < num_pts; i++){
        check_bounds (i, energy_size, "DA", 0);
        check_bounds (i, flag_size, "flag", 0);
        if (DA[i] >= 0.01*max) flag[i] = 1;
        else flag[i] = 0;
        if (i < 10) flag[i] = 0; //get rid of noise at the beginning of the recording
    }
    
    for (i = 0; i < num_pts; i++){
        check_bounds (i, flag_size, "flag", 0);
        fprintf (stderr, "%d", flag[i]);
    }
    fprintf (stderr, "\n\n");
    
    for (i = 1; i < num_pts - 1; i++){
        check_bounds (i, flag_size, "flag", 0);
        check_bounds (i-1, flag_size, "flag", 0);
        check_bounds (i+1, flag_size, "flag", 0);
        if (flag[i] == 1 && flag [i-1] == 0 && flag [i+1] == 0) flag [i] = 0;
        else if (flag [i] == 0 && flag [i-1] == 1 && flag [i+1] == 1) flag [i] = 1;
    }
    
    for (i = 0; i < num_pts; i++){
        check_bounds (i, flag_size, "flag", 0);
        fprintf (stderr, "%d", flag[i]);
    }
    fprintf (stderr, "\n");
    
    i = 0;
    int j=0;
    while (i < num_pts){
        //fprintf (stderr, "i = %d\n", i);
        check_bounds (i, flag_size, "flag", 0);
        while (flag [i] != seek_val){
            i++;
            if (i >= num_pts) break;
            check_bounds (i, flag_size, "flag", 0);
            j++;
        }
        if (i >= num_pts) break;
        
        if (seek_val == 1){
            check_bounds (peak, peak_start_size, "peak_start", 0);
            peak_start [peak] = i;
            seek_val = 0;
            j = 0;
        }
        else {
            check_bounds (peak, peak_end_size, "peak_end", 4);
            peak_end [peak] = i;
            peak ++;
            seek_val = 1;
            fprintf (stderr, "run_length = %d\n", j);
            j = 0;
        }
        
    }
    
    fprintf (stderr, "\n%d peaks\n", peak);
    for (i = 0; i < peak; i++){
        check_bounds (i, peak_start_size, "peak_start", 0);
        check_bounds (i, peak_end_size, "peak_end", 5);
        fprintf (stderr, "start = %d  end = %d\n", peak_start[i], peak_end[i]);
    }
    
    /*for (i = 0; i < peak; i++){
        check_bounds (i, peak_start_size, "peak_start", 0);
        //check_bounds (i-1, peak_end_size, "peak_end", 6);
        check_bounds (i, peak_end_size, "peak_end", 7);
        check_bounds (i+1, peak_start_size, "peak_start", 0);
        if (i == 0) {
            peak_start[0] = peak_start[0] - 5; //DEBUG this could be a problem
            peak_end [0] += (peak_start[1] - peak_end[0])/2;
        }
        else if (i == peak-1){
            check_bounds (i-1, peak_end_size, "peak_end", 10);
            peak_start [i] = peak_end [i-1] + 1;
            peak_end[i] = peak_end[i] + 5;//DEBUG this could be a problem
        }
        else {
            check_bounds (i-1, peak_end_size, "peak_end", 11);
            peak_start[i] = peak_end[i-1] + 1;
            peak_end [i] += (peak_start[i+1] - peak_end[i])/2;
        }
    }*/
    
    for (i = 0; i < peak; i++){
        check_bounds (i, peak_start_size, "peak_start", 0);
        check_bounds (i, peak_end_size, "peak_end", 8);
        fprintf (stderr, "start = %d  end = %d  width = %d\n", peak_start[i], peak_end[i], peak_end[i] - peak_start[i]);
    }
    
    fprintf (stderr, "++++++ ending find_peaks\n"); fflush (stderr);
    return peak;
    
    free (flag);
    
}

int check_bounds (int index, int array_size, char* func_name, int loc){
    if (index < 0){
        fprintf (stderr, "%s: out of bounds access index = %d loc = %d ***********\n", func_name, index, loc);
        fflush (stderr);
        return -1;
    }
    
    else if (index >= array_size){
        fprintf (stderr, "%s: out of bounds access index = %d loc = %d ************\n", func_name, index, loc);
        fflush (stderr);
        return -2;
    }
    else return 0;
}
