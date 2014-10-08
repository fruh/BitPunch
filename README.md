BitPunch
========

Goal of project is to create standalone McEliece implemetation in C.

Project is now only in development version.

External dependencies:
  There is now only dependency to openssl library (it uses hash function)

Build:
  - go to 'lib' folder
  - type 'make' to build static library with example of (keygen, enc, dec)
    - see lib/src/main.c for details
  - see makefile help, type 'make help' for further build options

