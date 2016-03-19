/*
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2015 Andrej Gulyas <andrej.guly[what here]gmail.com>

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

#ifndef BPU_QCMDPC_H
#define BPU_QCMDPC_H

#include <bitpunch/config.h>
#include <bitpunch/code/codectx.h>
#include <bitpunch/math/gf2.h>
#include <bitpunch/debugio.h>

/************************************************
DECODE PARAMS
************************************************/

// universal
#define BPU_QCMDPC_PARAM_MAX_ITER 10    ///< maximum count of iterations in decoding

// decode1 alg
#define BPU_QCMDPC_PARAM_DELTA 5        ///< starting param delta (threshold) for decode1 algorithm

// decode2 alg
#define BPU_QCMDPC_PARAM_DELTA_B 0      ///< threshold for decode2
#define BPU_QCMDPC_MAX_B_VALUES 5       ///< count of precalculated values for decode2 algorithm

#ifdef BPU_CONF_ENCRYPTION
/**
 * McEliece QC-MDPC encode
 * @param  out         	cipher text
 * @param  in     	   	message
 * @param  ctx 			QC-MDPC McEliece context
 * @return             	0 if OK, else error
 */
int BPU_mecsQcmdpcEncode(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                         const struct _BPU_T_Code_Ctx *ctx);
#endif

#ifdef BPU_CONF_DECRYPTION
/**
 * McEliece QC-MDPC decrypt
 * @param  out         	message
 * @param  in     	   	cipher text
 * @param  ctx 			QC-MDPC McEliece context
 * @return              0 if OK, else error
 */
int BPU_mecsQcmdpcDecrypt(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                          const struct _BPU_T_Code_Ctx *ctx);

/**
 * Decoding algorithm 1 for QC-MDPC codes capable of correct param_t errors.
 * It has non-zero probability of DECODING FAUILURE RATE (=<0.00000%).
 * Slower than algorithm 2.
 * Better DFR than algorithm 2.
 * Can be parametrized by param_max_iter, param_delta (threshold tolerance value).
 * @param  error_vec   output error vector  
 * @param  cipher_text input cipher text
 * @param  delta       param delta (threshold tolerance parameter)
 * @param  ctx 		   QC-MDPC McEliece context
 * @return             0 if OK, else error
 */
int BPU_mecsQcmdpcDecode1(BPU_T_GF2_Vector * error_vec,
                          const BPU_T_GF2_Vector * cipher_text, int delta,
                          const struct _BPU_T_Code_Ctx *ctx);

/**
 * Decoding algorithm 2 for QC-MDPC codes capable of correct param_t errors.
 * It has non-zero probability of DECODING FAUILURE RATE (=<0.00009%).
 * Faster than algorithm 1.
 * Worse DFR than algorithm 1.
 * Can be parametrized by param_max_iter, param_delta_B (precomputed threshold values).
 * @param  error_vec   output error vector    
 * @param  cipher_text input cipher text
 * @param  ctx 		   QC-MDPC McEliece context
 * @return             0 if OK, else error
 */
int BPU_mecsQcmdpcDecode2(BPU_T_GF2_Vector * error_vec,
                          const BPU_T_GF2_Vector * cipher_text,
                          const struct _BPU_T_Code_Ctx *ctx);

/**
 * Calc syndrom of cipher_text.
 * @param syndrom     output computed syndrom of cipher text
 * @param cipher_text cipher text
 * @param ctx 		  QC-MDPC McEliece context
 * 
 */
void BPU_mecsQcmdpcCalcSyndrom(BPU_T_GF2_Vector * syndrom,
                               const BPU_T_GF2_Vector * cipher_text,
                               const struct _BPU_T_Code_Ctx *ctx);
#endif

#ifdef BPU_CONF_KEY_GEN
/**
 * Test if generated matrices G x H^T = 0
 * @param  G     generator matrix of code
 * @param  H     parity-check matrix of code
 * @return       0 if OK, 1 if error
 */
int BPU_mecsQcmdpcTestGHmatrices(const BPU_T_GF2_QC_Matrix * G,
                                 const BPU_T_GF2_Sparse_Qc_Matrix * H);

/**
 * Generate key pair of QC-MDPC code for McEliece cryptosystem.
 * Generator matrix G is not sparse. Its public key.
 * Parity-check matrix H is in sparse form. Its private key.
 * Params of code are set as global constants in crypto.h header file.
 * @param ctx 		QC-MDPC McEliece context
 * @return          0 if OK, else error
 */
int BPU_mecsQcmdpcGenKeys(BPU_T_Code_Ctx * ctx);
#endif

#endif // BPU_QCMDPC_H
