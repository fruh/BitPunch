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
#ifndef BPU_CODECTX_H
#define BPU_CODECTX_H

#include <stdint.h>

#include <bitpunch/config.h>
#include <bitpunch/math/gf2.h>
#include <bitpunch/math/gf2x.h>

// addiotional codes
#include <bitpunch/code/goppa/goppatypes.h>
#include <bitpunch/code/qcmdpc/qcmdpctypes.h>

/// Available types of codes
typedef enum _BPU_T_EN_Code_Types {
    BPU_EN_CODE_GOPPA,
    BPU_EN_CODE_QCMDPC
        // add your code type HERE
} BPU_T_EN_Code_Types;

/// Code specifics union type
typedef union _BPU_T_UN_Code_Spec {
    BPU_T_Goppa_Spec *goppa;
    BPU_T_Qcmdpc_Spec *qcmdpc;
    // add your code specific structure HERE
} BPU_T_UN_Code_Spec;

/// Code specific parameters
typedef union _BPU_T_UN_Code_Params {
    BPU_T_Goppa_Params *goppa;
    BPU_T_Qcmdpc_Params *qcmdpc;
    // you can add your code specific parameters HERE
} BPU_T_UN_Code_Params;

typedef struct _BPU_T_Code_Ctx {
    BPU_T_EN_Code_Types type;
    int (*_encode) (BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in, const struct _BPU_T_Code_Ctx * ctx);   ///< This is reference to encode function. It will be set in BPU_codeInitCtx
    int (*_decode) (BPU_T_GF2_Vector * out, BPU_T_GF2_Vector * error, const BPU_T_GF2_Vector * in, const struct _BPU_T_Code_Ctx * ctx); ///< This is reference to decode function. It will be set in BPU_codeInitCtx
    BPU_T_Math_Ctx *math_ctx;   ///< Math context
    BPU_T_UN_Code_Spec *code_spec;      ///< Code specific structure, like generator matrix, control matrix, gen. poly ...

    uint16_t code_len;          ///< Code length
    uint16_t msg_len;           ///< Code dimension
    uint8_t t;                  ///< Error code correction capability
} BPU_T_Code_Ctx;

/**
 * @brief BPU_codeInitCtx Initialize (register) code functions encode, decode and code spec structure based on code type.
 * @param ctx
 * @param params
 * @param type
 * @return
 */
BPU_T_Code_Ctx * BPU_codeCtxNew(const BPU_T_UN_Code_Params * params,
                    const BPU_T_EN_Code_Types type);

/**
 * @brief BPU_codeInitMathCtx
 * @param ctx
 * @param m
 * @param t
 * @param mod
 * @return
 */
BPU_T_Math_Ctx * BPU_codeMathCtxNew(const uint16_t m,
                        const uint16_t t, const BPU_T_GF2_16x mod);

/**
 * @brief BPU_codeFreeCtx
 * @param ctx
 */
void BPU_codeCtxFree(BPU_T_Code_Ctx *ctx);

// Available init params for all codes, it is only wrapper for code params init
/**
 * @brief BPU_codeParamsGoppaNew
 * @param params
 * @param m
 * @param t
 * @param mod
 * @return
 */
BPU_T_UN_Code_Params * BPU_codeParamsGoppaNew(const uint16_t m,
                            const uint16_t t, const BPU_T_GF2_16x mod);

/**
 * @brief BPU_codeFreeParamsGoppa
 * @param params
 */
void BPU_codeFreeParamsGoppa(BPU_T_UN_Code_Params * params);

/**
 * Wrapper for param initialization. After work you have to free memory using call BPU_codeFreeParamsQcmdpc
 * @param  params pointer to structure
 * @param  m      size of cyclic matrix
 * @param  n0     number of cyclic matrices
 * @param  w      weight of parity-check matrix row
 * @param  t      count of errors
 * @return        [description]
 */
int BPU_codeInitParamsQcmdpc(BPU_T_UN_Code_Params * params, const uint16_t m,
                             const uint16_t n0, const uint16_t w,
                             const uint16_t t);

/**
 * Free memory for params.
 * @param params pointer to structure
 */
void BPU_codeFreeParamsQcmdpc(BPU_T_UN_Code_Params * params);

#endif // BPU_CODECTX_H
