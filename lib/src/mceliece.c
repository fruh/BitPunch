#include "mceliece.h"
#include "debugio.h"
#include "init.h"
#include "process.h"

int mceInitCtx(McEliece_Ctx *ctx, uint8_t t_errors) {
	int rc = 0;

	ctx->pub_key.t = t_errors;

	switch(t_errors) {
		case 5:
			rc = initLogExpTable(ctx, (GF2_16x)2, (GF2_16x) GF2_POLY_DEG_5);

			break;
		case 6:
			rc = initLogExpTable(ctx, (GF2_16x)2, (GF2_16x) GF2_POLY_DEG_6);
			
			break;
		case 7:
			rc = initLogExpTable(ctx, (GF2_16x)2, (GF2_16x) GF2_POLY_DEG_6);
			
			break;
		case 50:
			rc = initLogExpTable(ctx, (GF2_16x)2, (GF2_16x) GF2_POLY_DEG_11);
			
			break;
		default:
			printError("Not supported, only t error: 5, 6, 7, 50");

			rc = 1;
			break;
	}
	if (!rc) {
		ctx->max_pt_len_bit = (ctx->a_data.mod_deg <= 5) ? 1 << (ctx->a_data.mod_deg - 3) : 1 << (ctx->a_data.mod_deg - 2);
		// ctx->max_pt_len = (ctx->max_pt_len_bit >> 3) + (ctx->max_pt_len_bit % 8) ? 1 : 0;

		ctx->max_ct_len_bit = ctx->a_data.ord + 2 * ctx->max_pt_len_bit;
		// ctx->max_pt_len = (ctx->max_ct_len_bit >> 3) + (ctx->max_ct_len_bit % 8) ? 1 : 0;
	}
	return rc;
}

int mceEncrypt(Vector_GF2 *ct, const Vector_GF2 *pt, const McEliece_Ctx *ctx) {
	return encrypt(ct, pt, ctx);
	// return encryptCCA2KobaraImai(ct, pt, ctx);
}

int mceEncryptA(Vector_GF2 *ct, const Vector_GF2 *pt, const McEliece_Ctx *ctx) {
	return encryptA(ct, pt, ctx);
}

int mceDecrypt(Vector_GF2 *pt, Vector_GF2 *ct, const McEliece_Ctx *ctx) {
	return decrypt(pt, ct, ctx);
	// return decryptCCA2KobaraImai(pt, ct, ctx);
}

int mceDecryptA(Vector_GF2 *pt, Vector_GF2 *ct, const McEliece_Ctx *ctx) {
	return decryptA(pt, ct, ctx);
}

int mceGenKeyPair(McEliece_Ctx *ctx) {
	return genKeyPair(ctx, ctx->pub_key.t);
}

void mceFreeCtx(McEliece_Ctx *ctx) {
	freeMcElieceCtx(ctx);
}

int mceGetPtBlockSizeBits(McEliece_Ctx *ctx) {
	return ctx->max_pt_len_bit;
}

int mceGetCtBlockSizeBits(McEliece_Ctx *ctx) {
	return ctx->max_ct_len_bit;
}