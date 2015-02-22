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
CFLAGS = -Wall -DERROR_L -O3
#-fstack-protector-all

# curent working directory
CWD = $(shell pwd)

# build path
BUILD_TARGET = build

# build path
BUILD_TARGET_DEBUG = build-debug

# output, distribution files target, dynamic
DIST_TARGET = dist

# output, distribution files target, dynamic
DIST_TARGET_DEBUG = dist-debug

# static target dir
STATIC_TARGET = $(DIST_TARGET)/static

# static target dir
STATIC_TARGET_DEBUG = $(DIST_TARGET_DEBUG)/static

# shared target dir
SHARED_TARGET = $(DIST_TARGET)/shared

# debug-shared target dir
SHARED_TARGET_DEBUG = $(DIST_TARGET_DEBUG)/shared

# test target
TEST_TARGET = $(DIST_TARGET)/test

# test debug target
TEST_TARGET_DEBUG = $(DIST_TARGET_DEBUG)/test

######################################## TARGETS ###############################################
default: test

test: prepare-dir-test
	$(CC) $(CFLAGS) src/bitpunch/code/*.h src/bitpunch/code/*.c src/bitpunch/code/goppa/*.h src/bitpunch/code/goppa/*.c src/bitpunch/crypto/*.c src/bitpunch/crypto/*.h src/bitpunch/crypto/mecsbasic/*.c src/bitpunch/crypto/mecsbasic/*.h src/bitpunch/*.c src/bitpunch/math/*.c src/bitpunch/math/*.h src/bitpunch/prng/*.h src/bitpunch/prng/*.c  src/main.c -o $(TEST_TARGET)/BitPunch
	@echo ""
	@which tput > /dev/null && which printf > /dev/null && printf '%$(shell tput cols)s\n' | tr ' ' = || echo "========================================================="
	@echo "Now you can run test aplication: $(TEST_TARGET)/BitPunch"
	@echo "See source file in: $(CWD)/$(PREFIX_SRC)/main.c"

test-debug: prepare-dir-test
	$(CC) $(CFLAGS) -ggdb src/bitpunch/code/*.h src/bitpunch/code/*.c src/bitpunch/code/goppa/*.h src/bitpunch/code/goppa/*.c src/bitpunch/crypto/*.c src/bitpunch/crypto/*.h src/bitpunch/crypto/mecsbasic/*.c src/bitpunch/crypto/mecsbasic/*.h src/bitpunch/*.c src/bitpunch/math/*.c src/bitpunch/math/*.h src/bitpunch/prng/*.h src/bitpunch/prng/*.c  src/main.c -o $(TEST_TARGET)/BitPunch
	@echo ""
	@which tput > /dev/null && which printf > /dev/null && printf '%$(shell tput cols)s\n' | tr ' ' = || echo "========================================================="
	@echo "Now you can run test aplication: $(TEST_TARGET)/BitPunch"
	@echo "See source file in: $(CWD)/$(PREFIX_SRC)/main.c"

############################# prepare dirs
prepare-dir-test: prepare-dir
	@mkdir -vp $(TEST_TARGET)/

prepare-dir-test-debug: prepare-dir-debug
	@mkdir -vp $(TEST_TARGET_DEBUG)/

############################# directory matters
prepare-dir:
	@mkdir -vp $(BUILD_TARGET)/
	@mkdir -vp $(DIST_TARGET)/

prepare-dir-debug:
	@mkdir -vp $(BUILD_TARGET_DEBUG)/
	@mkdir -vp $(DIST_TARGET_DEBUG)/	

############################# clean up matters
clean: clean-obj
	rm -vRf $(DIST_TARGET)/
	rm -vRf $(DIST_TARGET_DEBUG)/
	
clean-obj:
	rm -vRf $(BUILD_TARGET)/
	rm -vRf $(BUILD_TARGET_DEBUG)/

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
	@echo "It will build files 'libbpumecs.so' or 'libbpumecs.a' or for debug 'libbpumecsd.so' or 'libbpumecsd.a', then you can link it to your project."
	@echo "Output will be in dist/static/ or dist/shared/ or dist-debug/static-debug/ or dist/shared-debug/"
	@echo ""
	@echo "You can also build a test run: \n\t'make test-*'\nWhere * can be static, shared, static-debug, shared-debug, output will be in dist/test/ or dist-debug/test/."
