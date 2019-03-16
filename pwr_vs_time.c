//graph sum of pixels in each row
//maybe normalize over number of rows (as a percent of the total pic)

//instead of intensities, try number of peaks
//use simple threshold for peak: if bigger than threshold its a peak (ignore ones right next to each other)

//feat_vec: is there a way to find number of syllables
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "fftRoutines.h"
#include "imgstuff.h"
#include "bmp_io.h"
#include "spectrum_compare_recent.h"
//#define DEBUG
#define FFTSIZE 64

int find_peaks (double* DA, int num_pts, int* peak_start, int* peak_end, int *flag, double energy_thresh);

int main(int argc, char *argv[] )
{
    int cols, rows;
    unsigned char *picdat;
    int i, j;
    int sum = 0;
    int max = 0;
    int place = 0;
    
    if (argc != 2){
        fprintf (stderr, "error usage is: %s img.bmp\n", argv[0]);
        exit (0);
    }
	
    input_bmp (argv[1], &cols, &rows, &picdat, MONO);
    
    /*for (i = 0; i < rows; i++){
        sum = 0;
        for (j = 0; j < cols; j++){
            sum += picdat [mi (i, j, cols)];
        }
        fprintf (stderr, "%d   %d\n", i, sum);
    }*/
    
    /*for (j = 0; j < cols; j++){
        sum = 0;
        for (i = 0; i < rows; i++){
            sum += picdat [mi (i, j, cols)];
        }
        fprintf (stderr, "%d   %d\n", j, sum);
    }*/
    
    for (i = 0; i < rows; i++){
        max = 0;
        place = 0;
        for (j = 0; j < cols; j++){
            if (picdat [mi (i, j, cols)] > max){
                max = picdat [mi (i, j, cols)];
                place = j;
            }
        }
        fprintf (stderr, "%d   %d\n", i, place);
    }

    free (picdat);
}

