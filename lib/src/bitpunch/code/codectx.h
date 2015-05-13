/**
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
#ifndef BPU_CODECTX_H
#define BPU_CODECTX_H

#include <stdint.h>

#include <bitpunch/config.h>
#include <bitpunch/math/gf2.h>
#include <bitpunch/math/gf2x.h>
#include <bitpunch/math/mathctx.h>

// addiotional codes
#include <bitpunch/code/goppa/goppatypes.h>

/// Possible types of codes
typedef enum _BPU_T_EN_Code_Types {
	BPU_EN_CODE_GOPPA
	// HERE you can add your code type
}BPU_T_EN_Code_Types;

/// Code specifics union type
typedef union _BPU_T_UN_Code_Spec{
	BPU_T_Goppa_Spec *goppa;
	// HERE you add your code spec structure
}BPU_T_UN_Code_Spec;

typedef union _BPU_T_UN_Code_Params{
	BPU_T_Goppa_Params *goppa;
	// HERE you add your code params structure
}BPU_T_UN_Code_Params;

typedef struct _BPU_T_Code_Ctx {
	BPU_T_EN_Code_Types type;
	int (* _encode)(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx); ///< This is reference to encode function. It will be set in BPU_codeInitCtx
	int (* _decode)(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx); ///< This is reference to decode function. It will be set in BPU_codeInitCtx
	BPU_T_Math_Ctx *math_ctx; ///< Math context
	BPU_T_GF2_Vector *e; ///< Error vector
	BPU_T_UN_Code_Spec *code_spec; ///< Code specific structure, like generator matrix, control matrix, gen. poly ...

	uint16_t code_len; ///< Code len
	uint16_t msg_len; ///< Code dimenzion
	uint8_t t; ///< Error code correction capability
}BPU_T_Code_Ctx;

/// Initialize (register) code functions encode, decode and code spec structure based on code type.
int BPU_codeInitCtx(BPU_T_Code_Ctx **ctx, const BPU_T_UN_Code_Params *params, const BPU_T_EN_Code_Types type);

int BPU_codeInitMathCtx(BPU_T_Math_Ctx **ctx, const uint16_t m, const uint16_t t, const BPU_T_GF2_16x mod);

void BPU_codeFreeCtx(BPU_T_Code_Ctx **ctx);

// Available init params for all codes, it is only wrapper for code params init
int BPU_codeInitParamsGoppa(BPU_T_UN_Code_Params *params, const uint16_t m, const uint16_t t, const BPU_T_GF2_16x mod);

void BPU_codeFreeParamsGoppa(BPU_T_UN_Code_Params *params);

// add yours additional params if needed
#endif // BPU_CODECTX_H
