#ifndef _ERROR_H_
#define _ERROR_H_

#include "../../include/settings.h"

#ifdef NEED_ERROR_TRANSLATION
#define TRANSLATE_ERROR(E) (translate_error(E))
int translate_error(int error);
#else
#define TRANSLATE_ERROR(E) (E)
#endif

#endif // _ERROR_H_


