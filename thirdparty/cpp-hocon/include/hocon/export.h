
#ifndef LIBCPP_HOCON_EXPORT_H
#define LIBCPP_HOCON_EXPORT_H

#ifdef LIBCPP_HOCON_STATIC_DEFINE
#  define LIBCPP_HOCON_EXPORT
#  define LIBCPP_HOCON_NO_EXPORT
#else
#  ifndef LIBCPP_HOCON_EXPORT
#    ifdef libcpp_hocon_EXPORTS
        /* We are building this library */
#      define LIBCPP_HOCON_EXPORT 
#    else
        /* We are using this library */
#      define LIBCPP_HOCON_EXPORT 
#    endif
#  endif

#  ifndef LIBCPP_HOCON_NO_EXPORT
#    define LIBCPP_HOCON_NO_EXPORT 
#  endif
#endif

#ifndef LIBCPP_HOCON_DEPRECATED
#  define LIBCPP_HOCON_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef LIBCPP_HOCON_DEPRECATED_EXPORT
#  define LIBCPP_HOCON_DEPRECATED_EXPORT LIBCPP_HOCON_EXPORT LIBCPP_HOCON_DEPRECATED
#endif

#ifndef LIBCPP_HOCON_DEPRECATED_NO_EXPORT
#  define LIBCPP_HOCON_DEPRECATED_NO_EXPORT LIBCPP_HOCON_NO_EXPORT LIBCPP_HOCON_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LIBCPP_HOCON_NO_DEPRECATED
#    define LIBCPP_HOCON_NO_DEPRECATED
#  endif
#endif

#endif /* LIBCPP_HOCON_EXPORT_H */
