/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/



#ifndef AVCODEC_LPC_H
#define AVCODEC_LPC_H

#include <stdint.h>
#include "dsputil.h"

#define ORDER_METHOD_EST     0
#define ORDER_METHOD_2LEVEL  1
#define ORDER_METHOD_4LEVEL  2
#define ORDER_METHOD_8LEVEL  3
#define ORDER_METHOD_SEARCH  4
#define ORDER_METHOD_LOG     5

#define MIN_LPC_ORDER        1
#define MAX_LPC_ORDER       32



int ff_lpc_calc_coefs(DSPContext *s,
                      const int32_t *samples, int blocksize, int min_order,
                      int max_order, int precision,
                      int32_t coefs[][MAX_LPC_ORDER], int *shift, int use_lpc,
                      int omethod, int max_shift, int zero_shift);

void ff_lpc_compute_autocorr(const int32_t *data, int len, int lag,
                             double *autoc);

#ifdef LPC_USE_DOUBLE
#define LPC_TYPE double
#else
#define LPC_TYPE float
#endif


static inline int compute_lpc_coefs(const LPC_TYPE *autoc, int max_order,
                                    LPC_TYPE *lpc, int lpc_stride, int fail,
                                    int normalize)
{
    int i, j;
    LPC_TYPE err;
    LPC_TYPE *lpc_last = lpc;

    if (normalize)
        err = *autoc++;

    if (fail && (autoc[max_order - 1] == 0 || err <= 0))
        return -1;

    for(i=0; i<max_order; i++) {
        LPC_TYPE r = -autoc[i];

        if (normalize) {
            for(j=0; j<i; j++)
                r -= lpc_last[j] * autoc[i-j-1];

            r /= err;
            err *= 1.0 - (r * r);
        }

        lpc[i] = r;

        for(j=0; j < (i+1)>>1; j++) {
            LPC_TYPE f = lpc_last[    j];
            LPC_TYPE b = lpc_last[i-1-j];
            lpc[    j] = f + r * b;
            lpc[i-1-j] = b + r * f;
        }

        if (fail && err < 0)
            return -1;

        lpc_last = lpc;
        lpc += lpc_stride;
    }

    return 0;
}

#endif 