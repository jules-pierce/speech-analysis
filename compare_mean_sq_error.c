
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
    unsigned char *bank, *word;
    int val;
    double sum;
    double *error;
    int num_compare;
    int end_word = 0;
    double max = 1E20;
    int place = 0;
    int sum_word = 0;
    int sum_bw = 0;
    
    if (argc != 3){
        fprintf (stderr, "error usage is: %s word.bmp bank.bmp\n", argv[0]);
        exit (0);
    }

    input_bmp (argv[1], &cols1, &rows1, &picdat1, MONO);
    input_bmp (argv[2], &cols2, &rows2, &picdat2, MONO);
    
    bank = (unsigned char*) malloc (sizeof (unsigned char) * rows2);
    word = (unsigned char*) malloc (sizeof (unsigned char) * rows1);
    
    num_compare = rows2-rows1;
    
    error = (double*) malloc (sizeof(double) * num_compare);
    
    if (cols1 != cols2){
        fprintf (stderr, "ERROR: pics must be same width\n");
        exit (1);
    }
    
    if (picdat1 == NULL || picdat2 == NULL){
        fprintf (stderr, "ERROR: allocating memory picdat\n");
        exit (1);
    }
    
    if (bank == NULL || word == NULL){
        fprintf (stderr, "ERROR: allocating bank/word memory\n");
        exit (1);
    }
    
    if (error == NULL){
        fprintf (stderr, "ERROR: allocating memory error\n");
        exit (1);
    }
    
    for (i = 0; i < rows1; i++){
        val = 0;
        sum = 0;
        for (j = 0; j < cols1; j++){
            sum += picdat1 [mi (i, j, cols1)];
        }
        if (sum >= 256) val = 1;
        word [i] = val;
        //fprintf (stderr, "i = %d   word[i]=%d\n", i, word[i]);
        sum_word += word[i];
    }
    //fprintf (stderr, "sum_word = %d\n", sum_word);
    
    for (i = 0; i < rows1; i++){
        if (word [i] == 1){
            if (i > end_word) end_word = i;
        }
    }
    
    for (i = 0; i < rows2; i++){
        val = 0;
        sum = 0;
        for (j = 0; j < cols2; j++){
            sum += picdat2 [mi (i, j, cols2)];
        }
        if (sum >= 256) val = 1;
        bank [i] = val;
        //fprintf (stderr, "i = %d   bank[i]=%d\n", i, bank[i]);
        
    }
    //exit (0);
    
    
    for (i = 0; i < num_compare; i++){
        sum = 0;
        sum_bw = 0;
        for (k = 0; k < rows1; k++){
            sum_bw += bank[i+k] * word [k];
        }
        error [i] = 1000;
        if (sum_bw < sum_word-2) continue;
        for (j = 0; j < rows1; j++){
            for (k = 0; k < cols1/3; k++){
                sum += (picdat1 [mi (j, k, cols1)] - picdat2 [mi (i + j, k, cols2)]) * (picdat1 [mi (j, k, cols1)] - picdat2 [mi (i + j, k, cols2)]);
            }
        }
        error [i] = sum/(rows1 * cols1);
    }
    
    for (i = 0; i < num_compare; i++){
        if (error[i] < max) {
            max = error [i];
            place = i;
        }
        fprintf (stderr, "%3d   %lf\n", i, error[i]);
    }
    
    //fprintf (stderr, "location = %d\n", place);

    free (picdat1);
    free (picdat2);
    free (error);
}
