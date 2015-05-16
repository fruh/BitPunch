/*
This file is part of BitPunch
Copyright (C) 2013-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
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
#include "permtypes.h"

#include <bitpunch/debugio.h>
#include <stdlib.h>

void BPU_permFree(BPU_T_Perm_Vector **p) {
    if (!*p) {
        return;
    }
    free((*p)->elements);
    free(*p);
}

int BPU_permMalloc(BPU_T_Perm_Vector **p, int size) {
	// allocate memory
	int i;
    *p = (BPU_T_Perm_Vector *) calloc(sizeof(BPU_T_Perm_Vector), 1);

    if (!*p) {
        BPU_printError("allocation error");
        return -1;
    }

    (*p)->size = size;
    (*p)->elements = (BPU_T_Perm_Element*) malloc(sizeof(BPU_T_Perm_Element) * size);

    if (!(*p)->elements) {
		BPU_printError("BPU_mallocPerm: can not allocate permutation vector");

		return -1;
	}
    for (i = 0; i < (*p)->size; i++) {
        (*p)->elements[i] = i;
	}
	return 0;
}
