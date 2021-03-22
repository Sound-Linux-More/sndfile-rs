/*
** snddenoise.c
**
** Public Domain Mark 1.0
** No Copyright
**
** Depends:
** 1) libsndfile
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sndfile.h>

#define BUFFER_LEN          256
#define CHANNEL_LEN         256
#define DELTA_LEN           131072

static void snddenoise_file (const char *infilename, const char *outfilename, double kdenoise) ;

void usage (const char *progname)
{
    printf ("\nSndDeNoise: DeNoise wav file.\n") ;
    printf ("    Usage : %s [options] <filename> <outfilename>\n", progname) ;
    printf ("           options:\n") ;
    printf ("          -k N.N  coefficient denoise (double, optional, default = 0.0[auto])\n");
    printf ("          -h      this help\n");
}

static void snddenoise_file (const char *infilename, const char *outfilename, double kdenoise)
{
    static double buffer [BUFFER_LEN] ;
    double bufferout [BUFFER_LEN] ;

    SNDFILE *infile, *outfile ;
    SF_INFO sfinfo ;
    unsigned c, k, readcount, channels, n = 0;
    double val, kdn, korigin, val0, vald, valdi, valp, kn, kp, vd;
    double vsumo = 0.0, vsumd = 0.0, vsum = 0.0;;
    unsigned valpi;
    double mva[CHANNEL_LEN], mvd[CHANNEL_LEN];
    double vdelta[DELTA_LEN];
    int ndelta[DELTA_LEN];

    for (c = 0; c < CHANNEL_LEN; c++)
    {
        mva[c] = 0;
        mvd[c] = 0;
    }
    for (c = 0; c < DELTA_LEN; c++)
    {
        ndelta[c] = 0;
        vdelta[c] = 0.0;
    }
    
    printf ("    %s -> %s ", infilename, outfilename) ;
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
    if (! (outfile = sf_open (outfilename, SFM_WRITE, &sfinfo)))
    {
        printf ("Error : could not open file : %s\n", outfilename);
        puts (sf_strerror (NULL));
        exit (1);
    }
    channels = sfinfo.channels;
    kdn = DELTA_LEN;
    kdn--;
    kdn /= 2;
    c = 0;
    n = 0;
    while ((readcount = sf_read_double (infile, buffer, BUFFER_LEN)) > 0)
    {
        for (k = 0; k < readcount; k++)
        {
            val0 = buffer[k];
            vald = val0;
            vald -= mva[c];
            vd = vald;
            vd *= vd;
            vsumo += vd;
            n++;
            valp = mvd[c];
            valp++;
            if (valp < 0.0) {valp = 0.0;}
            if (valp > 2.0) {valp = 2.0;}
            valp *= kdn;
            valpi = valp;
            mva[c] = val0;
            mvd[c] = vald;
            ndelta[valpi]++;
            kn = 1.0 / ndelta[valpi];
            kp = 1.0 - kn;
            vald *= kn;
            vdelta[valpi] *= kp;
            vdelta[valpi] += vald;
            c++;
            if (c > channels - 1) {c = 0;}
        }
    }
    vsumo *= 2;
    vsumo /= n;
    vsumo = sqrt(vsumo);

    sf_close (infile) ;

    if (kdenoise == 0.0)
    {
        for (c = 0; c < CHANNEL_LEN; c++)
        {
            mva[c] = 0;
            mvd[c] = 0;
        }

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
        c = 0;
        while ((readcount = sf_read_double (infile, buffer, BUFFER_LEN)) > 0)
        {
            for (k = 0; k < readcount; k++)
            {
                val0 = buffer[k];
                vald = val0;
                vald -= mva[c];
                valp = mvd[c];
                valp++;
                if (valp < 0.0) {valp = 0.0;}
                if (valp > 2.0) {valp = 2.0;}
                valp *= kdn;
                valpi = valp;
                mva[c] = val0;
                mvd[c] = vald;
                vald = vdelta[valpi];
                vd = vald;
                vd *= vd;
                vsumd += vd;
                c++;
                if (c > channels - 1) {c = 0;}
            }
        }

        vsumd *= 2;
        vsumd /= n;
        vsumd = sqrt(vsumd);

        sf_close (infile) ;
        
        vd = vsumo + vsumd;
        if (vd > 0)
        {
            kdenoise = vsumo / vd;
        }
    }
    korigin = 1.0 - kdenoise;
    printf (" :%f/%f->", kdenoise, korigin);
    
    for (c = 0; c < CHANNEL_LEN; c++)
    {
        mva[c] = 0;
        mvd[c] = 0;
    }

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
    c = 0;
    while ((readcount = sf_read_double (infile, buffer, BUFFER_LEN)) > 0)
    {
        for (k = 0; k < readcount; k++)
        {
            val0 = buffer[k];
            vald = val0;
            vald -= mva[c];
            valp = mvd[c];
            valp++;
            if (valp < 0.0) {valp = 0.0;}
            if (valp > 2.0) {valp = 2.0;}
            valp *= kdn;
            valpi = valp;
            mva[c] = val0;
            mvd[c] = vald;
            vald *= korigin;
            vald += (kdenoise * vdelta[valpi]);
            val0 -= mvd[c];
            val0 += vald;
            vald = mva[c];
            vald -= val0;
            vald *= vald;
            val = val0;
            if (val < -1.0) {val = -1.0;}
            if (val > 1.0) {val = 1.0;}
            bufferout[k] = val;
            vsum += vald;
            c++;
            if (c > channels - 1) {c = 0;}
        }
        sf_write_double (outfile, bufferout, readcount) ;
    }

    vsum *= 2;
    vsum /= n;
    vsum = sqrt(vsum);

    sf_close (infile) ;
    sf_close (outfile) ;

    printf ("ok [%f]\n", vsum);
    
} /* sndrs_file */

int main (int argc, char **argv)
{
    int opt;
    double kdenoise = 0.0;
    int fhelp = 0;
    while ((opt = getopt(argc, argv, ":k:h")) != -1)
    {
        switch(opt)
        {
            case 'k':
                kdenoise = atof(optarg);
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
    
    if(optind + 2 > argc || fhelp > 0)
    {
        usage(argv[0]);
        return 1;
    }

    snddenoise_file (argv[optind], argv[optind + 1], kdenoise);

    return 0 ;
} /* main */
