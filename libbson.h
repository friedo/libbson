/* libbson headers */

const int BSON_DOUBLE            = 0x01;
const int BSON_STRING            = 0x02;
const int BSON_DOCUMENT          = 0x03;
const int BSON_ARRAY             = 0x04;
const int BSON_BINRARY           = 0x05;
const int BSON_UNDEFINED         = 0x06;
const int BSON_OBJECT_ID         = 0x07;
const int BSON_BOOLEAN           = 0x08;
const int BSON_DATETIME          = 0x09;
const int BSON_NULL              = 0x0A;
const int BSON_REGEXP            = 0x0B;
const int BSON_DB_POINTER        = 0x0C;
const int BSON_JAVASCRIPT        = 0x0D;
const int BSON_SYMBOL            = 0x0E;
const int BSON_JAVASCRIPT_SCOPE  = 0x0F;
const int BSON_INT32             = 0x10;
const int BSON_TIMESTAMP         = 0x11;
const int BSON_INT64             = 0x12;
const int BSON_MINIMUM           = 0xFF;
const int BSON_MAXIMUM           = 0x7F;

extern void bson_register_handler( int type, void *func );
extern void bson_decode( char *buf );

/* typedefs for bson things. compound elements like 
   documents and arrays, and valueless elements like
   null, undef, and min/max, don't have types. */
typedef double     bson_double_t;
typedef struct { 
  int length;
  char *str;
}                  bson_string_t;
typedef struct { 
  int length;
  char *data;
}                  bson_binary_t;
typedef char *     bson_object_id_t;
typedef int        bson_bool_t;
typedef int        bson_datetime_t;
typedef struct { 
  char *pattern;
  char *flags;
}                  bson_regexp_t;
typedef struct {     
  char *namespace;
  bson_object_id_t *object_id;
}                  bson_db_pointer_t;
typedef struct {
  int length;
  char *code;
}                  bson_js_t;
typedef struct { 
  int length;
  char *sym;
}                  bson_symbol_t;
typedef struct { 
  int length;
  char *code;
}                  bson_js_w_s_t;
typedef int        bson_int32_t;
typedef int        bson_timestamp_t;
typedef int        bson_int64_t;


/* for deserialization support */
typedef union { 
  double dbl;
  char bytes[8];
} _dbl_tmp;
