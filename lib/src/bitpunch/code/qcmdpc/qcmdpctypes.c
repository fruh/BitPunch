/*
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2015 Andrej Gulyas <andrej.guly[what here]gmail.com>

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
#include <bitpunch/code/qcmdpc/qcmdpctypes.h>
#include <bitpunch/math/gf2.h>

void BPU_qcmdpcFreeSpec(BPU_T_Qcmdpc_Spec * spec) {
    BPU_gf2QcMatrixFree(&spec->G, 0);
    BPU_gf2SparseQcMatrixFree(&spec->H, 0);
    free(spec);
}

BPU_T_Qcmdpc_Params *BPU_qcmdpcInitParams(const uint16_t m,
                                          const uint16_t n0, const uint16_t w,
                                          const uint16_t t) {
    BPU_T_Qcmdpc_Params *params = NULL;
    BPU_T_Qcmdpc_Params *params_local = NULL;

    params_local = calloc(sizeof(BPU_T_Qcmdpc_Params), 1);
    if (NULL == params_local) {
        BPU_printError("Can't init Code params");
        goto err;
    }

    params_local->m = m;
    params_local->n0 = n0;
    params_local->w = w;
    params_local->t = t;

    params = params_local;
    params_local = NULL;
  err:
    BPU_SAFE_FREE(free, params_local);
    return params;
}

void BPU_qcmdpcFreeParams(BPU_T_Qcmdpc_Params * params) {
    BPU_SAFE_FREE(free, params);
}
