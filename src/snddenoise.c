/*
** Copyright (C) 2002-2011 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in
**       the documentation and/or other materials provided with the
**       distribution.
**     * Neither the author nor the names of any contributors may be used
**       to endorse or promote products derived from this software without
**       specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
** OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
** OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
** ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

void usage ()
{
        puts ("\nSndDeNoise: DeNoise wav file.\n") ;
        puts ("    Usage : sndfile-denoise [options] <filename> <outfilename>") ;
        puts ("           options:") ;
        puts ("          -k N.N  coefficient denoise (double, optional, default = 0.0[auto])");
        puts ("          -h      this help\n");
}

static void snddenoise_file (const char *infilename, const char *outfilename, double kdenoise)
{
    static float buffer [BUFFER_LEN] ;
    float bufferout [BUFFER_LEN] ;

    SNDFILE *infile, *outfile ;
    SF_INFO sfinfo ;
    unsigned c, k, readcount, channels, n = 0;
    float val;
    double kdn, korigin, val0, vald, valdi, valp, kn, kp, vd, vsumo = 0.0, vsumd = 0.0, vsum = 0.0;;
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
    while ((readcount = sf_read_float (infile, buffer, BUFFER_LEN)) > 0)
    {
        for (k = 0; k < BUFFER_LEN; k++)
        {
            val0 = (double)buffer[k];
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
        while ((readcount = sf_read_float (infile, buffer, BUFFER_LEN)) > 0)
        {
            for (k = 0; k < BUFFER_LEN; k++)
            {
                val0 = (double)buffer[k];
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
    while ((readcount = sf_read_float (infile, buffer, BUFFER_LEN)) > 0)
    {
        for (k = 0; k < BUFFER_LEN; k++)
        {
            val0 = (double)buffer[k];
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
            val = (float)val0;
            if (val < -1.0) {val = -1.0;}
            if (val > 1.0) {val = 1.0;}
            bufferout[k] = val;
            vsum += vald;
            c++;
            if (c > channels - 1) {c = 0;}
        }
        sf_write_float (outfile, bufferout, readcount) ;
    }

    vsum *= 2;
    vsum /= n;
    vsum = sqrt(vsum);

    sf_close (infile) ;
    sf_close (outfile) ;

    printf ("ok [%f]\n", vsum);
    
    return ;
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
        usage();
        return 1;
    }

    snddenoise_file (argv[optind], argv[optind + 1], kdenoise);

    return 0 ;
} /* main */
