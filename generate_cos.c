

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "fftRoutines.h"
#include "imgstuff.h"
#include "bmp_io.h"
//#define DEBUG

int main(int argc, char *argv[] )
{
    int f = 1000;
    int i;
    int Fs = 8000;
    double delta_t = 1.0/Fs;
    short val;
    double t;
    FILE *outdes;
	
    outdes = fopen ("out.raw", "wb");
    
#ifdef DEBUG
    fprintf (stderr, "loc 1\n");
#endif
    
    for (i = 1; i < 6; i++){
        f = i * 1000;
        if (f==4000) f = 3500;
        
#ifdef DEBUG
        fprintf (stderr, "loc 2 i = %d\n", i);
#endif
        
        for (t = 0; t < 0.8192; t += delta_t){
            val = 10000 * cos (2 * M_PI * f * t);
            fwrite (&val, sizeof(short), 1, outdes);
#ifdef DEBUG
            fprintf (stderr, "loc 3 t = %lf\n", t);
#endif

        }
    }
    
    fclose (outdes);
}
