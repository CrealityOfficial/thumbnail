
#ifndef THUMBNAIL_EXPORT
#define THUMBNAIL_EXPORT
#include "ccglobal/export.h"

#if USE_THUMBNAIL_DLL
#define THUMBNAIL_API CC_DECLARE_IMPORT
#elif USE_THUMBNAIL_STATIC
#define THUMBNAIL_API CC_DECLARE_STATIC
#else
#if THUMBNAIL_DLL
#define THUMBNAIL_API CC_DECLARE_EXPORT
#else
#define THUMBNAIL_API CC_DECLARE_STATIC
#endif
#endif

#endif // THUMBNAIL_EXPORT