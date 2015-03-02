#/**
#This file is part of BitPunch
#Copyright (C) 2013-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
#Copyright (C) 2013-2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
#Copyright (C) 2013-2014 Marek Klein  <kleinmrk[what here]gmail.com>
#Copyright (C) 2013-2014 Filip Machovec  <filipmachovec[what here]yahoo.com>
#Copyright (C) 2013-2014 Jozef Kudlac <jozef[what here]kudlac.sk>

#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.
#*/
# $@ - ciel pravidla
# $< - meno prveho zdroja
# $^ - vsetky zdroje
# $? - nazvy zdrojov, ktore su novsie ako ciel

PREFIX_SRC = src
INCLUDE_PATH = -Isrc/

# compiler
CC = gcc $(INCLUDE_PATH)

# compiler flags
#CFLAGS = -Wall -DERROR_L -pg -fprofile-arcs
CFLAGS = -Wall -DERROR_L -O2
#-fstack-protector-all

# curent working directory
CWD = $(shell pwd)

# build path
BUILD_TARGET = build

# output, distribution files target, dynamic
DIST_TARGET = dist

# static target dir
STATIC_TARGET = $(DIST_TARGET)/static

# shared target dir
SHARED_TARGET = $(DIST_TARGET)/shared

# test target
TEST_TARGET = $(DIST_TARGET)/test

SRC_FILES = src/bitpunch/code/*.h src/bitpunch/code/*.c src/bitpunch/code/goppa/*.h src/bitpunch/code/goppa/*.c src/bitpunch/crypto/*.c src/bitpunch/crypto/*.h src/bitpunch/crypto/mecsbasic/*.c src/bitpunch/crypto/mecsbasic/*.h src/bitpunch/*.c src/bitpunch/math/*.c src/bitpunch/math/*.h src/bitpunch/prng/*.h src/bitpunch/prng/*.c

######################################## TARGETS ###############################################
default: test-with-h print-info

test-wo-h: prepare-dir-test
	$(CC) $(CFLAGS) $(SRC_FILES) src/main.c -o $(TEST_TARGET)/BitPunch

test-wo-h-debug: prepare-dir-test
	$(CC) $(CFLAGS) -ggdb $(SRC_FILES) src/main.c -o $(TEST_TARGET)/BitPunch

speed-test-wo-h: prepare-dir-test
	$(CC) $(CFLAGS) $(SRC_FILES) src/test-speed.c -o $(TEST_TARGET)/BitPunch

test-with-h: prepare-dir-test
	$(CC) $(CFLAGS) -DBPU_GOPPA_WITH_H $(SRC_FILES) src/main.c -o $(TEST_TARGET)/BitPunch

test-with-h-debug: prepare-dir-test
	$(CC) $(CFLAGS) -ggdb -DBPU_GOPPA_WITH_H $(SRC_FILES) src/main.c -o $(TEST_TARGET)/BitPunch

speed-test-with-h: prepare-dir-test
	$(CC) $(CFLAGS) -DBPU_GOPPA_WITH_H $(SRC_FILES) src/test-speed.c -o $(TEST_TARGET)/BitPunch

print-info:
	@echo ""
	@[ "${TERM}" != "" ] && which tput > /dev/null && which printf > /dev/null && printf '%$(shell [ "${TERM}" != "" ] && tput cols)s\n' | tr ' ' = || echo "========================================================="
	@echo "Now you can run test aplication: $(TEST_TARGET)/BitPunch"
	@echo "See source file in: $(CWD)/$(PREFIX_SRC)/main.c"

############################# prepare dirs
prepare-dir-test: prepare-dir
	@mkdir -vp $(TEST_TARGET)/

prepare-dir:
	@mkdir -vp $(BUILD_TARGET)/
	@mkdir -vp $(DIST_TARGET)/	

############################# clean up matters
clean: clean-obj
	rm -vRf $(DIST_TARGET)/
	
clean-obj:
	rm -vRf $(BUILD_TARGET)/

############################# help
help:
	@echo "Some kind of help message:"
	@echo ""
	@echo "This is a McEliece library."
	@echo ""
	@echo "Default build makes test binary:"
	@echo "\tSource file: $(CWD)/$(PREFIX_SRC)/main.c"
	@echo "\tApplication: $(TEST_TARGET)/BitPunch"
	@echo ""
	@echo "It is possible to build library which has H matrix precomputed or not. See targets with '*-wo-h*' or '*-with-h*'"
#	@echo "It will build files 'libbpumecs.so' or for debug 'libbpumecsd.so' or 'libbpumecsd.a', then you can link it to your project."
#	@echo "Output will be in dist/static/ or dist/shared/"
#	@echo ""
#	@echo "You can also build a test run: \n\t'make test-*'\nWhere * can be static, shared, static-debug, shared-debug, output will be in dist/test/ or dist-debug/test/."
