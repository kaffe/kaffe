#
# Ultrix4.2 MIPS Makefile fragment
#
md$(OBJEXT):	md.c
	$(CC) -O $(ALL_CFLAGS) -c $(PIC) $< -o $@
