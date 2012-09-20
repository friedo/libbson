#include <stdio.h>
#include "libbson.h"

void handle_bson_string( char *name, void *value ) { 
  printf( "got name %s\n", name );
  printf( "got value %s\n", (char *)value );
}

int main() { 
  bson_register_handler( BSON_STRING, handle_bson_string );
  bson_decode( "this is a buffer", 42 );
}

