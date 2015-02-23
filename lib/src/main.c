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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char **argv) {
	BPU_T_Mecs_Ctx ctx;
	BPU_T_GF2_Vector ct, pt_in, pt_out;

	srand(time(NULL));

	/***************************************/
	// mce initialisation t = 50, m = 11
	fprintf(stderr, "Initialisation...\n");
	BPU_mecsInitCtx(&ctx, 11, 50, BPU_EN_MECS_BASIC_GOPPA);

	/***************************************/
	fprintf(stderr, "Key generation...\n");
	// key pair generation
	if (BPU_mecsGenKeyPair(&ctx)) {
		BPU_printError("Key generation error");
	}
	/***************************************/
	// prepare plain text, allocate memory and init random plaintext
	if (BPU_gf2VecRand(&pt_in, ctx.code_ctx->msg_len, 0)) {
		BPU_printError("PT initialisation error");

		BPU_mecsFreeCtx(&ctx);
		return 1;
	}
	// alocate cipher text vector
	if (BPU_gf2VecMalloc(&ct, ctx.code_ctx->code_len)) {
		BPU_printError("CT vector allocation error");

		BPU_gf2VecFree(&pt_in, 0);
		BPU_mecsFreeCtx(&ctx);
		return 1;
	}
	/***************************************/
	fprintf(stderr, "Encryption...\n");
	// BPU_encrypt plain text
	if (BPU_mecsEncrypt(&ct, &pt_in, &ctx)) {
		BPU_printError("Encryption error");

		BPU_gf2VecFree(&ct, 0);
		BPU_gf2VecFree(&pt_in, 0);
		BPU_mecsFreeCtx(&ctx);
		return 1;
	}
	/***************************************/
	fprintf(stderr, "Decryption...\n");
	// decrypt cipher text
	if (BPU_mecsDecrypt(&pt_out, &ct, &ctx)) {
		BPU_printError("Decryption error");

		BPU_gf2VecFree(&ct, 0);
		BPU_gf2VecFree(&pt_in, 0);
		BPU_mecsFreeCtx(&ctx);
		return 1;
	}
	/***************************************/
	// debug output
	fprintf(stderr, "\nCT:\n");
	BPU_printGf2Vec(&ct);
	fprintf(stderr, "\nOutput PT:\n");
	BPU_printGf2Vec(&pt_out);
	fprintf(stderr, "\nInput random PT:\n");
	BPU_printGf2Vec(&pt_in);

	// check for correct decryption
	if (BPU_gf2VecCmp(&pt_in, &pt_out)) {
		BPU_printError("\nOutput plain text differs from input");
	}
	else {
		fprintf(stderr, "\nSUCCESS: Input plain text is equal to output plain text.\n");
	}
	// clean up
	/***************************************/
	fprintf(stderr, "\nCleaning up...\n");
	BPU_gf2VecFree(&pt_in, 0);
	BPU_gf2VecFree(&pt_out, 0);
	BPU_gf2VecFree(&ct, 0);
	BPU_mecsFreeCtx(&ctx);

	return 0;
}
