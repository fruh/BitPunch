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
#include "prng.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint32_t BPU_prngGetRand(int from, int to) {
//      uint32_t tmp;
//      FILE *fp;

//      if (from <= to){
//              fp = fopen("/dev/urandom", "rb");

//              if (!fp) {
//                      fprintf(stderr, "dsadsa\n");
//                      tmp = rand();
//              }
//              else {
//                      fread(&tmp, sizeof(uint32_t), 1, fp);
//                      fclose(fp);
//              }
//              return (uint32_t) (tmp % (to - from) + from);
//      }
//      return 0;
    if (from <= to) {
        return rand() % (to - from) + from;
    }
    return 0;
}
