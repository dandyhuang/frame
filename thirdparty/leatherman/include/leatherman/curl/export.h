
#ifndef LEATHERMAN_CURL_EXPORT_H
#define LEATHERMAN_CURL_EXPORT_H

#ifdef LEATHERMAN_CURL_STATIC_DEFINE
#  define LEATHERMAN_CURL_EXPORT
#  define LEATHERMAN_CURL_NO_EXPORT
#else
#  ifndef LEATHERMAN_CURL_EXPORT
#    ifdef leatherman_curl_EXPORTS
        /* We are building this library */
#      define LEATHERMAN_CURL_EXPORT 
#    else
        /* We are using this library */
#      define LEATHERMAN_CURL_EXPORT 
#    endif
#  endif

#  ifndef LEATHERMAN_CURL_NO_EXPORT
#    define LEATHERMAN_CURL_NO_EXPORT 
#  endif
#endif

#ifndef LEATHERMAN_CURL_DEPRECATED
#  define LEATHERMAN_CURL_DEPRECATED 
#endif

#ifndef LEATHERMAN_CURL_DEPRECATED_EXPORT
#  define LEATHERMAN_CURL_DEPRECATED_EXPORT LEATHERMAN_CURL_EXPORT LEATHERMAN_CURL_DEPRECATED
#endif

#ifndef LEATHERMAN_CURL_DEPRECATED_NO_EXPORT
#  define LEATHERMAN_CURL_DEPRECATED_NO_EXPORT LEATHERMAN_CURL_NO_EXPORT LEATHERMAN_CURL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LEATHERMAN_CURL_NO_DEPRECATED
#    define LEATHERMAN_CURL_NO_DEPRECATED
#  endif
#endif

#endif /* LEATHERMAN_CURL_EXPORT_H */
