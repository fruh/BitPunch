/**
This file is part of BitPunch
Copyright (C) 2013-2014 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2013-2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
Copyright (C) 2013-2014 Marek Klein  <kleinmrk[what here]gmail.com>
Copyright (C) 2013-2014 Filip Machovec  <filipmachovec[what here]yahoo.com>
Copyright (C) 2013-2014 Jozef Kudlac <jozef[what here]kudlac.sk>

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
#include "mecs.h"
#include "mecsctx.h"

//void BPU_printMceCtx(const BPU_T_McEliece_Ctx *mce_ctx) {
//	fprintf(stderr, "Galois field:\n");
//	fprintf(stderr, "\tOrd: %d\n", mce_ctx->math_ctx->ord);
//	fprintf(stderr, "\tMod: 0x%X\n", mce_ctx->math_ctx->mod);
//	fprintf(stderr, "2^m: 2^%d\n", mce_ctx->math_ctx->mod_deg);

//	fprintf(stderr, "Goppa polynomial:\n");
//	fprintf(stderr, "\t");
//	BPU_printGf2xPoly(&mce_ctx->priv_key->g, mce_ctx->math_ctx);
//	fprintf(stderr, "\tdegre: %d\n", mce_ctx->pub_key->t);

//	fprintf(stderr, "Public key:\n");
//	fprintf(stderr, "\tG matrix size: %dx%d\n", mce_ctx->pub_key->g_mat.k, mce_ctx->pub_key->g_mat.n);

//	fprintf(stderr, "Private key:\n");
//	fprintf(stderr, "\tH matrix size: %dx%d\n", mce_ctx->priv_key->h_mat.k, mce_ctx->priv_key->h_mat.n);
//}

int BPU_mecsEncrypt(BPU_T_Vector_GF2 *ct, const BPU_T_Vector_GF2 *pt, const BPU_T_Mecs_Ctx *ctx) {
	return ctx->_encrypt(ct, pt, ctx);
}

int BPU_mecsDecrypt(BPU_T_Vector_GF2 *pt, BPU_T_Vector_GF2 *ct, const BPU_T_Mecs_Ctx *ctx) {
	return ctx->_decrypt(pt, ct, ctx);
}

int BPU_mecsGenKeyPair(BPU_T_Mecs_Ctx *ctx) {
	return ctx->_genKeyPair(ctx->code_ctx);
}
