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
#define extract_int32( buf, value )                               \
  *value =   buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;  \
  buf += 4;


#define extract_double( buf, value )                             \
  _dbl_tmp data;                                                 \
  data.bytes[0] = buf[0];                                        \
  data.bytes[1] = buf[1];                                        \
  data.bytes[2] = buf[2];                                        \
  data.bytes[3] = buf[3];                                        \
  data.bytes[4] = buf[4];                                        \
  data.bytes[5] = buf[5];                                        \
  data.bytes[6] = buf[6];                                        \
  data.bytes[7] = buf[7];                                        \
  *value = data.dbl;                                             \
  buf += 8;

#define extract_binary( buf, bin_struct )                        \
  extract_int32( buf, &(bin_struct.length) );                    \
  bin_struct.subtype = (int)buf[0]; buf++;                       \
  bin_struct.data    = buf;                                      \
  buf += bin_struct.length;

#define extract_oid( buf, oid )                                  \
  char[13] object_id;                                            \
  object_id[13] = '\0';                                          \
  strncpy( object_id, buf, 12 );                                 \
  *oid = object_id;                                              \
  buf += 12;                                                     \


void do_decode( char *buf, int type ) { 
  /* document length, first 4 bytes, little endian */
  int doc_size;
  printf( "buffer head is %p\n", buf );
  extract_int32( buf, &doc_size );
  char *buf_end = buf + doc_size;
  printf( "doc size is %d, end at %p\n", doc_size, buf_end ); 
  printf( "buffer head is %p\n", buf );

  while ( buf < buf_end ) { 
    printf( "buffer head is %p\n", buf );   
    /* element type ID */
    int elem_type = buf[0];
    buf += 1;

    /* key name, cstring */
    char *key_name = buf;
    buf += strlen(key_name) + 1;

    printf( "got key name %s, elem type %x\n", key_name, elem_type );

    if ( elem_type == BSON_DOUBLE ) { 
      bson_double_t dbl;
      extract_double( buf, &dbl );
      printf( "\tgot float [%f]\n", dbl );
    } else if ( elem_type == BSON_STRING ) { 
      bson_string_t string;
      extract_lstring( buf, string );
      printf( "\tgot string [%s], length [%d]\n", string.str, string.length );
    } else if ( elem_type == BSON_BINRARY ) {
      bson_binary_t bin;
      extract_binary( buf, bin );
      char *test = malloc( bin.length + 1 );
      test[ bin.length + 1 ] = '\0';
      strncpy( test, bin.data, bin.length );
      printf( "\tgot length [%d], type [%d], data [%s]\n", bin.length, bin.subtype, test );
      free( test );
    } else if ( elem_type == BSON_OBJECT_ID ) { 
      bson_object_id_t oid;
      extract_oid( buf, &oid );
      printf( "\tgot OID 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", oid );
    } else if ( elem_type == BSON_INT32 ) {
      int int32;
      extract_int32( buf, &int32 );
      printf( "\tgot int32 [%i]\n", int32 );
    } else if ( elem_type == 0 ) { 
      buf++;
    } else { 
      fprintf( stderr, "Can't handle elem type %x\n", elem_type );
      exit(1);
    }

  }
}
