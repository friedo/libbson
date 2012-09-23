/* libbson */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libbson.h"

int (*type_handlers[255])( char *name, void *value ) = {NULL};
void do_decode( char *buf, int type );

extern void bson_register_handler( int type, void *func ) { 
  type_handlers[ type ] = func;
}

extern void bson_decode( char *buf ) { 
  do_decode( buf, 0 );
}

#define extract_lstring( buf, str_struct )                   \
  extract_int32( buf, &(str_struct.length) );                \
  /* null terminated, so we can just give a pointer to the   \
     beginning of the string. */                             \
  printf( "lstring size is %i\n", str_struct.length );       \
  str_struct.str = buf;                                      \
  buf += str_struct.length;

/* four bytes, little endian */
#define extract_int32( buf, value ) \
  *value = buf[0] | buf[1] <<8 | buf[2] << 16 | buf[3] << 24;  \
  buf += 4;



void do_decode( char *buf, int type ) { 
  /* document length, first 4 bytes, little endian */
  int doc_size;
  printf( "buffer head is %p\n", buf );
  extract_int32( buf, &doc_size );
  printf( "doc size is %d\n", doc_size ); 
  printf( "buffer head is %p\n", buf );

  while ( buf < ( buf + doc_size ) ) { 
    /* element type ID */
    int elem_type = buf[0];
    buf += 1;

    /* key name, cstring */
    char *key_name = buf;
    buf += strlen(key_name) + 1;

    printf( "got key name %s, elem type %x\n", key_name, elem_type );

    if ( elem_type == BSON_STRING ) { 
      bson_string_t string;
      extract_lstring( buf, string );
      printf( "\tgot string [%s], length [%d]\n", string.str, string.length );
    } else if ( elem_type == BSON_INT32 ) {
      int int32;
      extract_int32( buf, &int32 );
      printf( "\tgot int32 [%i]\n", int32 );
    } else { 
      fprintf( stderr, "Can't handle elem type %x\n", elem_type );
      exit(1);
    }

  }
}
