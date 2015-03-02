BitPunch
========

Goal of project is to create standalone McEliece implemetation in C language.

Project is now only in *development version*.

### Documentation
  - clone repo and see doc/ or visit http://fruh.github.io/BitPunch

### Build:
  - go to `cd lib` folder
  - type make
    - it will build default aplication whic is target test-with-h
  Now you can build test aplication, realease or debug, you can specify wheter you want to use precomputed H matrix (more memory) or not:

    Targets:
      - make help - it is make help
      - make print-info - info where is your aplication
      - make test-wo-h - without precomputed H
      - make test-wo-h-debug - without precomputed H
      - make test-with - with precomputed H
      - make test-with-debug - with precomputed H
	
  Default optimization is -O2. To set up more verbosity define -DERRORL, -DWARNING_L or DDEBUG_L, this are using functions like BPU_printError, BPU_printWarning, BPU_printDebug.
### Example:
  - see [lib/src/main.c](lib/src/main.c) for example of usage keygen, encryption and decryption
