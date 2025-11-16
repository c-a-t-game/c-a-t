#define OBJ_INT_FIELD(offset) data.as_int[offset]
#define OBJ_FLT_FIELD(offset) data.as_flt[offset]
#define OBJ_PTR_FIELD(offset) data.as_ptr[offset]

#define oTouchingGround OBJ_INT_FIELD(0x00)