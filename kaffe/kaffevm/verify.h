/*
 * verify.h
 *
 * only publically accessible (i.e. required outside of the verifier)
 * are declared here.
 *
 * copyright 2003, Rob Gonzalez (rob@kaffe.org)
 */

#ifndef __verify_h
#define __verify_h


/**********************************************************
 * Pass 2
 **********************************************************/
bool verify2(Hjava_lang_Class* class, errorInfo *einfo);



/**********************************************************
 * Pass 3
 **********************************************************/
struct unitialized_types_double_list;
struct Type;

typedef struct Type
{
	uint32 tinfo;
	
	union {
	        /* different ways to refer to an object reference */
		const char* name;
		const char* sig;
		Hjava_lang_Class* class;
		
	        /* uninitialized object reference */
		struct unitialized_types_double_list* uninit;
		
	        /* list of supertypes in the event of multiple inheritence of interfaces. */
		Hjava_lang_Class** supertypes;
		
	        /* return address for TINFO_ADDR */
		uint32 addr;
	} data;
} Type;

/* status flags for opstack/local info arrays
 *
 *   TINFO_SYSTEM       internal type, such as UNSTABLE or VOID
 *   TINFO_ADDR         return address type
 *   TINFO_PRIMITIVE    Type.data.class is some primitive class, like intClass
 *   TINFO_CLASS        Type.data.class
 *   TINFO_NAME         Type.data.name represents the class' fully qualified name
 *   TINFO_SIG          Type.data.sig  represents the class' fully qualified type signature
 *   TINFO_UNINIT       is a class instance created by NEW that has yet to be initialized.
 *                      the type is really an (UninitializedType*), so that dups, moves, etc. ensure that whatever
 *                      copies of the type are around are all initialized when the <init>() is called.
 *   TINFO_UNINIT_SUPER reserved for the self-reference in a constructor method.  when the receiver of a call to <init>()
 *                      is of type TINFO_UNINIT_SUPER, then the <init>() referenced may be in the current class of in its
 *                      superclass.
 *   TINFO_SUPERLIST    a list of supertypes.  used when merging two types that have multiple common supertypes.
 *                      this can occur with the multiple inheritence of interfaces.
 *                      the zeroth element is always a common superclass, the rest are common superinterfaces.
 */
#define TINFO_SYSTEM       0
#define TINFO_ADDR         1
#define TINFO_PRIMITIVE    2
#define TINFO_SIG          4
#define TINFO_NAME         8
#define TINFO_CLASS        16
#define TINFO_UNINIT       32
#define TINFO_UNINIT_SUPER 96
#define TINFO_SUPERLIST    128

#define IS_ADDRESS(_TINFO) ((_TINFO)->tinfo & TINFO_ADDR)
#define IS_PRIMITIVE_TYPE(_TINFO) ((_TINFO)->tinfo & TINFO_PRIMITIVE)


/*
 * holds the list of uninitialized items.  that way, if we DUP some uninitialized
 * reference, put it into a local variable, dup it again, etc, all will point to
 * one item in this list, so when we <init> any of those we can init them all! :)
 *
 * doubly linked list to allow for easy removal of types
 */
typedef struct unitialized_types_double_list
{
	struct Type type;
	
	struct unitialized_types_double_list* prev;
	struct unitialized_types_double_list* next;
} UninitializedType;



/*
 * basic block header information
 */
typedef struct block_info
{
        /* address of start of block */
	uint32 startAddr;
        uint32 lastAddr;  /* whether it be the address of a GOTO, etc. */
	
        /* status of block...
	 * changed (needs to be re-evaluated), visited, etc. 
	 */
	uint32 status;
	
        /* array of local variables */
	Type*  locals;
	
        /* simulated operand stack */
	uint32 stacksz;
	Type*  opstack;
} BlockInfo;

/* status flags for a basic block.
 * these also pertain to the status[] array for the entire instruction array
 */
#define CHANGED            1
#define VISITED            2
#define IS_INSTRUCTION     4

/* if the instruction is preceeded by WIDE */
#define WIDE_MODDED        8

/* used at the instruction status level to find basic blocks */
#define START_BLOCK       16
#define END_BLOCK         32

#define EXCEPTION_HANDLER 64


/*
 * the sig stack is the stack of signatures that we have allocated memory for which
 * must be freed after verification.
 */
/* TODO: use the builtin hash table data structure instead 
 * so we can avoid repeats
 */
typedef struct sig_stack
{
	const char* sig;
	struct sig_stack* next;
} SigStack;



void initVerifierPrimTypes(void);
bool verify3(Hjava_lang_Class* class, errorInfo *einfo);


#endif /* __verify_h */
