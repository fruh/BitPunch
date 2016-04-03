#/**
#This file is part of BitPunch
#Copyright (C) 2013-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
#Copyright (C) 2013-2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
#Copyright (C) 2013-2016 Marek Klein  <kleinmrk[what here]gmail.com>
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
# $@ - target of rule
# $< - name f first source
# $^ - all sources
# $? - names of sources, which are newer than target

PREFIX_SRC = src
INCLUDE_PATH = -Isrc/include
DEMOS = demos
ifndef CFLAGS
	CFLAGS += -g -Wall -DBPU_CONF_FULL_MECS -DERROR_L -DBPU_CONF_PRINT
endif

# compiler
CC = gcc $(INCLUDE_PATH)

LDFLAGS = -lm
#-fstack-protector-all

# curent working directory
CWD = $(shell pwd)

# output, distribution files target, dynamic
DIST_DIR = dist
BUILD_DIR = build

# test target
TEST_DIR = $(DIST_DIR)/test

C_FILES = $(shell find src/bitpunch/ -type f -name '*.c')

OBJS := $(patsubst %.c, %.c.o, $(C_FILES))

PICS := $(patsubst %.c, %.c.o.pic, $(C_FILES))

######################################## TARGETS ###############################################
default: demos print-info

demos: prepare-dir static-lib
	$(CC) $(CFLAGS) -o $(TEST_DIR)/bigint $(DEMOS)/bigint.c $(DIST_DIR)/libbpumecs.a $(LDFLAGS)
	$(CC) $(CFLAGS) -o $(TEST_DIR)/plainmecs $(DEMOS)/plainmecs.c $(DIST_DIR)/libbpumecs.a $(LDFLAGS)
	$(CC) $(CFLAGS) -o $(TEST_DIR)/BitPunch $(DEMOS)/main.c $(DIST_DIR)/libbpumecs.a $(LDFLAGS)

test-debug: CFLAGS = -Wall -DBPU_CONF_FULL -ggdb -DDEBUG_L
test-debug: prepare-dir static-lib
	$(CC) $(CFLAGS) -o $(TEST_DIR)/BitPunch $(DEMOS)/main.c $(DIST_DIR)/libbpumecs.a $(LDFLAGS)

test-wo-h: CFLAGS += -DBPU_CONF_GOPPA_WO_H
test-wo-h: prepare-dir static-lib
	$(CC) $(CFLAGS) -o $(TEST_DIR)/BitPunch $(DEMOS)/main.c $(DIST_DIR)/libbpumecs.a $(LDFLAGS)

test-wo-h-debug: CFLAGS = -Wall -DBPU_CONF_FULL -ggdb -DDEBUG_L -DBPU_CONF_GOPPA_WO_H
test-wo-h-debug: prepare-dir static-lib
	$(CC) $(CFLAGS) -o $(TEST_DIR)/BitPunch $(DEMOS)/main.c $(DIST_DIR)/libbpumecs.a $(LDFLAGS)

speed-test: CFLAGS += -O3 -Wall -DBPU_CONF_FULL -DERROR_L
speed-test: prepare-dir static-lib
	$(CC) $(CFLAGS) -o $(TEST_DIR)/BitPunch $(DEMOS)/test-speed.c $(DIST_DIR)/libbpumecs.a $(LDFLAGS)

profile: prepare-dir static-lib
	$(CC) $(CFLAGS_PROF) $(C_FILES) $(DEMOS)/main.c -o $(TEST_DIR)/BitPunch $(LDFLAGS)
	@echo ""
	@[ "${TERM}" != "" ] && which tput > /dev/null && which printf > /dev/null && printf '%$(shell [ "${TERM}" != "" ] && tput cols)s\n' | tr ' ' = || echo "========================================================="
	@echo "Now you can run test aplication: $(TEST_DIR)/BitPunch"
	@echo "Then run: gprof $(TEST_DIR)/BitPunch | less"
	@echo "\t to see statistics."

print-info:
	@echo ""
	@[ "${TERM}" != "" ] && which tput > /dev/null && which printf > /dev/null && printf '%$(shell [ "${TERM}" != "" ] && tput cols)s\n' | tr ' ' = || echo "========================================================="
	@echo "Now you can run test aplication: $(TEST_DIR)/BitPunch"
	@echo "See source file in: $(CWD)/$(PREFIX_SRC)/main.c"

%.c.o.pic: %.c
	$(CC) $(CFLAGS) -c -fpic $< -o $@

# shared-lib: CFLAGS = -O3 -DBPU_CONF_ENCRYPTION
shared-lib: prepare-dir $(PICS)
	 gcc -shared $(PICS) -o $(DIST_DIR)/libbpumecs.so

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

static-lib: prepare-dir $(OBJS)
	cd $(BUILD_DIR)
	ar rcs $(DIST_DIR)/libbpumecs.a $(OBJS)

############################# prepare dirs
prepare-dir:
	@mkdir -vp $(BUILD_DIR)/
	@mkdir -vp $(DIST_DIR)/
	@mkdir -vp $(TEST_DIR)/

############################# clean up matters
clean: clean-obj
	rm -vRf $(DIST_DIR)/
	rm -vRf $(BUILD_DIR)/

clean-obj:
	find src/bitpunch/ -type f -name '*.o' -exec rm -v {} \;
	find src/bitpunch/ -type f -name '*.pic' -exec rm -v {} \;
	find src/bitpunch/ -type f -name '*.gch' -exec rm -v {} \;

############################# help
help:
	@echo ""
	@echo "This is a McEliece library."
	@echo ""
	@echo "Default build makes test binary:"
	@echo "\tSource file: $(DEMOS)/main.c"
	@echo "\tApplication: $(TEST_DIR)/BitPunch"
	@echo "\tLibraries: $(DIST_DIR)/libbpumecs.so or $(DIST_DIR)/libbpumecs.a"
	@echo ""
	@echo "It is possible to build library which has H matrix precomputed or not."
