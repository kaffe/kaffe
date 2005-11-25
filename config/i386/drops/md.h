#ifndef __i386_drops_md_h
 #define __i386_drops_md_h
 /**
  * defined in sys/resource.h
  * at the moment the dietlibc does not provide this file
  */
// typedef unsigned long rlim_t;
 /**
  * at the moment the dietlibc does not provide this
  */
 typedef unsigned int  u_int;

 #include "i386/common.h"
 #include "i386/sysdepCallMethod.h"
 #include "i386/threads.h"
 void drops_kaffe_main(int argc, char *argv[]);
 #define MAIN_MD drops_kaffe_main(argc, argv)


#endif
