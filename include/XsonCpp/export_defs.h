
#ifndef XSONCPP_EXPORT_H
#define XSONCPP_EXPORT_H

#ifdef XSONCPP_STATIC_DEFINE
#  define XSONCPP_EXPORT
#  define XSONCPP_NO_EXPORT
#else
#  ifndef XSONCPP_EXPORT
#    ifdef XsonCpp_EXPORTS
        /* We are building this library */
#      define XSONCPP_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define XSONCPP_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef XSONCPP_NO_EXPORT
#    define XSONCPP_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef XSONCPP_DEPRECATED
#  define XSONCPP_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef XSONCPP_DEPRECATED_EXPORT
#  define XSONCPP_DEPRECATED_EXPORT XSONCPP_EXPORT XSONCPP_DEPRECATED
#endif

#ifndef XSONCPP_DEPRECATED_NO_EXPORT
#  define XSONCPP_DEPRECATED_NO_EXPORT XSONCPP_NO_EXPORT XSONCPP_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef XSONCPP_NO_DEPRECATED
#    define XSONCPP_NO_DEPRECATED
#  endif
#endif

#endif /* XSONCPP_EXPORT_H */
