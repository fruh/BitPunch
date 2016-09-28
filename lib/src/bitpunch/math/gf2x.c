/*
This file is part of BitPunch
Copyright (C) 2013-2016 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
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
#include <stdlib.h>
#include <stdio.h>
#include <bitpunch/debugio.h>
#include <bitpunch/prng/prng.h>

#include "gf2x.h"
#include "int.h"

#ifdef BPU_CONF_PRINT
/* ==================================== Print functions ==================================== */
void BPU_printGf2xMat(const BPU_T_GF2_16x_Matrix * in) {
    uint32_t i;
    uint32_t j;

    fprintf(stderr, "Matrix size: %dx%d\n", in->k, in->n);

    for (i = 0; i < in->k; i++) {
        fprintf(stderr, "%3d: ", i);
        for (j = 0; j < in->n; j++) {
            fprintf(stderr, "%x ", in->elements[i][j]); //BPU_printBinary(in->elements[i][j], 4);
        }
        fprintf(stderr, "\n");
    }
}

void BPU_printGf2xPoly(const BPU_T_GF2_16x_Poly * p,
                       const BPU_T_Math_Ctx * math_ctx) {
    int i;

    fprintf(stderr, "Poly (deg = %d): ", p->deg);

    if (p->deg == -1) {
        fprintf(stderr, "0\n");

        return;
    }
    for (i = p->deg; i >= 0; i--) {
        if (p->coef[i] == 0)
            continue;
        if (i != p->deg) {
            fprintf(stderr, "+ ");
        }
        fprintf(stderr, "alpha^(%d).x^%d ", math_ctx->log_table[p->coef[i]], i);
    }
    fprintf(stderr, "\n");
}

void BPU_printGf2xVec(const BPU_T_GF2_16x_Vector * v) {
    int i;

    for (i = 0; i < v->len; i++) {
        fprintf(stderr, "%x ", v->elements[i]);
    }
    fprintf(stderr, "\n");
}

/* ------------------------------------ Print functions ------------------------------------ */
#endif

int BPU_gf2xMatMalloc(BPU_T_GF2_16x_Matrix ** m, int rows, int cols) {
    int i;

    *m = (BPU_T_GF2_16x_Matrix *) calloc(sizeof(BPU_T_GF2_16x_Matrix), 1);

    if (!*m) {
        BPU_printError("allocation error");
        return -1;
    }
    // rows
    (*m)->k = rows;
    // cols
    (*m)->n = cols;
    // allocate rows
    (*m)->elements =
        (BPU_T_GF2_16x **) malloc(sizeof(BPU_T_GF2_16x *) * (*m)->k);

    if (!(*m)->elements) {
        BPU_printError
            ("BPU_mallocMatrix: can not allocate memory for matrix rows");

        return 1;
    }
    // allocate cols
    for (i = 0; i < (*m)->k; i++) {
        (*m)->elements[i] =
            (BPU_T_GF2_16x *) calloc(1, sizeof(BPU_T_GF2_16x) * (*m)->n);

        if (!(*m)->elements[i]) {
            BPU_printError("allocation error");
            return -2;
        }
    }
    return 0;
}

int BPU_gf2xVecMalloc(BPU_T_GF2_16x_Vector ** vec, int size) {
    *vec = (BPU_T_GF2_16x_Vector *) calloc(sizeof(BPU_T_GF2_16x_Vector), 1);

    if (!*vec) {
        BPU_printError("allocation error");
        return -1;
    }
    (*vec)->len = size;
    (*vec)->elements = (BPU_T_GF2_16x *) calloc(size, sizeof(BPU_T_GF2_16x));

    if (!(*vec)->elements) {
        BPU_printError("allocation error");
        return -2;
    }
    return 0;
}

void BPU_gf2xVecFree(BPU_T_GF2_16x_Vector ** vec) {
    if (!*vec) {
        return;
    }
    free((*vec)->elements);
    free(*vec);
    *vec = NULL;
}

void BPU_gf2xMatFree(BPU_T_GF2_16x_Matrix ** m) {
    int i;

    if (!*m) {
        return;
    }
    // first free cols
    for (i = 0; i < (*m)->k; i++) {
        free((*m)->elements[i]);
    }
    // then free rows
    free((*m)->elements);
    free((*m));
    *m = NULL;
}

void BPU_gf2xPolyFree(BPU_T_GF2_16x_Poly ** p) {
    if (!*p) {
        return;
    }
    free((*p)->coef);
    free(*p);
    *p = NULL;
}

int BPU_gf2xPolyMalloc(BPU_T_GF2_16x_Poly ** p, int16_t max_deg) {
    *p = (BPU_T_GF2_16x_Poly *) calloc(sizeof(BPU_T_GF2_16x_Poly), 1);

    if (!*p) {
        BPU_printError("allocation error");
        return -1;
    }
    return BPU_gf2xPolyMallocCoef(*p, max_deg);
}

int BPU_gf2xPolyResize(BPU_T_GF2_16x_Poly * p, int16_t max_deg) {
    if (p->coef) {
        free(p->coef);
    }
    return BPU_gf2xPolyMallocCoef(p, max_deg);
}

int BPU_gf2xPolyMallocCoef(BPU_T_GF2_16x_Poly * p, int16_t max_deg) {
    // allocate memory
    p->deg = -1;
    p->max_deg = max_deg;

//    if (p->max_deg < 0) {
//        BPU_printError("BPU_mallocPoly: max_deg must be at least 0");

//        return -1;
//    }
    p->coef = (BPU_T_GF2_16x *) calloc(max_deg + 1, sizeof(BPU_T_GF2_16x));

    if (!p->coef) {
        BPU_printError("BPU_mallocPoly: can not allocate polynomial");

        return -1;
    }
    return 0;
}

void BPU_gf2xMatNull(BPU_T_GF2_16x_Matrix * mat) {
    int i, j;

    for (i = 0; i < mat->k; i++) {
        for (j = 0; j < mat->n; j++) {
            mat->elements[i][j] = 0;
        }
    }
}

BPU_T_GF2_16x BPU_gf2xMulMod(BPU_T_GF2_16x a, BPU_T_GF2_16x b,
                             BPU_T_GF2_16x mod) {
    BPU_T_GF2_16x tmp, tmp2;

    if (a == 0) {
        return 0;
    }
    // (a/x * b)
    tmp = BPU_gf2xMulMod(a >> 1, b, mod);

    // tmp * x
    tmp = (tmp << 1);

    // tmp + mod
    tmp2 = tmp ^ mod;

    if (tmp2 < tmp) {
        tmp = tmp2;
    }
    if ((a & 1) == 1) {
        tmp ^= b;
    }
    return tmp;
}

BPU_T_GF2_16x BPU_gf2xMulModC(BPU_T_GF2_16x a, BPU_T_GF2_16x b, BPU_T_GF2_16x mod, BPU_T_GF2_16x mod_deg) {
    BPU_T_GF2_16x ret=0, i;
    for(i = 0; i < mod_deg; i++) {
        b ^= ((b >> mod_deg) & 1) * mod;
        ret ^= ((a >> i) & 1) * b;
        b = b << 1;
    }
    return ret;
}

BPU_T_GF2_16x BPU_gf2xMulModT(const BPU_T_GF2_16x a, const BPU_T_GF2_16x b,
                              const BPU_T_Math_Ctx * math_ctx) {
    BPU_T_GF2_32x condition;
    BPU_T_GF2_16x candidate;

    candidate =
        math_ctx->exp_table[(math_ctx->log_table[a] + math_ctx->log_table[b]) %
                            math_ctx->ord];
    if ((condition = (a * b)))
        return candidate;
    return condition;
}

//not possible to apply on cryptosystem where parameter $n \neq 2^m$
//BPU_T_GF2_16x BPU_gf2xMulModT(BPU_T_GF2_16x a, BPU_T_GF2_16x b, const BPU_T_Math_Ctx *math_ctx) {
//  BPU_T_GF2_16x candidate;
//  BPU_T_GF2_16x exp, bit, carry_mask = 1 << math_ctx->mod_deg;
//  BPU_T_GF2_32x condition;
//  exp = math_ctx->log_table[a] + math_ctx->log_table[b];
//  exp = exp + 1;
//  bit = (exp & carry_mask);
//  exp = (exp & math_ctx->ord);
//  exp = (exp & math_ctx->ord) - !bit;
//  candidate = math_ctx->exp_table[exp];
//  if (condition = (a * b))
//    return candidate;
//  return condition;
//}

BPU_T_GF2_16x BPU_gf2xPowerModT(BPU_T_GF2_16x a, int e,
                                const BPU_T_Math_Ctx * math_ctx) {
    if (e == 0) {
        return 1;
    }
    if (a == 0) {
        return 0;
    }
    // look into log table to find i (b^i) = a
    e = e * math_ctx->log_table[a];
    e = e % math_ctx->ord;

    if (e < 0) {
        e = e + math_ctx->ord;
    }
    // look into exp table
    return math_ctx->exp_table[e];
}

BPU_T_GF2_16x BPU_gf2xMulModTC(BPU_T_GF2_16x a, BPU_T_GF2_16x b, const BPU_T_Math_Ctx *math_ctx) {
	BPU_T_GF2_16x candidate;
	BPU_T_GF2_16x exp, bit, carry_mask = 1 << math_ctx->mod_deg;
	BPU_T_GF2_32x condition;
	exp = math_ctx->log_table[a] + math_ctx->log_table[b];
	exp = exp + 1;
	bit = (exp & carry_mask);
	exp = (exp & math_ctx->ord);
	exp = (exp & math_ctx->ord) - !bit;
	candidate = math_ctx->exp_table[exp];

    if ((condition = (a * b)))
		return candidate;
	return condition;
//	a = !a;
//	b = !b;
//	a = a | b;
////	a = a - 1;
//	a = !a;
//	return candidate * a;
//	candidate = BPU_gf2xMulModC(a, b, math_ctx->mod, math_ctx->mod_deg);
//	return candidate;
}

/*** PZ: speedup critical instructions ***/
int BPU_gf2xMatMul(BPU_T_GF2_16x_Matrix * x, const BPU_T_GF2_16x_Matrix * a,
                   const BPU_T_GF2_16x_Matrix * b,
                   const BPU_T_Math_Ctx * math_ctx) {
    uint32_t i, j, k;
    int loga;

    if (a->n != b->k || x->k != a->k || x->n != b->n) {
        BPU_printError("Wrong mat dimension.");

        return -1;
    }
    BPU_gf2xMatNull(x);

    for (i = 0; i < a->k; i++) {
        for (j = 0; j < b->n; j++) {
            x->elements[i][j] = 0;
        }
    }

    for (i = 0; i < a->k; i++) {
        for (k = 0; k < a->n; k++) {
            if (a->elements[i][k] == 0)
                continue;
            loga = math_ctx->log_table[a->elements[i][k]];
            for (j = 0; j < b->n; j++) {
                if (b->elements[k][j] == 0)
                    continue;
                x->elements[i][j] ^=
                    math_ctx->exp_table[(loga +
                                         math_ctx->log_table[b->
                                                             elements[k][j]]) %
                                        math_ctx->ord];
            }
        }
    }
    return 0;
}

void BPU_gf2xPolyAdd(BPU_T_GF2_16x_Poly * out, const BPU_T_GF2_16x_Poly * a,
                     const BPU_T_GF2_16x_Poly * b) {
    int16_t out_deg;
    int i = 0;

    out_deg = a->deg > b->deg ? a->deg : b->deg;

    if (out->max_deg < out_deg) {
        BPU_gf2xPolyResize(out, out_deg);
    }
    else {
        BPU_gf2xPolyNull(out);
    }
    for (i = 0; i <= out_deg; i++) {
        if (i <= a->deg) {
            out->coef[i] ^= a->coef[i];
        }
        if (i <= b->deg) {
            out->coef[i] ^= b->coef[i];
        }
    }
    out->deg = BPU_gf2xPolyGetDeg(out);
}

void BPU_gf2xPolyAddC(BPU_T_GF2_16x_Poly *out, const BPU_T_GF2_16x_Poly *a, const BPU_T_GF2_16x_Poly *b) {
    int16_t out_deg;
    int i = 0;

    out_deg = a->max_deg > b->max_deg ? a->max_deg : b->max_deg;

    if (out->max_deg < out_deg) {
        BPU_gf2xPolyFree(&out);
        BPU_gf2xPolyMalloc(&out, out_deg);
    }
    else {
        BPU_gf2xPolyNull(out);
    }
    for (i = 0; i <= out_deg; i++) {
        if (i <= a->max_deg) {
            out->coef[i] ^= a->coef[i];
        }
        if (i <= b->max_deg) {
            out->coef[i] ^= b->coef[i];
        }
    }
    out->deg = BPU_gf2xPolyGetDegC(out);
}

void BPU_gf2xPolyDiv(BPU_T_GF2_16x_Poly * q, BPU_T_GF2_16x_Poly * r,
                     const BPU_T_GF2_16x_Poly * a,
                     const BPU_T_GF2_16x_Poly * b,
                     const BPU_T_Math_Ctx * math_ctx) {
    // a:b = q+r
    BPU_T_GF2_16x_Poly *tmp;
    BPU_T_GF2_16x leader;
    BPU_T_GF2_16x_Poly *dividend;
    const BPU_T_GF2_16x_Poly *divider = b;
    int exponent;
    int i;
    int max_deg_q;

    BPU_gf2xPolyMalloc(&dividend, a->max_deg);
    BPU_gf2xPolyCopy(dividend, a);

    max_deg_q = a->deg - b->deg;

    // check size of outputs
    if (q->max_deg < max_deg_q) {
        BPU_gf2xPolyResize(q, max_deg_q);
    }
    else {
        BPU_gf2xPolyNull(q);
    }
    if (r->max_deg < (b->max_deg - 1)) {
        BPU_gf2xPolyResize(r, b->max_deg - 1);
    }
    else {
        BPU_gf2xPolyNull(r);
    }
    BPU_gf2xPolyMalloc(&tmp, a->max_deg);

    for (i = a->deg; i >= 0; i--) {
        if (dividend->deg < divider->deg) {
            BPU_gf2xPolyCopy(r, dividend);
            break;
        }
        BPU_gf2xPolyNull(tmp);
        leader =
            BPU_gf2xMulModT(dividend->coef[i],
                            BPU_gf2xPowerModT(divider->coef[divider->deg], -1,
                                              math_ctx), math_ctx);
        exponent = dividend->deg - divider->deg;
        q->coef[exponent] = leader;

        if (q->deg == -1) {
            q->deg = BPU_gf2xPolyGetDeg(q);
        }
        BPU_gf2xPolyMul(tmp, divider, q, math_ctx);

        BPU_gf2xPolyAdd(dividend, a, tmp);
    }
    BPU_gf2xPolyFree(&dividend);
    BPU_gf2xPolyFree(&tmp);
}

void BPU_gf2xPolyMul(BPU_T_GF2_16x_Poly * out, const BPU_T_GF2_16x_Poly * a,
                     const BPU_T_GF2_16x_Poly * b,
                     const BPU_T_Math_Ctx * math_ctx) {
    int i;
    int j;
    int max_deg = a->deg + b->deg;

    if (out->max_deg < max_deg) {
        BPU_gf2xPolyResize(out, max_deg);
    }
    else {
        BPU_gf2xPolyNull(out);
    }

    for (i = a->deg; i >= 0; i--) {
        for (j = b->deg; j >= 0; j--) {
            out->coef[i + j] ^=
                BPU_gf2xMulModT(a->coef[i], b->coef[j], math_ctx);
        }
    }
    out->deg = BPU_gf2xPolyGetDeg(out);
}

void BPU_gf2xPolyMulC(BPU_T_GF2_16x_Poly *out, const BPU_T_GF2_16x_Poly *a,
                      const BPU_T_GF2_16x_Poly *b,
                      const BPU_T_Math_Ctx *math_ctx) {
    int i;
    int j;
    int max_deg = a->max_deg + b->max_deg;

    if (out->max_deg < max_deg) {
        BPU_gf2xPolyFree(&out);
        BPU_gf2xPolyMalloc(&out, max_deg);
    }
    else {
        BPU_gf2xPolyNull(out);
    }

    for (i = a->max_deg; i >= 0; i--) {
        for (j = b->max_deg; j >= 0; j--) {
            out->coef[i+j] ^= BPU_gf2xMulModC(a->coef[i], b->coef[j], math_ctx->mod, math_ctx->mod_deg);
        }
    }
    out->deg = BPU_gf2xPolyGetDeg(out);
}

void BPU_gf2xPolyShr(BPU_T_GF2_16x_Poly * a, int n) {
    BPU_T_GF2_16x_Poly *tmp;

    // if there is nothing to shift, return
    if (a->deg == -1 || n <= 0) {
        return;
    }
    BPU_gf2xPolyMalloc(&tmp, a->deg);
    BPU_gf2xPolyCopy(tmp, a);
    BPU_gf2xPolyNull(a);

    if (n < tmp->deg + 1) {
        memcpy((void *) (a->coef), (void *) (tmp->coef + n),
               (tmp->deg + 1 - n) * sizeof(BPU_T_GF2_16x));

        a->deg = BPU_gf2xPolyGetDeg(a);
    }
    BPU_gf2xPolyFree(&tmp);
}

void BPU_gf2xPolyShl(BPU_T_GF2_16x_Poly * a, int n) {
    BPU_T_GF2_16x_Poly *tmp;

    // if there is nothing to shift, return
    if (a->deg == -1 || n <= 0) {
        return;
    }
    BPU_gf2xPolyMalloc(&tmp, a->deg);
    BPU_gf2xPolyCopy(tmp, a);

    if (a->max_deg < a->deg + n) {
        BPU_gf2xPolyResize(a, a->deg + n);
    }
    else {
        BPU_gf2xPolyNull(a);
    }
    memcpy((void *) (a->coef + n), (void *) tmp->coef,
           (tmp->deg + 1) * sizeof(BPU_T_GF2_16x));
    a->deg = BPU_gf2xPolyGetDeg(a);

    BPU_gf2xPolyFree(&tmp);
}

void BPU_gf2xPolyShlC(BPU_T_GF2_16x_Poly *a, int n) {
    BPU_T_GF2_16x_Poly *tmp;

    BPU_gf2xPolyMalloc(&tmp, a->deg);
    BPU_gf2xPolyCopy(tmp, a);

    if (a->max_deg < a->deg + n) {
        BPU_gf2xPolyFree(&a);

        BPU_gf2xPolyMalloc(&a, a->deg + n);
    }
    else {
        BPU_gf2xPolyNull(a);
    }
    memcpy((void *)(a->coef + n), (void *)tmp->coef, (tmp->deg + 1) * sizeof(BPU_T_GF2_16x));
    a->deg += (!!a->deg) * n;
    BPU_gf2xPolyFree(&tmp);
}

void BPU_gf2xPolyPower(BPU_T_GF2_16x_Poly * a, int e,
                       const BPU_T_Math_Ctx * math_ctx) {
    int i;
    BPU_T_GF2_16x_Poly *tmp, *tmp_2;

    if (e < 0) {
        BPU_printError("gf2xPolyPower: e < 0, NOT IMPLEMENTED YET");

        return;
    }
    if (e == 0) {
        BPU_gf2xPolyNull(a);
        a->coef[0] = 1;
        a->deg = 0;
    }
    else if (e == 1 || a->deg < 0) {
        return;
    }
    else {
        BPU_gf2xPolyMalloc(&tmp, a->deg * e);
        BPU_gf2xPolyMalloc(&tmp_2, a->deg * e);

        BPU_gf2xPolyCopy(tmp, a);

        for (i = 1; i < e; i++) {
            BPU_gf2xPolyCopy(tmp_2, tmp);

            BPU_gf2xPolyMul(tmp, tmp_2, a, math_ctx);
        }
        BPU_gf2xPolyCopy(a, tmp);

        BPU_gf2xPolyFree(&tmp);
        BPU_gf2xPolyFree(&tmp_2);
    }
}

void BPU_gf2xPolyMulEl(BPU_T_GF2_16x_Poly * a, BPU_T_GF2_16x el,
                       const BPU_T_Math_Ctx * math_ctx) {
    int i;

    for (i = a->deg; i >= 0; i--) {
        a->coef[i] = BPU_gf2xMulModT(a->coef[i], el, math_ctx);
    }
    a->deg = BPU_gf2xPolyGetDeg(a);
}

void BPU_gf2xPolyMulElC(BPU_T_GF2_16x_Poly *a, BPU_T_GF2_16x el, const BPU_T_Math_Ctx *math_ctx) {
    int i;

    for (i = a->max_deg; i >= 0; i--) {
//		a->coef[i] = BPU_gf2xMulModT(a->coef[i], el, math_ctx);
        a->coef[i] = BPU_gf2xMulModTC(a->coef[i], el, math_ctx);
//		a->coef[i] = BPU_gf2xMulModC(a->coef[i], el, math_ctx->mod, math_ctx->mod_deg);
    }
    a->deg = a->deg * (!!el);
}

void BPU_gf2xPolyMod(BPU_T_GF2_16x_Poly * out, const BPU_T_GF2_16x_Poly * a,
                     const BPU_T_GF2_16x_Poly * mod,
                     const BPU_T_Math_Ctx * math_ctx) {
    int i;
    BPU_T_GF2_16x_Poly *tmp_out, *tmp_mod;
    BPU_T_GF2_16x lead;

    if (mod->deg < 0) {
        return;
    }
    if (out->max_deg < a->deg) {
        BPU_gf2xPolyResize(out, a->deg);
    }
    else {
        BPU_gf2xPolyNull(out);
    }
    // if there is nothing to do
    if (a->deg < mod->deg) {
        BPU_gf2xPolyCopy(out, a);

        return;
    }
    // prepare tmp variables
    BPU_gf2xPolyMalloc(&tmp_mod, a->deg);
    BPU_gf2xPolyMalloc(&tmp_out, a->deg);
    BPU_gf2xPolyCopy(tmp_out, a);

    for (i = a->deg; i >= mod->deg; i--) {
        BPU_gf2xPolyCopy(tmp_mod, mod);

        lead =
            BPU_gf2xGetPseudoInv(BPU_gf2xPolyLeadCoef(mod), tmp_out->coef[i],
                                 math_ctx);

        BPU_gf2xPolyMulEl(tmp_mod, lead, math_ctx);
        BPU_gf2xPolyShl(tmp_mod, tmp_out->deg - mod->deg);
        BPU_gf2xPolyAdd(out, tmp_out, tmp_mod);

        if (i > mod->deg) {
            BPU_gf2xPolyCopy(tmp_out, out);
        }
    }
    BPU_gf2xPolyFree(&tmp_mod);
    BPU_gf2xPolyFree(&tmp_out);
}

void BPU_gf2xMatRoot(BPU_T_GF2_16x_Matrix * out,
                     const BPU_T_GF2_16x_Poly * mod,
                     const BPU_T_Math_Ctx * math_ctx) {
    int i, j;
    BPU_T_GF2_16x_Poly *row, *tmp;
    BPU_T_GF2_16x_Matrix *bigMat;       //, test;//, test_out; // matrix (S | I)

    // create square matrix
    BPU_gf2xMatNull(out);
    BPU_gf2xMatMalloc(&bigMat, mod->deg, mod->deg * 2);
    BPU_gf2xPolyMalloc(&tmp, 0);
    BPU_gf2xPolyMalloc(&row, (2 * out->k));

    for (i = 0; i < out->k; i++) {
        BPU_gf2xPolyNull(row);
        row->coef[2 * i] = 1;
        row->deg = 2 * i;
        // compute line
        BPU_gf2xPolyMod(tmp, row, mod, math_ctx);
        // copy elements from polynomial into matrix 
        BPU_gf2xMatInsertPoly(out, tmp, i);
    }
    BPU_gf2xPolyFree(&tmp);
    BPU_gf2xPolyFree(&row);

    for (i = 0; i < out->k; i++) {
        for (j = 0; j < out->k; j++) {
            bigMat->elements[i][j] = out->elements[i][j];
        }
        bigMat->elements[i][out->n + i] = 1;
    }
    BPU_gf2xMatGEM(bigMat, math_ctx);

    for (i = 0; i < out->k; i++) {
        for (j = 0; j < out->k; j++) {
            out->elements[i][j] =
                BPU_gf2xRoot(bigMat->elements[i][out->k + j], math_ctx);
        }
    }
    BPU_gf2xMatFree(&bigMat);
}

void BPU_gf2xVecMulMat(BPU_T_GF2_16x_Vector * out,
                       const BPU_T_GF2_16x_Vector * x,
                       const BPU_T_GF2_16x_Matrix * mat,
                       const BPU_T_Math_Ctx * math_ctx) {
    int i, j;
    BPU_T_GF2_16x element;

    for (i = 0; i < x->len; i++) {
        element = 0;
        for (j = 0; j < mat->n; j++) {
            element =
                element ^ BPU_gf2xMulModT(x->elements[j], mat->elements[j][i],
                                          math_ctx);
        }
        out->elements[i] = element;
    }
}

BPU_T_GF2_16x BPU_gf2xRoot(BPU_T_GF2_16x element,
                           const BPU_T_Math_Ctx * math_ctx) {
    BPU_T_GF2_16x sqr;
    BPU_T_GF2_16x sqr_alpha;
    int exponent;

    if (element == 0) {
        return 0;
    }
    exponent = math_ctx->log_table[element];
    sqr = math_ctx->exp_table[exponent >> 1];
    if ((exponent & 1) == 1) {
        sqr_alpha = math_ctx->exp_table[(math_ctx->ord + 1) / 2];
        sqr = BPU_gf2xMulModT(sqr, sqr_alpha, math_ctx);
    }
    return sqr;
}

void BPU_gf2xPolyRoot(BPU_T_GF2_16x_Poly * out,
                      const BPU_T_GF2_16x_Poly * poly,
                      const BPU_T_GF2_16x_Poly * mod,
                      const BPU_T_Math_Ctx * math_ctx) {
    BPU_T_GF2_16x_Vector *tmp, *tmp_out;
    BPU_T_GF2_16x_Matrix *squareMat;
    int i;

    BPU_gf2xVecMalloc(&tmp_out, mod->deg);

    if (out->deg < mod->deg) {
        BPU_gf2xPolyResize(out, mod->deg);
    }
    BPU_gf2xVecMalloc(&tmp, mod->deg);
    BPU_gf2xPolyToVec(tmp, poly, mod->deg);
    BPU_gf2xMatMalloc(&squareMat, mod->deg, mod->deg);
    BPU_gf2xMatRoot(squareMat, mod, math_ctx);

    for (i = 0; i < tmp->len; i++) {
        tmp->elements[i] = BPU_gf2xRoot(tmp->elements[i], math_ctx);
    }
    BPU_gf2xVecMulMat(tmp_out, tmp, squareMat, math_ctx);
    BPU_gf2xPolyNull(out);
    BPU_gf2xVecToPoly(out, tmp_out);

    BPU_gf2xMatFree(&squareMat);
    BPU_gf2xVecFree(&tmp);
    BPU_gf2xVecFree(&tmp_out);
}

int BPU_gf2xGetDeg(BPU_T_GF2_32x poly) {
    int i = 31;

    while (i >= 0) {
        if (BPU_getBit(poly, i)) {
            return i;
        }
        i--;
    }
    return -1;
}

int BPU_gf2xPolyGetDeg(BPU_T_GF2_16x_Poly * poly) {
    int i = poly->max_deg;

    while (i >= 0) {
        if (poly->coef[i] != 0) {
            return i;
        }
        i--;
    }
    return -1;
}

int BPU_gf2xPolyGetDegC(BPU_T_GF2_16x_Poly *poly) {
    int i;
    int deg = 0;
    for (i = poly->max_deg; i >= 0; i--) {
        deg = deg ^ ((i + 1) * !deg * !!poly->coef[i]);
    }
    deg = deg - 1;
    return deg;
}

int BPU_gf2xMatPermute(BPU_T_GF2_16x_Matrix * out,
                       const BPU_T_GF2_16x_Matrix * m,
                       const BPU_T_Perm_Vector * permutation) {
    int i, j;

    // check if the size is correct
    if (m->n != permutation->size) {
        BPU_printError
            ("BPU_gf2MatPermute: permutation size not correct m->n = %d, p->size = %d",
             m->n, permutation->size);

        return -1;
    }
    // permute
    for (j = 0; j < m->n; j++) {        // column loop
        for (i = 0; i < m->k; i++) {    // row loop
            out->elements[i][j] = m->elements[i][permutation->elements[j]];     // permute the columns
        }
    }
    return 0;
}

int BPU_gf2xMatConvertToGf2Mat(BPU_T_GF2_Matrix * out,
                               const BPU_T_GF2_16x_Matrix * m,
                               int element_bit_size) {
    int i, j, bit, bit_in_element = -1, act_element = 0;

    if (out->k != m->k * element_bit_size || out->n != m->n) {
        BPU_printError("Wrong matrix dimension.");

        return -1;
    }
    // converting
    for (j = 0; j < m->n; j++) {        // column loop
        // check if there is shift through elements
        if ((j - act_element * out->element_bit_size) >= out->element_bit_size) {       // next elemenet, first bit
            act_element++;
            bit_in_element = 0;
        }
        else                    // same element, next bit
            bit_in_element++;
        for (i = 0; i < m->k; i++) {    // row loop
            for (bit = 0; bit < element_bit_size; bit++) {      // bit loop through element of matrix
                out->elements[i * element_bit_size + bit][act_element] ^= BPU_getBit(m->elements[i][j], bit) << (bit_in_element);       // get bit from element and shift it
            }
        }
    }
    return 0;
}

int BPU_gf2xPolyExtEuclid(BPU_T_GF2_16x_Poly * d, BPU_T_GF2_16x_Poly * s,
                          BPU_T_GF2_16x_Poly * t, const BPU_T_GF2_16x_Poly * a,
                          const BPU_T_GF2_16x_Poly * b, const int end_deg,
                          const BPU_T_Math_Ctx * math_ctx) {
    BPU_T_GF2_16x_Poly *tmp, *tmp_2, *old_s, *old_t, *old_r, *r, *q;
    BPU_T_GF2_16x inv_lead;
    int deg;

    deg = (a->deg > b->deg) ? a->deg : b->deg;

    // check GCD qoutient size
    if (d->max_deg < deg) {
        BPU_gf2xPolyResize(d, deg);
    }
    if (s->max_deg < deg) {
        BPU_gf2xPolyResize(s, deg);
    }
    if (t->max_deg < deg) {
        BPU_gf2xPolyResize(t, deg);
    }
    BPU_gf2xPolyMalloc(&tmp, deg);
    BPU_gf2xPolyMalloc(&tmp_2, deg);
    BPU_gf2xPolyMalloc(&old_s, deg);
    BPU_gf2xPolyMalloc(&old_t, deg);
    BPU_gf2xPolyMalloc(&old_r, deg);
    BPU_gf2xPolyMalloc(&r, deg);
    BPU_gf2xPolyMalloc(&q, deg);

    BPU_gf2xPolyCopy(r, b);
    BPU_gf2xPolyCopy(old_r, a);

    if (a->deg == -1) {
        BPU_gf2xPolyCopy(old_r, b);
        old_t->coef[0] = 1;
        old_t->deg = 0;
    }
    else if (b->deg == -1) {
        BPU_gf2xPolyCopy(old_r, a);
        old_s->coef[0] = 1;
        old_s->deg = 0;
    }
    else {
        old_s->coef[0] = 1;
        old_s->deg = 0;

        t->coef[0] = 1;
        t->deg = 0;

        while (old_r->deg > end_deg && r->deg > -1) {
            BPU_gf2xPolyDiv(q, tmp, old_r, r, math_ctx);

            // save old reminder
            BPU_gf2xPolyCopy(old_r, r);
            // save current reminder
            BPU_gf2xPolyCopy(r, tmp);

            // save s quocient
            BPU_gf2xPolyCopy(tmp, old_s);
            BPU_gf2xPolyCopy(old_s, s);
            BPU_gf2xPolyMul(tmp_2, q, s, math_ctx);
            BPU_gf2xPolyAdd(s, tmp, tmp_2);

            // save t quocient
            BPU_gf2xPolyCopy(tmp, old_t);
            BPU_gf2xPolyCopy(old_t, t);
            BPU_gf2xPolyMul(tmp_2, q, t, math_ctx);
            BPU_gf2xPolyAdd(t, tmp, tmp_2);
        }
    }
    // prepare return values
    BPU_gf2xPolyCopy(d, old_r);
    BPU_gf2xPolyCopy(s, old_s);
    BPU_gf2xPolyCopy(t, old_t);

    // make monic, if it is not
    inv_lead = BPU_gf2xPolyMakeMonic(d, math_ctx);

    if (inv_lead != 0) {
        BPU_gf2xPolyMulEl(s, inv_lead, math_ctx);
        BPU_gf2xPolyMulEl(t, inv_lead, math_ctx);
    }
    BPU_gf2xPolyFree(&tmp);
    BPU_gf2xPolyFree(&tmp_2);
    BPU_gf2xPolyFree(&old_s);
    BPU_gf2xPolyFree(&old_t);
    BPU_gf2xPolyFree(&old_r);
    BPU_gf2xPolyFree(&r);
    BPU_gf2xPolyFree(&q);

    return 0;
}

int BPU_gf2xPolyExtEuclidC(BPU_T_GF2_16x_Poly *d, BPU_T_GF2_16x_Poly *s, BPU_T_GF2_16x_Poly *t, const BPU_T_GF2_16x_Poly *a, const BPU_T_GF2_16x_Poly *b, int end_deg, const BPU_T_Math_Ctx *math_ctx) {
    BPU_T_GF2_16x_Poly *tmp, *tmp_2, *old_s, *old_t, *old_r, *r, *q, *helper1, *helper2;
    BPU_T_GF2_16x inv_lead, leader;
    int deg, leader_exp;
    int counter = 0;
    int act_deg, parity = 0;

    deg = (a->max_deg > b->max_deg) ? a->max_deg : b->max_deg;

    // check GCD qoutient size
    // check GCD qoutient size
    if (d->max_deg < deg) {
        BPU_gf2xPolyResize(d, deg);
    }
    if (s->max_deg < deg) {
        BPU_gf2xPolyResize(s, deg);
    }
    if (t->max_deg < deg) {
        BPU_gf2xPolyResize(t, deg);
    }

    BPU_gf2xPolyMalloc(&tmp, deg);
    BPU_gf2xPolyMalloc(&tmp_2, deg);
    BPU_gf2xPolyMalloc(&old_s, deg);
    BPU_gf2xPolyMalloc(&old_t, deg);
    BPU_gf2xPolyMalloc(&old_r, deg);
    BPU_gf2xPolyMalloc(&r, deg);
    BPU_gf2xPolyMalloc(&q, deg);
    BPU_gf2xPolyMalloc(&helper1, deg);
    BPU_gf2xPolyMalloc(&helper2, deg);

    BPU_gf2xPolyCopy(r, b);
    BPU_gf2xPolyCopy(old_r, a);

    if (a->deg == -1) {
        BPU_gf2xPolyCopy(old_r, b);
        old_t->coef[0] = 1;
        old_t->deg = 0;
    }
    else if (b->deg == -1) {
        BPU_gf2xPolyCopy(old_r, a);
        old_s->coef[0] = 1;
        old_s->deg = 0;
    }
    else {
        old_s->coef[0] = 1;
        old_s->deg = 0;

        t->coef[0] = 1;
        t->deg = 0;

        act_deg = old_r->max_deg;
        parity = (!(end_deg & 1)) * 2;
        for (counter = 0; counter < end_deg*2 + parity; counter++) {

            //// leader = old_r.coef[act_deg] / r.coef[r.deg];
            leader_exp = math_ctx->log_table[old_r->coef[act_deg]] - math_ctx->log_table[r->coef[r->deg]];
            if (leader_exp < 0){
                leader_exp += math_ctx->ord;
            }
            leader = math_ctx->exp_table[leader_exp];
            leader = leader * (!!old_r->coef[act_deg]);

            //// old_r = old_r + r q_i(X);
            BPU_gf2xPolyCopy(helper1, r);
            BPU_gf2xPolyMulEl(helper1, leader, math_ctx);
            BPU_gf2xPolyShlC(helper1, act_deg - r->deg);
            BPU_gf2xPolyAddC(helper2, old_r, helper1);
            BPU_gf2xPolyCopy(old_r, helper2);

            //// q = q + tmp_q(X);
            BPU_gf2xPolyCopy(helper1, t);
            BPU_gf2xPolyMulEl(helper1, leader, math_ctx);
            BPU_gf2xPolyShlC(helper1, act_deg - r->deg);
            BPU_gf2xPolyCopy(helper2, tmp_2);
            BPU_gf2xPolyAddC(tmp_2, helper1, helper2);

            BPU_gf2xPolyCopy(helper1, old_r);
            BPU_gf2xPolyCopy(old_r, r);
//			BPU_gf2xPolyCopy(&tmp, &old_t);
            if (r->deg < act_deg) {
                act_deg -= 2;
                BPU_gf2xPolyCopy(old_r, helper1);

                //// Perform dummy operations
                BPU_gf2xPolyCopy(tmp, old_t);
                BPU_gf2xPolyCopy(old_s, t);
                BPU_gf2xPolyAddC(s, tmp, tmp_2);
                BPU_gf2xPolyNull(tmp);
            }
            else {
                act_deg -= 1;
                BPU_gf2xPolyCopy(r, helper1);

                //// save t quocient
                BPU_gf2xPolyCopy(tmp, old_t);
                BPU_gf2xPolyCopy(old_t, t);
                BPU_gf2xPolyAddC(t, tmp, tmp_2);
                BPU_gf2xPolyNull(tmp_2);
            }
            act_deg += 1;
        }
    }

    // prepare return values
    BPU_gf2xPolyCopy(d, old_r);
    BPU_gf2xPolyCopy(s, old_s);
    BPU_gf2xPolyCopy(t, old_t);

    // make monic, if it is not
    inv_lead = BPU_gf2xPolyMakeMonic(d, math_ctx);

    if (inv_lead != 0) {
        BPU_gf2xPolyMulEl(s, inv_lead, math_ctx);
        BPU_gf2xPolyMulEl(t, inv_lead, math_ctx);
    }
    BPU_gf2xPolyFree(&tmp);
    BPU_gf2xPolyFree(&tmp_2);
    BPU_gf2xPolyFree(&old_s);
    BPU_gf2xPolyFree(&old_t);
    BPU_gf2xPolyFree(&old_r);
    BPU_gf2xPolyFree(&r);
    BPU_gf2xPolyFree(&q);
    BPU_gf2xPolyFree(&helper1);
    BPU_gf2xPolyFree(&helper2);

    return 0;
}

BPU_T_GF2_16x BPU_gf2xPolyEval(const BPU_T_GF2_16x_Poly * poly,
                               const BPU_T_GF2_16x x,
                               const BPU_T_Math_Ctx * math_ctx) {
    int i;
    BPU_T_GF2_16x ret = poly->coef[poly->deg];

    for (i = poly->deg; i > 0; i--) {
        ret = BPU_gf2xMulModT(ret, x, math_ctx) ^ poly->coef[i - 1];
    }
    return ret;
}


BPU_T_GF2_16x BPU_gf2xPolyEvalC(const BPU_T_GF2_16x_Poly *poly,
                                const BPU_T_GF2_16x x,
                                const BPU_T_Math_Ctx *math_ctx) {
    int i;
    BPU_T_GF2_16x ret = poly->coef[poly->deg];

    for (i = poly->deg; i > 0; i--) {
        ret = BPU_gf2xMulModC(ret, x, math_ctx->mod, math_ctx->mod_deg) ^ poly->coef[i-1];
    }
    return ret;
}

int BPU_gf2xPolyCmp(const BPU_T_GF2_16x_Poly * p1,
                    const BPU_T_GF2_16x_Poly * p2) {
    int i;

    if (p1->deg != p2->deg) {
        return -1;
    }
    for (i = 0; i <= p1->deg; i++) {
        if (p1->coef[i] != p2->coef[i]) {
            return i + 1;
        }
    }
    return 0;
}

int BPU_gf2xPolyIrredTest(const BPU_T_GF2_16x_Poly * p,
                          const BPU_T_Math_Ctx * math_ctx) {
    // x^(q^n) = x^((2^m)^n) = x^(2^(m*n))
    // q = 2^m
    // check if gcd(x^(q^n) - x, f)==f. if not, f is reducible
    // find all prime factors of n = deg(f)
    // for every prime factor t of n check if gcd(x^(q^(n/t)), f) == 1. if not f is reducible
    // otherwise f is ireducible
    int i, j;
    int is_irred = 1;
    int m = math_ctx->mod_deg;
    int n = p->deg;
    int exponent = m * n;
    BPU_T_GF2_16x_Poly *tmp, *out, *qr, *x, *gcd, *s, *t, *one;

    // test if some alpha is root
    for (i = 0; i < math_ctx->ord; i++) {
        if (BPU_gf2xPolyEval(p, math_ctx->exp_table[i], math_ctx) == 0) {
            return 0;
        }
    }
    BPU_gf2xPolyMalloc(&out, 0);
    BPU_gf2xPolyMalloc(&one, 0);
    BPU_gf2xPolyMalloc(&qr, 0);
    BPU_gf2xPolyMalloc(&tmp, 1);
    BPU_gf2xPolyMalloc(&x, 1);
    // gcd, s, t, will be allocated inside gf2xPolyExtEuclidA

    // set tmp polynomial tmp(x) = x
    tmp->coef[0] = 0;
    tmp->coef[1] = 1;
    tmp->deg = BPU_gf2xPolyGetDeg(tmp);

    x->coef[0] = 0;
    x->coef[1] = 1;
    x->deg = BPU_gf2xPolyGetDeg(x);

    one->coef[0] = 1;
    one->deg = 0;

    // simplify polynomial with big coeffitient
    for (i = 0; i < exponent; i++) {
        BPU_gf2xPolyMul(out, tmp, tmp, math_ctx);
        BPU_gf2xPolyMod(tmp, out, p, math_ctx);
    }
    BPU_gf2xPolyAdd(qr, tmp, x);

    BPU_gf2xPolyMalloc(&gcd, (qr->deg > p->deg) ? qr->deg : p->deg);
    BPU_gf2xPolyMalloc(&s, gcd->max_deg);
    BPU_gf2xPolyMalloc(&t, gcd->max_deg);

    BPU_gf2xPolyExtEuclid(gcd, s, t, qr, p, -1, math_ctx);

    // if differs
    if (BPU_gf2xPolyCmp(p, gcd)) {
        BPU_gf2xPolyFree(&out);
        BPU_gf2xPolyFree(&one);
        BPU_gf2xPolyFree(&qr);
        BPU_gf2xPolyFree(&tmp);
        BPU_gf2xPolyFree(&x);
        BPU_gf2xPolyFree(&gcd);
        BPU_gf2xPolyFree(&s);
        BPU_gf2xPolyFree(&t);
        return 0;
    }

    for (j = 2; j <= p->deg; j++) {
        if (p->deg % j != 0 || BPU_isPrime(j) == 0) {
            continue;
        }
        BPU_gf2xPolyNull(tmp);
        tmp->coef[0] = 0;
        tmp->coef[1] = 1;
        tmp->deg = BPU_gf2xPolyGetDeg(tmp);

        exponent = m * (n / j);

        for (i = 0; i < exponent; i++) {
            BPU_gf2xPolyMul(out, tmp, tmp, math_ctx);
            BPU_gf2xPolyMod(tmp, out, p, math_ctx);
        }
        BPU_gf2xPolyExtEuclid(gcd, s, t, tmp, p, -1, math_ctx);
        // if differs
        if (BPU_gf2xPolyCmp(one, gcd)) {
            is_irred = 0;
            break;
        }
    }
    BPU_gf2xPolyFree(&out);
    BPU_gf2xPolyFree(&one);
    BPU_gf2xPolyFree(&qr);
    BPU_gf2xPolyFree(&tmp);
    BPU_gf2xPolyFree(&x);
    BPU_gf2xPolyFree(&gcd);
    BPU_gf2xPolyFree(&s);
    BPU_gf2xPolyFree(&t);

    return is_irred;
}

void BPU_gf2xPolyCopy(BPU_T_GF2_16x_Poly * dest, const BPU_T_GF2_16x_Poly * src) {
    // if there is not enough space resize
    if (dest->max_deg < src->max_deg) {
        BPU_gf2xPolyResize(dest, src->max_deg);
    }
    else {
        BPU_gf2xPolyNull(dest);
    }
    memcpy((void *) (dest->coef), (void *) (src->coef),
           sizeof(BPU_T_GF2_16x) * (src->max_deg + 1));

    dest->deg = src->deg;
}

void BPU_gf2xPolyInv(BPU_T_GF2_16x_Poly * out, const BPU_T_GF2_16x_Poly * a,
                     const BPU_T_GF2_16x_Poly * mod,
                     const BPU_T_Math_Ctx * math_ctx) {
    BPU_T_GF2_16x_Poly *d, *t;

    BPU_gf2xPolyMalloc(&d, (a->deg > mod->deg) ? a->deg : mod->deg);
    BPU_gf2xPolyMalloc(&t, d->max_deg);

    BPU_gf2xPolyExtEuclid(d, out, t, a, mod, 0, math_ctx);

    if (d->deg != 0 || d->coef[0] != 1) {
        BPU_printDebug("inverse polynomial NOT found");
        BPU_printError("degree: %d\nelement: %d", d->deg, d->coef[0]);
#ifdef BPU_CONF_PRINT
        BPU_printGf2xPoly(out, math_ctx);
#endif
        BPU_gf2xPolyNull(out);
    }
    BPU_gf2xPolyFree(&d);
    BPU_gf2xPolyFree(&t);
}

BPU_T_GF2_16x BPU_gf2xPolyMakeMonic(BPU_T_GF2_16x_Poly * a,
                                    const BPU_T_Math_Ctx * math_ctx) {
    BPU_T_GF2_16x inv_lead = 0;

    // if it is already monic do nothing
    if ((a->deg > -1 && a->coef[a->deg] == 1) || a->deg == -1) {
        return inv_lead;
    }
    inv_lead = BPU_gf2xInvElement(BPU_gf2xPolyLeadCoef(a), math_ctx);
    BPU_gf2xPolyMulEl(a, inv_lead, math_ctx);

    return inv_lead;
}

BPU_T_GF2_16x BPU_gf2xGetPseudoInv(const BPU_T_GF2_16x a,
                                   const BPU_T_GF2_16x res,
                                   const BPU_T_Math_Ctx * math_ctx) {
    return BPU_gf2xMulModT(BPU_gf2xInvElement(a, math_ctx), res, math_ctx);
}

void BPU_gf2xMatInsertPoly(BPU_T_GF2_16x_Matrix * mat,
                           const BPU_T_GF2_16x_Poly * poly, int i) {
    int j;

    for (j = 0; j <= poly->deg; j++) {
        mat->elements[i][j] = poly->coef[j];
    }
}

void BPU_gf2xPolyToVec(BPU_T_GF2_16x_Vector * vec,
                       const BPU_T_GF2_16x_Poly * poly, int len) {
    int i;

    if (poly->deg >= len) {
        BPU_printError("dimension missmatch");

        exit(-1);
    }
    vec->len = len;

    for (i = 0; i <= poly->deg; i++) {
        vec->elements[i] = poly->coef[i];
    }
}

void BPU_gf2xVecToPoly(BPU_T_GF2_16x_Poly * poly,
                       const BPU_T_GF2_16x_Vector * vec) {
    int i;

    for (i = 0; i < vec->len; i++) {
        poly->coef[i] = vec->elements[i];
    }
    poly->deg = BPU_gf2xPolyGetDeg(poly);
}

void BPU_gf2xSwap(BPU_T_GF2_16x * a, BPU_T_GF2_16x * b) {
    BPU_T_GF2_16x tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

void BPU_gf2xMatSwapRows(BPU_T_GF2_16x_Matrix * mat, int i, int j) {
    int k;

    for (k = 0; k < mat->n; k++) {
        BPU_gf2xSwap(&(mat->elements[i][k]), &(mat->elements[j][k]));
    }
}

void BPU_gf2xMatMulElRow(BPU_T_GF2_16x_Matrix * mat,
                         const BPU_T_GF2_16x element, int row,
                         const BPU_T_Math_Ctx * math_ctx) {
    int i;

    for (i = 0; i < mat->n; i++) {
        mat->elements[row][i] =
            BPU_gf2xMulModT(element, mat->elements[row][i], math_ctx);
    }
}

int BPU_gf2xMatFindPivot(const BPU_T_GF2_16x_Matrix * mat, int index) {
    int i;

    for (i = index; i < mat->k; i++) {
        if (mat->elements[i][index] != 0) {
            return i;
        }
    }
    return -1;
}

void BPU_gf2xVecMulEl(BPU_T_GF2_16x_Vector * vec, BPU_T_GF2_16x element,
                      const BPU_T_Math_Ctx * math_ctx) {
    int i;

    for (i = 0; i < vec->len; i++) {
        vec->elements[i] = BPU_gf2xMulModT(element, vec->elements[i], math_ctx);
    }
}

void BPU_gf2xMatXorRows(BPU_T_GF2_16x_Matrix * mat, int index, int j,
                        const BPU_T_Math_Ctx * math_ctx) {
    int k;
    BPU_T_GF2_16x element;
    BPU_T_GF2_16x_Vector *tmp;

    BPU_gf2xVecMalloc(&tmp, mat->n);
    for (k = 0; k < tmp->len; k++) {
        tmp->elements[k] = mat->elements[index][k];
    }
    element = mat->elements[j][index];

    BPU_gf2xVecMulEl(tmp, element, math_ctx);

    for (k = 0; k < tmp->len; k++) {
        mat->elements[j][k] = mat->elements[j][k] ^ tmp->elements[k];
    }
    BPU_gf2xVecFree(&tmp);
}

void BPU_gf2xMatClearCol(BPU_T_GF2_16x_Matrix * mat, int index,
                         const BPU_T_Math_Ctx * math_ctx) {
    int i;

    for (i = 0; i < mat->k; i++) {
        if (i == index) {
            continue;
        }
        BPU_gf2xMatXorRows(mat, index, i, math_ctx);
    }
}

void BPU_gf2xMatGEM(BPU_T_GF2_16x_Matrix * mat, const BPU_T_Math_Ctx * math_ctx) {
    int i, pivot;
    BPU_T_GF2_16x element;

    for (i = 0; i < mat->k; i++) {
        pivot = BPU_gf2xMatFindPivot(mat, i);
        if (pivot == -1)
            BPU_printError
                ("################# unbeliviable 'badness' ###########");
        if (pivot != i) {
            BPU_gf2xMatSwapRows(mat, i, pivot);
        }
        if (mat->elements[i][i] != 1) {
            element = BPU_gf2xPowerModT(mat->elements[i][i], -1, math_ctx);
            BPU_gf2xMatMulElRow(mat, element, i, math_ctx);
        }
        BPU_gf2xMatClearCol(mat, i, math_ctx);
    }
}

void BPU_gf2xPolyGenRandom(BPU_T_GF2_16x_Poly * p, int t,
                           const BPU_T_Math_Ctx * math_ctx) {
    int i;

    p->coef[t] = 1;
    p->coef[0] = BPU_prngGetRand(1, math_ctx->ord);

    for (i = 1; i < t; i++) {
        p->coef[i] = BPU_prngGetRand(0, math_ctx->ord);
    }
    p->deg = t;
}

void BPU_gf2xPolyGenGoppa(BPU_T_GF2_16x_Poly * p, int t,
                          const BPU_T_Math_Ctx * math_ctx) {
#if defined(DEBUG_L) || defined(WARNING_L)
    int i = 1;
#endif
    while (1) {
        BPU_gf2xPolyGenRandom(p, t, math_ctx);

        if (BPU_gf2xPolyIrredTest(p, math_ctx) == 1) {
            break;
        }
        BPU_printWarning("new irreducibility test #%d", i++);
    }
}
