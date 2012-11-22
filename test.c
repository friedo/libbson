#include <stdio.h>
#include <stdlib.h>
#include "libbson.h"

void handle_bson_string( char *name, void *str_ptr ) { 
  bson_string_t *str = (bson_string_t *)str_ptr;
  printf( "string:\n" );
  printf( "\t\"%s\" = \"%s\" [len: %d]\n", name, str->value, str->length.value );
}

void handle_bson_binary( char *name, void *bin_ptr ) { 
  bson_binary_t *bin = (bson_binary_t *)bin_ptr;
  printf( "binary:\n" );
  printf( "\t\"%s\" = \"%.*s\" [len: %d]\n", name, bin->length.value, bin->data, bin->length.value );
}

void handle_bson_double( char *name, void *flt_ptr ) { 
  bson_double_t *flt = (bson_double_t *)flt_ptr;
  printf( "double:\n" );
  printf( "\t\"%s\" = \%f\n", name, flt->value );
}

void handle_bson_int32( char *name, void *num_ptr ) { 
  bson_int32_t *num = (bson_int32_t *)num_ptr;
  printf( "int32:\n" );
  printf( "\t\"%s\" = %d\n", name, num->value );
}

void handle_bson_document( char *name, void *len_ptr ) { 
  bson_int32_t *len = (bson_int32_t *)len_ptr;

  if ( len->value ) { 
    printf( "document:\n" );
    printf( "\t[len: %d], \"%s\" = {\n", len->value, name );
  } else { 
    printf( "} end of document\n" );
  }
}


void handle_bson_array( char *name, void *len_ptr ) { 
  bson_int32_t *len = (bson_int32_t *)len_ptr;

  if ( len->value ) { 
    printf( "array:\n" );
    printf( "\t[len: %d], \"%s\" = [\n", len->value, name );
  } else { 
    printf( "] end of array\n" );
  }
}

void handle_bson_bool( char *name, void *val_ptr ) { 
  char *val = (char *)val_ptr;

  if ( *val == 0 ) { 
    printf( "\"%s\": true\n", name );
  } else { 
    printf( "\"%s\": false\n", name );
  }
}

void handle_bson_oid( char *name, void *oid_ptr ) { 
  bson_object_id_t *oid = (bson_object_id_t *)oid_ptr;

  printf( "\"%s\": ObjectId(", name );

  int i;

  // is there a better way to do this? :/
  for ( i = 0; i < 11; i++ ) { 
    printf( "%02hhx", (int)oid->value[i] );
  }

  printf( ")\n" );
}

int main() { 
  bson_register_handler( BSON_STRING,    handle_bson_string );
  bson_register_handler( BSON_BINARY,    handle_bson_binary );
  bson_register_handler( BSON_DOUBLE,    handle_bson_double );
  bson_register_handler( BSON_INT32,     handle_bson_int32  );
  bson_register_handler( BSON_DOCUMENT,  handle_bson_document );
  bson_register_handler( BSON_ARRAY,     handle_bson_array );
  bson_register_handler( BSON_OBJECT_ID, handle_bson_oid );
  bson_register_handler( BSON_BOOLEAN,   handle_bson_bool );

  /* read test data */
  long f_size;
  size_t bson_size;
  FILE *test_file = fopen( "test2.bson", "r" );
  fseek( test_file, 0, SEEK_END );
  bson_size = ftell( test_file );

  char *bson = malloc( sizeof(char) * bson_size );
  fseek( test_file, 0, SEEK_SET );
  int res = fread( bson, 1, bson_size, test_file );

  bson_decode( bson );
}

