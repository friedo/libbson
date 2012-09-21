/* libbson */

#include <stdlib.h>
#include <stdio.h>
#include "libbson.h"

int (*type_handlers[255])( char *name, void *value ) = {NULL};

extern void bson_register_handler( int type, void *func ) { 
  type_handlers[ type ] = func;
}

extern void bson_decode( char *buf ) { 
  do_decode( buf, 0 );
}

void do_decode( char *buf, int type ) { 
  /* document length, first 4 bytes, little endian */
  int doc_size = ( buf[0] | ( buf[1] << 8 | ( buf[2] << 16 ) | ( buf[3] << 24 ) ) );
  buf += 4;
  
  while ( buf < ( buf + doc_size ) ) { 
    /* element type ID */
    int elem_type = buf[0];
    buf += 1;

    /* key name, cstring */
    char *key_name = buf;
    buf += (int)(strlen(key_name));

    /* if it's a type with a single value, extract it
       and get a pointer */
    void *value = NULL;
    switch( elem_type ) { 
    case BSON_DOUBLE:
      value = extract_float( buf );
      break;
    case BSON_STRING:
      value = extract_lstring( buf );
      break;
    case BSON_BINARY:
      value = extract_binary( buf );
      break;
    case BSON_OBJECT_ID:
      value = extract_bytes( buf, 12 );
      break;
    case BSON_BOOLEAN:
      value = extract_bytes( buf, 1 );
      break;
    case BSON_DATETIME:
      value = extract_int64( buf );
      break;
    case BSON_REGEXP:
      value = extract_regexp( buf );
      break;
    case BSON_DB_POINTER:
      value = extract_bytes( buf, 12 );
      break;
    case BSON_JAVASCRIPT:
      value = extract_lstring( buf );
      break;
    case BSON_SYMBOL:
      value = extract_lstring( buf );
      break;
    case BSON_JAVASCRIPT_SCOPE:
      value = extract_js_scope( buf );
      break;
    case BSON_INT32:
      value = extract_int32( buf );
      break;
    case BSON_TIMESTAMP:
      value = extract_int64( buf );
      break;
    case BSON_INT64:
      value = extract_int64( buf );
      break;
    }
      
    if ( type_handlers[ elem_type ] == NULL ) { 
      fprintf( stderr, "libbson: no type handler for BSON type %x\n" );
      exit(1);
    }

    type_handlers[ elem_type ]( key_name, value );
}
