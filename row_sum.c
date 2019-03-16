

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
    int i, j;
    unsigned char *picdat;
    int rows;
    int cols;
    double sum = 0;
    
    if (argc != 2){
        fprintf (stderr, "error usage is: %s bank.bmp\n", argv[0]);
        exit (0);
    }

    input_bmp (argv[1], &cols, &rows, &picdat, MONO);
    
    for (i = 0; i < rows; i++){
        sum = 0;
        for (j = 0; j < cols; j++){
            sum += picdat [mi (i, j, cols)];
        }
        fprintf (stderr, "%3d     %lf\n", i, sum);
    }

}
