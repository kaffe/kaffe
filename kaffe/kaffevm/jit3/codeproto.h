/* codeproto.h
 * Code prototypes.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __codeproto_h
#define __codeproto_h

#include "config.h"

struct Hjava_lang_Class;

void _start_basic_block(void);
void _end_basic_block(void);
void _start_sub_block(void);
void _end_sub_block(void);
void _start_instruction(uintp);
void _start_exception_block(uintp);
void finish_function(void);
void check_stack_limit(void);
void begin_sync(void);
void end_sync(void);
void begin_func_sync(void);
void end_func_sync(void);

void move_any(SlotInfo*, SlotInfo*);
void move_anylong(SlotInfo*, SlotInfo*);

void move_int_const(SlotInfo*, jint);
void move_ref_const(SlotInfo*, void*);
void move_long_const(SlotInfo*, jlong);
void move_float_const(SlotInfo*, jfloat);
void move_double_const(SlotInfo*, jdouble);
void move_label_const(SlotInfo*, label*);
void move_string_const(SlotInfo*, void*);

void move_int(SlotInfo*, SlotInfo*);
void move_ref(SlotInfo*, SlotInfo*);
void move_long(SlotInfo*, SlotInfo*);
void move_float(SlotInfo*, SlotInfo*);
void move_double(SlotInfo*, SlotInfo*);

void add_int(SlotInfo*, SlotInfo*, SlotInfo*);
void sub_int(SlotInfo*, SlotInfo*, SlotInfo*);
void mul_int(SlotInfo*, SlotInfo*, SlotInfo*);
void div_int(SlotInfo*, SlotInfo*, SlotInfo*);
void rem_int(SlotInfo*, SlotInfo*, SlotInfo*);
void neg_int(SlotInfo*, SlotInfo*);
void and_int(SlotInfo*, SlotInfo*, SlotInfo*);
void or_int(SlotInfo*, SlotInfo*, SlotInfo*);
void nor_int(SlotInfo*, SlotInfo*, SlotInfo*);
void xor_int(SlotInfo*, SlotInfo*, SlotInfo*);
void ashr_int(SlotInfo*, SlotInfo*, SlotInfo*);
void lshr_int(SlotInfo*, SlotInfo*, SlotInfo*);
void lshl_int(SlotInfo*, SlotInfo*, SlotInfo*);
void add_int_const(SlotInfo*, SlotInfo*, jint);
void sub_int_const(SlotInfo*, SlotInfo*, jint);
void lshl_int_const(SlotInfo*, SlotInfo*, jint);
void cmp_int(SlotInfo*, SlotInfo*, SlotInfo*);
void cmp_offset_int(SlotInfo*, SlotInfo*, SlotInfo*, jint);
void cmp_int_const(SlotInfo*, SlotInfo*, jint);
void or_int_const(SlotInfo*, SlotInfo*, jint);
void xor_int_const(SlotInfo*, SlotInfo*, jint);
void and_int_const(SlotInfo*, SlotInfo*, jint);
void mul_int_const(SlotInfo*, SlotInfo*, jint);
void div_int_const(SlotInfo*, SlotInfo*, jint);
void rem_int_const(SlotInfo*, SlotInfo*, jint);
void ashr_int_const(SlotInfo*, SlotInfo*, jint);
void lshr_int_const(SlotInfo*, SlotInfo*, jint);

void swap_any(SlotInfo*, SlotInfo*);

void add_ref(SlotInfo*, SlotInfo*, SlotInfo*);
void add_ref_const(SlotInfo*, SlotInfo*, jint);

void cmp_ref(SlotInfo*, SlotInfo*, SlotInfo*);
void cmp_ref_const(SlotInfo*, SlotInfo*, void*);

void add_long(SlotInfo*, SlotInfo*, SlotInfo*);
void sub_long(SlotInfo*, SlotInfo*, SlotInfo*);
void mul_long(SlotInfo*, SlotInfo*, SlotInfo*);
void div_long(SlotInfo*, SlotInfo*, SlotInfo*);
void rem_long(SlotInfo*, SlotInfo*, SlotInfo*);
void neg_long(SlotInfo*, SlotInfo*);
void lshl_long(SlotInfo*, SlotInfo*, SlotInfo*);
void ashr_long(SlotInfo*, SlotInfo*, SlotInfo*);
void lshr_long(SlotInfo*, SlotInfo*, SlotInfo*);
void and_long(SlotInfo*, SlotInfo*, SlotInfo*);
void or_long(SlotInfo*, SlotInfo*, SlotInfo*);
void xor_long(SlotInfo*, SlotInfo*, SlotInfo*);

void add_float(SlotInfo*, SlotInfo*, SlotInfo*);
void sub_float(SlotInfo*, SlotInfo*, SlotInfo*);
void mul_float(SlotInfo*, SlotInfo*, SlotInfo*);
void div_float(SlotInfo*, SlotInfo*, SlotInfo*);
void rem_float(SlotInfo*, SlotInfo*, SlotInfo*);
void neg_float(SlotInfo*, SlotInfo*);

void add_double(SlotInfo*, SlotInfo*, SlotInfo*);
void sub_double(SlotInfo*, SlotInfo*, SlotInfo*);
void mul_double(SlotInfo*, SlotInfo*, SlotInfo*);
void div_double(SlotInfo*, SlotInfo*, SlotInfo*);
void rem_double(SlotInfo*, SlotInfo*, SlotInfo*);
void neg_double(SlotInfo*, SlotInfo*);

void pusharg_int(SlotInfo*, int);
void pusharg_int_const(jint, int);
void pusharg_ref(SlotInfo*, int);
void pusharg_ref_const(void*, int);
void pusharg_long(SlotInfo*, int);
void pusharg_long_const(jlong, int);
void pusharg_float(SlotInfo*, int);
void pusharg_double(SlotInfo*, int);
void pusharg_string_const(void*, int);
void pusharg_class_const(struct Hjava_lang_Class*, int);
void pusharg_utf8_const(Utf8Const*, int);
void popargs_internal(int does_return);
#define popargs() popargs_internal(1)
#define popargs_noreturn() popargs_internal(0)

void return_int(SlotInfo*);
void return_ref(SlotInfo*);
void return_long(SlotInfo*);
void return_float(SlotInfo*);
void return_double(SlotInfo*);

void returnarg_int(SlotInfo*);
void returnarg_ref(SlotInfo*);
void returnarg_long(SlotInfo*);
void returnarg_float(SlotInfo*);
void returnarg_double(SlotInfo*);

void pop_slot(SlotInfo*, int len);

void call(SlotInfo*);
void call_indirect_method(Method*);
void call_soft(void*);
void ret(void);
void exit_method(void);

label* build_code_ref(uint8*, uintp);
label* reference_code_label(uintp);
label* reference_table_label(int32);
label* reference_label(int32, int32);
SlotInfo* stored_code_label(SlotInfo*);
SlotInfo* table_code_label(SlotInfo*);
void set_label(int, int);
void load_code_ref(SlotInfo*, SlotInfo*);
void load_key(SlotInfo*, SlotInfo*);
void build_key(uint8*);

void prologue(Method*);
void mon_enter(Method*, SlotInfo*);
void mon_exit(Method*, SlotInfo*);
void epilogue(Method*);
void softcall_breakpoint(void);

void load_int(SlotInfo*, SlotInfo*);
void load_ref(SlotInfo*, SlotInfo*);
void load_long(SlotInfo*, SlotInfo*);
void load_float(SlotInfo*, SlotInfo*);
void load_double(SlotInfo*, SlotInfo*);
void load_byte(SlotInfo*, SlotInfo*);
void load_char(SlotInfo*, SlotInfo*);
void load_short(SlotInfo*, SlotInfo*);

void store_int(SlotInfo*, SlotInfo*);
void store_ref(SlotInfo*, SlotInfo*);
void store_long(SlotInfo*, SlotInfo*);
void store_float(SlotInfo*, SlotInfo*);
void store_double(SlotInfo*, SlotInfo*);
void store_byte(SlotInfo*, SlotInfo*);
void store_char(SlotInfo*, SlotInfo*);
void store_short(SlotInfo*, SlotInfo*);

void load_addr_int(SlotInfo*, void*);
void load_addr_ref(SlotInfo*, void*);
void load_addr_long(SlotInfo*, void*);
void load_addr_float(SlotInfo*, void*);
void load_addr_double(SlotInfo*, void*);
void load_addr_byte(SlotInfo*, void*);
void load_addr_char(SlotInfo*, void*);
void load_addr_short(SlotInfo*, void*);

void store_addr_int(void*, SlotInfo*);
void store_addr_ref(void*, SlotInfo*);
void store_addr_long(void*, SlotInfo*);
void store_addr_float(void*, SlotInfo*);
void store_addr_double(void*, SlotInfo*);
void store_addr_byte(void*, SlotInfo*);
void store_addr_char(void*, SlotInfo*);
void store_addr_short(void*, SlotInfo*);

void spill_int(SlotData*);
void spill_ref(SlotData*);
void spill_long(SlotData*);
void spill_float(SlotData*);
void spill_double(SlotData*);

void reload_int(SlotData*);
void reload_ref(SlotData*);
void reload_long(SlotData*);
void reload_float(SlotData*);
void reload_double(SlotData*);

void load_offset_int(SlotInfo*, SlotInfo*, jint);
void store_offset_int(SlotInfo*, jint, SlotInfo*);
void load_offset_ref(SlotInfo*, SlotInfo*, jint);
void store_offset_ref(SlotInfo*, jint, SlotInfo*);
void load_offset_short(SlotInfo*, SlotInfo*, jint);
void store_offset_short(SlotInfo*, jint, SlotInfo*);
void load_offset_byte(SlotInfo*, SlotInfo*, jint);
void store_offset_byte(SlotInfo*, jint, SlotInfo*);
void load_offset_char(SlotInfo*, SlotInfo*, jint);
void store_offset_char(SlotInfo*, jint, SlotInfo*);
void load_offset_float(SlotInfo*, SlotInfo*, jint);
void store_offset_float(SlotInfo*, jint, SlotInfo*);
void load_offset_double(SlotInfo*, SlotInfo*, jint);
void store_offset_double(SlotInfo*, jint, SlotInfo*);
void load_offset_long(SlotInfo*, SlotInfo*, jint);
void store_offset_long(SlotInfo*, jint, SlotInfo*);

void load_offset_scaled_int(SlotInfo*, SlotInfo*, SlotInfo*, jint);
void store_offset_scaled_int(SlotInfo*, SlotInfo*, jint, SlotInfo*);
void load_offset_scaled_ref(SlotInfo*, SlotInfo*, SlotInfo*, jint);
void store_offset_scaled_ref(SlotInfo*, SlotInfo*, jint, SlotInfo*);
void load_offset_scaled_long(SlotInfo*, SlotInfo*, SlotInfo*, jint);
void store_offset_scaled_long(SlotInfo*, SlotInfo*, jint, SlotInfo*);
void load_offset_scaled_float(SlotInfo*, SlotInfo*, SlotInfo*, jint);
void store_offset_scaled_float(SlotInfo*, SlotInfo*, jint, SlotInfo*);
void load_offset_scaled_double(SlotInfo*, SlotInfo*, SlotInfo*, jint);
void store_offset_scaled_double(SlotInfo*, SlotInfo*, jint, SlotInfo*);
void load_offset_scaled_short(SlotInfo*, SlotInfo*, SlotInfo*, jint);
void store_offset_scaled_short(SlotInfo*, SlotInfo*, jint, SlotInfo*);
void load_offset_scaled_byte(SlotInfo*, SlotInfo*, SlotInfo*, jint);
void store_offset_scaled_byte(SlotInfo*, SlotInfo*, jint, SlotInfo*);
void load_offset_scaled_char(SlotInfo*, SlotInfo*, SlotInfo*, jint);
void store_offset_scaled_char(SlotInfo*, SlotInfo*, jint, SlotInfo*);

void store_const_offset_int(SlotInfo*, jint, jint);
void store_const_offset_byte(SlotInfo*, jint, jint);
void store_const_offset_scaled_byte(SlotInfo*, SlotInfo*, jint, jint);

void cbranch_int(SlotInfo*, SlotInfo*, label*, int);
void cbranch_int_const(SlotInfo*, jint, label*, int);
void cbranch_ref(SlotInfo*, SlotInfo*, label*, int);
void cbranch_ref_const(SlotInfo*, void*, label*, int);
void branch(label*, int);
void branch_indirect(SlotInfo*);

void cvt_int_long(SlotInfo*, SlotInfo*);
void cvt_int_float(SlotInfo*, SlotInfo*);
void cvt_int_double(SlotInfo*, SlotInfo*);
void cvt_long_int(SlotInfo*, SlotInfo*);
void cvt_long_float(SlotInfo*, SlotInfo*);
void cvt_long_double(SlotInfo*, SlotInfo*);
void cvt_float_int(SlotInfo*, SlotInfo*);
void cvt_float_long(SlotInfo*, SlotInfo*);
void cvt_float_double(SlotInfo*, SlotInfo*);
void cvt_double_int(SlotInfo*, SlotInfo*);
void cvt_double_long(SlotInfo*, SlotInfo*);
void cvt_double_float(SlotInfo*, SlotInfo*);
void cvt_int_byte(SlotInfo*, SlotInfo*);
void cvt_int_char(SlotInfo*, SlotInfo*);
void cvt_int_short(SlotInfo*, SlotInfo*);

void lcmp(SlotInfo*, SlotInfo*, SlotInfo*);
void cmpl_float(SlotInfo*, SlotInfo*, SlotInfo*);
void cmpg_float(SlotInfo*, SlotInfo*, SlotInfo*);
void cmpl_double(SlotInfo*, SlotInfo*, SlotInfo*);
void cmpg_double(SlotInfo*, SlotInfo*, SlotInfo*);

void check_array_index(SlotInfo*, SlotInfo*);
void check_array_store(SlotInfo*, SlotInfo*);
void check_array_constindex(SlotInfo*, jint);

void softcall_lookupinterfacemethod(SlotInfo*, Method*, SlotInfo*);
void softcall_nullpointer(void);
void softcall_divzero(void);
void softcall_new(SlotInfo*, struct Hjava_lang_Class*);
void softcall_newarray(SlotInfo*, SlotInfo*, int);
void softcall_anewarray(SlotInfo*, SlotInfo*, struct Hjava_lang_Class*);
void softcall_multianewarray(SlotInfo*, int, SlotInfo*, struct Hjava_lang_Class*);
void softcall_athrow(SlotInfo*);
void softcall_checkcast(SlotInfo*, SlotInfo*, struct Hjava_lang_Class*);
void softcall_get_method_code(SlotInfo*, SlotInfo*);
void softcall_get_method_code_const(SlotInfo*, Method*);
void softcall_instanceof(SlotInfo*, SlotInfo*, struct Hjava_lang_Class*);
void softcall_monitorenter(SlotInfo*);
void softcall_monitorexit(SlotInfo*);
void softcall_initialise_class(struct Hjava_lang_Class*);
void softcall_addreference(SlotInfo*, SlotInfo*);
void softcall_addreference_static(void*, SlotInfo*);
void softcall_nosuchclass(Utf8Const*);
void softcall_nosuchfield(Utf8Const*, Utf8Const*);
void softcall_linkage(Utf8Const*, Utf8Const*);
void softcall_illegalaccess(Utf8Const*, Utf8Const*);
void softcall_incompatibleclasschange(Utf8Const*, Utf8Const*);
void softcall_abstractmethod(Utf8Const*, Utf8Const*);
void softcall_nosuchmethod(struct Hjava_lang_Class*, Utf8Const*, Utf8Const*);
void softcall_newarray_const(SlotInfo*, int, int);
void softcall_anewarray_const(SlotInfo*, int, struct Hjava_lang_Class*);
void softcall_trace(Method*);
void softcall_enter_method(SlotInfo*, Method*);
void softcall_exit_method(Method*);

void build_call_frame(Utf8Const*, SlotInfo*, int);
void load_globals(bool);

void softcall_debug1(void*, void*, void*);
void softcall_debug2(void*, void*, void*);
void get_arg_ptr(SlotInfo*);

void* jit_soft_multianewarray(struct Hjava_lang_Class* class, jint dims, ...);

#endif
