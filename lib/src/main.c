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
 
#define _GNU_SOURCE
#include <sched.h>
#include <bitpunch/bitpunch.h>
#include "bitpunch/tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int basicTest();
int elpMeasurementsBB();

int main(int argc, char **argv) {
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(1, &mask);
	sched_setaffinity(0, sizeof(cpu_set_t), &mask);
	return elpMeasurementsBB();
}

int elpMeasurementsBB() {
	int test, number_of_tests;
	BPU_T_Mecs_Ctx ctx;
	BPU_T_GF2_Vector ct, pt_in, pt_out, error;
	int i, iter;
#ifdef ATTACK_BB
	unsigned long long int start, stop, delta;
#endif
	srand(0);
//	srand(time(NULL));
	BPU_mecsInitCtx(&ctx, 11, 50, BPU_EN_MECS_BASIC_GOPPA);
	BPU_mecsGenKeyPair(&ctx);
//	Generate random input vector
	BPU_gf2VecRand(&pt_in, ctx.pt_len, 0);
//	Prepare cipher text
	BPU_gf2VecMalloc(&ct, ctx.ct_len);
//	Prepare output plain text
	BPU_gf2VecMalloc(&pt_out, ctx.pt_len);

//	Encryption
//	Create error vector
	ctx.code_ctx->_encode(&ct, &pt_in, ctx.code_ctx);
//	BPU_gf2VecRand(&error, ctx.ct_len, ctx.code_ctx->t);

//	Decryption
	number_of_tests = 2;
//	removeErrorBit(&ct, &error, 4);
	BPU_gf2VecRand(&error, ctx.ct_len, ctx.code_ctx->t);
	BPU_gf2VecXor(&ct, &error);

	iter = 1;
	for (test = 0; test < number_of_tests; test++){
		for (i = 0; i < iter; i++) {
#ifdef ATTACK_BB
			start = rdtsc();
#endif
			BPU_mecsDecrypt(&pt_out, &ct, &ctx);
#ifdef ATTACK_BB
			stop = rdtsc();
			delta = stop - start;
			fprintf(stdout, "%d\n", delta);
#endif
		}
		removeErrorBit(&ct, &error, 26);
//		addErrorBit(&ct, &error, 2);
	}




	if (!BPU_gf2VecCmp(&pt_in, &pt_out))
		fprintf(stderr, "success\n");
	else
		fprintf(stderr, "failure\n");
//	Clean up
	BPU_gf2VecFree(&pt_in, 0);
	BPU_gf2VecFree(&pt_out, 0);
	BPU_gf2VecFree(&ct, 0);
	BPU_mecsFreeCtx(&ctx);
	return 0;
}

int basicTest() {
	int rc = 0;
	BPU_T_Mecs_Ctx ctx;
	BPU_T_GF2_Vector ct, pt_in, pt_out;

	srand(time(NULL));

	/***************************************/
	// mce initialisation t = 50, m = 11
	fprintf(stderr, "Initialisation...\n");
	if (BPU_mecsInitCtx(&ctx, 11, 50, BPU_EN_MECS_BASIC_GOPPA)) {
//	 if (BPU_mecsInitCtx(&ctx, 5, 5, BPU_EN_MECS_BASIC_GOPPA)) {
		return 1;
	}

	/***************************************/
	fprintf(stderr, "Key generation...\n");
	// key pair generation
	if (BPU_mecsGenKeyPair(&ctx)) {
		BPU_printError("Key generation error");

		return 1;
	}
	/***************************************/
	// prepare plain text, allocate memory and init random plaintext
	if (BPU_gf2VecRand(&pt_in, ctx.pt_len, 0)) {
		BPU_printError("PT initialisation error");

		BPU_mecsFreeCtx(&ctx);
		return 1;
	}
	// alocate cipher text vector
	if (BPU_gf2VecMalloc(&ct, ctx.ct_len)) {
		BPU_printError("CT vector allocation error");

		BPU_gf2VecFree(&pt_in, 0);
		BPU_mecsFreeCtx(&ctx);
		return 1;
	}
	// prepare plain text, allocate memory and init random plaintext
	if (BPU_gf2VecRand(&pt_out, ctx.pt_len, 0)) {
		BPU_printError("PT out initialisation error");

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

	// check for correct decryption
	if (BPU_gf2VecCmp(&pt_in, &pt_out)) {
		BPU_printError("\nOutput plain text differs from input");

		rc = 2;
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

	return rc;
}
