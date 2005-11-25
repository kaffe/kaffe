/*
 * l4_loader.h
 * Support for shared libraries
 * 
 * Copyright (c) 2004, 2005
 *	TU Dresden, Operating System Group.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * written by Alexander Boettcher <ab764283@os.inf.tu-dresden.de>
 */

#ifndef __l4_loader_h
#define __l4_loader_h

#if defined(DROPS_SHARED_LIBRARIES)
  #include <l4/l4vfs/basic_name_server.h>
  #include <l4/log/l4log.h> /* LOG */
  #include <l4/util/util.h> /* for l4_sleep*/
  #include <l4/sys/types.h>
  #include <l4/l4vfs/name_server.h>

  #include <stdio.h>
  #include <l4/names/libnames.h>
  #include <l4/loader/loader-client.h>
  #include <l4/env/env.h>
  #include <l4/loader/loader.h>


  #define LIBRARYHANDLE                   void *

  extern const struct {
 		const char *name;
		void * address;
  }
  lt_preloaded_symbols[];

  envpage_t envpage;

  static inline void *
  KaffeLib_Load(const char *LIB){
    LIBRARYHANDLE HAND = 0;
    unsigned int error,len;							
    CORBA_Environment _env = dice_default_environment;			
    l4env_infopage_t *env;						
    l4_threadid_t loader_id,fprov_id;				
    char * lpath = getenv(LIBRARYPATH);                                 
									
    if (lpath != 0){
      len = strlen(lpath);
      if (memcmp(LIB,lpath,len)==0){
        if (memcmp(LIB+len,"/libnative",10)==0)
          HAND = (LIBRARYHANDLE) 1;
        else if (memcmp(LIB+len,"/libjavaio.so",13)==0)
          HAND = (LIBRARYHANDLE) 1;
        else if (memcmp(LIB+len,"/libjavanio.so",14)==0)
          HAND = (LIBRARYHANDLE) 1;
        else if (memcmp(LIB+len,"/libnet",7)==0)
          HAND = (LIBRARYHANDLE) 1;
        else if (memcmp(LIB+len,"/libqtawt",9)==0)
          HAND = (LIBRARYHANDLE) 1;
        else if (memcmp(LIB+len,"/libjavalang",12)==0)
          HAND = (LIBRARYHANDLE) 1;
      }
    }

    if (HAND == NULL){
     if (names_query_name("fprov_proxy_fs",&fprov_id)){
      if (!names_waitfor_name("LOADER", &loader_id, 5000))
       LOG_Error("Dynamic loader LOADER not found\n");
      else
       if ((error = l4loader_app_lib_open_call(&loader_id, LIB,	
                                 &fprov_id, 0, &envpage, &_env))	
                 || _env.major != CORBA_NO_EXCEPTION){			
        LOG_Error("Loading lib failed (error %d, exc=%d.%d)\n",		
                                error, _env.major, _env.repos_id);	
       }else{								
        env = (l4env_infopage_t*)envpage;					
									
        l4loader_attach_relocateable(env);				
 									
        if ((error = l4loader_app_lib_link_call(&loader_id, &envpage, &_env))	
	      || _env.major != CORBA_NO_EXCEPTION){				
          LOG_Error("Linking lib failed (error %d, exc=%d.%d)\n",		
                          error, _env.major, _env.repos_id);		
        }else HAND = (LIBRARYHANDLE) 1;
       }
     }
    }
    return HAND;
  }

  static inline void
  KaffeLib_Unload(LIBRARYHANDLE handle UNUSED)
  {
  }

  static inline void *
  KaffeLib_GetSymbol(LIBRARYHANDLE handle UNUSED, const char *STUB)
  {
    void * FUNC = 0;
    int z;                                                  
    int error;					
    l4_addr_t addr = 0;					
    CORBA_Environment _env = dice_default_environment;			
    l4_threadid_t loader_id;				

    for (z = 0; lt_preloaded_symbols[z].name != 0; z++) {		
      if (strcmp(lt_preloaded_symbols[z].name, STUB) == 0) {	
        FUNC = (void *)lt_preloaded_symbols[z].address;	
        break;                                  
      } 
    }  
    if (FUNC==0){							
      if (!names_waitfor_name("LOADER", &loader_id, 5000)){	
        LOG_Error("Dynamic loader LOADER not found\n");	
      }else{					
        if ((error = l4loader_app_lib_dsym_call(&loader_id, STUB,
                                    &envpage, &addr, &_env))	
		      || _env.major != CORBA_NO_EXCEPTION){
	//		LOG("%s not found",STUB);
        }else
        {
          FUNC = (void *)addr;
        }
      }
    }

    return FUNC;
  }  

  static inline const char *
  KaffeLib_GetError(void)
  {
    return "No specific error support";
  }

  static inline void
  KaffeLib_Init(void)
  {
  }

#endif

#endif /* __l4_loader_h */

