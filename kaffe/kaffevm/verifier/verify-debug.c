/*
 * verify-debug.c
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Debugging code for the verifier.
 */

#include "debug.h"
#include "verify.h"
#include "verify-uninit.h"

/* for debugging */
#if !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG))

/* change these indentations to whatever makes more sense, but
 * but these values produce some easy-to-read debugging output.
 */
const char* indent  = "                ";
const char* indent2 = "                        ";

/* these prototypes are here to keep gcc from complaining */
uint32 printConstantPoolEntry(const Hjava_lang_Class* class, uint32 idx);
void printConstantPool(const Hjava_lang_Class* class);
void printInstruction(const int opcode);
void printType(const Type*);
void printBlock(const Method* method, const BlockInfo* binfo, const char* id);




uint32
printConstantPoolEntry(const Hjava_lang_Class* class, uint32 idx)
{
	const constants* pool = CLASS_CONSTANTS(class);
	
	switch (pool->tags[idx]) {
	case CONSTANT_Utf8:
		DBG(VERIFY2, dprintf("   UTF8: %s", CONST_UTF2CHAR(idx, pool)) );
		break;
			
			
	case CONSTANT_Long:
	case CONSTANT_Double:
		idx++;
	case CONSTANT_Integer:
	case CONSTANT_Float:
		DBG(VERIFY2, dprintf("   NUMERICAL"); );
		break;
			
			
	case CONSTANT_ResolvedString:
	case CONSTANT_ResolvedClass:
		DBG(VERIFY2, dprintf("   RESOLVED: %s",
				     ((Hjava_lang_Class*)pool->data[idx])->name->data); );
		break;
			
			
			
	case CONSTANT_Class:
		DBG(VERIFY2, dprintf("   UNRESOLVED CLASS: %s", CLASS_NAMED(idx, pool)); );
		break;
			
	case CONSTANT_String:
		DBG(VERIFY2, dprintf("   STRING: %s", CONST_STRING_NAMED(idx, pool)); );
		break;
			
			
			
	case CONSTANT_Fieldref:
		DBG(VERIFY2, dprintf("   FIELDREF: %s  --type--  %s",
				     FIELDREF_NAMED(idx, pool), FIELDREF_SIGD(idx, pool)); );
		break;
			
	case CONSTANT_Methodref:
		DBG(VERIFY2, dprintf("   METHODREF: %s  --type--  %s",
				     METHODREF_NAMED(idx, pool), METHODREF_SIGD(idx, pool)); );
		break;
			
			
	case CONSTANT_InterfaceMethodref:
		DBG(VERIFY2, dprintf("   INTERFACEMETHODREF: %s  --type--  %s",
				     INTERFACEMETHODREF_NAMED(idx, pool), INTERFACEMETHODREF_SIGD(idx, pool)); );
		break;
			
			
	case CONSTANT_NameAndType:
		DBG(VERIFY2, dprintf("   NAMEANDTYPE: %s  --and--  %s",
				     NAMEANDTYPE_NAMED(idx, pool), NAMEANDTYPE_SIGD(idx, pool)); );
		break;
			
	default:
		DBG(VERIFY2, dprintf("   *** UNRECOGNIZED CONSTANT POOL ENTRY in class %s *** ",
				     CLASS_CNAME(class)); );
	}
	
	return idx;
}

void
printConstantPool(const Hjava_lang_Class* class)
{
	uint32 idx;
	const constants *pool = CLASS_CONSTANTS(class);
	
	DBG(VERIFY2, dprintf("    CONSTANT POOL FOR %s\n", class->name->data); );
	
	for (idx = 1; idx < pool->size; idx++) {
		DBG(VERIFY2, dprintf("      %d", idx); );
		
		idx = printConstantPoolEntry(class, idx);
		
		DBG(VERIFY2, dprintf("\n"); );
	}
}

/*
 * printInstruction()
 *     prints out a string representation of the instruction.
 *
 *     TODO: print out extra information with the instruction.
 */
void
printInstruction(const int opcode)
{
#define PRINT(_OP) dprintf("%s", _OP); return;
	
	switch(opcode) {
	case 0: PRINT("NOP");
		
	case 1: PRINT("ACONST-null");
		
	case 2:   PRINT("ICONST_M1");
	case 3:   PRINT("ICONST_0");
	case 4:   PRINT("ICONST_1");
	case 5:   PRINT("ICONST_2");
	case 6:   PRINT("ICONST_3");
	case 7:   PRINT("ICONST_4");
	case 8:   PRINT("ICONST_5");
		
	case 9:   PRINT("LCONST_0");
	case 10:  PRINT("LCONST_1");
		
	case 11:  PRINT("FCONST_0");
	case 12:  PRINT("FCONST_1");
	case 13:  PRINT("FCONST_2");
		
	case 14:  PRINT("DCONST_0");
	case 15:  PRINT("DCONST_1");
		
	case 16:  PRINT("BIPUSH");
	case 17:  PRINT("SIPUSH");
		
	case 18:  PRINT("LDC");
	case 19:  PRINT("LDC_W");
	case 20:  PRINT("LDC2_W");
		
	case 21:  PRINT("ILOAD");
	case 22:  PRINT("LLOAD");
	case 23:  PRINT("FLOAD");
	case 24:  PRINT("DLOAD");
	case 25:  PRINT("ALOAD");
		
	case 26:  PRINT("ILOAD_0");
	case 27:  PRINT("ILOAD_1");
	case 28:  PRINT("ILOAD_2");
	case 29:  PRINT("ILOAD_3");
		
	case 30:  PRINT("LLOAD_0");
	case 31:  PRINT("LLOAD_1");
	case 32:  PRINT("LLOAD_2");
	case 33:  PRINT("LLOAD_3");
		
	case 34:  PRINT("FLOAD_0");
	case 35:  PRINT("FLOAD_1");
	case 36:  PRINT("FLOAD_2");
	case 37:  PRINT("FLOAD_3");
		
	case 38:  PRINT("DLOAD_0");
	case 39:  PRINT("DLOAD_1");
	case 40:  PRINT("DLOAD_2");
	case 41:  PRINT("DLOAD_3");
		
	case 42:  PRINT("ALOAD_0");
	case 43:  PRINT("ALOAD_1");
	case 44:  PRINT("ALOAD_2");
	case 45:  PRINT("ALOAD_3");
		
	case 46:  PRINT("IALOAD");
	case 47:  PRINT("LALOAD");
	case 48:  PRINT("FALOAD");
	case 49:  PRINT("DALOAD");
	case 50:  PRINT("AALOAD");
	case 51:  PRINT("BALOAD");
	case 52:  PRINT("CALOAD");
	case 53:  PRINT("SALOAD");
		
	case 54:  PRINT("ISTORE");
	case 55:  PRINT("LSTORE");
	case 56:  PRINT("FSTORE");
	case 57:  PRINT("DSTORE");
	case 58:  PRINT("ASTORE");
		
	case 59:  PRINT("ISTORE_0");
	case 60:  PRINT("ISTORE_1");
	case 61:  PRINT("ISTORE_2");
	case 62:  PRINT("ISTORE_3");
		
	case 63:  PRINT("LSTORE_0");
	case 64:  PRINT("LSTORE_1");
	case 65:  PRINT("LSTORE_2");
	case 66:  PRINT("LSTORE_3");
		
	case 67:  PRINT("FSTORE_0");
	case 68:  PRINT("FSTORE_1");
	case 69:  PRINT("FSTORE_2");
	case 70:  PRINT("FSTORE_3");
		
	case 71:  PRINT("DSTORE_0");
	case 72:  PRINT("DSTORE_1");
	case 73:  PRINT("DSTORE_2");
	case 74:  PRINT("DSTORE_3");
		
	case 75:  PRINT("ASTORE_0");
	case 76:  PRINT("ASTORE_1");
	case 77:  PRINT("ASTORE_2");
	case 78:  PRINT("ASTORE_3");
		
	case 79:  PRINT("IASTORE");
	case 80:  PRINT("LASTORE");
	case 81:  PRINT("FASTORE");
	case 82:  PRINT("DASTORE");
	case 83:  PRINT("AASTORE");
	case 84:  PRINT("BASTORE");
	case 85:  PRINT("CASTORE");
	case 86:  PRINT("SASTORE");
		
	case 87:  PRINT("POP");
	case 88:  PRINT("POP_W");
		
	case 89:  PRINT("DUP");
	case 90:  PRINT("DUP_X1");
	case 91:  PRINT("DUP_X2");
	case 92:  PRINT("DUP2");
	case 93:  PRINT("DUP2_X1");
	case 94:  PRINT("DUP2_X2");
		
	case 95:  PRINT("SWAP");
		
	case 96:  PRINT("IADD");
	case 97:  PRINT("LADD");
	case 98:  PRINT("FADD");
	case 99:  PRINT("DADD");
		
	case 100: PRINT("ISUB");
	case 101: PRINT("LSUB");
	case 102: PRINT("FSUB");
	case 103: PRINT("DSUB");
		
	case 104: PRINT("IMUL");
	case 105: PRINT("LMUL");
	case 106: PRINT("FMUL");
	case 107: PRINT("DMUL");
		
	case 108: PRINT("IDIV");
	case 109: PRINT("LDIV");
	case 110: PRINT("FDIV");
	case 111: PRINT("DDIV");
		
	case 112: PRINT("IREM");
	case 113: PRINT("LREM");
	case 114: PRINT("FREM");
	case 115: PRINT("DREM");
		
	case 116: PRINT("INEG");
	case 117: PRINT("LNEG");
	case 118: PRINT("FNEG");
	case 119: PRINT("DNEG");
		
	case 120: PRINT("ISHL");
	case 121: PRINT("LSHL");
	case 122: PRINT("FSHL");
	case 123: PRINT("DSHL");
		
	case 124: PRINT("IUSHR");
	case 125: PRINT("LUSHR");
		
	case 126: PRINT("IAND");
	case 127: PRINT("LAND");
		
	case 128: PRINT("IOR");
	case 129: PRINT("LOR");
		
	case 130: PRINT("IXOR");
	case 131: PRINT("LXOR");
		
	case 132: PRINT("IINC");
		
	case 133: PRINT("I2L");
	case 134: PRINT("I2F");
	case 135: PRINT("I2D");
	case 136: PRINT("L2I");
	case 137: PRINT("L2F");
	case 138: PRINT("L2D");
	case 139: PRINT("F2I");
	case 140: PRINT("F2L");
	case 141: PRINT("F2D");
	case 142: PRINT("D2I");
	case 143: PRINT("D2L");
	case 144: PRINT("D2F");
	case 145: PRINT("I2B");
	case 146: PRINT("I2C");
	case 147: PRINT("I2S");
		
	case 148: PRINT("LCMP");
	case 149: PRINT("FCMPL");
	case 150: PRINT("FCMPG");
	case 151: PRINT("DCMPL");
	case 152: PRINT("DCMPG");
		
	case 153: PRINT("IFEQ");
	case 154: PRINT("IFNE");
	case 155: PRINT("IFLT");
	case 156: PRINT("IFGE");
	case 157: PRINT("IFGT");
	case 158: PRINT("IFLE");
		
	case 159: PRINT("IF_ICMPEQ");
	case 160: PRINT("IF_ICMPNE");
	case 161: PRINT("IF_ICMPLT");
	case 162: PRINT("IF_ICMPGE");
	case 163: PRINT("IF_ICMPGT");
	case 164: PRINT("IF_ICMPLE");
	case 165: PRINT("IF_ACMPEQ");
	case 166: PRINT("IF_ACMPNE");
		
	case 167: PRINT("GOTO");
		
	case 168: PRINT("JSR");
	case 169: PRINT("RET");
		
	case 170: PRINT("TABLESWITCH");
	case 171: PRINT("LOOKUPSWITCH");
		
	case 172: PRINT("IRETURN");
	case 173: PRINT("LRETURN");
	case 174: PRINT("FRETURN");
	case 175: PRINT("DRETURN");
	case 176: PRINT("ARETURN");
	case 177: PRINT("RETURN");
		
	case 178: PRINT("GETSTATIC");
	case 179: PRINT("PUTSTATIC");
		
	case 180: PRINT("GETFIELD");
	case 181: PRINT("PUTFIELD");
		
	case 182: PRINT("INVOKEVIRTUAL");
	case 183: PRINT("INVOKESPECIAL");
	case 184: PRINT("INVOKESTATIC");
	case 185: PRINT("INVOKEINTERFACE");
		
	case 187: PRINT("NEW");
		
	case 188: PRINT("NEWARRAY");
	case 189: PRINT("ANEWARRAY");
	case 190: PRINT("ARRAYLENGTH");
		
	case 191: PRINT("ATHROW");
		
	case 192: PRINT("CHECKCAST");
	case 193: PRINT("INSTANCEOF");
		
	case 194: PRINT("MONITORENTER");
	case 195: PRINT("MONITOREXIT");
		
	case 196: PRINT("WIDE");
		
	case 197: PRINT("MULTIANEWARRAY");
		
	case 198: PRINT("IFNULL");
	case 199: PRINT("IFNONNULL");
		
	case 200: PRINT("GOTO_W");
		
	case 201: PRINT("JSR_W");
		
	case 202: PRINT("BREAKPOINT");
		
	case 254: PRINT("IMPDEP1");
	case 255: PRINT("IMPDEP2");
		
	default:  PRINT("UNRECOGNIZED OPCODE");
	}
	
#undef PRINT
}
void
printType(const Type* t)
{
	const Hjava_lang_Class* type = t->data.class;
	
	dprintf("(%d)", t->tinfo);
	switch(t->tinfo) {
	case TINFO_SYSTEM:
		if (type == getTUNSTABLE()->data.class) {
			dprintf("TUNSTABLE");
		}
		else if (isWide(t)) {
			dprintf("TWIDE");
		}
		else {
			dprintf("UNKNOWN SYSTEM TYPE");
		}
		break;
		
	case TINFO_ADDR:
		dprintf("TADDR: %d", t->data.addr);
		break;
		
	case TINFO_PRIMITIVE:
		if (type == getTINT()->data.class) {
			dprintf("TINT");
		}
		else if (type == getTLONG()->data.class) {
			dprintf("TLONG");
		}
		else if (type == getTFLOAT()->data.class) {
			dprintf("TFLOAT");
		}
		else if (type == getTDOUBLE()->data.class) {
			dprintf("TDOUBLE");
		}
		else {
			dprintf("UKNOWN PRIMITIVE TYPE");
		}
		break;
		
	case TINFO_SIG:
		dprintf("%s", t->data.sig);
		break;
		
	case TINFO_NAME:
		dprintf("%s", t->data.name);
		break;
		
	case TINFO_CLASS:
		if (type == NULL) {
			dprintf("NULL");
		}
		else if (isNull(t)) {
			dprintf("TNULL");
		}
		
		else if (type == getTCHARARR()->data.class) {
			dprintf("TCHARARR");
		}
		else if (type == getTBOOLARR()->data.class) {
			dprintf("TBOOLARR");
		}
		else if (type == getTBYTEARR()->data.class) {
			dprintf("TBYTEARR");
		}
		else if (type == getTSHORTARR()->data.class) {
			dprintf("TSHORTARR");
		}
		else if (type == getTINTARR()->data.class) {
			dprintf("TINTARR");
		}
		else if (type == getTLONGARR()->data.class) {
			dprintf("TLONGARR");
		}
		else if (type == getTFLOATARR()->data.class) {
			dprintf("TFLOATARR");
		}
		else if (type == getTDOUBLEARR()->data.class) {
			dprintf("TDOUBLEARR");
		}
		else if (type == getTOBJARR()->data.class) {
			dprintf("TOBJARR");
		}
		else {
			if (type->name == NULL || CLASS_CNAME(type) == NULL) {
				dprintf("<NULL NAME>");
			} else {
				dprintf("%s", CLASS_CNAME(type));
			}
		}
		break;
		
	case TINFO_UNINIT:
	case TINFO_UNINIT_SUPER:
		printType(&(t->data.uninit->type));
		break;
		
	case TINFO_SUPERTYPES: {
		uint32 i;
		dprintf("TINFO_SUPERTYPES: ");
		for (i = 0; i < t->data.supertypes->count; i++) {
			dprintf("%s, ", CLASS_CNAME(t->data.supertypes->list[i]));
		}
	}
		
	default:
		dprintf("UNRECOGNIZED TINFO");
		break;
	}
}


/*
 * printBlock()
 *    For debugging.  Prints out a basic block.
 */
void
printBlock(const Method* method, const BlockInfo* binfo, const char* local_indent)
{
	uint32 n;
	
	dprintf("%slocals:\n", local_indent);
	for (n = 0; n < method->localsz; n++) {
		dprintf("%s    %d: ", local_indent, n);
		printType(&binfo->locals[n]);
		dprintf("\n");
	}
	dprintf("%sopstack (%d):\n", local_indent, binfo->stacksz);
	for (n = 0; n < method->stacksz; n++) {
		dprintf("%s    %d: ", local_indent, n);
		printType(&binfo->opstack[n]);
		dprintf("\n");
	}
}


#endif /* !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG)) */
