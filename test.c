#include <stdio.h>
#include <stdlib.h>
#include "libbson.h"

void handle_bson_string( char *name, void *value ) { 
  printf( "got name %s\n", name );
  printf( "got value %s\n", (char *)value );
}

int main() { 
  bson_register_handler( BSON_STRING, handle_bson_string );

  /* read test data */
  long f_size;
  size_t bson_size;
  FILE *test_file = fopen( "test.bson", "r" );
  fseek( test_file, 0, SEEK_END );
  bson_size = ftell( test_file );

  char *bson = malloc( sizeof(char) * bson_size );
  fseek( test_file, 0, SEEK_SET );
  int res = fread( bson, 1, bson_size, test_file );

  bson_decode( bson );
}

