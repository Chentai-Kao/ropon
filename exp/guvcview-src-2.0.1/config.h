/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* always defined to indicate that i18n is enabled */
#define ENABLE_NLS 1

/* set to 1 if sdl2 is enabled */
#define ENABLE_SDL2 1

/* guvcview */
#define GETTEXT_PACKAGE "guvcview"

/* gview_v4l2core */
#define GETTEXT_PACKAGE_V4L2CORE "gview_v4l2core"

/* set to 1 if gsl is enabled */
#define HAS_GSL 1

/* set to 1 if pulseaudio installed */
#define HAS_PULSEAUDIO 1

/* Define to 1 if you have the <avcodec.h> header file. */
/* #undef HAVE_AVCODEC_H */

/* Define to 1 if you have the `bind_textdomain_codeset' function. */
#define HAVE_BIND_TEXTDOMAIN_CODESET 1

/* Define to 1 if you have the `dcgettext' function. */
#define HAVE_DCGETTEXT 1

/* Define to 1 if you have the `dirfd' function. */
#define HAVE_DIRFD 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* libm includes exp */
#define HAVE_EXP 1

/* Define to 1 if you have the <ffmpeg/avcodec.h> header file. */
/* #undef HAVE_FFMPEG_AVCODEC_H */

/* Define to 1 if you have the `fpathconf' function. */
#define HAVE_FPATHCONF 1

/* Define if the GNU gettext() function is already present or preinstalled. */
#define HAVE_GETTEXT 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if your <locale.h> file defines LC_MESSAGES. */
#define HAVE_LC_MESSAGES 1

/* Define to 1 if you have the <libavcodec/avcodec.h> header file. */
#define HAVE_LIBAVCODEC_AVCODEC_H 1

/* Define to 1 if you have the <libavutil/version.h> header file. */
#define HAVE_LIBAVUTIL_VERSION_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the <math.h> header file. */
#define HAVE_MATH_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* libm includes pow */
#define HAVE_POW 1

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* libm includes roundf */
#define HAVE_ROUNDF 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* set to 1 if builtin decoder is enabled */
/* #undef MJPG_BUILTIN */

/* Name of package */
#define PACKAGE "guvcview"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://guvcview.sourceforge.net/"

/* Define to the full name of this package. */
#define PACKAGE_NAME "guvcview"

/* 2 */
#define PACKAGE_RELEASE "2"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "guvcview 2.0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "guvcview"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.0.1"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* set to 1 if iyuv is enabled */
/* #undef USE_PLANAR_YUV */

/* Version number of package */
#define VERSION "2.0.1"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif
