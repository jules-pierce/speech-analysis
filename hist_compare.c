
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
    int *bank;
    int word[256];
    int peaks;
    int pcount = 0;
    int num_black = 0;
    FILE *indes;
    int *peak_start, *peak_end;
    
    if (argc != 4){
        fprintf (stderr, "error usage is: %s word.bmp bank.bmp peak_data.txt\n", argv[0]);
        exit (0);
    }

    input_bmp (argv[1], &cols1, &rows1, &picdat1, MONO);
    input_bmp (argv[2], &cols2, &rows2, &picdat2, MONO);
    indes = fopen (argv[3], "r");
    if (indes == NULL) {
        fprintf (stderr, "ERROR: unable to open peak_data\n");
        exit (1);
    }
    
    //fprintf (stderr, "loc 1\n");
    
    fscanf (indes, "%d", &peaks);
    
    peak_start = (int*) malloc (peaks * sizeof (int));
    peak_end = (int*) malloc (peaks * sizeof (int));
    
    bank = (int*) malloc (sizeof (int) * peaks * 256);
    
    if (cols1 != cols2){
        fprintf (stderr, "ERROR: pics must be same width\n");
        exit (1);
    }
    
    if (picdat1 == NULL || picdat2 == NULL){
        fprintf (stderr, "ERROR: allocating memory picdat\n");
        exit (1);
    }
    
    //fprintf (stderr, "loc 2\n");
    if (bank == NULL){
        fprintf (stderr, "ERROR: allocating memory bank\n");
        exit (1);
    }
    
    if (peak_start == NULL || peak_end == NULL){
        fprintf (stderr, "ERROR: allocating memory peak_start or peak_end\n");
        exit (1);
    }
    
    for (i = 0; i < peaks; i++){
        fscanf (indes, "%d %d", &peak_start[i], &peak_end[i]);
    }
    
    for (i = 0; i < 256; i++){
        word [i] = 0;
    }
    //fprintf (stderr, "loc 3\n");
    for (i = 0; i < peaks; i++){
        for (j = 0; j < 256; j++){
            bank [i*256 + j] = 0;
        }
    }
    //fprintf (stderr, "loc 4\n");
    
    for (i = 0; i < peaks; i++){
        for (j = peak_start[i]; j < peak_end[i]; j++){
            for (k = 0; k < cols2; k++){
                bank [i * 256 + picdat2 [mi (j, k, cols2)]]++;
            }
        }
    }
    //fprintf (stderr, "loc 5\n");
    
    for (i = 0; i < peaks * 256; i++){
        fprintf (stderr, "%d    %d\n", i, bank [i]);
    }
    
   
    free (picdat1);
    free (picdat2);
    free (bank);
    free (word);
    free (peak_start);
    free (peak_end);
    fclose (indes);
}


