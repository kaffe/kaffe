
#include "config.h"
#include "gtypes.h"
#include "classMethod.h"
#include "native-wrapper.h"

void
engine_create_wrapper (Method *meth, void *func)
{
	SET_METHOD_NATIVECODE(meth, func);
	return meth;
}
