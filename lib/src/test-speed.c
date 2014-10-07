/**
 This file is part of BitPunch
 Copyright (C) 2014 Frantisek Uhrecky <frantisek.uhrecky[at]gmail.com>
 Copyright (C) 2014 Andrej Gulyas <andrej.guly[at]gmail.com>
 Copyright (C) 2014 Marek Klein  <kleinmrk[at]gmail.com>
 Copyright (C) 2014 Filip Machovec  <filipmachovec[at]yahoo.com>
 Copyright (C) 2014 Jozef Kudlac Uhrecky <kudalc.jozef[at]gmail.com>

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
#include "mceliece.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "debugio.h"

#define BYTES_TO_TEST (uint64_t)(1 << 16)
#define TEST_ROUNDS 300

void encLoop(Vector_GF2 *ct, Vector_GF2 *pt, McEliece_Ctx *ctx) {
	uint64_t i = 0;
	uint64_t end = (BYTES_TO_TEST * 8) / ctx->max_pt_len_bit;
	
	for (i = 0; i < end; i++) {
		mceEncrypt(ct, pt, ctx);
	}
}

void decLoop(Vector_GF2 *pt, Vector_GF2 *ct, McEliece_Ctx *ctx) {
	uint64_t i = 0;
	uint64_t end = (BYTES_TO_TEST * 8) / ctx->max_pt_len_bit;

	for (i = 0; i < end; i++) {
		mceDecrypt(pt, ct, ctx);
	}
}

double keyGenLoop() {
	int i;
	time_t t, te, tr = 0;
	McEliece_Ctx ctx;

	for (i = 0; i < TEST_ROUNDS; i++) {
		t = time(NULL);

		mceInitCtx(&ctx, 50);
		mceGenKeyPair(&ctx);
		freeMcElieceCtx(&ctx);

		te = time(NULL) - t;
		tr += te;
	}
	return tr / (double)TEST_ROUNDS;
} 

int main(int argc, char **argv) {
	McEliece_Ctx ctx;
	Vector_GF2 ct, pt;
	// double kt;
	// time_t t, te;
	int i;
	struct timeval tv, tv_end;
  double res = 0;
  double res_2 = 0;
  double res_3 = 0;
  fprintf(stderr, "\n\n\n====== SPEED TEST ======\n");
  srand(time(NULL));
  


  for (i = 0; i < TEST_ROUNDS; i++){
		mceInitCtx(&ctx, 50);

    gettimeofday(&tv, NULL);

    mceGenKeyPair(&ctx);
		gettimeofday(&tv_end, NULL);
    res += (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000));
    // fprintf(stderr, "priv: %lu\n", sizeof(ctx.priv_key));
    // fprintf(stderr, "pub: %lu\n", sizeof(ctx.pub_key));
    // fprintf(stderr, "%i\n", ctx.priv_key.g.deg);
    // fprintf(stderr, "%f\n", (double)(ctx.priv_key.g.max_deg * 2 + ctx.priv_key.permutation.size / (double) 4 + ctx.priv_key.h_mat.k * ctx.priv_key.h_mat.n * 2 + ctx.priv_key.g.deg * 2));
    // fprintf(stderr, "%f\n", (double)(ctx.pub_key.g_mat.elements_in_row * ctx.pub_key.g_mat.k * 4 + 2));

    	

    mallocVectorGF2(&ct, ctx.priv_key.h_mat.n);
  	// printError("%d", ctx.priv_key.h_mat.n);
    initRandVector(&pt, ctx.pub_key.g_mat.k, 0);
    // printGf2Vec(&pt);
		  
    gettimeofday(&tv, NULL);
    mceEncrypt(&ct, &pt, &ctx);
    gettimeofday(&tv_end, NULL);
    res_2 += (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000));
    // gf2VecNull(&pt);
    // printGf2Vec(&ct);
    gettimeofday(&tv, NULL);
    mceDecrypt(&pt, &ct, &ctx);
    // printGf2Vec(&pt);
    // decrypt2(&ct, &pt, &ctx);
    gettimeofday(&tv_end, NULL);
    res_3 += (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000));

    freeVecGF2(&pt, 0);
    freeVecGF2(&ct, 0);
		freeMcElieceCtx(&ctx);
  }
  fprintf(stderr, "%0.6lf\n", res / TEST_ROUNDS);
  fprintf(stderr, "%0.6lf\n", res_2 / TEST_ROUNDS);
  fprintf(stderr, "%0.6lf\n", res_3 / TEST_ROUNDS);

	return 0;
}

int main2(int argc, char **argv) {
	McEliece_Ctx ctx;
	Vector_GF2 ct, pt;
	double kt;
	time_t t, te;

	fprintf(stderr, "\n\n\n====== SPEED TEST ======\n");
	fprintf(stderr, "\nGenerating key pair...\n");
	mceInitCtx(&ctx, 50);
	mceGenKeyPair(&ctx); 
	fprintf(stderr, "Size of plaintext: %li B, (%.2lf KB)\n", BYTES_TO_TEST, BYTES_TO_TEST / (double) (1 << 10));
	fprintf(stderr, "McEliece parameters: n = %d, m = %d, t = %d\n", ctx.a_data.ord, ctx.pub_key.m, ctx.pub_key.t);

	fprintf(stderr, "\nKey gen test:\n");
	kt = keyGenLoop();
	fprintf(stderr, "Mean time: %.2lf s/key\n", kt);

	initRandVector(&pt, ctx.max_pt_len_bit - 1, 0);
	mallocVectorGF2(&ct, ctx.max_pt_len_bit);

	fprintf(stderr, "\nEncryption test: encrypting n-times %d bit plaintext message\n", ctx.max_pt_len_bit);
	t = time(NULL);
	
	encLoop(&ct, &pt, &ctx);
	
	te = time(NULL) - t;
	fprintf(stderr, "Time elapsed %li seconds, speed %.2lf KB/s\n", time(NULL) - t, (BYTES_TO_TEST / (double) (1 << 10)) / (double)te);
	
	fprintf(stderr, "\nDecryption test:\n");
	t = time(NULL);
	decLoop(&pt, &ct, &ctx);
	te = time(NULL) - t;
	fprintf(stderr, "Time elapsed %li seconds, speed %.2lf KB/s\n", time(NULL) - t, (BYTES_TO_TEST / (double) (1 << 10)) / (double)te);

	freeVecGF2(&pt, 0);
	freeVecGF2(&ct, 0);
	freeMcElieceCtx(&ctx);

	return 0;
}