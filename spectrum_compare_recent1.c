#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "imgstuff.h"
#include "bmp_io.h"
//#define DEBUG

typedef struct feat_vector{
    int energy; //sum of pixel intensities
    int x_cog; //center of cols
    int y_cog; //center of rows
    int max_vert; //max column
    int max_hori; //max row
}feat_vector_t;

int main(int argc, char *argv[] )
{
    int i, j, k;
    unsigned char *picdat1, *picdat2;
    int rows1, rows2;
    int cols1, cols2;
    int num_prop = 5;
    int num_peaks;
    feat_vector_t *bank;
    feat_vector_t word;
    int *error;
    int sum = 0;
    int max = 0;
    int place = 0;
    int *peak_start;
    int *peak_end;
    double total = 0;
    double moment_total;
    double weight_total;
    
    if (argc != 4){
        fprintf (stderr, "error usage is: %s word.bmp bank.bmp num_peaks\n", argv[0]);
        exit (0);
    }

    input_bmp (argv[1], &cols1, &rows1, &picdat1, MONO);
    input_bmp (argv[2], &cols2, &rows2, &picdat2, MONO);
    
    num_peaks = atoi (argv[3]);
    
    bank = (feat_vector_t*) malloc (num_peaks * sizeof (feat_vector_t));
    error = (int*) malloc (num_peaks * sizeof (double));
    peak_start = (int*) malloc (num_peaks * sizeof (int));
    peak_end = (int*) malloc (num_peaks * sizeof (int));
    
    if (cols1 != cols2){
        fprintf (stderr, "ERROR: images aren't the same width\n");
        exit (1);
    }
    
    if (picdat1 == NULL || picdat2 == NULL){
        fprintf (stderr, "ERROR: allocating memory picdat\n");
        exit (1);
    }
    
    //finding peaks
    place = 0;
    k = 0;
    for (i = 0; i < rows2; i++){
        sum = 0;
        for (j = 0; j < cols2; j++){
            sum += picdat2 [mi (i, j, cols2)];
        }
        if (sum > 5 && k == place){
            peak_start [place] = i;
            k++;
        }
        
        else if (sum < 5 && k != place){
            peak_end [place] = i;
            place++;
        }
        
        if (place > num_peaks){
            fprintf (stderr, "Error: place out of range finding peaks\n");
            exit (0);
        }
    }
    
    for (i = 0; i < num_peaks; i++){
        fprintf (stderr, "start = %d  end = %d\n", peak_start[i], peak_end[i]);
    }
    
    for (i = 0; i < num_peaks; i++){
        //calculate total energy
        total = 0;
        for (j = peak_start[i]; j <= peak_end[i]; j++){
            for (k = 0; k < cols2; k++){
                total += picdat2 [mi (j, k, cols2)];
            }
        }
        bank[i].energy = total;
        
        //calculate x center of gravity
        moment_total = 0;
        weight_total = 0;
        for (k = 0; k < cols2; k++){
            sum = 0;
            for (j = peak_start[i]; j <= peak_end[i]; j++){
                sum += picdat2 [mi (j, k, cols2)];
            }
            weight_total += sum;
            moment_total += sum * k;
        }
        bank[i].x_cog = floor (moment_total/weight_total);
        
        //calculate y center of gravity
        moment_total = 0;
        weight_total = 0;
        for (k = peak_start[i]; k <= peak_end[i]; k++){
            sum = 0;
            for (j = 0; j < cols2; j++){
                sum += picdat2 [mi (k, j, cols2)];
            }
            weight_total += sum;
            moment_total += sum * k;
        }
        bank[i].y_cog = floor (moment_total/weight_total);
        
        //calculate max_vert for the bank
        max = 0;
        for (j = 0; j < cols2; j++){
            sum = 0;
            for (k = peak_start[i]; k <= peak_end[i]; k++){
                sum += picdat2 [mi (k, j, cols2)];
            }
            if (sum > max){
                max = sum;
                place = i;
            }
        }
        bank[i].max_vert = place;
        
        //calculate max_hori
        max = 0;
        for (j = peak_start[i]; j <= peak_end[i]; j++){
            sum = 0;
            for (k = 0; k < cols2; k++){
                sum += picdat2 [mi (j, k, cols2)];
            }
            if (sum > max){
                max = sum;
                place = i;
            }
        }
        bank[i].max_hori = place;
    }
    
    total = 0;
    //calculate total energy for the word
    for (i = 0; i < rows1; i++){
        for (j = 0; j < cols1; j++){
            total += picdat1 [mi (i, j, cols1)];
        }
    }
    word.energy = total;
    
    //calculate x center of gravity for the word
    moment_total = 0;
    weight_total = 0;
    for (i = 0; i < cols2; i++){
        sum = 0;
        for (j = 0; j < rows2; j++){
            sum += picdat1 [mi (j, i, cols2)];
        }
        weight_total += sum;
        moment_total += sum * i;
    }
    word.x_cog = floor (moment_total/weight_total);
    
    //calculate y center of gravity for the word
    moment_total = 0;
    weight_total = 0;
    for (i = 0; i < rows2; i++){
        sum = 0;
        for (j = 0; j < cols2; j++){
            sum += picdat1 [mi (i, j, cols2)];
        }
        weight_total += sum;
        moment_total += sum * i;
    }
    word.y_cog = floor (moment_total/weight_total);
    
    //calculate max_vert for the word
    max = 0;
    for (i = 0; i < cols1; i++){
        sum = 0;
        for (j = 0; j < rows1; j++){
            sum += picdat1 [mi (j, i, cols1)];
        }
        if (sum > max){
            max = sum;
            place = i;
        }
    }
    word.max_vert = place;
    
    //calculate max_hori for the word
    max = 0;
    for (i = 0; i < rows1; i++){
        sum = 0;
        for (j = 0; j < cols1; j++){
            sum += picdat1 [mi (i, j, cols1)];
        }
        if (sum > max){
            max = sum;
            place = i;
        }
    }
    word.max_hori = place;
    
    for (i = 0; i < num_peaks; i++){
        error [i] = abs (word.energy - bank[i].energy) + abs (word.x_cog - bank[i].x_cog) + abs (word.y_cog - bank[i].y_cog) + abs (word.max_hori - bank[i].max_hori) + abs (word.max_vert - bank[i].max_vert);
    }
    
    for (i = 0; i < num_peaks; i++){
        fprintf (stderr, "%d    %d\n", i, error [i]);
    }

    free (picdat1);
    free (picdat2);
}
