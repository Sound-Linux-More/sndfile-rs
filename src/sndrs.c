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

static void sndrs_file (const char *infilename, const char *outfilename, int fmode, int frev) ;

void usage ()
{
        puts ("\nSndRS: ReSpectr wav file.\n") ;
        puts ("    Usage : sndfile-rs [options] <filename> <outfilename>") ;
        puts ("           options:") ;
        puts ("          -m str  mode (str, optional, default = copy)");
        puts ("             mode:");
        puts ("                  copy - copy wav file,)");
        puts ("                  drs - diff respectr wav file (main function),");
        puts ("                  crs - complite respectr wav file,");
        puts ("                  rs - respectr wav file,");
        puts ("                  pow2 - power 2 wav file,");
        puts ("                  sqrt - sqrt wav file.");
        puts ("                  kalman - kalman filter wav file,");
        puts ("                  diff - differencial wav file,");
        puts ("                  int - integral wav file.");
        puts ("          -r      reverse filter (bool, optional, default = false)");
        puts ("          -h      this help\n");
}

static void sndrs_file (const char *infilename, const char *outfilename, int fmode, int frev)
{
    static float buffer [BUFFER_LEN] ;
    float bufferout [BUFFER_LEN] ;

    SNDFILE *infile, *outfile ;
    SF_INFO sfinfo ;
    int c, k, readcount, channels;
    long int n = 0;
    float val;
    double val0, vals, vala, dval;
    double rval = 0, tval = 0, vsum = 0.0;
    double mbval[CHANNEL_LEN], mcval[CHANNEL_LEN], meval[CHANNEL_LEN], mfval[CHANNEL_LEN];

    for (c = 0; c < CHANNEL_LEN; c++)
    {
        mbval[c] = 0;
        mcval[c] = 0;
        meval[c] = 0;
        mfval[c] = 0;
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
    c = 0;
    while ((readcount = sf_read_float (infile, buffer, BUFFER_LEN)) > 0)
    {
        for (k = 0; k < BUFFER_LEN; k++)
        {
            val0 = (double)buffer[k];
            vals = val0;
            switch(fmode)
            {
                case 0:
                    // copy
                break;
                case 1:
                    // diff rs
                    vals -= meval[c];
                    if (vals < 0) {vala = -vals;} else {vala = vals;}
                    vala *= vala;
                    tval = vala;
                    rval = tval;
                    rval += mbval[c];
                    rval /= 2;
                    mbval[c] = tval;
                    rval -= tval;
                    tval = rval;
                    rval += mcval[c];
                    rval /= 2;
                    mcval[c] = tval;
                    vala += rval;
                    if (vala < 0) {vala = -vala;}
                    vala = sqrt(vala);
                    if (vals < 0) {vals = -vala;} else {vals = vala;}
                    vals += meval[c];
                    meval[c] = val0;
                    
                break;
                case 2:
                    // complite rs
                    if (vals < 0) {vala = -vals;} else {vala = vals;}
                    vala *= vala;
                    tval = vala;
                    rval = tval;
                    rval += mbval[c];
                    rval /= 2;
                    mbval[c] = tval;
                    rval -= tval;
                    tval = rval;
                    rval += mcval[c];
                    rval /= 2;
                    mcval[c] = tval;
                    vala += rval;
                    tval = vala;
                    rval = tval;
                    rval += meval[c];
                    rval /= 2;
                    meval[c] = tval;
                    rval -= tval;
                    tval = rval;
                    rval += mfval[c];
                    rval /= 2;
                    mfval[c] = tval;
                    vala -= rval;
                    if (vala < 0) {vala = -vala;}
                    vala = sqrt(vala);
                    if (vals < 0) {vals = -vala;} else {vals = vala;}
                    if (vals > 1) {vals = 1 - vals;}
                    if (vals < -1) {vals = -1 - vals;}
                break;
                case 3:
                    // rs
                    if (vals < 0) {vala = -vals;} else {vala = vals;}
                    vala *= vala;
                    tval = vala;
                    rval = tval;
                    rval += mbval[c];
                    rval /= 2;
                    mbval[c] = tval;
                    rval -= tval;
                    tval = rval;
                    rval += mcval[c];
                    rval /= 2;
                    mcval[c] = tval;
                    vala += rval;
                    if (vala < 0) {vala = -vala;}
                    vala = sqrt(vala);
                    if (vals < 0) {vals = -vala;} else {vals = vala;}
                    if (vals > 1) {vals = 1 - vals;}
                    if (vals < -1) {vals = -1 - vals;}
                break;
                case 4:
                    // power 2
                    if (vals < 0) {vala = -vals;} else {vala = vals;}
                    vals *= vala;
                break;
                case 5:
                    // sqrt
                    if (vals < 0) {vala = -vals;} else {vala = vals;}
                    vala = sqrt(vala);
                    if (vals < 0) {vals = -vala;} else {vals = vala;}
                break;
                case 6:
                    // kalman
                    vals += mbval[c];
                    vals *= 0.5;
                    mbval[c] = vals;
                break;
                case 7:
                    // diff
                    vals -= mbval[c];
                    mbval[c] = val0;
                break;
                case 8:
                    // integral
                    vals += mbval[c];
                    mbval[c] = vals;
                break;
            }
            dval = val0 - vals;
            if (frev > 0)
            {
                if (fmode > 1)
                {
                    // mirror
                    vals = val0 + dval;
                } else {
                    if (fmode < 7)
                    {
                        // invert
                        vals = -vals;
                    } else {
                        // halt mirror
                        vals = dval;
                        dval = val0 - vals;
                    }
                }
            }
            dval *= dval;
            vsum += dval;
            n++;
            val = (float)vals;
            if (val < -1.0) {val = -1.0;}
            if (val > 1.0) {val = 1.0;}
            bufferout[k] = val;
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

    if (frev > 0)
    {
        printf ("(inv) ");
    }
    printf ("ok [%f]\n", vsum);

    return ;
} /* sndrs_file */

int main (int argc, char **argv)
{
    int opt;
    int fmode = 0;
    int frev = 0;
    int fhelp = 0;
    while ((opt = getopt(argc, argv, ":m:rh")) != -1)
    {
        switch(opt)
        {
            case 'm':
                if (strcmp(optarg, "drs") == 0) {fmode = 1;}
                if (strcmp(optarg, "crs") == 0) {fmode = 2;}
                if (strcmp(optarg, "rs") == 0) {fmode = 3;}
                if (strcmp(optarg, "pow2") == 0) {fmode = 4;}
                if (strcmp(optarg, "sqrt") == 0) {fmode = 5;}
                if (strcmp(optarg, "kalman") == 0) {fmode = 6;}
                if (strcmp(optarg, "diff") == 0) {fmode = 7;}
                if (strcmp(optarg, "int") == 0) {fmode = 8;}
            break;
            case 'r':
                frev = 1;
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

    sndrs_file (argv[optind], argv[optind + 1], fmode, frev) ;

    return 0 ;
} /* main */
