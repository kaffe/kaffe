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
typedef struct type_info
{
	Hjava_lang_Class* type;
	uint32 tinfo;
} Type;

// status flags for opstack/local info arrays
//
//   CLASS_SIGSTR means that the type is really a (const char*) representing the class' type signature
//   CLASS_NAMESTR means that the type is really a (const char*) representing the class' name
//   UNINIT        is a class instance created by NEW that has yet to be initialized.
//                 the type is really an (UninitializedType*), so that dups, moves, etc. ensure that whatever
//                 copies of the type are around are all initialized when the <init>() is called.
//   UNINIT_SUPER  reserved for the self-reference in a constructor method.  when the receiver of a call to <init>()
//                 is of type UNINIT_SUPER, then the <init>() referenced may be in the current class of in its
//                 superclass.
#define CLASS_SIGSTR       1
#define CLASS_NAMESTR      2
#define UNINIT             4
#define UNINIT_SUPER       12



/*
 * basic block header information
 */
typedef struct block_info
{
	// address of start of block
	uint32 startAddr;
	uint32 lastAddr;  // whether it be the address of a GOTO, etc.
	
	// status of block...changed (needs to be re-evaluated), visited, etc.
	uint32 status;
	
	// array of local variables
	Type*  locals;
	
	// simulated operand stack
	uint32 stacksz;
	Type*  opstack;
} BlockInfo;

// status flags for a basic block.
// these also pertain to the status[] array for the entire instruction array
#define CHANGED            1
#define VISITED            2
#define IS_INSTRUCTION     4

// if the instruction is preceeded by WIDE
#define WIDE_MODDED        8

// used at the instruction status level to find basic blocks
#define START_BLOCK       16
#define END_BLOCK         32

#define EXCEPTION_HANDLER 64


/*
 * the sig stack is the stack of signatures that we have allocated memory for which
 * must be freed after verification.
 */
// TODO: use the builtin hash table data structure instead so we can avoid repeats
typedef struct sig_stack
{
	const char* sig;
	struct sig_stack* next;
} SigStack;


/*
 * holds the list of uninitialized items.  that way, if we DUP some uninitialized
 * reference, put it into a local variable, dup it again, etc, all will point to
 * one item in this list, so when we <init> any of those we can init them all! :)
 *
 * doubly linked list to allow for easy removal of types
 */
typedef struct unitialized_types_double_list
{
	Type type;
	
	struct unitialized_types_double_list* prev;
	struct unitialized_types_double_list* next;
} UninitializedType;



void initVerifierTypes(void);
bool verify3(Hjava_lang_Class* class, errorInfo *einfo);


#endif // __verify_h
