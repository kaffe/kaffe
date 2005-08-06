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

struct Hjava_lang_Class;

void _start_basic_block(void);
void _end_basic_block(uintp, uintp);
void _start_sub_block(void);
void _end_sub_block(uintp, uintp);
void _fixup_function_call(void);
void _prepare_function_call(uintp, uintp);
void _slot_nowriteback(SlotInfo*);
void _start_instruction(uintp);
void _start_exception_block(uintp);
void finish_function(void);

void move_any(SlotInfo*, SlotInfo*);
void move_anylong(SlotInfo*, SlotInfo*);

void move_int_const(SlotInfo*, jint);
void move_ref_const(SlotInfo*, void*);
void move_long_const(SlotInfo*, jlong);
void move_float_const(SlotInfo*, jfloat);
void move_double_const(SlotInfo*, jdouble);
void move_label_const(SlotInfo*, label*);

void move_int(SlotInfo*, SlotInfo*);
void move_ref(SlotInfo*, SlotInfo*);
void move_long(SlotInfo*, SlotInfo*);
void move_float(SlotInfo*, SlotInfo*);
void move_double(SlotInfo*, SlotInfo*);

void add_int(SlotInfo*, SlotInfo*, SlotInfo*);
void and_int_const(SlotInfo*, SlotInfo*, jint);
void sub_int(SlotInfo*, SlotInfo*, SlotInfo*);
void mul_int(SlotInfo*, SlotInfo*, SlotInfo*);
void div_int(SlotInfo*, SlotInfo*, SlotInfo*);
void rem_int(SlotInfo*, SlotInfo*, SlotInfo*);
void neg_int(SlotInfo*, SlotInfo*);
void and_int(SlotInfo*, SlotInfo*, SlotInfo*);
void or_int(SlotInfo*, SlotInfo*, SlotInfo*);
void xor_int(SlotInfo*, SlotInfo*, SlotInfo*);
void ashr_int(SlotInfo*, SlotInfo*, SlotInfo*);
void lshr_int(SlotInfo*, SlotInfo*, SlotInfo*);
void lshl_int(SlotInfo*, SlotInfo*, SlotInfo*);
void add_int_const(SlotInfo*, SlotInfo*, jint);
void sub_int_const(SlotInfo*, SlotInfo*, jint);
void lshl_int_const(SlotInfo*, SlotInfo*, jint);
void cmp_int(SlotInfo*, SlotInfo*, SlotInfo*);
void cmp_int_const(SlotInfo*, SlotInfo*, jint);

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
void pusharg_class_const(void*, int);
void pusharg_utf8_const(void*, int);
void popargs(void);

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
void epilogue(void);
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

void spill_int(SlotInfo*);
void spill_ref(SlotInfo*);
void spill_long(SlotInfo*);
void spill_float(SlotInfo*);
void spill_double(SlotInfo*);

void reload_int(SlotInfo*);
void reload_ref(SlotInfo*);
void reload_long(SlotInfo*);
void reload_float(SlotInfo*);
void reload_double(SlotInfo*);

void load_offset_int(SlotInfo*, SlotInfo*, jint);
void store_offset_int(SlotInfo*, jint, SlotInfo*);
void load_offset_ref(SlotInfo*, SlotInfo*, jint);
void store_offset_ref(SlotInfo*, jint, SlotInfo*);

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
void check_array_constindex(SlotInfo*, jint);


void softcall_lookupinterfacemethod(SlotInfo*, Method*, SlotInfo*);
void softcall_nullpointer(void);
void softcall_new(SlotInfo*, struct Hjava_lang_Class*);
void softcall_newarray(SlotInfo*, SlotInfo*, int);
void softcall_anewarray(SlotInfo*, SlotInfo*, struct Hjava_lang_Class*);
void softcall_multianewarray(SlotInfo*, int, SlotInfo*,
			     struct Hjava_lang_Class*);
void softcall_athrow(SlotInfo*);
void softcall_checkcast(SlotInfo*, SlotInfo*, struct Hjava_lang_Class*);
void softcall_get_method_code(SlotInfo*, SlotInfo*);
void softcall_get_method_code_const(SlotInfo*, Method*);
void softcall_instanceof(SlotInfo*, SlotInfo*, struct Hjava_lang_Class*);
void softcall_monitorenter(SlotInfo*);
void softcall_monitorexit(SlotInfo*);
void softcall_initialise_class(struct Hjava_lang_Class*);
void softcall_checkarraystore(SlotInfo*, SlotInfo*);
void softcall_addreference(SlotInfo*, SlotInfo*);
void softcall_addreference_static(void*, SlotInfo*);
void softcall_abstractmethod(Utf8Const*, Utf8Const*);
void softcall_exit_method(Method *meth);
void softcall_enter_method(SlotInfo *obj, Method *meth);

#endif
