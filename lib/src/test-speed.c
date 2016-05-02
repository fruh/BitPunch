/*
 This file is part of BitPunch
 Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
 Copyright (C) 2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
 Copyright (C) 2014 Marek Klein  <kleinmrk[what here]gmail.com>
 Copyright (C) 2014 Filip Machovec  <filipmachovec[what here]yahoo.com>
 Copyright (C) 2014 Jozef Kudlac <jozef[what here]kudlac.sk>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <bitpunch/bitpunch.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define BPU_TEST_ROUNDS 300

int main(int argc, char **argv) {
    // MUST BE INITIALIZED TO NULL
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_UN_Mecs_Params params;
    BPU_T_GF2_Vector *ct, *pt;
    int i;
    struct timeval tv, tv_end;
    double res = 0;
    double res_2 = 0;
    double res_3 = 0;

    fprintf(stderr, "====== SPEED TEST ======\n");
    srand(time(NULL));

    for (i = 0; i < BPU_TEST_ROUNDS; i++) {
        BPU_mecsInitParamsGoppa(&params, 11, 50, 0);
        BPU_mecsInitCtx(&ctx, &params, BPU_EN_MECS_BASIC_GOPPA);
        gettimeofday(&tv, NULL);

        BPU_mecsGenKeyPair(ctx);
        gettimeofday(&tv_end, NULL);
        res +=
            (tv_end.tv_sec - tv.tv_sec +
             ((tv_end.tv_usec - tv.tv_usec) / (double) 1000000));

        BPU_gf2VecMalloc(&ct, ctx->ct_len);
        BPU_gf2VecMalloc(&pt, ctx->pt_len);
        BPU_gf2VecRand(pt, 0);

        gettimeofday(&tv, NULL);
        BPU_mecsEncrypt(ct, pt, ctx);
        gettimeofday(&tv_end, NULL);
        res_2 +=
            (tv_end.tv_sec - tv.tv_sec +
             ((tv_end.tv_usec - tv.tv_usec) / (double) 1000000));

        gettimeofday(&tv, NULL);
        BPU_mecsDecrypt(pt, ct, ctx);
        gettimeofday(&tv_end, NULL);
        res_3 +=
            (tv_end.tv_sec - tv.tv_sec +
             ((tv_end.tv_usec - tv.tv_usec) / (double) 1000000));

        BPU_gf2VecFree(&pt);
        BPU_gf2VecFree(&ct);
        BPU_mecsFreeCtx(&ctx);
        BPU_mecsFreeParamsGoppa(&params);
    }
    fprintf(stderr, "%0.6lf\n", res / BPU_TEST_ROUNDS);
    fprintf(stderr, "%0.6lf\n", res_2 / BPU_TEST_ROUNDS);
    fprintf(stderr, "%0.6lf\n", res_3 / BPU_TEST_ROUNDS);

    return 0;
}
