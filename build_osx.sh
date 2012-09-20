gcc -c -fno-common libbson.c
gcc -dynamiclib -o libbson.dylib -dylib libbson.o
gcc -L./ -o test -lbson test.c
