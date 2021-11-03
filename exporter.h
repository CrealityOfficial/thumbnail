#ifndef THUMBNAIL_EXPORT
#define THUMBNAIL_EXPORT

#ifdef WIN32
	#ifdef THUMBNAIL_DLL
		#define THUMBNAIL_API __declspec(dllexport)
	#else
		#define THUMBNAIL_API __declspec(dllimport)
	#endif
#else
	#ifdef THUMBNAIL_DLL
		#define THUMBNAIL_API __attribute__((visibility("default")))
	#else
		#define THUMBNAIL_API
	#endif
#endif

#endif // THUMBNAIL_EXPORT