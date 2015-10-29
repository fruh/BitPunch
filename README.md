BitPunch
========

The goal of the project is to create a standalone implementation of the McEliece cryptosystem in C language.

Project is now only in *development version*.

### Version 0.0.4:
  - see [CHANGES.txt](CHANGES.txt)

### Documentation
  - clone repo and see doc/ or visit http://fruh.github.io/BitPunch

### Build:
  - go to `cd lib` folder
  - type make
    - it builds default application which is target *test*
  
  Now you can build test aplication in realease or debug mode, you can specify whether you want to use precomputed H  matrix (parity-check matrix - requires more memory) or not:
  - make help - it is make help
  - make print-info - info about where your aplication is
  - make test - with H matrix
  - make test-debug - with H matrix
  - make test-wo-h - without H matrix
  - make test-wo-debug - without H matrix
  - make static - to build static lib
  - make shared - to build shared lib

  Built libraries are located in dist/ folder. For more preprocessor options see [lib/src/bitpunch/config.h](lib/src/bitpunch/config.h) file.
  Default optimization is -O2. To set up more verbosity define -DERRORL, -DWARNING_L or DDEBUG_L - these are using functions like BPU_printError, BPU_printWarning, BPU_printDebug.

### Example:
  - examples of keygen, encryption and decryption can be found in [lib/src/main.c](lib/src/main.c) 
