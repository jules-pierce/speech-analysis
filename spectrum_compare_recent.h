#ifndef __SPECTRUM_COMPARE_RECENT_H__
#define __SPECTRUM_COMPARE_RECENT_H__

typedef struct feat_vector{
    double x_cog; //center of gravity
    double y_cog;
    double max_row;
    int peaks;
    
}feat_vector_t;

feat_vector_t spectrum_compare_recent(double *fft_dat, int start, int end, int cols);
							 
#endif
