/* libbson */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libbson.h"

#define CHECK_HANDLER(NAME) \
if ( type_handlers[ NAME ] == NULL ) { \
  fprintf( stderr, "libbson: no type handler registered for " #NAME "\n" );   \
  exit(1); \
}

int (*type_handlers[255])( char *name, void *value ) = {NULL};
void do_decode( char *buf, int type );

extern void bson_register_handler( int type, void *func ) { 
  type_handlers[ type ] = func;
}

extern void bson_decode( char *buf ) { 
  do_decode( buf, 0 );
}

bson_int32_t extract_int32( char *buf ) { 
  bson_int32_t val = { .value = 0 };

  int i;
  for( i = 0; i <= 3; i++ ) { 
    val._bytes[i] = buf[i];
  }

  return val;
}

bson_int64_t extract_int64( char *buf ) { 
  bson_int64_t val = { .value = 0 };

  int i;
  for ( i = 0; i <= 7; i++ ) { 
    val._bytes[i] = buf[i];
  }

  return val;
}

bson_double_t extract_double( char *buf ) { 
  bson_double_t val = { .value = 0 };

  /* eight bytes, little endian */
  int i;
  for( i = 0; i <= 7; i++ ) {
    val._bytes[i] = buf[i];
  }

  return val;
}

bson_string_t extract_string( char *buf ) { 
  bson_string_t str = { .value = NULL };
  str.length = extract_int32( buf );
  str.value = buf + 4;
  return str;
}

bson_binary_t extract_binary( char *buf ) { 
  bson_binary_t blob = { .data = NULL };
  blob.length  = extract_int32( buf );
  blob.subtype = buf[4];
  blob.data    = buf + 5;
  return blob;
}


bson_object_id_t extract_oid( char *buf ) { 
  bson_object_id_t oid;
  
  int i;
  for ( i = 0; i <= 11; i++ ) { 
    oid.value[i] = buf[i];
  }

  return oid;
}

void do_decode( char *buf, int type ) { 
  /* document length, first 4 bytes, little endian */
  bson_int32_t doc_size = extract_int32( buf );
  char *buf_end = buf + doc_size.value;

  /* advance pointer to beginning of data */
  buf += 4;

  while ( buf < buf_end ) { 
    /* read type byte */
    char elem_type = buf[0];
    buf++;

    char *elem_name = buf;    // null terminated
    buf += strlen( elem_name ) + 1;
    if ( elem_type == BSON_DOUBLE ) { 
      CHECK_HANDLER(BSON_DOUBLE);

      bson_double_t flt = extract_double( buf );
      type_handlers[ BSON_DOUBLE ]( elem_name, &flt );
      buf += 8;
    } else if ( elem_type == BSON_STRING ) { 
      CHECK_HANDLER(BSON_STRING);

      bson_string_t str = extract_string( buf );
      // printf( "string name %s, length %d, value %s\n", elem_name, str.length.value, str.value );
        
      type_handlers[ BSON_STRING ]( elem_name, &str );
      buf += str.length.value + 1;
    } else if ( elem_type == BSON_DOCUMENT ) { 
      CHECK_HANDLER(BSON_DOCUMENT);
      bson_int32_t emb_doc_size = extract_int32( buf );
      type_handlers[ BSON_DOCUMENT ]( elem_name, &emb_doc_size ); // open new doc

      do_decode( buf, 0 );

      bson_int32_t flag = { .value = 0 };
      type_handlers[ BSON_DOCUMENT ]( elem_name, &flag ); // close new doc
      buf += emb_doc_size.value;
    } else if ( elem_type == BSON_ARRAY ) { 
      CHECK_HANDLER(BSON_ARRAY);
      bson_int32_t emb_arr_size = extract_int32( buf );
      type_handlers[ BSON_ARRAY ]( elem_name, &emb_arr_size ); // open new array

      do_decode( buf, 1 );

      bson_int32_t flag = { .value = 0 };
      type_handlers[ BSON_ARRAY ]( elem_name, &flag ); // close new array
      buf += emb_arr_size.value;
 
    } else if ( elem_type == BSON_BINARY ) { 
      CHECK_HANDLER(BSON_BINARY);
      
      bson_binary_t blob = extract_binary( buf );
      type_handlers[ BSON_BINARY ]( elem_name, &blob );
      buf += blob.length.value + 5;
    } else if ( elem_type == BSON_INT32 ) { 
      CHECK_HANDLER(BSON_INT32);

      bson_int32_t num = extract_int32( buf );
      type_handlers[ BSON_INT32 ]( elem_name, &num );
      buf += 4;
    } else if ( elem_type == BSON_INT64 ) { 
      CHECK_HANDLER(BSON_INT64);

      bson_int64_t num = extract_int64( buf );
      type_handlers[ BSON_INT64 ]( elem_name, &num );
      buf += 8;
    } else if ( elem_type == BSON_OBJECT_ID ) { 
      CHECK_HANDLER(BSON_OBJECT_ID);
      
      bson_object_id_t oid = extract_oid( buf );
      type_handlers[ BSON_OBJECT_ID ]( elem_name, &oid );
      buf += 12;
    } else if ( elem_type == BSON_BOOLEAN ) { 
      CHECK_HANDLER(BSON_BOOLEAN);
      char val = buf[0];
      type_handlers[ BSON_BOOLEAN ]( elem_name, &val );
      buf++;
      
    } else { 
      //fprintf( stderr, "libbson: unknown type number %d\n", (int)elem_type );
      //exit(2);
    }
  }
}
