/*
 This file is part of BitPunch
 Copyright (C) 2014 Frantisek Uhrecky <frantisek.uhrecky[at]gmail.com>

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

int main(int argc, char **argv) {
  McEliece_Ctx ctx;
  Vector_GF2 ct, pt_in, pt_out;

  srand(time(NULL));

  /***************************************/
  // mce initialisation t = 50, m = 11
  fprintf(stderr, "Initialisation...\n");
  mceInitCtx(&ctx, 50);

  /***************************************/
  fprintf(stderr, "Key generation...\n");
  // key pair generation
  if (mceGenKeyPair(&ctx)) {
    printError("Key generation error");
  }
  /***************************************/
  // prepare plain text, allocate memory and init random plaintext
  if (initRandVector(&pt_in, ctx.pub_key.g_mat.k, 0)) {
    printError("PT initialisation error");

    freeMcElieceCtx(&ctx);
    return 1;
  }
  // alocate cipher text vector
  if (mallocVectorGF2(&ct, ctx.priv_key.h_mat.n)) {
    printError("CT vector allocation error");

    freeVecGF2(&pt_in, 0);
    freeMcElieceCtx(&ctx);
    return 1;
  }
  /***************************************/
  fprintf(stderr, "Encryption...\n");
  // encrypt plain text
  if (mceEncrypt(&ct, &pt_in, &ctx)) {
    printError("Encryption error");

    freeVecGF2(&ct, 0);
    freeVecGF2(&pt_in, 0);
    freeMcElieceCtx(&ctx);
    return 1;
  }
  /***************************************/
  fprintf(stderr, "Decryption...\n");
  // decrypt cipher text
  if (mceDecrypt(&pt_out, &ct, &ctx)) {
    printError("Decryption error");

    freeVecGF2(&ct, 0);
    freeVecGF2(&pt_in, 0);
    freeMcElieceCtx(&ctx);
    return 1;
  }
  /***************************************/
  // debug output
  fprintf(stderr, "\nCT:\n");
  printGf2Vec(&ct);
  fprintf(stderr, "\nOutput PT:\n");
  printGf2Vec(&pt_out);
  fprintf(stderr, "\nInput random PT:\n");
  printGf2Vec(&pt_in);

  // check for correct decryption
  if (gf2VecCmp(&pt_in, &pt_out)) {
    printError("\nOutput plain text differs from input");
  }
  else {
    fprintf(stderr, "\nSUCCESS: Input plain text is equal to output plain text.\n");
  }
  // clean up
  /***************************************/
  fprintf(stderr, "\nCleaning up...\n");
  freeVecGF2(&pt_in, 0);
  freeVecGF2(&pt_out, 0);
  freeVecGF2(&ct, 0);
  freeMcElieceCtx(&ctx);

  return 0;
}
