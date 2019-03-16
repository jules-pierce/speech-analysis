#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "imgstuff.h"
#include "bmp_io.h"
//#define DEBUG

int main(int argc, char *argv[] )
{
    int i, j, k;
    unsigned char *picdat1, *picdat2;
    int rows1, rows2;
    int cols1, cols2;
    int num_compare;
    double *error;
    double sum;
    double max = 1E20;
    int place = 0;
    
    if (argc != 3){
        fprintf (stderr, "error usage is: %s word.bmp bank.bmp\n", argv[0]);
        exit (0);
    }

    input_bmp (argv[1], &cols1, &rows1, &picdat1, MONO);
    input_bmp (argv[2], &cols2, &rows2, &picdat2, MONO);
    
    num_compare = rows2-rows1 + 1;
    
    error = (double*) malloc (sizeof(double) * num_compare);
    
    if (cols1 != cols2){
        fprintf (stderr, "ERROR: images aren't the same width\n");
        exit (1);
    }
    
    if (picdat1 == NULL || picdat2 == NULL){
        fprintf (stderr, "ERROR: allocating memory picdat\n");
        exit (1);
    }

    for (i = 0; i < num_compare; i++){
        //i is the starting row for the bank image
        //i also corresponds to the position in the error array
        sum = 0;
        for (k = 0; k < rows1; k++){
            for (j = 0; j < cols1; j++){
                sum += (picdat1 [mi (k, j, cols1)] - picdat2 [mi (i+k, j, cols1)]) * (picdat1 [mi (k, j, cols1)] - picdat2 [mi (i+k, j, cols1)]);
            }
        }
        error [i] = sum/(rows1*cols1);
        fprintf (stderr, "%3d     %lf\n", i, error[i]);
    }
    
    for (i = 0; i < num_compare; i++){
        if (error[i] < max) {
            max = error [i];
            place = i;
        }
    }
    
    //fprintf (stderr, "location = %d\n", place);

    free (picdat1);
    free (picdat2);
}
