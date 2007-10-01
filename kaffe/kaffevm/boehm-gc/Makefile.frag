# Used in kaffe/kaffevm/systems/unix-pthreads/Makefile.am as gc_frag.

GC_DEFS= -DGC_THREADS -DKAFFE_BOEHM_GC $(BOEHMGC_SPECIFIC_FLAGS)
