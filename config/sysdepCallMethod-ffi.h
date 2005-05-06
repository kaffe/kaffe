/*
 * sysdepCallMethod-ffi.h
 * Implementation of sysdepCallMethod using libffi
 *
 * Copyright (c) 2004
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __sysdepCallMethod_ffi_h__
#define __sysdepCallMethod_ffi_h__

#include <ffi.h>
static inline ffi_type *j2ffi(char type)
{
        ffi_type *ftype;

        switch (type) {
        case 'J':
                ftype = &ffi_type_sint64;
                break;
        case 'D':
                ftype = &ffi_type_double;
                break;
        case 'F':
                ftype = &ffi_type_float;
                break;
        default:
                ftype = &ffi_type_uint;
        }
        return ftype;
}

#define sysdepCallMethod(CALL)                                          \
do {                                                                    \
        int i;                                                          \
        ffi_cif cif;                                                    \
        ffi_type *rtype;                                                \
        ffi_type *argtypes[(CALL)->nrargs + 1];                         \
        void *argvals[(CALL)->nrargs + 1];                              \
        int fargs = 0;                                                  \
        for (i = 0; i < (CALL)->nrargs; i++) {                          \
                switch ((CALL)->callsize[i]) {                          \
                case 1:                                                 \
                        argtypes[fargs] = j2ffi((CALL)->calltype[i]);   \
                        argvals[fargs] = &((CALL)->args[i].i);          \
                        fargs++;                                        \
                        break;                                          \
                case 2:                                                 \
                        argtypes[fargs] = j2ffi((CALL)->calltype[i]);   \
                        argvals[fargs] = &((CALL)->args[i].j);          \
                        fargs++;                                        \
                default:                                                \
                        break;                                          \
                }                                                       \
        }                                                               \
        rtype = j2ffi ((CALL)->rettype);                                \
        if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, fargs,                  \
            rtype, argtypes) == FFI_OK) {                               \
                ffi_call(&cif, (CALL)->function, (CALL)->ret, argvals); \
        }                                                               \
        else                                                            \
                KAFFEVM_ABORT();                                                \
} while (0);

#endif /* __sysdepCallMethod_ffi_h__ */
