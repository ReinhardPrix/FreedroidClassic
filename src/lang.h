#ifndef _LANG_H
#define _LANG_H

#ifndef HAVE_LOCALE_H
#undef ENABLE_NLS
#endif

#if ENABLE_NLS

#include <locale.h>
#include <libintl.h>

#define LOCALE_PACKAGE "freedroidrpg"

#define LOCALE_DIR FD_DATADIR"/locale/"

#define _(String) gettext(String)
#define N_(String) String

#else

#define _(String) String
#define N_(String) String

#endif

#endif
