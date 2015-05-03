/**
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#include "goppatypes.h"
#include <stdlib.h>

#include <bitpunch/math/gf2types.h>
#include <bitpunch/math/gf2xtypes.h>
#include <bitpunch/math/permtypes.h>

void BPU_goppaFreeSpec(BPU_T_Goppa_Spec *spec) {
    BPU_gf2xPolyFree(&spec->g);
    BPU_gf2MatFree(&spec->g_mat);
#ifndef BPU_CONF_GOPPA_WO_H
    BPU_gf2xMatFree(&spec->h_mat);
#endif
    BPU_permFree(&spec->permutation);
}
