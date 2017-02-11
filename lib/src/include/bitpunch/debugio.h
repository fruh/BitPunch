/*
 This file is part of BitPunch
 Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
 Copyright (C) 2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
 Copyright (C) 2014 Marek Klein  <kleinmrk[what here]gmail.com>
 Copyright (C) 2014 Filip Machovec  <filipmachovec[what here]yahoo.com>
 Copyright (C) 2014 Jozef Kudlac <jozef[what here]kudlac.sk>

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
/**
* Here are some helpful print functions. i.e. print number as binary.
*/
#ifndef DEBUGIO_H
#define DEBUGIO_H

#include <stdio.h>

#ifndef _WIN32
#include <inttypes.h>

  /// print macro for 64bit data types
#define BPU_PRINT_U_64 PRIu64
#else
#define BPU_PRINT_U_64 "I64u"
#endif

#if defined(DEBUG_L) || defined(WARNING_L) || defined(ERROR_L)
#include <stdlib.h>
#include <errno.h>

  /// print error message with filename, line
#define BPU_printError(fmt, ...) fprintf(stderr, "ERROR::%s::%s::%d: "fmt"\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); if (errno) {perror("ERROR MSG:"); errno = 0;}
#else
  /// print error message with filename, line
#define BPU_printError(fmt, ...)
#endif

#if defined(DEBUG_L) || defined(WARNING_L)
  /// print warning message with filename, line
#define BPU_printWarning(fmt, ...) fprintf(stderr, "WARNING::%s::%s::%d: "fmt"\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); if (errno) {perror("ERROR MSG:"); errno = 0;}
#else
  /// print warning message with filename, line
#define BPU_printWarning(fmt, ...)
#endif

#if defined(DEBUG_L)
  /// print debug message with filename, line
#define BPU_printDebug(fmt, ...) fprintf(stderr, "DEBUG::%s::%s::%d: "fmt"\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#else
  /// print debug message with filename, line
#define BPU_printDebug(fmt, ...)
#endif

#endif // DEBUGIO_H
