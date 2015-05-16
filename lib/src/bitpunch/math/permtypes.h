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
#ifndef BPU_PERMTYPES_H
#define BPU_PERMTYPES_H

#include <stdint.h>

/// Definition of permutation element type.
typedef uint16_t BPU_T_Perm_Element;

/**
* Representation of permutation.
* Elements are numbers from 0 to size-1.
* Size corresponds to count of columns in matrix.
*/
typedef struct _BPU_T_Perm_Vector {
    BPU_T_Perm_Element *elements; ///< permutation vector
    BPU_T_Perm_Element size; ///< permutation size
}BPU_T_Perm_Vector;

/**
 * Free dynamically or statically alocated permutation vector.
 * @param p      permutation to free
 * @param is_dyn 0 - staticaly allocated BPU_T_Perm_Vector object or 1 when dynamically
 */
/// Free dynamically or statically alocated permutation vector.
void BPU_permFree(BPU_T_Perm_Vector **p);

/**
 * Allocate permutation vector elements of size size using malloc.  After work you have to free memory using call BPU_freePerm(p)
 * @param  p    vector to be allocated  
 * @param  size size, permutation len
 * @return      on succes 0 else error
 */
/// Allocate permutation vector elements of size size.
int BPU_permMalloc(BPU_T_Perm_Vector **p, int size);

#endif // BPU_PERMTYPES_H
