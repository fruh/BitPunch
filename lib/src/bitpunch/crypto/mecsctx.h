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
#ifndef MECSCTX_H
#define MECSCTX_H

#include <bitpunch/config.h>
#include <bitpunch/math/gf2.h>
#include <bitpunch/code/codectx.h>

/// Possible types of MECS
typedef enum _BPU_T_EN_Mecs_Types {
    BPU_EN_MECS_BASIC_GOPPA = 1,        ///< basic mecs, without any conversion
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA, ///< adapted Pointcheval's cca2 conversion
#endif
    BPU_EN_MECS_BASIC_QCMDPC,   ///< basic qc-mdpc mecs, without any conversion
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
    BPU_EN_MECS_CCA2_POINTCHEVAL_QCMDPC,        ///< adapted Pointcheval's cca2 conversion for qcmdpc
#endif
} BPU_T_EN_Mecs_Types;

typedef struct _BPU_T_Mecs_Ctx {
    BPU_T_EN_Mecs_Types type;
    int (*_encrypt) (BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                     const struct _BPU_T_Mecs_Ctx * ctx);
    int (*_decrypt) (BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                     const struct _BPU_T_Mecs_Ctx * ctx);
    int (*_genKeyPair) (struct _BPU_T_Code_Ctx * ctx);

    BPU_T_Code_Ctx *code_ctx;
    uint16_t pt_len;            ///< PT len in bits
    uint16_t ct_len;            ///< CT len in bits
} BPU_T_Mecs_Ctx;

typedef union _BPU_T_UN_Code_Params BPU_T_UN_Mecs_Params;

/// Initialize (register) mecs functions for encryption, decryption and key gen based on type.
int BPU_mecsInitCtx(BPU_T_Mecs_Ctx ** ctx, const BPU_T_UN_Mecs_Params * params,
                    const BPU_T_EN_Mecs_Types type);

int BPU_mecsFreeCtx(BPU_T_Mecs_Ctx ** ctx);

// Available init params for all codes, it is only wrapper for code params init
int BPU_mecsInitParamsGoppa(BPU_T_UN_Mecs_Params * params, const uint16_t m,
                            const uint16_t t, const BPU_T_GF2_16x mod);

void BPU_mecsFreeParamsGoppa(BPU_T_UN_Mecs_Params * params);

/**
 * Wrapper for param initialization. After work you have to free memory using call BPU_mecsFreeParamsQcmdpc
 * @param  params pointer to structure
 * @param  m      size of cyclic matrix
 * @param  n0     number of cyclic matrices
 * @param  w      weight of parity-check matrix row
 * @param  t      count of errors
 * @return        [description]
 */
int BPU_mecsInitParamsQcmdpc(BPU_T_UN_Mecs_Params * params, const uint16_t m,
                             const uint16_t n0, const uint16_t w,
                             const uint16_t t);

/**
 * Free memory for params.
 * @param params pointer to structure
 */
void BPU_mecsFreeParamsQcmdpc(BPU_T_UN_Mecs_Params * params);

// add yours additional params if needed

#endif // MECSCTX_H
