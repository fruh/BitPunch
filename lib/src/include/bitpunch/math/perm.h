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
#ifndef BPU_PERM_H
#define BPU_PERM_H

#include <bitpunch/config.h>

#include <stdint.h>

/// Definition of permutation element type.
typedef uint16_t BPU_T_Perm_Element;

/**
* Representation of permutation.
* Elements are numbers from 0 to size-1.
* Size corresponds to count of columns in matrix.
*/
typedef struct _BPU_T_Perm_Vector {
    BPU_T_Perm_Element *elements;       ///< permutation vector
    BPU_T_Perm_Element size;    ///< permutation size
} BPU_T_Perm_Vector;

#ifdef BPU_CONF_PRINT
/**
* Print permutation.
* @param permutation permutation
*/
void BPU_printPerm(const BPU_T_Perm_Vector * permutation);
#endif // BPU_CONF_PRINT

/**
 * Free dynamically or statically alocated permutation vector.
 * @param p      permutation to free
 * @param is_dyn 0 - staticaly allocated BPU_T_Perm_Vector object or 1 when dynamically
 */
/// Free dynamically or statically alocated permutation vector.
void BPU_permFree(BPU_T_Perm_Vector ** p);

/**
 * Allocate permutation vector elements of size size using malloc.  After work you have to free memory using call BPU_freePerm(p)
 * @param  p    vector to be allocated
 * @param  size size, permutation len
 * @return      on succes 0 else error
 */
/// Allocate permutation vector elements of size size.
int BPU_permMalloc(BPU_T_Perm_Vector ** p, int size);

/**
 * Generate random permutation.
 * @param permutation pointer to structure BPU_T_Perm_Vector 
 * @param size size of permutation vector
 * @return on succes 0, else error
 */
int BPU_permRandomize(BPU_T_Perm_Vector * permutation);

/**
 * @brief BPU_swap
 * @param a
 * @param b
 */
void BPU_permSwap(BPU_T_Perm_Element * a, BPU_T_Perm_Element * b);

/**
 * Get inverse permutation to permutation.
 * @param  out inverted permutation must be allocated
 * @param  in  permutation
 * @return     on succes 0, else error
 */
int BPU_permGetInv(BPU_T_Perm_Vector * out, const BPU_T_Perm_Vector * in);

/**
 * Compose permutations.
 * @param  to_permute  row vector to be permuted
 * @param  permutation
 * @return 0 success, -1 when size of vectors is not the same, -2 on memory allocation error 
 */
int BPU_permPermute(BPU_T_Perm_Vector * to_permute,
                    const BPU_T_Perm_Vector * permutation);

/**
 * @brief BPU_permIsValid Check wheter is permutaion valid.
 * @param p
 * @return on succes 0, else error
 */
int BPU_permIsValid(const BPU_T_Perm_Vector * p);

#endif // BPU_PERM_H
