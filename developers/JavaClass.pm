#
# Functions for reading in and writing out a Java .class file.
# Also does a bit of consistency checking of the file.
#
# The only really nasty thing I've done (because of poor perl skils more
# than anything else) is to make the %class a local() in a number of
# places so that the check routines can see it.
#
# Class structure:  Generally references to hashes. Tables are implemented as arrays.
#
# TODO:
#	make a &checkClass() function.
#	change a lot of 'local's to 'my's. (not local(%class), though)
#	Make CLASSIN and CLASSOUT parameters to read/write functions.
#	POD documentation
#	Cannot handle modifying float values.  I can read and decode, but don't
#	have the math to convert back to a binary format (both floats and doubles).

#
# Copyright (c) 1999 University of Utah CSL.
#
# This file is distributed under the terms of the GNU Public License.
#


package JavaClass;

###
### Define constants for Java Classes
###

*classMagic = \0xcafebabe;	# The magic header every .class file starts with

## The magic identifiers for entries in the .class Constant Table.
*CONSTANT_Class = \7;
*CONSTANT_FieldRef = \9;
*CONSTANT_MethodRef = \10;
*CONSTANT_InterfaceMethodRef = \11;
*CONSTANT_String = \8;
*CONSTANT_Integer = \3;
*CONSTANT_Float = \4;
*CONSTANT_Long = \5;
*CONSTANT_Double = \6;
*CONSTANT_NameAndType = \12;
*CONSTANT_Utf8 = \1;

## String names associated with each type of Constant Table entry.
%CONSTANTNames = (
    $CONSTANT_Class => "Class",
    $CONSTANT_FieldRef => "Field",
    $CONSTANT_MethodRef => "Method",
    $CONSTANT_InterfaceMethodRef => "Inteface Method",
    $CONSTANT_String => "String",
    $CONSTANT_Float => "Float",
    $CONSTANT_Integer => "Integer",
    $CONSTANT_Double => "Double",
    $CONSTANT_Long => "Long",
    $CONSTANT_NameAndType => "Name&Type",
    $CONSTANT_Utf8 => "Utf8"
    );

## String names for the shorthand used in signatures
$sig{'V'} = 'void';
$sig{'I'} = 'int';
$sig{'J'} = 'long';
$sig{'Z'} = 'boolean';
$sig{'F'} = 'float';
$sig{'D'} = 'double';
$sig{'B'} = 'byte';
$sig{'S'} = 'short';
$sig{'C'} = 'char';

## Access control flags for classes, methods and fields.
*ACC_PUBLIC    = \0x0001;
*ACC_PRIVATE   = \0x0002;
*ACC_PROTECTED = \0x0004;
*ACC_STATIC    = \0x0008;
*ACC_FINAL     = \0x0010;
*ACC_SUPER     = \0x0020;		# class only
*ACC_SYNCHRONIZED = \0x0020;		# field/method
*ACC_VOLATILE  = \0x0040;
*ACC_TRANSIENT = \0x0080;
*ACC_INTERFACE = \0x0200;
*ACC_ABSTRACT  = \0x0400;
*ACC_NATIVE    = \0x0100;
*ACC_STRICT    = \0x0800;

*ACC_UNKNOWN   = \0xF000;

###
###  Global variables
###

# Control the verbosity of &printClass()
$detailedFields = 0;
$detailedMethods = 0;

###
### Conversion functions
###

## parseJavaSig() takes a single argument, a single Java-internal
## method signature and returns a list ($package, $return, $class,
## $method, @args) where the items have been converted to a more
## source-like format (e.g., english).
sub parseJavaSig() {
  ## Parameters
  my $jsig = shift;

  ## Local variables
  my $class = '';
  my $package = '';
  my $method = '';
  my @args = ();
  my $ret = '';

  ## Temporaries
  my $depth = 0;
  my $repct = 0;
  my $arg = '';

  ### First is the class (all chars until a ".")
  $jsig =~ s/^([^.]*).//;
  $class = $1;
  $class =~ s,/,.,g; # / -> .

  # Peel the package name out of the class name (everything before last ".")
  if ($class =~ m/(.*)\.[^\.]*$/) {
    $package = $1;
  }

  ### Second comes the method name (all chars until a left paren)
  $jsig =~ s/^([^\(]*)\(//;
  $method = $1;

  ### Now the arguments
 SIGPARSE:
  while(1) {
    $repct = $jsig =~ s/^(I|J|Z|F|D|B|S|C|L|\[|\))//;  ## No V types
    die "badly formed signature at $jsig" if ($repct == 0);
    $arg = $1;

    ## Stop if we hit the end paren
    last SIGPARSE if $arg eq "\)";

    if ($arg eq '[') {
      $depth++;
      # continue parsing array type...
      next SIGPARSE;
    } elsif ($arg eq 'L') {
      $jsig =~ s/^([^;]*);//;
      $arg = $1;
      $arg =~ s,/,.,g;
    } else {
      ## convert single-char identifier to english
      $arg = $sig{$arg};
    }

    ## If we hit an array, tack the array depth on the end
    if ($depth > 0) {
      $arg = $arg . "[]" x $depth;
      $depth = 0;
    }

    # Put the arg at the end of the list of args
    push (@args, $arg)
  }

  ### Last is the return type
  $depth = 0;
  $repct = $jsig =~ s/^(I|J|Z|F|D|B|S|C|L|V|\[)//; ## Adds V over argument types
  die "badly formed return type: \'$jsig\'" if ($repct == 0);
  $ret = $1;

  # If its an array, eat the [ and re-set $ret
  if ($ret eq '[') {
    $depth = 1;
    while ($jsig =~ s/\[//) {
      $depth++;
    }
    $jsig =~ s/^(I|J|Z|F|D|B|S|C|L)//; ## No [ or V
    die "badly formed return type: \'$jsig\'" if ($repct == 0);
    $ret = $1;
  }

  if ($ret eq 'L') {
    $jsig =~ s/^([^;]*);//;
    $ret = $1;
    $ret =~ s,/,.,g;
  } else {
    ## Convert single char identifier to english
    $ret = $sig{"$ret"};
  }

  # Tack the array brackets on
  if ($depth > 0) {
    $ret = $ret . "[]" x $depth;
  }

  ### Return the info in an easy-to-use list
  return ($package, $ret, $class, $method, @args);
}

###
### Print functions
###

sub printClass {
    my $r_cl = shift;
    my %class = %{$r_cl};

    my $flStr = &ACCFlagsToString($class{accessFlags}, 1);
    print "$flStr\n";

    &printConstantPool($r_cl);

    ## Print 'this_class'
    my $thisClName = %{$class{constantPool}[$class{thisClass}]}->{nameIndex};
    $thisClName = %{$class{constantPool}[$thisClName]}->{val};
    print "this_class @ $class{thisClass} ($thisClName)\n";

    ## Print 'super_class'
    if ($class{superClass} != 0) {
      my $superClName = %{$class{constantPool}[$class{superClass}]}->{nameIndex};
      $superClName = %{$class{constantPool}[$superClName]}->{val};
      print "super_class @ $class{superClass} ($superClName)\n";
    } else {
      print "No super class\n";
    }

    ## Print direct super interfaces
    &printInterfaces($r_cl);

    ## Print fields
    &printFields($r_cl);

    ## Print methods
    &printMethods($r_cl);

    ## Print attributes
    &printAttributes("", $r_cl, $class{attributes});
}

sub printMethods {
    my $r_cl = shift;
    local(%class) = %{$r_cl};

    if ($class{methodCt} == 0) {
	print "No methods.\n";
    } else {
	$i = 0;
	print "Methods:\n";
	while ($i < $class{methodCt}) {
	    my %method = %{$class{methods}[$i]};

	    my $accflags = ACCFlagsToString($method{accessFlags}, 0);
	    my $name = $class{constantPool}[$method{nameIndex}]->{val};
	    my $desc = $class{constantPool}[$method{descriptorIndex}]->{val};

	    if ($detailedMethods) {
		print ("\t$i: ");
		print (".accessFlags=$accflags; ");
		print (".name @ $method{nameIndex} ($name); ");
		print (".descriptor @ $method{descriptorIndex} ($desc); ");
		print (".attrCt = $method{attributesCt};\n");
		&printAttributes("\t\t", \%class, $method{attributes});
	    } else {
		print ("\t$accflags $name $desc\n");
	    }
	} continue {
	    $i++;
	}
    }
}

sub printFields {
    my $r_cl = shift;
    local(%class) = %{$r_cl};

    if ($class{fieldCt} == 0) {
	print "No fields.\n";
    } else {
	$i = 0;
	print "Fields:\n";
	while ($i < $class{fieldCt}) {
	    my %field = %{$class{fields}[$i]};

	    my $accflags = ACCFlagsToString($field{accessFlags}, 0);
	    my $name = $class{constantPool}[$field{nameIndex}]->{val};
	    my $desc = $class{constantPool}[$field{descriptorIndex}]->{val};

	    if ($detailedFields) {
		print ("\t$i: ");
		print (".accessFlags=$accflags; ");
		print (".name @ $field{nameIndex} ($name); ");
		print (".descriptor @ $field{descriptorIndex} ($desc); ");
		print (".attrCt = $field{attributesCt};\n");
		&printAttributes("\t\t", \%class, $field{attributes});
	    } else {
		print ("\t$accflags $desc $name\n");
	    }
	} continue {
	    $i++;
	}
    }
}

sub printInterfaces {
    my $r_cl = shift;
    local(%class) = %{$r_cl};

    if ($class{interfaceCt} == 0) {
	print "No interfaces.\n";
    } else {
	my $i = 0;
	print "Interfaces:\n";
	while ($i < $class{interfaceCt}) {
	    my $iClass = $class{interfaces}[$i];
	    my %iClassConst = %{$class{constantPool}[$iClass]};
	    my $iClassName = $iClassConst{nameIndex};
	    my $interfaceName = %{$class{constantPool}[$iClassName]}->{val};
	    print "\t$i] @ $iClass ($interfaceName)\n";
	} continue {
	    $i++;
	}
    }
}

sub ACCFlagsToString {
    my $flags = shift;
    my $isClass = shift;
    my @flags = ();

    push(@flags, "public") if $flags & $ACC_PUBLIC;
    push(@flags, "private") if $flags & $ACC_PRIVATE;
    push(@flags, "protected") if $flags & $ACC_PROTECTED;
    push(@flags, "abstract") if $flags & $ACC_ABSTRACT;
    push(@flags, "static") if $flags & $ACC_STATIC;
    push(@flags, "final") if $flags & $ACC_FINAL;
    if ($isClass) {
	push(@flags, "super") if $flags & $ACC_SUPER;
    }
    else {
	push(@flags, "synchronized") if $flags & $ACC_SYNCHRONIZED;
    }
    push(@flags, "native") if $flags & $ACC_NATIVE;
    push(@flags, "volatile") if $flags & $ACC_VOLATILE;
    push(@flags, "transient") if $flags & $ACC_TRANSIENT;
    push(@flags, "strictfp") if $flags & $ACC_STRICT;
    push(@flags, "interface") if $flags & $ACC_INTERFACE;
    push(@flags, "UNKNOWN") if $flags & $ACC_UNKNOWN;

    return join(',', @flags);
}

sub printConstantPool {
    my $r_cl = shift;
    local(%class) = %{$r_cl};		# cvt the class reference to the class hash

    print("Constant Pool Entries: $class{constantPoolCt}\n");

    $i = 1;
    while ($i < $class{constantPoolCt}) {
	my %cpEntry = %{$class{constantPool}[$i]};

	print "$i] $CONSTANTNames{$cpEntry{tag}}: ";

    	if ($cpEntry{tag} eq $CONSTANT_Class) {
	    my $ni = $cpEntry{nameIndex};

    	    &checkIndex($ni, "Name", $CONSTANT_Utf8);

	    my $nm = $class{constantPool}[$ni]->{val};

	    print (".name @ $ni ($nm);");

    	} elsif (($cpEntry{tag} eq $CONSTANT_FieldRef)
		 || ($cpEntry{tag} eq $CONSTANT_MethodRef)
		 || ($cpEntry{tag} eq $CONSTANT_InterfaceMethodRef)) {
    	    &checkIndex($cpEntry{classIndex}, "Class", $CONSTANT_Class);
    	    &checkIndex($cpEntry{nameTypeIndex}, "Name & Type", $CONSTANT_NameAndType);

	    print (".class @ $cpEntry{classIndex}; .name&type @ $cpEntry{nameTypeIndex};");
    	} elsif ($cpEntry{tag} eq $CONSTANT_String) {
	    my $si = $cpEntry{stringIndex};
    	    &checkIndex($si, "String", $CONSTANT_Utf8);

	    my $str = $class{constantPool}[$si]->{val};

	    print (".string @ $cpEntry{stringIndex} ($str);");
    	} elsif ($cpEntry{tag} eq $CONSTANT_NameAndType) {
	    my $ni = $cpEntry{nameIndex};
	    my $di = $cpEntry{descriptorIndex};

    	    &checkIndex($ni, "Name", $CONSTANT_Utf8);
    	    &checkIndex($di, "Descriptor", $CONSTANT_Utf8);

	    my $nstr = $class{constantPool}[$ni]->{val};
	    my $dstr = $class{constantPool}[$di]->{val};

	    print (".name @ $ni ($nstr); .descriptor @ $di ($dstr); ");
    	} elsif ($cpEntry{tag} eq $CONSTANT_Integer) {
	    print (".value = $cpEntry{val}");
    	} elsif ($cpEntry{tag} eq $CONSTANT_Utf8) {
	    print (".length=$cpEntry{len}; ");
	    print (".val=$cpEntry{val}; ");
    	} elsif ($cpEntry{tag} eq $CONSTANT_Float) {
	    print (".val=$cpEntry{strVal}; ");
    	} elsif ($cpEntry{tag} eq $CONSTANT_Double) {
	    print (".val=$cpEntry{strVal}; ");
	    $i++; ## Ick.  8-byte entries take two constant pool entries
    	} elsif ($cpEntry{tag} eq $CONSTANT_Long) {
	    print (".val=$cpEntry{strVal}; ");
	    $i++; ## Ick.  8-byte entries take two constant pool entries
    	} else {
    	    &fatal("Unknown Constant type $cpEntry{tag}!\n");
    	}

	print ("\n");

	$i++;
    }
}

sub printAttributes {
    my ($prefix, $r_class, $r_attrs) = @_;

    return if (!defined($r_attrs));

    my $i = 0;
    my %class = %{$r_class};

    print ("${prefix}Attributes:\n");
    foreach $r_attr (@{$r_attrs}) {

	my $name = $class{constantPool}[$r_attr->{nameIndex}]->{val};
	print ("${prefix}\t.name=$name; ");
	print (".length=" . $r_attr->{len} . ";");

	if ($name eq 'SourceFile') {
	  if ($r_attr->{len} != 2) {
	    print ("!Badly formed SourceFile Attribute, must be 2!");
	  } else {
	    my ($high, $low) = unpack("CC", $r_attr->{attr});
	    my $idx = ($high * 256) + $low;
	    my $name = $class{constantPool}[$idx]->{val};
	    print (" @ " . $idx . " (\"" . $name . "\")");
	  }
	}
	elsif ($name eq 'InnerClasses') {
	    my ($high, $low) = unpack("CC", $r_attr->{attr});
	    my $nr = ($high * 256) + $low;

	    print (" @ $nr entries");

	    my $array = substr ($r_attr->{attr}, 2);
	    for (my $i = 0; $i < $nr; $i++, $array = substr ($array, 8)) {
		my ($inner, $outer, $name, $acces) = unpack ("nnnn", $array);

		print("\n");
		# print("${prefix}\t\t$i] $inner, $outer, $name, $acces");

		print("${prefix}\t\t$i]");
		print(" .inner = " . $inner);
		print(" (" . $class{constantPool}[$class{constantPool}[$inner]->{nameIndex}]->{val} . ")") if $inner;

		print(" .outer = $outer");
		print(" (" . $class{constantPool}[$class{constantPool}[$outer]->{nameIndex}]->{val} . ")") if $outer;

		print(" .name = $name");
		print(" ($class{constantPool}[$name]->{val})") if ($name);

		print(" .acces = $acces (" . &ACCFlagsToString($acces, 1) .")");
	    }
	}
	print ("\n");
    }
}

###
### Read Class function
###

sub readClass {
    my $classFile = shift;
    local(%class) = (());

    open(CLASSIN, $classFile)
	|| open(CLASSIN, "${classFile}.class")
	    || die ("Cannot open $classFile for reading");

    ###
    ### Header Magic
    ###

    $class{magic} = read_u4();
    if ($class{magic} != $classMagic) {
    	fatal("Bad class magic '$class{magic}' --expected '$classMagic'.  $classFile is probably not a Java class file.");
    }

    ## Read in the major and minor version numbers
    $class{minorVersion} = &read_u2();
    $class{majorVersion} = &read_u2();

    print("Version: $class{majorVersion}.$class{minorVersion}  (expected 45.3)\n")
    	if ($class{minorVersion} ne 3) || ($class{majorVersion} ne 45);

    ###
    ### Constant Pool
    ###
    $class{constantPoolCt} = &read_u2();
    $class{constantPool} = [];

    $i = 1; # constant pool actually starts with entry 1...
    while ($i < $class{constantPoolCt}) {
    	my %cpEntry;
    	$cpEntry{tag} = &read_u1();

    	if ($cpEntry{tag} eq $CONSTANT_Class) {
    	    $cpEntry{nameIndex} = &read_u2();

    	    &checkIndex($cpEntry{nameIndex}, "Name");
    	} elsif ($cpEntry{tag} eq $CONSTANT_FieldRef) {
    	    $cpEntry{classIndex} = &read_u2();
    	    $cpEntry{nameTypeIndex} = &read_u2();

    	    &checkIndex($cpEntry{classIndex}, "Class");
    	    &checkIndex($cpEntry{nameTypeIndex}, "Name & Type");
    	} elsif ($cpEntry{tag} eq $CONSTANT_MethodRef) {
    	    $cpEntry{classIndex} = &read_u2();
    	    $cpEntry{nameTypeIndex} = &read_u2();

    	    &checkIndex($cpEntry{classIndex}, "Class");
    	    &checkIndex($cpEntry{nameTypeIndex}, "Name & Type");
    	} elsif ($cpEntry{tag} eq $CONSTANT_InterfaceMethodRef) {
    	    $cpEntry{classIndex} = &read_u2();
    	    $cpEntry{nameTypeIndex} = &read_u2();

    	    &checkIndex($cpEntry{classIndex}, "Class");
    	    &checkIndex($cpEntry{nameTypeIndex}, "Name & Type");
    	} elsif ($cpEntry{tag} eq $CONSTANT_String) {
    	    $cpEntry{stringIndex} = &read_u2();

    	    &checkIndex($cpEntry{stringIndex}, "String");
    	} elsif ($cpEntry{tag} eq $CONSTANT_NameAndType) {
    	    $cpEntry{nameIndex} = &read_u2();
    	    $cpEntry{descriptorIndex} = &read_u2();

    	    &checkIndex($cpEntry{nameIndex}, "Name");
    	    &checkIndex($cpEntry{descriptorIndex}, "Descriptor");
    	} elsif ($cpEntry{tag} eq $CONSTANT_Integer) {
    	    $cpEntry{val} = &read_u4();
    	} elsif ($cpEntry{tag} eq $CONSTANT_Utf8) {
    	    $cpEntry{len} = &read_u2();
    	    $cpEntry{val} = &read_utf8($cpEntry{len});
    	} elsif ($cpEntry{tag} eq $CONSTANT_Float) {
	    $cpEntry{val} = &read_u4();
	    $cpEntry{strVal} = &read_float($cpEntry{val});
    	} elsif ($cpEntry{tag} eq $CONSTANT_Double) {
	    $cpEntry{val} = &read_u8();
	    $cpEntry{strVal} = &read_double($cpEntry{val});
    	} elsif ($cpEntry{tag} eq $CONSTANT_Long) {
	    $cpEntry{val} = &read_u8();
	    $cpEntry{strVal} = "<Unknown>";
    	} else {
    	    &fatal("Unknown Constant type $cpEntry{tag}!\n");
    	}

    	$class{constantPool}[$i] = \%cpEntry;

	## Ick.  8-byte entries take two constant pool entries
	$i++ if (($cpEntry{tag} == $CONSTANT_Long)
		 || ($cpEntry{tag} == $CONSTANT_Double));
    } continue {
    	$i++;
    }

    ###
    ### Misc. Class Info
    ###

    $class{accessFlags} = &read_u2();

    $class{thisClass} = &read_u2();
    &checkIndex($class{thisClass}, "this_class", $CONSTANT_Class);

    $class{superClass} = &read_u2();
    if ($class{superClass} != 0) {
      &checkIndex($class{superClass}, "super_class", $CONSTANT_Class);
    }
    # so what if it's java.lang.Object
    # else {
    #  print ("Warning: class has no super class.  Must be java.lang.Object\n");
    #}

    ###
    ### Direct super-interfaces
    ###
    $class{interfaceCt} = &read_u2();
    $class{interfaces} = [];

    $i = 0;
    while ($i < $class{interfaceCt}) {
    	$class{interfaces}[$i] = &read_u2();

    	&checkIndex($class{interfaces}[$i], "Interface \#$i", $CONSTANT_Class);
    } continue {
    	$i++;
    }

    ###
    ### Fields
    ###
    $class{fieldCt} = &read_u2();
    $class{fields} = [];

    $i = 0;
    while ($i < $class{fieldCt}) {
    	my %field;
    	$field{accessFlags} = &read_u2();
    	$field{nameIndex} = &read_u2();
    	$field{descriptorIndex} = &read_u2();
    	$field{attributesCt} = &read_u2();
    	$field{attributes} = &readAttributes($field{attributesCt});

    	&checkIndex($field{nameIndex}, "Field Name", $CONSTANT_Utf8);
    	&checkIndex($field{descriptorIndex}, "Field Descriptor", $CONSTANT_Utf8);

    	$class{fields}[$i] = \%field;
    } continue {
    	$i++;
    }

    ###
    ### Methods
    ###
    $class{methodCt} = &read_u2();
    $class{methods} = [];

    $i = 0;
    while ($i < $class{methodCt}) {
    	my %method;

    	$method{accessFlags} = &read_u2();
    	$method{nameIndex} = &read_u2();
    	$method{descriptorIndex} = &read_u2();
    	$method{attributesCt} = &read_u2();
    	$method{attributes} = &readAttributes($method{attributesCt});

    	&checkIndex($method{nameIndex}, "Method Name", $CONSTANT_Utf8);
    	&checkIndex($method{descriptorIndex}, "Method Descriptor", $CONSTANT_Utf8);

    	$class{methods}[$i] = \%method;
    } continue {
    	$i++;
    }

    ###
    ### Class attributes
    ###
    $class{attributesCt} = &read_u2();
    $class{attributes} = &readAttributes($class{attributesCt});

    ###
    ### End of .class file
    ###

    return \%class;
}

###
### Write Class function
###

sub writeClass {
    my $r_class = shift;
    my $classFile = shift;
    local(%class) = %{$r_class};

    if ($classFile =~ /\.class$/) {
	open(CLASSOUT, ">$classFile")
	    || die ("Cannot open $classFile for writing");
    } else {
	open(CLASSOUT, ">$classFile.class")
	    || die ("Cannot open $classFile.class for writing");
    }

    ###
    ### Header Magic
    ###

    if ($class{magic} != $classMagic) {
    	fatal("Bad class magic '$class{magic}' --expected '$classMagic'.  Not writing class file.");
    }

    &write_u4($class{magic});

    ## Write major/minor version numbers
    &write_u2($class{minorVersion});
    &write_u2($class{majorVersion});

    ###
    ### Constant Pool
    ###
    &write_u2($class{constantPoolCt});

    $i = 1; # constant pool actually starts with entry 1...
    while ($i < $class{constantPoolCt}) {
    	my %cpEntry = %{$class{constantPool}[$i]};
    	&write_u1($cpEntry{tag});

    	if ($cpEntry{tag} eq $CONSTANT_Class) {
    	    &checkIndex($cpEntry{nameIndex}, "Name", $CONSTANT_Utf8);

    	    &write_u2($cpEntry{nameIndex});
    	} elsif (($cpEntry{tag} eq $CONSTANT_FieldRef)
		 || ($cpEntry{tag} eq $CONSTANT_MethodRef)
		 || ($cpEntry{tag} eq $CONSTANT_InterfaceMethodRef)) {
    	    &checkIndex($cpEntry{classIndex}, "Class", $CONSTANT_Class);
    	    &checkIndex($cpEntry{nameTypeIndex}, "Name & Type", $CONSTANT_NameAndType);

    	    &write_u2($cpEntry{classIndex});
    	    &write_u2($cpEntry{nameTypeIndex});
    	} elsif ($cpEntry{tag} eq $CONSTANT_String) {
    	    &checkIndex($cpEntry{stringIndex}, "String", $CONSTANT_Utf8);

    	    &write_u2($cpEntry{stringIndex});
    	} elsif ($cpEntry{tag} eq $CONSTANT_NameAndType) {
    	    &checkIndex($cpEntry{nameIndex}, "Name", $CONSTANT_Utf8);
    	    &checkIndex($cpEntry{descriptorIndex}, "Descriptor", $CONSTANT_Utf8);

    	    &write_u2($cpEntry{nameIndex});
    	    &write_u2($cpEntry{descriptorIndex});
    	} elsif ($cpEntry{tag} eq $CONSTANT_Integer) {
    	    &write_u4($cpEntry{val});
    	} elsif ($cpEntry{tag} eq $CONSTANT_Utf8) {
    	    &write_u2($cpEntry{len});
    	    &write_utf8($cpEntry{val});
    	} elsif ($cpEntry{tag} eq $CONSTANT_Float) {
	    &write_u4($cpEntry{val});
    	} elsif ($cpEntry{tag} eq $CONSTANT_Double) {
	    &write_u8($cpEntry{val});
	    $i++; ## Ick.  8-byte entries take two constant pool entries
    	} elsif ($cpEntry{tag} eq $CONSTANT_Long) {
	    &write_u8($cpEntry{val});
	    $i++; ## Ick.  8-byte entries take two constant pool entries
    	} else {
    	    &fatal("Unknown Constant type $cpEntry{tag}!\n");
    	}
    } continue {
    	$i++;
    }

    ###
    ### Misc. Class Info
    ###

    &write_u2($class{accessFlags});
    &write_u2($class{thisClass});
    &write_u2($class{superClass});

    ###
    ### Direct super-interfaces
    ###
    &write_u2($class{interfaceCt});

    $i = 0;
    while ($i < $class{interfaceCt}) {
    	&write_u2($class{interfaces}[$i]);
    } continue {
    	$i++;
    }

    ###
    ### Fields
    ###
    &write_u2($class{fieldCt});

    $i = 0;
    while ($i < $class{fieldCt}) {
    	my %field = %{$class{fields}[$i]};
    	&write_u2($field{accessFlags});
    	&write_u2($field{nameIndex});
    	&write_u2($field{descriptorIndex});
    	&write_u2($field{attributesCt});
    	&writeAttributes($field{attributes});
    } continue {
    	$i++;
    }

    ###
    ### Methods
    ###
    &write_u2($class{methodCt});

    $i = 0;
    while ($i < $class{methodCt}) {
    	my %method = %{$class{methods}[$i]};

    	&write_u2($method{accessFlags});
    	&write_u2($method{nameIndex});
	&write_u2($method{descriptorIndex});
    	&write_u2($method{attributesCt});
    	&writeAttributes($method{attributes});
    } continue {
    	$i++;
    }

    ###
    ### Class attributes
    ###
    &write_u2($class{attributesCt});
    &writeAttributes($class{attributes});

    ###
    ### End of .class file
    ###

    return \%class;
}

###
### Integrity check functions
###

sub checkIndex {
    my ($val, $name, $type) = @_;

    # $class is a global

    if ($val == 0) {
      &fatal("ERROR: Found constant pool index 0 for $name.  (Expecting a CONSTANT_$CONSTANTNames{$type} entry.)");
    }

    if ($val >= $class{constantPoolCt}) {
	&fatal("ERROR: $name index for current constant is $val, must be less than $class{constantPoolCt}\n");
    }

    if (defined($type)) {
	my $actualTag = $class{constantPool}[$val]{tag};
	if ($actualTag != $type) {
	    &fatal("ERROR: $name expects a CONSTANT_$CONSTANTNames{$type} entry at $val, but found a CONSTANT_$CONSTANTNames{$actualTag} entry\n");
	}
    }
}

###
### Read primitives
###

sub read_u8 {
    my $long = 0;
    (read(CLASSIN, $long, 8) == 8) || die ("premature eof in read_u8()\n");
    my ($b1, $b2, $b3, $b4, $b5, $b6, $b7, $b8) = unpack("CCCCCCCC", $long);
    return (($b1 << 56) + ($b2 << 48) + ($b3 << 40) + ($b4 << 32)
	    + ($b5 << 24) + ($b6 << 16) + ($b7 << 8) + $b8);
}

sub read_u4 {
    my $long = 0;
    (read(CLASSIN, $long, 4) == 4) || die ("premature eof in read_u4()\n");
    my ($top, $highmid, $lowmid, $low) = unpack("CCCC", $long);
    return ($top * (256*256*256)) + ($highmid * (256*256)) + ($lowmid * 256) + $low;
}

sub read_u2 {
    my $short = 0;
    (read(CLASSIN, $short, 2) == 2) || die ("premature eof in read_u2()\n");
    my ($high, $low) = unpack("CC", $short);
    #print("read_u2: $high, $low\n");
    return ($high * 256) + $low;
}

sub read_u1 {
    my $byte = 0;
    (read(CLASSIN, $byte, 1) == 1) || die ("premature eof in read_u1()\n");
    my $val = unpack("C", $byte);
    return $val;
}

sub read_n {
    my $byteCt = shift;
    my $foo = '';
    (read(CLASSIN, $foo, $byteCt) == $byteCt) || die ("premature eof in read_n($byteCt)\n");

    return $foo;
}

sub read_float {
    my $intVal = shift;

    return "+INF" if ($intVal == 0x7f800000);
    return "-INF" if ($intVal == 0xff800000);
    if ((($intVal >= 0x7f800001) && ($intVal <= 0x7fffffff))
	|| (($intVal >= 0xff800001) && ($intVal <= 0xffffffff))) {
	return "NaN";
    }

    ## Otherwise, convert to a floating point number
    $sign     = (($intVal >> 31) == 0) ?  1 : -1;
    $exponent = (($intVal >> 23) & 0xFF);
    $mantissa = ($exponent == 0) ? ($intVal & 0x7fffff) << 1 : ($intVal & 0x7fffff) | 0x800000;

    return $sign * $mantissa * 2 ** ($exponent - 150);
}

sub read_double {
    my $intVal = shift;

    return "+INF" if ($intVal == 0x7f800000);
    return "-INF" if ($intVal == 0xff800000);
    if ((($intVal >= 0x7f800001) && ($intVal <= 0x7fffffff))
	|| (($intVal >= 0xff800001) && ($intVal <= 0xffffffff))) {
	return "NaN";
    }

    ## Otherwise, convert to a floating point number
    $sign     = (($intVal >> 31) == 0) ?  1 : -1;
    $exponent = (($intVal >> 23) & 0xFF);
    $mantissa = ($exponent == 0) ? ($intVal & 0x7fffff) << 1 : ($intVal & 0x7fffff) | 0x800000;

    return $sign * $mantissa * 2 ** ($exponent - 150);
}

sub read_utf8 {
    my $byteCt = shift;
    my $utf = '';
    (read(CLASSIN, $utf, $byteCt) == $byteCt) || die ("premature eof in read_utf8($byteCt)\n");

    my $str = unpack("A$byteCt", $utf);
    return $str;
}

sub readAttributes {
    my ($ct) = @_;

    my @attrs = [];

    return undef if ($ct < 1);

    my $i = 0;
    while ($i < $ct) {
	my %attribute;
        $attribute{nameIndex} = &read_u2();
	$attribute{len} = &read_u4();
	$attribute{attr} = &read_n($attribute{len});

	&checkIndex($attribute{nameIndex}, "Attribute Name", $CONSTANT_Utf8);

	$attrs[$i] = \%attribute;
    } continue {
	$i++;
    }

    return \@attrs;
}

###
### Write primitives
###

sub write_u8 {
    my $val = shift;
    $b8 = $val & 255;
    $b7 = ($val >> 8) & 255;
    $b6 = ($val >> 16) & 255;
    $b5 = ($val >> 24) & 255;
    $b4 = ($val >> 32) & 255;
    $b3 = ($val >> 40) & 255;
    $b2 = ($val >> 48) & 255;
    $b1 = ($val >> 56) & 255;
    print CLASSOUT pack("CCCCCCCC", $b1, $b2, $b3, $b4, $b5, $b6, $b7, $b8);
}

sub write_u4 {
    my $val = shift;
    print CLASSOUT pack("N", $val);
}

sub write_u2 {
    my $val = shift;
    print CLASSOUT pack("n", $val);
}

sub write_u1 {
    my $byte = shift;
    print CLASSOUT pack("C", $byte);
}

sub write_n {
    my $val = shift;
    print CLASSOUT $val;	# XXX assumes $val."length" is 'n'
}

sub write_utf8 {
    my $utf8 = shift;
    print CLASSOUT $utf8
}

sub writeAttributes {
    my $r_attrs = shift;

    return if (!defined $r_attrs);

    my $i = 0;
    foreach $r_attr (@{$r_attrs}) {
	my %attribute = %{$r_attr};
	&write_u2($attribute{nameIndex});
	&write_u4($attribute{len});
	&write_n($attribute{attr});
    }
}

sub fatal {
    print STDERR @_;
    print STDERR "\n";
    exit 11;
}

1;

# eof
