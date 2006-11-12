#ifndef __i386_drops_md_h
 #define __i386_drops_md_h
 #ifdef USE_DIETLIBC
  /**
   * dietlibc does not provide u_int
   */
  typedef unsigned int  u_int;
 #endif

 #include "i386/common.h"
 #include "i386/sysdepCallMethod.h"
 #include "i386/threads.h"
 void drops_kaffe_main(int argc, char *argv[]);
 #define MAIN_MD drops_kaffe_main(argc, argv)
#endif
