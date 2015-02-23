/**
 This file is part of BitPunch
 Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
 Copyright (C) 2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
 Copyright (C) 2014 Marek Klein  <kleinmrk[what here]gmail.com>
 Copyright (C) 2014 Filip Machovec  <filipmachovec[what here]yahoo.com>
 Copyright (C) 2014 Jozef Kudlac <jozef[what here]kudlac.sk>

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

#define BYTES_TO_TEST (uint64_t)(1 << 16)
#define BPU_TEST_ROUNDS 300

void encLoop(Vector_GF2 *ct, Vector_GF2 *pt, BPU_T_McEliece_Ctx *ctx) {
	uint64_t i = 0;
	uint64_t end = (BYTES_TO_TEST * 8) / ctx->max_pt_len_bit;
	
	for (i = 0; i < end; i++) {
		BPU_mecsEncrypt(ct, pt, ctx);
	}
}

void decLoop(Vector_GF2 *pt, Vector_GF2 *ct, BPU_T_McEliece_Ctx *ctx) {
	uint64_t i = 0;
	uint64_t end = (BYTES_TO_TEST * 8) / ctx->max_pt_len_bit;

	for (i = 0; i < end; i++) {
		BPU_mecsDecrypt(pt, ct, ctx);
	}
}

double keyGenLoop() {
	int i;
	time_t t, te, tr = 0;
	BPU_T_McEliece_Ctx ctx;

	for (i = 0; i < BPU_TEST_ROUNDS; i++) {
		t = time(NULL);

		BPU_mecsInitCtx(&ctx, 50);
		BPU_mecsGenKeyPair(&ctx);
		BPU_freeMcElieceCtx(&ctx);

		te = time(NULL) - t;
		tr += te;
	}
	return tr / (double)BPU_TEST_ROUNDS;
} 

int main(int argc, char **argv) {
	BPU_T_McEliece_Ctx ctx;
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
  


  for (i = 0; i < BPU_TEST_ROUNDS; i++){
		BPU_mecsInitCtx(&ctx, 50);

    gettimeofday(&tv, NULL);

    BPU_mecsGenKeyPair(&ctx);
		gettimeofday(&tv_end, NULL);
    res += (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000));
    // fprintf(stderr, "priv: %lu\n", sizeof(ctx.priv_key));
    // fprintf(stderr, "pub: %lu\n", sizeof(ctx.pub_key));
    // fprintf(stderr, "%i\n", ctx.priv_key.g.deg);
    // fprintf(stderr, "%f\n", (double)(ctx.priv_key.g.max_deg * 2 + ctx.priv_key.permutation.size / (double) 4 + ctx.priv_key.h_mat.k * ctx.priv_key.h_mat.n * 2 + ctx.priv_key.g.deg * 2));
    // fprintf(stderr, "%f\n", (double)(ctx.pub_key.g_mat.elements_in_row * ctx.pub_key.g_mat.k * 4 + 2));

    	

	BPU_gf2VecMalloc(&ct, ctx.priv_key.h_mat.n);
  	// BPU_printError("%d", ctx.priv_key.h_mat.n);
	BPU_gf2VecRand(&pt, ctx.pub_key.g_mat.k, 0);
    // BPU_printGf2Vec(&pt);
		  
    gettimeofday(&tv, NULL);
    BPU_mecsEncrypt(&ct, &pt, &ctx);
    gettimeofday(&tv_end, NULL);
    res_2 += (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000));
    // BPU_gf2VecNull(&pt);
    // BPU_printGf2Vec(&ct);
    gettimeofday(&tv, NULL);
    BPU_mecsDecrypt(&pt, &ct, &ctx);
    // BPU_printGf2Vec(&pt);
    // BPU_decrypt2(&ct, &pt, &ctx);
    gettimeofday(&tv_end, NULL);
    res_3 += (tv_end.tv_sec - tv.tv_sec + ((tv_end.tv_usec - tv.tv_usec) / (double)1000000));

	BPU_gf2VecFree(&pt, 0);
	BPU_gf2VecFree(&ct, 0);
		BPU_freeMcElieceCtx(&ctx);
  }
  fprintf(stderr, "%0.6lf\n", res / BPU_TEST_ROUNDS);
  fprintf(stderr, "%0.6lf\n", res_2 / BPU_TEST_ROUNDS);
  fprintf(stderr, "%0.6lf\n", res_3 / BPU_TEST_ROUNDS);

	return 0;
}

int main2(int argc, char **argv) {
	BPU_T_McEliece_Ctx ctx;
	Vector_GF2 ct, pt;
	double kt;
	time_t t, te;

	fprintf(stderr, "\n\n\n====== SPEED TEST ======\n");
	fprintf(stderr, "\nGenerating key pair...\n");
	BPU_mecsInitCtx(&ctx, 50);
	BPU_mecsGenKeyPair(&ctx); 
	fprintf(stderr, "Size of plaintext: %li B, (%.2lf KB)\n", BYTES_TO_TEST, BYTES_TO_TEST / (double) (1 << 10));
	fprintf(stderr, "McEliece parameters: n = %d, m = %d, t = %d\n", ctx.a_data.ord, ctx.pub_key.m, ctx.pub_key.t);

	fprintf(stderr, "\nKey gen test:\n");
	kt = keyGenLoop();
	fprintf(stderr, "Mean time: %.2lf s/key\n", kt);

	BPU_gf2VecRand(&pt, ctx.max_pt_len_bit - 1, 0);
	BPU_gf2VecMalloc(&ct, ctx.max_pt_len_bit);

	fprintf(stderr, "\nEncryption test: BPU_encrypting n-times %d bit plaintext message\n", ctx.max_pt_len_bit);
	t = time(NULL);
	
	encLoop(&ct, &pt, &ctx);
	
	te = time(NULL) - t;
	fprintf(stderr, "Time elapsed %li seconds, speed %.2lf KB/s\n", time(NULL) - t, (BYTES_TO_TEST / (double) (1 << 10)) / (double)te);
	
	fprintf(stderr, "\nDecryption test:\n");
	t = time(NULL);
	decLoop(&pt, &ct, &ctx);
	te = time(NULL) - t;
	fprintf(stderr, "Time elapsed %li seconds, speed %.2lf KB/s\n", time(NULL) - t, (BYTES_TO_TEST / (double) (1 << 10)) / (double)te);

	BPU_gf2VecFree(&pt, 0);
	BPU_gf2VecFree(&ct, 0);
	BPU_freeMcElieceCtx(&ctx);

	return 0;
}
