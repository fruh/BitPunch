/*
This file is part of BitPunch
Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#include <bitpunch/math/gf2x.h>

#include <bitpunch/debugio.h>
#include <stdlib.h>

BPU_T_Math_Ctx *BPU_mathCtxNew(const BPU_T_GF2_16x g, const BPU_T_GF2_16x mod)
{
    BPU_T_GF2_16x b = 1;
    int i = 0;
    int mod_deg = -1;
    BPU_T_Math_Ctx *math_ctx = NULL;
    BPU_T_Math_Ctx *math_ctx_local = NULL;
    BPU_T_GF2_16x *log_table = NULL;
    BPU_T_GF2_16x *exp_table = NULL;

    math_ctx_local = (BPU_T_Math_Ctx *) calloc(1, sizeof(BPU_T_Math_Ctx));
    if (NULL == math_ctx_local) {
        BPU_printError("Can not malloc BPU_T_Math_Ctx");
        goto err;
    }

    mod_deg = BPU_gf2xGetDeg(mod);
    if (-1 == mod_deg) {
        BPU_printError("Invalid degree of mod");
        goto err;
    }
    // get group ord, number of elements
    BPU_T_GF2_16x ord = ((1 << mod_deg) - 1);

    // alocate memory for tables
    math_ctx_local->mod = mod;
    math_ctx_local->mod_deg = mod_deg;
    log_table = (BPU_T_GF2_16x *) malloc(sizeof(BPU_T_GF2_16x) * (ord + 1));
    if (NULL == log_table) {
        BPU_printError("malloc failed");
        goto err;
    }

    exp_table = (BPU_T_GF2_16x *) malloc(sizeof(BPU_T_GF2_16x) * (ord + 1));
    if (NULL == exp_table) {
        BPU_printError("malloc failed")
            goto err;
    }
    // set ord
    math_ctx_local->ord = ord;

    do {
        exp_table[i] = b;
        log_table[b] = i;

        b = BPU_gf2xMulMod(b, g, mod);
        i++;
    } while (b != 1);

    exp_table[ord] = 0;
    log_table[0] = ord;

    if (i != ord) {
        BPU_printError("element 0x%x is not generator", g);
        goto err;
    }

    math_ctx_local->log_table = log_table;
    log_table = NULL;
    math_ctx_local->exp_table = exp_table;
    exp_table = NULL;
    math_ctx = math_ctx_local;
    math_ctx_local = NULL;
 err:
    BPU_SAFE_FREE(free, math_ctx_local);
    BPU_SAFE_FREE(free, log_table);
    BPU_SAFE_FREE(free, exp_table);
    return math_ctx;
}

void BPU_mathCtxFree(BPU_T_Math_Ctx * ctx)
{
    if (NULL == ctx) {
        return;
    }

    BPU_SAFE_FREE(free, ctx->exp_table);
    BPU_SAFE_FREE(free, ctx->log_table);
    BPU_SAFE_FREE(free, ctx);
}
