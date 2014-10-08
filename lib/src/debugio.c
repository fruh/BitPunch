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
#include "debugio.h"

void BITP_printBinary(uint64_t in, int len) {
    if (len > 0) {
        BITP_printBinary(in >> 1, len - 1);

        fprintf(stderr, "%d", (int) (in & (0x1ul)));
    }
}

void BITP_printBinaryLn(uint64_t in, int len) {
	BITP_printBinary(in, len);
	fprintf(stderr, "\n");
}

void BITP_printBinary64(uint64_t in) {
	BITP_printBinary(in, 64);
}

void BITP_BIT_printBinary64Ln(uint64_t in) {
	BITP_printBinaryLn(in, 64);
}

void BITP_printBinaryLe(uint64_t in, int len) {
    if (len > 0) {
        fprintf(stderr, "%d", (int) (in & (0x1ul)));

        BITP_printBinaryLe(in >> 1, len - 1);
    }
}

void BITP_BIT_printBinaryLnLe(uint64_t in, int len) {
    BITP_printBinaryLe(in, len);
    fprintf(stderr, "\n");
}

void BITP_BIT_printBinary64Le(uint64_t in) {
    BITP_printBinaryLe(in, 64);
}

void BITP_BIT_BIT_printBinary64LnLe(uint64_t in) {
    BITP_BIT_printBinaryLnLe(in, 64);
}

void BITP_printGf2xMat(const Matrix_GF2_16x* in) {
	uint32_t i;
	uint32_t j;
    fprintf(stderr, "Matrix size: %dx%d\n", in->k, in->n);

	for(i = 0; i < in->k; i++) {
        fprintf(stderr, "%3d: ",i);
		for(j = 0; j < in->n; j++) {
			fprintf(stderr, "%x ", in->elements[i][j]);//BITP_printBinary(in->elements[i][j], 4);
		}
		fprintf(stderr, "\n");
	}
}

void BITP_printGf2Mat(const Matrix_GF2* m) {
	int i, j, bits_to_print;

    fprintf(stderr, "Matrix size: %dx%d\n", m->k, m->n);

    for (i = 0; i < m->k; i++) {
        fprintf(stderr, "%4d: ",i);

        for (j = 0; j <= m->elements_in_row - 1; j++) {
            if (j == m->elements_in_row-1) {
                if (m->n%(m->element_bit_size) != 0) {
                    bits_to_print = m->n % m->element_bit_size;
                }
                else {
                    bits_to_print = m->element_bit_size;
                }
            }
            else {
                bits_to_print = m->element_bit_size;
            }
            BITP_printBinaryLe(m->elements[i][j], bits_to_print);
            // fprintf(stderr, " "); // medzera medzi elementami
        }
        fprintf(stderr, "\n");
    }
}

void BITP_printGf2Vec(const Vector_GF2* v) {
    int j, bits_to_print;

    fprintf(stderr, "Vec (%4d): ", v->len);
    for (j = 0; j <= v->elements_in_row - 1; j++) {
        if (j == v->elements_in_row-1) {
            if (v->len % (v->element_bit_size) != 0) {
                bits_to_print = v->len % v->element_bit_size;
            }
            else {
                bits_to_print = v->element_bit_size;
            }
        }
        else {
            bits_to_print = v->element_bit_size;
        }
        BITP_printBinaryLe(v->elements[j], bits_to_print);
        fprintf(stderr, " "); // medzera medzi elementami
    }
    fprintf(stderr, "\n");
}

void BITP_printGf2VecBe(const Vector_GF2* v) {
    int j, bits_to_print;

    fprintf(stderr, "Vec (%4d): ", v->len);
    for (j = 0; j <= v->elements_in_row - 1; j++) {
        if (j == v->elements_in_row-1) {
            if (v->len % (v->element_bit_size) != 0) {
                bits_to_print = v->len % v->element_bit_size;
            }
            else {
                bits_to_print = v->element_bit_size;
            }
        }
        else {
            bits_to_print = v->element_bit_size;
        }
        BITP_printBinaryLn(v->elements[j], bits_to_print);
        fprintf(stderr, " "); // medzera medzi elementami
    }
    fprintf(stderr, "\n");
}

void BITP_printPerm(const Permutation_Vector *permutation) {
    int i;

    fprintf(stderr, "Perm (%4d): ", permutation->size);
    for (i = 0; i < permutation->size; i++) {
        fprintf(stderr, "%i ",permutation->elements[i]);
    }
    fprintf(stderr, "\n");
}

void BITP_printGf2xPoly(const Polynomial_GF2_16x *p, const Arithmetic_Data *a_data) {
    int i;
    
    fprintf(stderr, "Poly (deg = %d): ", p->deg);

    if (p->deg == -1) {
        fprintf(stderr, "0\n");

        return;
    }
    for (i = p->deg; i >= 0; i--) {
        if(p->coef[i] == 0)
            continue;
        if (i != p->deg) {
            fprintf(stderr, "+ ");
        }
        fprintf(stderr, "alpha^(%d).x^%d ", a_data->log_table[p->coef[i]], i);
    }
    fprintf(stderr, "\n");
}

void BITP_printGf2xVec(const Vector_GF2_16x *v) {
    int i;
    for(i = 0; i < v->len; i++) {
        fprintf(stderr, "%x ", v->elements[i]);
    }
    fprintf(stderr, "\n");
}

void BITP_printMceCtx(const McEliece_Ctx *mce_ctx) {
    fprintf(stderr, "Galois field:\n");
    fprintf(stderr, "\tOrd: %d\n", mce_ctx->a_data.ord);
    fprintf(stderr, "\tMod: 0x%X\n", mce_ctx->a_data.mod);
    fprintf(stderr, "2^m: 2^%d\n", mce_ctx->a_data.mod_deg);

    fprintf(stderr, "Goppa polynomial:\n");
    fprintf(stderr, "\t");
    BITP_printGf2xPoly(&mce_ctx->priv_key.g, &mce_ctx->a_data);
    fprintf(stderr, "\tdegre: %d\n", mce_ctx->pub_key.t);

    fprintf(stderr, "Public key:\n");
    fprintf(stderr, "\tG matrix size: %dx%d\n", mce_ctx->pub_key.g_mat.k, mce_ctx->pub_key.g_mat.n);

    fprintf(stderr, "Private key:\n");
    fprintf(stderr, "\tH matrix size: %dx%d\n", mce_ctx->priv_key.h_mat.k, mce_ctx->priv_key.h_mat.n);
}

void BITP_printGf2VecOnes(const Vector_GF2 *vec) {
    int i;
    for (i = 0; i < vec->len; ++i)
    {
        if (gf2VecGetBit(vec, i)) {
            fprintf(stderr, "%d ", i);
        }
    }
    fprintf(stderr, "\n");
}