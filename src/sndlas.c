/*
** sndlas.c
**
** Public Domain Mark 1.0
** No Copyright
**
** Depends:
** 1) libsndfile
** 2) zxfft (https://github.com/Sound-Linux-More/zxfft)
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sndfile.h>

#include "zxfft/zx_fft.h"

#define BUFFER_LEN 256
#define SAMPLE_LEN 32768
#define CHANNEL_LEN 256
unsigned THRESHOLD_FALL = 5;
float THRESHOLD_LAST = 1.5f;
float THRESHOLD_NEXT = 3.0f;
float THRESHOLD_FINAL = 25.0f;
unsigned THRESHOLD_HZ = 20000;
static double samples[SAMPLE_LEN];  
COMPLEX ffts[SAMPLE_LEN];  
static double hann[SAMPLE_LEN];

void usage (const char *progname)
{
        printf ("\nSndLAS: Lossy Audio Spotter wav file.\n") ;
        printf ("    Usage : %s [options] <filename>\n", progname) ;
        printf ("           options:\n") ;
        printf ("          -f N    threshold fall (default = %d)\n", THRESHOLD_FALL);
        printf ("          -l N    threshold value last (default = %f)\n", THRESHOLD_LAST);
        printf ("          -n N    threshold value next (default = %f)\n", THRESHOLD_NEXT);
        printf ("          -e N    threshold value final (default = %f)\n", THRESHOLD_FINAL);
        printf ("          -h      this help\n");
}

static void precalc_hann(void)
{
    int i;
    for (i = 0; i < SAMPLE_LEN; i++)
        hann[i] = .5f * (1 - cos(2 * M_PI * i / (SAMPLE_LEN-1)));
}

static void sndlas_file (const char *infilename)
{
    static double buffer [BUFFER_LEN] ;

    SNDFILE *infile;
    SF_INFO sfinfo ;
    int readcount, hfreq;
    unsigned k, j, jn = 0, bn = 0, last, t;
    unsigned channels, samplerate;
    long unsigned n = 0, hfreqs = 0;
    double re, im, sr;
    
    printf ("    %s\n", infilename) ;
    fflush (stdout) ;

    if (! (infile = sf_open (infilename, SFM_READ, &sfinfo)))
    {
        printf ("Error : could not open file : %s\n", infilename) ;
        puts (sf_strerror (NULL)) ;
        exit (1) ;
    }
    if (! sf_format_check (&sfinfo))
    {
        sf_close (infile) ;
        printf ("Invalid encoding\n") ;
        return ;
    }
    channels = sfinfo.channels;
    samplerate = sfinfo.samplerate;
    sr = (double)samplerate / SAMPLE_LEN;
    jn = 0;
    while ((readcount = sf_readf_double (infile, buffer, BUFFER_LEN)) > 0)
    {
        for (k = 0; k < readcount; k++)
        {
            j = (jn + k) / channels;
            if (j < SAMPLE_LEN)
                samples[j] += buffer[k];
        }
        jn += readcount;
        n += readcount;
        if (jn >= (channels * SAMPLE_LEN))
        {
            jn = 0;
            bn++;
            for (k = 0; k < SAMPLE_LEN; k++)
            {
                samples[k] /= channels;
            }
            for (k = 0; k < SAMPLE_LEN; k++)
            {
                samples[k] *= hann[k];
            }
            for (k = 0; k < SAMPLE_LEN; k++)
            {
                ffts[k].real = (float)samples[k];
                ffts[k].imag = 0.0f;
            }
            fft(ffts, SAMPLE_LEN);
            for (k = 0; k < SAMPLE_LEN; k++)
            {
                re = (double)ffts[k].real;
                im = (double)ffts[k].imag;
                samples[k] = re * re + im * im;
            }
            last = SAMPLE_LEN / 2;
            for (k = last - 1; k > 0; k--)
            {
                if (fabsf(samples[k] - samples[last]) > THRESHOLD_LAST ||
                    fabsf(samples[k] - samples[k + 1]) > THRESHOLD_NEXT)
                    break;
            }
            for (t = k - THRESHOLD_FALL; t; t--)
                if ((samples[t] - samples[t + THRESHOLD_FALL]) > THRESHOLD_FINAL)
                    break;

            hfreq = t * sr;
            if (hfreq < 0) hfreq = 0;
            printf("# len=%lu higher freq=%dHz → %s\n", n / channels, hfreq, (hfreq < THRESHOLD_HZ ? "lossy" : "lossless"));
            for (k = 0; k < SAMPLE_LEN; k++)
            {
                samples[k] = 0.0;
            }
            hfreqs += hfreq;
        }
    }
    n /= channels;
    hfreqs /= bn;
    printf ("ok\n");
    printf("# higher freq=%luHz → %s\n", hfreqs, (hfreqs < THRESHOLD_HZ ? "lossy" : "lossless"));

} /* sndlas_file */

int main (int argc, char **argv)
{
    int opt;
    int fhelp = 0;
    while ((opt = getopt(argc, argv, ":f:l:n:e:h")) != -1)
    {
        switch(opt)
        {
            case 'f':
                THRESHOLD_FALL = atoi(optarg);
                printf ("    threshold fall = %d\n", THRESHOLD_FALL);
            break;
            case 'l':
                THRESHOLD_LAST = atof(optarg);
                printf ("    threshold value last = %f\n", THRESHOLD_LAST);
            break;
            case 'n':
                THRESHOLD_NEXT = atof(optarg);
                printf ("    threshold value next = %f\n", THRESHOLD_NEXT);
            break;
            case 'e':
                THRESHOLD_FINAL = atof(optarg);
                printf ("    threshold value final = %f\n", THRESHOLD_FINAL);
            break;
            case 'h':
                fhelp = 1;
            break;
            case ':':
                printf("option needs a value\n");
            break;
            case '?':
                printf("unknown option: %c\n", optopt);
            break;
        }
    }
    
    if(optind + 1 > argc || fhelp > 0)
    {
        usage(argv[0]);
        return 1;
    }

    precalc_hann();
    sndlas_file (argv[optind]);

    return 0 ;
} /* main */
