/*
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
#ifndef ERRORCODES_H
#define ERRORCODES_H

#define OK 0

// general error codes: prefix 0x0000XXXX
#define BPU_EC_MALLOC_ERROR 0x00000001

// code error codes: prefix 0x0001XXXX
#define BPU_EC_CODE_TYPE_NOT_SUPPORTED 0x00010001
#define BPU_EC_CODE_PARAMS_NOT_SUPPORTED 0x00010002

// mecs error codes: prefix 0x0002XXXX
#define BPU_EC_MECS_TYPE_NOT_SUPPORTED 0x00020001

#endif // ERRORCODES_H
