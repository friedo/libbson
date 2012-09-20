/* libbson */

#include <stdlib.h>
#include <stdio.h>
#include "libbson.h"

int (*type_handlers[255])( char *name, void *value ) = {NULL};

extern void bson_register_handler( int type, void *func ) { 
  type_handlers[ type ] = func;
}

extern void bson_decode( char *buf, int length ) { 
  /* for testing, just call the func for strings */
  if ( type_handlers[ BSON_STRING ] == NULL ) { 
    fprintf( stderr, "No callback registered for type\n" );
    exit(1);
  }

  type_handlers[ BSON_STRING ]( "this is a name", "this is a value" );
}
