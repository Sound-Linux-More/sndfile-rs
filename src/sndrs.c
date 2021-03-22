/*
** sndrs.c
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

static void sndrs_file (const char *infilename, const char *outfilename, int fmode, int frev) ;

void usage (const char *progname)
{
    printf ("\nSndRS: ReSpectr wav file.\n") ;
    printf ("    Usage : %s [options] <filename> <outfilename>\n", progname) ;
    printf ("           options:\n") ;
    printf ("          -m str  mode (str, optional, default = copy)\n");
    printf ("             mode:\n");
    printf ("                  copy - copy wav file,)\n");
    printf ("                  drs - diff respectr wav file (main function),\n");
    printf ("                  crs - complite respectr wav file,\n");
    printf ("                  rs - respectr wav file,\n");
    printf ("                  pow2 - power 2 wav file,\n");
    printf ("                  sqrt - sqrt wav file,\n");
    printf ("                  kalman - kalman filter wav file,\n");
    printf ("                  diff - differencial wav file,\n");
    printf ("                  int - integral wav file.\n");
    printf ("          -r      reverse filter (bool, optional, default = false)\n");
    printf ("          -h      this help\n");
}

static void sndrs_file (const char *infilename, const char *outfilename, int fmode, int frev)
{
    static double buffer [BUFFER_LEN] ;
    double bufferout [BUFFER_LEN] ;

    SNDFILE *infile, *outfile ;
    SF_INFO sfinfo ;
    int c, k, readcount, channels;
    long int n = 0;
    double val, val0, vals, vala, dval;
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
    while ((readcount = sf_read_double (infile, buffer, BUFFER_LEN)) > 0)
    {
        for (k = 0; k < readcount; k++)
        {
            val0 = buffer[k];
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
            val = vals;
            if (val < -1.0) {val = -1.0;}
            if (val > 1.0) {val = 1.0;}
            bufferout[k] = val;
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

    if (frev > 0)
    {
        printf ("(inv) ");
    }
    printf ("ok [%f]\n", vsum);

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
        usage(argv[0]);
        return 1;
    }

    sndrs_file (argv[optind], argv[optind + 1], fmode, frev) ;

    return 0 ;
} /* main */
