#
# mnemonicizer.awk
# Interprets a "defs" file filled with definitions for a processor instruction
# set architecture (ISA).
#
# Copyright (c) 2002, 2004 The University of Utah and the Flux Group.
# All rights reserved.
#
# @JANOSVM_TOOL_LICENSE@
#

#
# The mnemonicizer is a simple tool that is meant to ease the burden of
# writing and reading jitter code.  Instead of writing raw bits culled from
# a programming reference or manually #defining a bunch of stuff we just write
# a short defs file that can be automagically translated.  Hopefully, this
# eases the maintenance burden by making it trivial to change the generated
# macros as well as making it easier for fresh eyes to understand the code.
#
# XXX This code probably very ppc specific at the moment...
#

BEGIN {
    if( ARGC < 2 )
    {
	printf("Usage: awk -f %s <defs file>\n", ARGV[0]);
	exit -1;
    }
}

# A comment, just ignore it.
#
# Usage: # Foo bar
# Result: [empty]
/^\#/ {
}

# A C++ style comment that the user wishes to be included in the file.
#
# Usage: // Foo bar
# Result: // Foo bar
/^\/\// {
    printf("%s\n", $0);
}

# Specify the prefix for all generated symbols.
#
# Usage: prefix <name>
# Result: [empty] XXX
/^prefix[\t ]/ {
    prefix = $2;
    tall_prefix = toupper(prefix);
    printf("\n");
    printf("#ifndef _%s_isa_h_\n", prefix);
    printf("#define _%s_isa_h_\n", prefix);
    printf("\n");
    printf("#ifndef %s_op_debug\n", prefix);
    printf("#define %s_op_ctxt 0\n", prefix);
    printf("#define %s_op_debug(x) 0\n", prefix);
    printf("#endif\n");
    printf("\n");
}

# Specify the C type for instructions.
#
# Usage: prefix <type>
# Result: typedef <type> <prefix>_code_t;
/^opcode_type[\t ]/ {
    printf("typedef ");
    for( lpc = 2; lpc <= NF; lpc++ )
    {
	printf("%s ", $lpc);
    }
    printf("%s_code_t;\n\n", prefix);
}

# Specify a bit encoded value.
#
# Usage: bit <name> <value>
# Result:
#   #define <tall prefix>_<tall name> <value>
# or
#   #define <tall prefix>_<tall name>(<params>) <value> | <params>
#
# The value is made up of zero's, one's, y's, and z's.  The zero's and one's
# give a fixed value while the z's correspond to a reserved value that should
# be zero for now, and the y's make the bitmask parameterized.
#
# XXX Only one y argument is allowed at the least significant bit position.
/^bits[\t ]/ {
    name = $2;
    value = 0;
    has_arg = 0;
    for( lpc = 1; lpc < (length($3) + 1); lpc++ )
    {
	ch = substr($3, lpc, 1);
	if( ch == "1" )
	{
	    value += 2 ^ (length($3) - lpc);
	}
	else if( ch == "0" || ch == "z" )
	{
	}
	else if( ch == "y" )
	{
	    has_arg = 1;
	}
    }
    bits[name] = value;
    if( has_arg )
    {
	printf("#define %s_%s(y) (0x%x | !!(y))\n\n",
	       tall_prefix, toupper(name), value);
    }
    else
    {
	printf("#define %s_%s 0x%x\n\n", tall_prefix, toupper(name), value);
    }
}

# Specify the number of registers.
#
# Usage: registers <int register count>
# Result: enum {
#   <tall prefix>_R0,
#   <tall prefix>_R1,
#   <tall prefix>_R2,
#   ...
# };
/^registers[\t ]/ {
    printf("enum {\n");
    for( lpc = 0; lpc < $2; lpc++ )
    {
	printf("\t%s_R%d,\n", tall_prefix, lpc);
    }
    printf("};\n\n");

    int_registers = int($2);
}

# Specify the number of floating point registers.
#
# Usage: float_registers <float register count>
# Result: enum {
#   <tall prefix>_FPR = <int register count>
#   <tall prefix>_FPR0,
#   <tall prefix>_FPR1,
#   ...
# };
/^float_registers[\t ]/ {
    printf("enum {\n");
    printf("\t%s_FPR = %d,\n", tall_prefix, int_registers - 1);
    for( lpc = 0; lpc < $2; lpc++ )
    {
	printf("\t%s_FPR%d,\n", tall_prefix, lpc);
    }
    printf("};\n\n");

    float_registers = int($2);
}

# Specify the Kaffe-specific attributes for integer registers.
#
# Usage: register_attribute <start reg> <end reg> <reg type> <reg flags>
# Result: The result of these aren't printed out till the end and have the
#   form:
# #define REGISTER_SET \
#   { /* r0 */ 0, 0, <reg type>, <reg flags>, 0, 0, 0 }, \
#   { /* r1 */ 0, 0, <reg type>, <reg flags>, 0, 0, 1 }, \
#   { /* r2 */ 0, 0, <reg type>, <reg flags>, 0, 0, 2 }, \
#   ...
#   { /* rN */ 0, 0, <reg type>, <reg flags>, 0, 0, N }, \
/^register_attribute[\t ]/ {
    for( lpc = $2; lpc <= $3; lpc++ )
    {
	register_attributes[lpc] = $4;
	if( NF >= 5 )
	    register_flags[lpc] = $5;
    }
}

# Specify the Kaffe-specific attributes for floating point registers.
#
# Usage: float_register_attribute <start reg> <end reg> <reg type> <reg flags>
# Result: Same as above, except the floating point registers are appended
#   to the end...
# #define REGISTER_SET \
#   { /* r0 */ 0, 0, <reg type>, <reg flags>, 0, 0, 0 }, \
#   { /* r1 */ 0, 0, <reg type>, <reg flags>, 0, 0, 1 }, \
#   { /* r2 */ 0, 0, <reg type>, <reg flags>, 0, 0, 2 }, \
#   ...
#   { /* rN */ 0, 0, <reg type>, <reg flags>, 0, 0, N }, \
/^float_register_attribute[\t ]/ {
    for( lpc = $2; lpc <= $3; lpc++ )
    {
	float_register_attributes[lpc] = $4;
	if( NF >= 5 )
	    float_register_flags[lpc] = $5;
    }
}

# Define a register alias.
#
# Usage: register_alias <name> <register index>
# Result: #define <tall prefix>_R<tall name> <tall prefix>_R<index>
/^register_alias[\t ]/ {
    printf("#define %s_R%s %s_R%d\n\n",
	   tall_prefix,
	   toupper($2),
	   tall_prefix,
	   $3);
}

# Define a floating point register alias.
#
# Usage: float_register_alias <name> <register index>
# Result: #define <tall prefix>_FPR<tall name> <tall prefix>_FPR<index>
/^float_register_alias[\t ]/ {
    printf("#define %s_FPR%s %s_FPR%d\n\n",
	   tall_prefix,
	   toupper($2),
	   tall_prefix,
	   $3);
}

# Define an opcode field.
#
# Usage: op_field <name> <begin offset> <end offset>
# Result:
#   #define <tall prefix>_<tall name>_OFFSET
#   #define <tall prefix>_<tall name>_MASK
#   #define <tall prefix>_SET_<tall name>(x)
#   #define <tall prefix>_GET_<tall name>(x)
#
# The begin and end offsets are inclusive and start at the most significant
# bit (e.g. 2 ^ 31 is offset 0).
/^op_field[\t ]/ {
    name = $2;
    offset = $3;
    size = $4 - offset + 1;
    op_fields[name] = offset;
    printf("#define %s_%s_OFFSET %dUL\n",
	   tall_prefix,
	   toupper(name),
	   32 - offset - size);
    printf("#define %s_%s_MASK (0x%xUL << %s_%s_OFFSET)\n",
	   tall_prefix,
	   toupper(name),
	   sizemask(size),
	   tall_prefix,
	   toupper(name));
    printf("#define %s_SET_%s(x) \\\n",
	   tall_prefix,
	   toupper(name));
    printf("\t(((x) & 0x%x) << %s_%s_OFFSET)\n",
	   sizemask(size),
	   tall_prefix,
	   toupper(name));
    printf("#define %s_GET_%s(x) \\\n",
	   tall_prefix,
	   toupper(name));
    printf("\t(((x) >> %s_%s_OFFSET) & 0x%x)\n",
	   tall_prefix,
	   toupper(name),
	   sizemask(size));
    printf("\n");
}

# Define a single bit opcode field.
#
# Usage: op_option <name> <offset>
# Result: 
#   #define <tall prefix>_<tall name>_OFFSET
#   #define <tall prefix>_OPTION_<tall name>
/^op_option[\t ]/ {
    name = $2;
    offset = $3;
    op_options[name] = offset;
    op_fields[name] = offset;
    printf("#define %s_%s_OFFSET %dUL\n",
	   tall_prefix,
	   toupper(name),
	   31 - offset);
    printf("#define %s_OPTION_%s (1L << %d)\n\n",
	   tall_prefix,
	   toupper(name),
	   31 - offset);
}

# Define an opcode.
#
# Usage: op <name> <op_field|op_option|int> [...]
# Result: #define <prefix>_op_<name>(...)
#
# Opcodes are defined using the op_fields and op_options that were previously
# defined as well as raw integers.  Any op_fields can be used with or without
# an argument (e.g. ra(1) vs. ra) to specify whether the generated macro should
# contain a parameter for the field.  The op_option fields are ignored because
# they are probably better served by or'ing the argument afterwards
# (e.g. op_blr() | PPC_OPTION_LK).  However, they must be given so that the
# bits they use are accounted for and to ensure proper placement of raw integer
# values.  Otherwise, the script is unable to properly fill in the fields.
#
#
# Example:
#
# prefix ppc
#
# # Define the opcode field.
# op_field opcd 0 5
# # Define the register fields.
# op_field ra 11 15
# op_field rb 16 20
# op_field rd 6 10
#
# op_option oe 21
# op_option rc 31
#
# # rd = ra + rb
# op add opcd(31) rd ra rb oe 266 rc
# # rd = r0 + rb
# op addto0 opcd(31) rd ra(0) rb oe 266 rc
#
#
# Example Output:
#
# ...
# #define ppc_op_add(rd, ra, rb) \
#   (PPC_SET_OPCD(31) |
#    PPC_SET_RD(rd) |
#    PPC_SET_RA(ra) |
#    PPC_SET_RB(rb) |
#    (266 << 1))
#
# #define ppc_op_addto0(rd, rb) \
#   (PPC_SET_OPCD(31) |
#    PPC_SET_RD(rd) |
#    PPC_SET_RA(0) |
#    PPC_SET_RB(rb) |
#    (266 << 1))
/^op[\t ]/ {
    name = $2;
    printf("#define %s_op_%s(", prefix, name);
    first_field = 1;
    for( lpc = 3; lpc <= NF; lpc++ )
    {
	if( $lpc in op_options )
	{
	}
	else if( $lpc in op_fields )
	{
	    printf("%s%s", (first_field ? "" : ", "), $lpc);
	    first_field = 0;
	}
    }
    printf(") \\\n");
    printf("\t((void)%s_op_debug((%s_op_ctxt, \"%%s:%s ", prefix, prefix, name);
    for( lpc = 3; lpc <= NF; lpc++ )
    {
	if( $lpc in op_options )
	{
	}
	else if( $lpc in op_fields )
	{
	    printf("%s(%%d) ", $lpc);
	}
    }
    printf("\", __FUNCTION__");
    for( lpc = 3; lpc <= NF; lpc++ )
    {
	if( $lpc in op_options )
	{
	}
	else if( $lpc in op_fields )
	{
	    printf(", %s", $lpc);
	}
    }
    printf(")), \\\n");
    printf("\t (");
    bar = "";
    the_bar = " | \\\n\t  ";
    for( lpc = 3; lpc <= NF; lpc++ )
    {
	if( split($lpc, args, "[()]") == 3 )
	{
	    if( args[2] in bits )
		args[2] = bits[args[2]];
	    if( args[1] in op_options )
	    {
		printf("%s%s_OPTION_%s",
		       bar, tall_prefix, toupper(args[1]));
	    }
	    else
	    {
		printf("%s%s_SET_%s(%s)",
		       bar, tall_prefix, toupper(args[1]), args[2]);
	    }
	    bar = the_bar;
	}
	else if( $lpc in op_options )
	{
	}
	else if( $lpc in op_fields )
	{
	    printf("%s%s_SET_%s(%s)", bar, tall_prefix, toupper($lpc), $lpc);
	    bar = the_bar;
	}
	else if( lpc < NF )
	{
	    if( split($(lpc + 1), args, "[()]") == 3 )
	    {
		field_name = args[1];
	    }
	    else
	    {
		field_name = $(lpc + 1);
	    }
	    printf("%s(%dUL << %d)", bar, $lpc, 32 - op_fields[field_name]);
	    bar = the_bar;
	}
	else
	{
	    printf("%s%d", bar, $lpc);
	    bar = the_bar;
	}
    }
    printf("))\n\n");
}

# Print out the REGISTER_SET define for Kaffe.
END {
    printf("#define REGISTER_SET \\\n");
    for( lpc = 0; lpc < int_registers; lpc++ )
    {
	attr = "Rint|Rref";
	flags = "0";
	if( lpc in register_attributes )
	    attr = register_attributes[lpc];
	if( lpc in register_flags )
	    flags = register_flags[lpc];
	printf("\t{ /* r%d */\t0, 0, %s,\t%s, 0, %d  }, \\\n",
	       lpc,
	       attr,
	       flags,
	       lpc);
    }
    for( lpc = 0; lpc < float_registers; lpc++ )
    {
	attr = "Rfloat|Rdouble";
	flags = "0";
	if( lpc in float_register_attributes )
	    attr = float_register_attributes[lpc];
	if( lpc in float_register_flags )
	    flags = float_register_flags[lpc];
	printf("\t{ /* fpr%d */\t0, 0, %s,\t%s, 0, %d  }, \\\n",
	       lpc,
	       attr,
	       flags,
	       int_registers + lpc);
    }
    printf("\n");
    printf("#define NR_REGISTERS %d\n", int_registers + float_registers);
    printf("\n");
    printf("#endif\n");
}

function sizemask(size,  lpc, retval)
{
    retval = (2 ^ size) - 1;
    return retval;
}
