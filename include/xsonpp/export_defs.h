
#ifndef XSONPP_EXPORT_H
#define XSONPP_EXPORT_H

#ifdef XSONPP_STATIC_DEFINE
#  define XSONPP_EXPORT
#  define XSONPP_NO_EXPORT
#else
#  ifndef XSONPP_EXPORT
#    ifdef xsonpp_EXPORTS
        /* We are building this library */
#      define XSONPP_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define XSONPP_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef XSONPP_NO_EXPORT
#    define XSONPP_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef XSONPP_DEPRECATED
#  define XSONPP_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef XSONPP_DEPRECATED_EXPORT
#  define XSONPP_DEPRECATED_EXPORT XSONPP_EXPORT XSONPP_DEPRECATED
#endif

#ifndef XSONPP_DEPRECATED_NO_EXPORT
#  define XSONPP_DEPRECATED_NO_EXPORT XSONPP_NO_EXPORT XSONPP_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef XSONPP_NO_DEPRECATED
#    define XSONPP_NO_DEPRECATED
#  endif
#endif

#endif /* XSONPP_EXPORT_H */
