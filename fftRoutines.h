#ifndef FFTROUTINES_H
#define FFTROUTINES_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>

typedef struct cmplx_fp{
   double re;
	 double im;
} cmplx_fp_t;	 



/***** Don't use the below "kiss" elements in your program!!!! ****/
#define MAXFACTORS 32
typedef struct{
	double r;
	double i;
}kiss_fft_cpx;	
struct kiss_fft_state{
    int nfft;
    int inverse;
    int factors[2*MAXFACTORS];
    kiss_fft_cpx twiddles[1];
};
typedef struct kiss_fft_state* kiss_fft_cfg;
/***** Don't use the above "kiss" elements in your program!!!! ****/



class fftRoutines{
  public:
		fftRoutines();
		~fftRoutines();

		void fft1d( cmplx_fp_t *in, cmplx_fp_t *out, int size);
		void ifft1d( cmplx_fp_t *in, cmplx_fp_t *out, int size);

	private:
		// Original "kiss" static variables
		kiss_fft_cpx *scratchbuf;
		size_t nscratchbuf;
		kiss_fft_cpx *tmpbuf;
		size_t ntmpbuf;

		// Original "kiss" interface functions
		kiss_fft_cfg kiss_fft_alloc(int nfft,int inverse_fft,
																void * mem,size_t * lenmem); 
		void kiss_fft(kiss_fft_cfg cfg,const kiss_fft_cpx *fin,kiss_fft_cpx *fout);
		void kiss_fft_stride(kiss_fft_cfg cfg,const kiss_fft_cpx *fin,
												 kiss_fft_cpx *fout,int fin_stride);
		int kiss_fft_next_fast_size(int n);

		// Original "kiss" helper functions
		void kf_bfly2(kiss_fft_cpx *Fout, const size_t fstride,
		              kiss_fft_cfg st, int m);
		void kf_bfly4(kiss_fft_cpx *Fout, const size_t fstride,
		              const kiss_fft_cfg st, const size_t m);
		void kf_bfly3(kiss_fft_cpx *Fout, const size_t fstride,
		              const kiss_fft_cfg st, const size_t m);
		void kf_bfly5(kiss_fft_cpx *Fout, const size_t fstride,
		              const kiss_fft_cfg st, int m);
		void kf_bfly_generic(kiss_fft_cpx *Fout, const size_t fstride,
		              const kiss_fft_cfg st, int m, int p);
		void kf_work(kiss_fft_cpx *Fout, const kiss_fft_cpx *f,
		             const size_t fstride, int in_stride, int *factors,
								 const kiss_fft_cfg st);
		void kf_factor(int n, int *facbuf);						 
};
#endif
