/* icode_internal.h
 * Declare functions defined in jit/icode.c
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

extern void load_offset_scaled_int(SlotInfo*, SlotInfo*, SlotInfo*, int);
extern void load_offset_scaled_ref(SlotInfo*, SlotInfo*, SlotInfo*, int);
extern void load_offset_scaled_long(SlotInfo*, SlotInfo*, SlotInfo*, int);
extern void load_offset_scaled_float(SlotInfo*, SlotInfo*, SlotInfo*, int);
extern void load_offset_scaled_double(SlotInfo*, SlotInfo*, SlotInfo*, int);
extern void load_offset_scaled_byte(SlotInfo*, SlotInfo*, SlotInfo*, int);
extern void load_offset_scaled_char(SlotInfo*, SlotInfo*, SlotInfo*, int);
extern void load_offset_scaled_short(SlotInfo*, SlotInfo*, SlotInfo*, int);
extern void store_offset_scaled_int(SlotInfo*, SlotInfo*, int, SlotInfo*);
extern void store_offset_scaled_ref(SlotInfo*, SlotInfo*, int, SlotInfo*);
extern void store_offset_scaled_long(SlotInfo*, SlotInfo*, int, SlotInfo*);
extern void store_offset_scaled_float(SlotInfo*, SlotInfo*, int, SlotInfo*);
extern void store_offset_scaled_double(SlotInfo*, SlotInfo*, int, SlotInfo*);
extern void store_offset_scaled_byte(SlotInfo*, SlotInfo*, int, SlotInfo*);
extern void store_offset_scaled_char(SlotInfo*, SlotInfo*, int, SlotInfo*);
extern void store_offset_scaled_short(SlotInfo*, SlotInfo*, int, SlotInfo*);

extern void load_offset_int(SlotInfo*, SlotInfo*, jint);
extern void load_offset_long(SlotInfo*, SlotInfo*, jint);
extern void load_offset_byte(SlotInfo*, SlotInfo*, jint);
extern void load_offset_char(SlotInfo*, SlotInfo*, jint);
extern void load_offset_short(SlotInfo*, SlotInfo*, jint);
extern void load_offset_float(SlotInfo*, SlotInfo*, jint);
extern void load_offset_double(SlotInfo*, SlotInfo*, jint);
extern void store_offset_int(SlotInfo*, jint, SlotInfo*);
extern void store_offset_long(SlotInfo*, jint, SlotInfo*);
extern void store_offset_float(SlotInfo*, jint, SlotInfo*);
extern void store_offset_double(SlotInfo*, jint, SlotInfo*);
extern void store_offset_byte(SlotInfo*, jint, SlotInfo*);
extern void store_offset_char(SlotInfo*, jint, SlotInfo*);
extern void store_offset_short(SlotInfo*, jint, SlotInfo*);

extern void move_string_const(SlotInfo*, void*);
extern void build_call_frame(Utf8Const*, SlotInfo*, int);
extern void softcall_nosuchclass(Utf8Const*);
extern void softcall_nosuchmethod(Hjava_lang_Class*, Utf8Const*, Utf8Const*);
extern void softcall_nosuchfield(Utf8Const*, Utf8Const*);
extern void softcall_linkage(Utf8Const*, Utf8Const*);
extern void softcall_illegalaccess(Utf8Const*, Utf8Const*);
extern void softcall_incompatibleclasschange(Utf8Const*,Utf8Const*);

