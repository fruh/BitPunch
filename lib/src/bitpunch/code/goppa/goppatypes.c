/*
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#include <bitpunch/code/goppa/goppatypes.h>
#include <bitpunch/debugio.h>

#include <stdlib.h>

#include <bitpunch/math/gf2.h>
#include <bitpunch/math/gf2x.h>
#include <bitpunch/math/perm.h>

void BPU_goppaFreeSpec(BPU_T_Goppa_Spec * spec)
{
    BPU_SAFE_FREE(BPU_gf2xPolyFree, spec->g);
    BPU_gf2MatFree(&spec->g_mat);
#ifndef BPU_CONF_GOPPA_WO_H
    BPU_gf2xMatFree(&spec->h_mat);
#endif
    BPU_permFree(&spec->permutation);
}

BPU_T_Goppa_Params *BPU_goppaParamsNew(const uint16_t m,
                                       const uint16_t t,
                                       const BPU_T_GF2_16x mod)
{
    BPU_T_Goppa_Params *params_local = NULL;
    BPU_T_Goppa_Params *params = NULL;

    params_local = (BPU_T_Goppa_Params *) calloc(sizeof(BPU_T_Goppa_Params), 1);
    if (NULL == params_local) {
        BPU_printError("Unable to allocate memory");
        goto err;
    }

    params_local->m = m;
    params_local->t = t;
    params_local->mod = mod;

    params = params_local;
    params_local = NULL;

 err:
    BPU_SAFE_FREE(free, params_local);
    return params;
}

void BPU_goppaFreeParams(BPU_T_Goppa_Params * params)
{
    BPU_SAFE_FREE(free, params);
}
