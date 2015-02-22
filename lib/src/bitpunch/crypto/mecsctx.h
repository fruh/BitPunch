/**
This file is part of PROGRAM
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
#ifndef MECSCTX_H
#define MECSCTX_H

#include <bitpunch/math/gf2.h>
#include <bitpunch/code/codectx.h>

/// Possible types of MECS
typedef enum _BPU_EN_Mecs_Types {
	BPU_EN_MECS_BASIC_GOPPA ///< basic mecs, without any conversion
}BPU_EN_Mecs_Types;

typedef struct _BPU_T_Mecs_Ctx {
	BPU_EN_Mecs_Types type;
	int (* _encrypt)(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in, const struct _BPU_T_Mecs_Ctx *ctx);
	int (* _decrypt)(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in, const struct _BPU_T_Mecs_Ctx *ctx);
	int (* _genKeyPair)(struct _BPU_T_Code_Ctx *ctx);

	BPU_T_Code_Ctx *code_ctx;
}BPU_T_Mecs_Ctx;

/// Initialize (register) mecs functions for encryption, decryption and key gen based on type.
int BPU_mecsInitCtx(BPU_T_Mecs_Ctx *ctx, const uint16_t m, const uint16_t t, const BPU_EN_Mecs_Types type);

int BPU_mecsFreeCtx(BPU_T_Mecs_Ctx *ctx);

#endif // MECSCTX_H
