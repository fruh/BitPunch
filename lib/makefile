# $@ - ciel pravidla
# $< - meno prveho zdroja
# $^ - vsetky zdroje
# $? - nazvy zdrojov, ktore su novsie ako ciel

PREFIX_SRC = src

# compiler
CC = gcc

# compiler flags
#CFLAGS = -Wall -DERROR_L -pg -fprofile-arcs
CFLAGS = -Wall -DERROR_L -O3
#-fstack-protector-all

# dynamically linked libraries for dynamic build
LDFLAGS_SHARED = -lbpumceliece -lcrypto

# dynamically linked libraries for dynamic build
LDFLAGS_SHARED_DEBUG = -lbpumcelieced -lcrypto

# dynamically linked libraries for static build
LDFLAGS_STATIC = -lcrypto

# object file compiling
OBJ_CFLAGS = $(CFLAGS) -c

# object file position independent code compiling
OBJ_PIC_CFLAGS = $(CFLAGS) -c -fpic

# shared lib code compile
SHARED_CFLAGS = -shared 

# shared lib code compile
STATIC_CFLAGS = -c

# compiler debug flags
CFLAGS_DEBUG = -Wall -ggdb -DDEBUG_L -fstack-protector-all

# object file compiling debug
CFLAGS_OBJ_DEBUG = $(CFLAGS_DEBUG) -c

# object file position independent code compiling debug
CFLAGS_OBJ_PIC_DEBUG = $(CFLAGS_DEBUG) -c -fpic

# shared lib code compile debug
CFLAGS_SHARED_DEBUG = -shared -ggdb

# test
TEST_SPEED_SRC = \
	$(PREFIX_SRC)/test-speed.c \
	$(PREFIX_SRC)/arithmetics.h \
	$(PREFIX_SRC)/types.h \
	$(PREFIX_SRC)/globals.h \
	$(PREFIX_SRC)/init.h \
	$(PREFIX_SRC)/operations.h \
	$(PREFIX_SRC)/process.h \
	$(PREFIX_SRC)/mceliece.h

# test main
MAIN_SRC = \
	$(PREFIX_SRC)/main.c \
	$(PREFIX_SRC)/arithmetics.h \
	$(PREFIX_SRC)/types.h \
	$(PREFIX_SRC)/globals.h \
	$(PREFIX_SRC)/init.h \
	$(PREFIX_SRC)/operations.h \
	$(PREFIX_SRC)/process.h \
	$(PREFIX_SRC)/mceliece.h

# libs sources
DEBUGIO_SRC = \
	$(PREFIX_SRC)/debugio.c \
	$(PREFIX_SRC)/types.h
ARITHMETICS_SRC = \
	$(PREFIX_SRC)/arithmetics.c \
	$(PREFIX_SRC)/arithmetics.h \
	$(PREFIX_SRC)/globals.h \
	$(PREFIX_SRC)/init.h \
	$(PREFIX_SRC)/operations.h
INIT_SRC = \
	$(PREFIX_SRC)/init.c \
	$(PREFIX_SRC)/init.h \
	$(PREFIX_SRC)/globals.h \
	$(PREFIX_SRC)/arithmetics.h \
	$(PREFIX_SRC)/debugio.h \
	$(PREFIX_SRC)/operations.h \
	$(PREFIX_SRC)/process.h
OPERATIONS_SRC = \
	$(PREFIX_SRC)/operations.c \
	$(PREFIX_SRC)/operations.h \
	$(PREFIX_SRC)/arithmetics.h \
	$(PREFIX_SRC)/globals.h \
	$(PREFIX_SRC)/debugio.h \
	$(PREFIX_SRC)/init.h
PROCESS_SRC = \
	$(PREFIX_SRC)/process.c \
	$(PREFIX_SRC)/process.h \
	$(PREFIX_SRC)/arithmetics.h \
	$(PREFIX_SRC)/globals.h \
	$(PREFIX_SRC)/debugio.h \
	$(PREFIX_SRC)/init.h \
	$(PREFIX_SRC)/operations.h
MCELIECE_SRC = \
	$(PREFIX_SRC)/mceliece.c \
	$(PREFIX_SRC)/debugio.h \
	$(PREFIX_SRC)/init.h \
	$(PREFIX_SRC)/types.h

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

default: main-static
	@echo ""
	@which tput > /dev/null && which printf > /dev/null && printf '%$(shell tput cols)s\n' | tr ' ' = || echo "========================================================="
	@echo "Now you can run test aplication: $(TEST_TARGET)/main-static"
	@echo "See source file in: $(CWD)/$(PREFIX_SRC)/main.c"

############################# static test main
main-static: prepare-dir-test static-lib $(BUILD_TARGET)/main-static.o
	$(CC) $(CFLAGS) -o $(TEST_TARGET)/$@ $(BUILD_TARGET)/main-static.o $(STATIC_TARGET)/libbpumceliece.a $(LDFLAGS_STATIC)

############################# static test
test-speed-static: prepare-dir-test static-lib $(BUILD_TARGET)/test-speed-static.o
	$(CC) $(CFLAGS) -o $(TEST_TARGET)/$@ $(BUILD_TARGET)/test-speed-static.o $(STATIC_TARGET)/libbpumceliece.a $(LDFLAGS_STATIC)

############################# shared test
# test-shared: prepare-dir-test shared-lib $(BUILD_TARGET)/main.o
# 	$(CC) -L$(SHARED_TARGET)/ -o $(TEST_TARGET)/$@ $(BUILD_TARGET)/main.o $(LDFLAGS_SHARED)
# 	echo "export LD_LIBRARY_PATH=$(CWD)/$(SHARED_TARGET)/:$$LD_LIBRARY_PATH && $(CWD)/$(TEST_TARGET)/$@ " > $(TEST_TARGET)/run
# 	chmod u+x $(TEST_TARGET)/run

############################# prepare dirs
prepare-dir-test: prepare-dir
	@mkdir -vp $(TEST_TARGET)/

prepare-dir-test-debug: prepare-dir-debug
	@mkdir -vp $(TEST_TARGET_DEBUG)/

############################# main test obj build
$(BUILD_TARGET)/main-static.o: $(MAIN_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

$(BUILD_TARGET_DEBUG)/main-static.o: $(MAIN_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

############################# speed test obj build
$(BUILD_TARGET)/test-speed-static.o: $(TEST_SPEED_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

$(BUILD_TARGET_DEBUG)/test-speed-static.o: $(TEST_SPEED_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

############################# static library
prepare-dir-static: prepare-dir
	@mkdir -vp $(STATIC_TARGET)/

static-lib: prepare-dir-static $(STATIC_TARGET)/libbpumceliece.a

$(BUILD_TARGET)/debugio.o: $(DEBUGIO_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

$(BUILD_TARGET)/arithmetics.o: $(ARITHMETICS_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

$(BUILD_TARGET)/operations.o: $(OPERATIONS_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

$(BUILD_TARGET)/init.o: $(INIT_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

$(BUILD_TARGET)/process.o: $(PROCESS_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

$(BUILD_TARGET)/mceliece.o: $(MCELIECE_SRC)
	$(CC) $(OBJ_CFLAGS) -o $@ $<

$(STATIC_TARGET)/libbpumceliece.a: $(BUILD_TARGET)/mceliece.o $(BUILD_TARGET)/process.o $(BUILD_TARGET)/init.o $(BUILD_TARGET)/operations.o $(BUILD_TARGET)/debugio.o $(BUILD_TARGET)/arithmetics.o
	ar rcs $@ $^

############################# static DEBUG library
prepare-dir-static-debug: prepare-dir-debug
	@mkdir -vp $(STATIC_TARGET_DEBUG)/

static-lib-debug: prepare-dir-static-debug $(STATIC_TARGET_DEBUG)/libbpumcelieced.a

$(BUILD_TARGET_DEBUG)/debugio.o: $(DEBUGIO_SRC)
	$(CC) $(CFLAGS_OBJ_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/arithmetics.o: $(ARITHMETICS_SRC)
	$(CC) $(CFLAGS_OBJ_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/operations.o: $(OPERATIONS_SRC)
	$(CC) $(CFLAGS_OBJ_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/init.o: $(INIT_SRC)
	$(CC) $(CFLAGS_OBJ_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/process.o: $(PROCESS_SRC)
	$(CC) $(CFLAGS_OBJ_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/mceliece.o: $(MCELIECE_SRC)
	$(CC) $(CFLAGS_OBJ_DEBUG) -o $@ $<

$(STATIC_TARGET_DEBUG)/libbpumcelieced.a: $(BUILD_TARGET_DEBUG)/mceliece.o $(BUILD_TARGET_DEBUG)/process.o $(BUILD_TARGET_DEBUG)/init.o $(BUILD_TARGET_DEBUG)/operations.o $(BUILD_TARGET_DEBUG)/debugio.o $(BUILD_TARGET_DEBUG)/arithmetics.o
	ar rcs $@ $^

############################# dynamic library
prepare-dir-shared: prepare-dir
	@mkdir -vp $(SHARED_TARGET)/

shared-lib: prepare-dir-shared $(SHARED_TARGET)/libbpumceliece.so

$(BUILD_TARGET)/debugio.o.pic: $(DEBUGIO_SRC)
	$(CC) $(OBJ_PIC_CFLAGS) -o $@ $<

$(BUILD_TARGET)/arithmetics.o.pic: $(ARITHMETICS_SRC)
	$(CC) $(OBJ_PIC_CFLAGS) -o $@ $<

$(BUILD_TARGET)/operations.o.pic: $(OPERATIONS_SRC)
	$(CC) $(OBJ_PIC_CFLAGS) -o $@ $<

$(BUILD_TARGET)/init.o.pic: $(INIT_SRC)
	$(CC) $(OBJ_PIC_CFLAGS) -o $@ $<

$(BUILD_TARGET)/process.o.pic: $(PROCESS_SRC)
	$(CC) $(OBJ_PIC_CFLAGS) -o $@ $<

$(BUILD_TARGET)/mceliece.o.pic: $(MCELIECE_SRC)
	$(CC) $(OBJ_PIC_CFLAGS) -o $@ $<

$(SHARED_TARGET)/libbpumceliece.so: $(BUILD_TARGET)/mceliece.o.pic $(BUILD_TARGET)/process.o.pic $(BUILD_TARGET)/init.o.pic $(BUILD_TARGET)/operations.o.pic $(BUILD_TARGET)/debugio.o.pic $(BUILD_TARGET)/arithmetics.o.pic
	$(CC) $(SHARED_CFLAGS) -o $@ $^

############################# dynamic DEBUG library
prepare-dir-shared-debug: prepare-dir-debug
	@mkdir -vp $(SHARED_TARGET_DEBUG)/

shared-lib-debug: prepare-dir-shared-debug $(SHARED_TARGET_DEBUG)/libbpumcelieced.so

$(BUILD_TARGET_DEBUG)/debugio.o.pic: $(DEBUGIO_SRC)
	$(CC) $(CFLAGS_OBJ_PIC_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/arithmetics.o.pic: $(ARITHMETICS_SRC)
	$(CC) $(CFLAGS_OBJ_PIC_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/operations.o.pic: $(OPERATIONS_SRC)
	$(CC) $(CFLAGS_OBJ_PIC_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/init.o.pic: $(INIT_SRC)
	$(CC) $(CFLAGS_OBJ_PIC_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/process.o.pic: $(PROCESS_SRC)
	$(CC) $(CFLAGS_OBJ_PIC_DEBUG) -o $@ $<

$(BUILD_TARGET_DEBUG)/mceliece.o.pic: $(MCELIECE_SRC)
	$(CC) $(CFLAGS_OBJ_PIC_DEBUG) -o $@ $<

$(SHARED_TARGET_DEBUG)/libbpumcelieced.so: $(BUILD_TARGET_DEBUG)/mceliece.o.pic $(BUILD_TARGET_DEBUG)/process.o.pic $(BUILD_TARGET_DEBUG)/init.o.pic $(BUILD_TARGET_DEBUG)/operations.o.pic $(BUILD_TARGET_DEBUG)/debugio.o.pic $(BUILD_TARGET_DEBUG)/arithmetics.o.pic
	$(CC) $(CFLAGS_SHARED_DEBUG) -o $@ $^

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
	@echo "This is a McEliece library, you can build static or shared library."
	@echo ""
	@echo "To build a static library use: \n\t'make static-lib'"
	@echo "Or if you want debug mode, use: \n\t'make static-lib-debug'"
	@echo "To build a shared library use: \n\t'make shared-lib'"
	@echo "Or if you want debug mode, use: \n\t'make shared-lib-debug'"
	@echo ""
	@echo "Default build makes static library with test binary:"
	@echo "\tSource file: $(CWD)/$(PREFIX_SRC)/main.c"
	@echo "\tApplication: $(TEST_TARGET)/main-static"
	@echo "\tLibrary: $(STATIC_TARGET)/libbpumceliece.a"
	@echo ""
	@echo "It will build files 'libbpumceliece.so' or 'libbpumceliece.a' or for debug 'libbpumcelieced.so' or 'libbpumcelieced.a', then you can link it to your project."
	@echo "Output will be in dist/static/ or dist/shared/ or dist-debug/static-debug/ or dist/shared-debug/"
	@echo ""
	@echo "You can also build a test run: \n\t'make test-*'\nWhere * can be static, shared, static-debug, shared-debug, output will be in dist/test/ or dist-debug/test/."