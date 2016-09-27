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
#include "gf2x.h"

#include <bitpunch/debugio.h>
#include <stdlib.h>

int BPU_mathInitCtx(BPU_T_Math_Ctx ** ctx, const BPU_T_GF2_16x g,
                    const BPU_T_GF2_16x mod) {
    BPU_T_GF2_16x b = 1;
    int i = 0;
    BPU_T_Math_Ctx *math_ctx;

    *ctx = (BPU_T_Math_Ctx *) calloc(1, sizeof(BPU_T_Math_Ctx));
    if (!*ctx) {
        BPU_printError("Can not malloc BPU_T_Math_Ctx");

        return -1;
    }
    math_ctx = *ctx;
    // get group ord, number of elements
    BPU_T_GF2_16x ord = ((1 << BPU_gf2xGetDeg(mod)) - 1);

    // alocate memory for tables
    math_ctx->mod = mod;
    math_ctx->mod_deg = BPU_gf2xGetDeg(mod);
    math_ctx->log_table =
        (BPU_T_GF2_16x *) malloc(sizeof(BPU_T_GF2_16x) * (ord + 1));
    math_ctx->exp_table =
        (BPU_T_GF2_16x *) malloc(sizeof(BPU_T_GF2_16x) * (ord + 1));

    // set ord
    math_ctx->ord = ord;

    do {
        math_ctx->exp_table[i] = b;
        math_ctx->log_table[b] = i;

        b = BPU_gf2xMulMod(b, g, mod);
        i++;
    } while (b != 1);

    if (i != ord) {
        BPU_printError("element 0x%x is not generator", g);
		return 1;
	}

	math_ctx->exp_table[ord] = 0;
	math_ctx->log_table[0] = ord;
	return 0;
}

void BPU_mathFreeCtx(BPU_T_Math_Ctx ** ctx) {
    if (!*ctx) {
        return;
    }
    if ((*ctx)->exp_table) {
        free((*ctx)->exp_table);
    }
    if ((*ctx)->log_table) {
        free((*ctx)->log_table);
    }
    free(*ctx);
    *ctx = NULL;
}
