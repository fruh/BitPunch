BitPunch
========

Goal of project is to create standalone McEliece implemetation in C language.

Project is now only in *development version*.

### Version 0.0.4:
  - see [CHANGES.txt](CHANGES.txt)

### Documentation
  - clone repo and see doc/ or visit http://fruh.github.io/BitPunch

### Build:
  - go to `cd lib` folder
  - type make
    - it will build default aplication which is target *test*
  
  Now you can build test aplication, realease or debug, you can specify wheter you want to use precomputed H matrix (more memory) or not:
  - make help - it is make help
  - make print-info - info where is your aplication
  - make test - with H matrix
  - make test-debug - with H matrix
  - make test-wo-h - without H matrix
  - make test-with-debug - without H matrix
  - make static - to build static lib
  - make shared - to build shared lib

  Builded libraries are located in dist/ folder. For more preprocesor options see [lib/src/bitpunch/config.h](lib/src/bitpunch/config.h) file.
  Default optimization is -O2. To set up more verbosity define -DERRORL, -DWARNING_L or DDEBUG_L, this are using functions like BPU_printError, BPU_printWarning, BPU_printDebug.

### Example:
  - see [lib/src/main.c](lib/src/main.c) for example of usage keygen, encryption and decryption
