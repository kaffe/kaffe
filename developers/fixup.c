/*
 * Create a C++ module which will fix up unresolved dependencies 
 * in a .so file produced by gcj for use with Kaffe.
 *
 * Run with "fixup X" to fix up X.so.
 * 
 * NB: the output file must be compiled with -I<libgcj-install-path>
 *
 * Godmar Back <gback@cs.utah.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

static int verbose;

#define CLASS_PREFIX			"_CL_"
#define VTABLE_PREFIX			"__vt_"
#define JV_PREFIX			"_Jv_"
#define CONSTRUCTOR_PREFIX		"__"
#define ARRAY_TYPE_PREFIX		"t6JArray1Z"
#define ARRAY_TYPE_PREFIX0		't'

#define STARTS_WITH(x, y, b)	(sscanf(x, ##y"%s", b) == 1)

typedef struct symbol {
    enum { CLASS = 1, STATICFIELD = 2, METHODREF = 3, VTABLE = 4 } type;
    char	*symbol;
    char 	*data1;
    char 	*data2;
    char 	*data3;
    struct symbol *next;
} *symbol_t;

static symbol_t symbols;
static char currentSym[1024];

static void
emitSymbolTable()
{
    symbol_t s;
    printf ("struct {					\n");
    printf ("	void *symbol;				\n");
    printf ("	int type;				\n");
    printf ("	char* data1;				\n");
    printf ("	char* data2;				\n");
    printf ("	char* data3;				\n");
    printf ("} _gcjSymbols[] = {			\n");

    for (s = symbols; s; s = s->next) {
	printf("    { &%s, %d,\n\t\"%s\",\n\t\"%s\",\n\t\"%s\" },\n\n",
		s->symbol, s->type, s->data1, s->data2, s->data3);
    }
    printf("    { 0, -1, \"\", \"\" }			\n");
    printf ("};\n"					);
}

static int
addSymbol(char *symbol, int type, char *data1, char *data2, char *data3) 
{
    static int nsymbols = 0;
    symbol_t t, s = (symbol_t)calloc(sizeof (*s), 1);

    s->symbol = strdup(symbol);
    s->data1 = strdup(data1);
    s->data2 = strdup(data2);
    s->data3 = strdup(data3);
    s->type = type;

    if (symbols == 0) {
    	symbols = s;
    } else {
	for (t = symbols; t->next; t = t->next) {
		;
	}
	t->next = s;
    }
    return (nsymbols++);
}

/* e0ither single-digit %d or "_%d_" */
int
readInt(char **ptr)
{
    if (isdigit(**ptr)) {
	return (*(*ptr)++ - '0');
    } else {
	int x;
	assert(**ptr == '_');
	(*ptr)++;
	x = strtol(*ptr, ptr, 10);
	assert(**ptr == '_');
	(*ptr)++;
	return (x);
    }
}

char *
ununicodeString(char **ptr)
{
    int len;
    static char buf[1024];

    assert (**ptr != 'U' || !!!"Unicode not yet implemented");
    if (!isdigit(**ptr)) {
    	return (0);
    }
    len = strtol(*ptr, ptr, 10);
    strncpy(buf, *ptr, len);
    buf[len] = '\0';
    *ptr += len;
    return (buf);
}

/* either single-digit %d or "%d_" */
int 
readInt2(char **ptr, int first)
{
    char d0 = (*ptr)[0];
    char d1 = (*ptr)[1];
    assert(isdigit(d0));
    if (d1 && isdigit(d1)) {
	/* This is annoying: we can't tell with 
	 * one character look ahead anymore.
	 * Could be "N11_10_" or "N310_"
	 * Assume that's all that's left on this line
	 * and count underscores.
	 * XXX use regexps.
	 */
	int d, us = 0;
	char *pp = *ptr;
	while (*pp) {
	    if (*pp++ == '_') {
		us++;
	    }
	}

	assert(us < 3);

	/* leave underscore for second */
	if (us == 0 || (us == 1 && first)) {
	    return *(*ptr)++ - '0';
	}

	if (!first) {
	    assert(us == 1);
	}

	/* else it's our underscore */
	d = strtol(*ptr, ptr, 10);
	assert(**ptr == '_');
	(*ptr)++;
	return (d);
    } else {
	return *(*ptr)++ - '0';
    }
}

static int argn;
static char * argv[1024];

/* Q34java4lang12StringBuffer
 * or 13HelloWorldApp
 */
char *
unmangleType(char **symbol)
{
    char buf[1024];
    char *t;

skip_p:
    switch (*(*symbol)++) {
    /* T1 or T13_ single repeat of arg1 or arg13, resp. */
    case 'T':		
    {
    	int argi = readInt2(symbol, 1);
	assert(argi < argn);
	strcpy(buf, argv[argi]);
    	break;
    }

    /* N21 double repeat of arg1, N23_14_ is 23x arg 14 */
    case 'N':		
    {
    	int rep = readInt2(symbol, 1);
    	int argi = readInt2(symbol, 0);
	assert(argi < argn);
	strcpy(buf, argv[argi]);
	while (--rep > 0) {
	    strcat(buf, argv[argi]);
	}
	break;
    }

    case 'P':
	goto skip_p;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
	/* put digit back, it's part of unqualified type */
	(*symbol)--;	
	t = ununicodeString(symbol);
	if (!t) {
		return (0);
	}
	strcpy(buf, "L");
	strcat(buf, t);
	strcat(buf, ";");
	break;

    case 'Q':
    {
	int i, qualifiers = readInt(symbol);
	if (qualifiers == 0) {
		return (0);
	}
	strcpy(buf, "L");
	for (i = 0; i < qualifiers; i++) {
	    strcat(buf, ununicodeString(symbol));
	    if (i < qualifiers - 1) {
	    	strcat(buf, "/");
	    }
	}
	strcat(buf, ";");
	break;
    }
    case 'b': return "Z";
    case 'w': return "C";
    case 'v': return "V";
    case 'c': return "B";
    case 's': return "S";
    case 'i': return "I";
    case 'x': return "J";
    case 'f': return "F";
    case 'd': return "D";

    case ARRAY_TYPE_PREFIX0:
    {
    	if (!strncmp((*symbol)-1, ARRAY_TYPE_PREFIX, strlen(ARRAY_TYPE_PREFIX))) {
		char mbuf[1024];
		char *type;

		strcpy(mbuf, "[");
		*symbol += strlen(ARRAY_TYPE_PREFIX) - 1;
		type = unmangleType(symbol);
		strcat(mbuf, type);
		strcpy(buf, mbuf);
		break;
	} 
	/* FALL THROUGH */
    }

    default:
    	if (verbose) {
	    fprintf(stderr, "unmangleType: Cannot unmangle `%s'\n", *symbol - 1);
	}
	return (0);
    }
    return (strdup(buf));
}

static void replacechar(char *from, char *to, char r, char s)
{
    while (*from) {
	*to++ = *from == r ? s : *from;
	from++;
    }
}

void
handleClassType(char *symbol)
{
    char *classref = unmangleType(&symbol);
    printf("/* Class reference for %s */\n", classref);
    replacechar(classref, classref, '.', '/');

    printf("int %s[sizeof(java::lang::Class)/sizeof(int)];\n\n", currentSym);
    addSymbol(currentSym, CLASS, classref, "", "");
}

void
handleVTable(char *symbol)
{
    char *t = unmangleType(&symbol);
    static int vtNr;
    static char vtableSym[1024];
    sprintf(vtableSym, "_vtable_symbol_%d_", ++vtNr);

    printf("/* Vtable reference %s for %s */\n", currentSym, t); 
    printf("int %s[128 /* XXX */] __asm__(\"%s\");\n\n", 
    	vtableSym, currentSym);
    addSymbol(vtableSym, VTABLE, t, "", "");
}

static void
emitMethod(char *class, char *name, char *sig)
{
    int midx;
    replacechar(class, class, '.', '/');
    midx = addSymbol(currentSym, METHODREF, class, name, sig);

    /* See config/i386/trampolines.c */
    printf("int %s[2] = { 0xE9909090, /* nop, nop, jmp loc */\n"
    	"\t(int)&__kaffe_i386_gcj_fixup - 4 - (int)&%s[1]};\n\n",
            currentSym, currentSym);
}

static char *
makeLegal(char *sym)
{
    char *r = strdup(sym);
    replacechar(sym, r, '.', '_');
    return (r);
}

static void
handleStaticField(char *type, char *name)
{
    char *t = unmangleType(&type);
    char *cSym = makeLegal(currentSym);

    printf("/* Static field reference to %s. %s */\n", t, name);
    replacechar(t, t, '.', '/');

    /* avoid dots in symbols */
    printf("int	%s[2] __asm__(\"%s\");\n\n", cSym, currentSym);
    addSymbol(cSym, STATICFIELD, t, name, "");
}

/* equals__Q34java4lang6ObjectPQ34java4lang6Object passed as
 * "equals" "Q34java4lang6ObjectPQ34java4lang6Object"
 */
static void
handleMethodRef(char *name, char *typesig)
{
    char buf[2048] = "";
    char *clazz =  unmangleType(&typesig);
    char *sig;

    if (clazz == 0) {
	goto bad;
    }
    clazz =  strdup(clazz);

    if (typesig[0] == 0) {
    	sig = "()";
    } else {
    	argn = 1;	/* one-based counter */
	strcpy(buf, "(");
	do {
	    char *t = unmangleType(&typesig);
	    if (t == 0) {
	    	goto bad;
	    }
	    argv[argn++] = t;
	    strcat(buf, t);
	} while (typesig[0]);
	strcat(buf, ")");
	sig = buf;
    }

    printf("/* Reference to %s. %s %s */\n", clazz, name, sig);
    emitMethod(clazz, name, sig);
    return;

bad:
    if (clazz) {
    	free(clazz);
    }
    printf("/* Skipped symbol %s */\n", currentSym);
    return;
}

/* Q34java4lang12StringBufferPQ34java4lang6String 
 * __13HelloWorldApp
 */
void
handleConstructor(char *symbol)
{
    handleMethodRef("<init>", symbol);
}

static const char *header =
"#include <java/lang/Class.h>
#include <unistd.h>

extern \"C\" void __kaffe_i386_gcj_fixup(void);

";
int
main(int ac, char *argv[]) 
{
    FILE *input;
    char buf1[1024], buf2[1024];
    char *sofile = argv[1];

    if (!strcmp(argv[1], "-v")) {
	sofile = argv[2];
	verbose = 1;
    }
    if (!strcmp(argv[1], "-vv")) {
	sofile = argv[2];
	verbose = 2;
    }

    sprintf(buf1, "nm %s", sofile);
    /* append .so if not given */
    if (strlen(sofile) < 3 || strcmp(sofile + strlen(sofile) - 3, ".so")) {
	    strcat(buf1, ".so");
    }
    strcat(buf1, " | egrep 'U ' | awk '{print $2}'\n");
    input = popen(buf1, "r");

    if (input == 0) {
	fprintf(stderr, "Can't open pipe for `%s'\n", buf1);
    	exit(-1);
    }
    printf("/****************************************************************\n");
    printf(" * Automatically generated kaffe fixup module\n");
    printf(" *\n");
    printf(" * Module %s.so \n", sofile);
    printf(" ****************************************************************/\n\n");
    printf("%s", header);

    while (fgets(currentSym, sizeof currentSym, input) != 0) {
    	currentSym[strlen(currentSym) - 1] = '\0'; 	/* cut off \n */

	if (verbose == 2) {
	    fprintf(stderr, "processing `%s'\n", currentSym);
	}

	if (STARTS_WITH(currentSym, JV_PREFIX, buf1)) {
	    continue;	/* ignore those */
	} else
	if (STARTS_WITH(currentSym, CLASS_PREFIX, buf1)) {
	    handleClassType(buf1);
	} else 
	if (STARTS_WITH(currentSym, VTABLE_PREFIX, buf1)) {
	    handleVTable(buf1);
	} else
	if (sscanf(currentSym, "%[^_]__%s", buf1, buf2) == 2) {
	    handleMethodRef(buf1, buf2);
	} else 
	if (sscanf(currentSym, "_%[^.].%s", buf1, buf2) == 2) {
	    handleStaticField(buf1, buf2);
	} else
	if (!strncmp(currentSym, CONSTRUCTOR_PREFIX, strlen(CONSTRUCTOR_PREFIX))) {
	    handleConstructor(currentSym + strlen(CONSTRUCTOR_PREFIX));
	} else {
	    if (verbose) {
		fprintf(stderr, "Unknown external symbol `%s'\n", currentSym);
	    }
	}
    }

/* Don't do that or else you'd have to use the fixup's definitions...

    addSymbol("_Jv_intClass", CLASS, "int", "", "");
    addSymbol("_Jv_longClass", CLASS, "long", "", "");
    addSymbol("_Jv_booleanClass", CLASS, "boolean", "", "");
    addSymbol("_Jv_charClass", CLASS, "char", "", "");
    addSymbol("_Jv_floatClass", CLASS, "float", "", "");
    addSymbol("_Jv_doubleClass", CLASS, "double", "", "");
    addSymbol("_Jv_byteClass", CLASS, "byte", "", "");
    addSymbol("_Jv_shortClass", CLASS, "short", "", "");
    addSymbol("_Jv_voidClass", CLASS, "void", "", "");
*/
    emitSymbolTable();
    return (0);
}

