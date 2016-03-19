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
#ifndef QCMDPCTYPES_H
#define QCMDPCTYPES_H
#include <bitpunch/math/gf2.h>
#include <bitpunch/debugio.h>
#include <stdlib.h>

/**
 * QC-MDPC McEliece code matrices
 */
typedef struct _BPU_T_Qcmdpc_Spec {
    BPU_T_GF2_QC_Matrix G;      ///< generator matrix
    BPU_T_GF2_Sparse_Qc_Matrix H;       ///< parity-check matrix
    uint16_t m;                 ///< size of cyclic matrix
    uint16_t n0;                ///< number of cyclic matrices
    uint16_t w;                 ///< weight of parity-check matrix row
} BPU_T_Qcmdpc_Spec;

/**
  QC-MDPC McEliece input code params
  */
typedef struct _BPU_T_Qcmdpc_Params {
    uint16_t m;                 ///< size of cyclic matrix
    uint16_t n0;                ///< number of cyclic matrices
    uint16_t w;                 ///< weight of parity-check matrix row
    uint16_t t;                 ///< count of errors
} BPU_T_Qcmdpc_Params;

/**
 * Free QC-MDPC McEliece code matrices
 * @param spec pointer to structure
 */
void BPU_qcmdpcFreeSpec(BPU_T_Qcmdpc_Spec * spec);

/**
 * Allocate memory for QC-MDPC code params. After work you have to free memory using call BPU_qcmdpcFreeParams
 * @param  params pointer to structure
 * @param  m      size of cyclic matrix
 * @param  n0     number of cyclic matrices
 * @param  w      weight of parity-check matrix row
 * @param  t      count of errors
 * @return        0 if OK, else error
 */
int BPU_qcmdpcInitParams(BPU_T_Qcmdpc_Params ** params, const uint16_t m,
                         const uint16_t n0, const uint16_t w, const uint16_t t);

/**
 * Free memory for QC-MDPC code params.
 * @param params pointer to structure
 */
void BPU_qcmdpcFreeParams(BPU_T_Qcmdpc_Params ** params);

#endif // QCMDPCTYPES_H
