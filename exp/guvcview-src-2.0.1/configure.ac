dnl Process this file with autoconf to produce a configure script.

# Save the original $CFLAGS so we can distinguish whether the user set those
# in the environment, or whether autoconf added -O and -g options:
ORIGINAL_CFLAGS="$CFLAGS"

dnl --------------------------------
dnl Initialization macros (sets PACKAGE and VERSION)
dnl --------------------------------
m4_define([guvcview_major], [2])
m4_define([guvcview_minor], [0])
m4_define([guvcview_micro], [1])

m4_define([guvcview_version],
          [guvcview_major.guvcview_minor.guvcview_micro])

AC_INIT([guvcview], [guvcview_version], [http://guvcview.sourceforge.net/])

PACKAGE_RELEASE=2
AC_SUBST(PACKAGE_RELEASE)
AC_DEFINE_UNQUOTED(PACKAGE_RELEASE,"$PACKAGE_RELEASE", 2)

dnl ------------------------------------------------
dnl Pretty output
dnl ------------------------------------------------
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS(config.h)
AM_MAINTAINER_MODE(disable)

dnl -----------------------------------------------
dnl Package release
dnl -----------------------------------------------

# autoconf 2.5x defaults to no cache file; we need the cache file's information
# for building the config page.  But start with it empty to avoid confusion by
# people who don't do a "make distclean" after applying patches.
cache_file=config.cache
rm -f config.cache; touch config.cache

AC_PREFIX_DEFAULT(/usr/local)

AC_PROG_CC
AM_PROG_CC_C_O
AC_HEADER_STDC
AC_PROG_LIBTOOL
AM_SANITY_CHECK

dnl --------------------------------------------------------------------------
dnl set pthread_libs and pthread_cflags
dnl --------------------------------------------------------------------------
dnl ACX_PTHREAD([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])

ACX_PTHREAD()

AC_SUBST(CFLAGS)

dnl --------------------------------------------------------------------------
dnl Set gettext package name
dnl --------------------------------------------------------------------------

GETTEXT_PACKAGE_V4L2CORE=gview_v4l2core
AC_SUBST(GETTEXT_PACKAGE_V4L2CORE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE_V4L2CORE,"$GETTEXT_PACKAGE_V4L2CORE", [gview_v4l2core])

GETTEXT_PACKAGE=guvcview
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [guvcview])

IT_PROG_INTLTOOL([0.40])
IT_PO_SUBDIR(po/gview_v4l2core)

dnl --------------------------------------------------------------------------
dnl Languages supported by guvcview.
dnl --------------------------------------------------------------------------

ALL_LINGUAS="bg bs cs da de en_AU es eu fo fr gl he hr it ja lv nl pl pt pt_BR ru si sr tr uk zh_TW"
AM_GLIB_GNU_GETTEXT([external])

dnl -----------------------------------------------
dnl  check for some required headers
dnl -----------------------------------------------
AC_STDC_HEADERS
AC_HAVE_HEADERS(math.h)
AC_CHECK_LIB([m],[exp],[AC_DEFINE([HAVE_EXP],[1],[libm includes exp])])
AC_CHECK_LIB([m],[roundf],[AC_DEFINE([HAVE_ROUNDF],[1],[libm includes roundf])])
AC_CHECK_LIB([m],[pow],[AC_DEFINE([HAVE_POW],[1],[libm includes pow])])

dnl -----------------------------------------------
dnl libgviewv4l2core name and version number
dnl -----------------------------------------------

GVIEWV4L2CORE_LIBRARY_NAME=libgviewv4l2core
AC_SUBST(GVIEWV4L2CORE_LIBRARY_NAME)
GVIEWV4L2CORE_LD_NAME=gviewv4l2core
AC_SUBST(GVIEWV4L2CORE_LD_NAME)

#release versioning
GVIEWV4L2CORE_MAJOR_VERSION=1
GVIEWV4L2CORE_MINOR_VERSION=0
GVIEWV4L2CORE_MICRO_VERSION=1

#API version (SONAME)
GVIEWV4L2CORE_API_VERSION=$GVIEWV4L2CORE_MAJOR_VERSION.$GVIEWV4L2CORE_MINOR_VERSION
AC_SUBST(GVIEWV4L2CORE_API_VERSION)

#shared library versioning
GVIEWV4L2CORE_LIBRARY_VERSION=$GVIEWV4L2CORE_MAJOR_VERSION:$GVIEWV4L2CORE_MINOR_VERSION:$GVIEWV4L2CORE_MICRO_VERSION
#
#             current:revision:age
#                |       |     |
#                |       |     +- increment if interfaces have been added
#                |       |        set to zero if interfaces have been removed
#                                  or changed
#                |       +- increment if source code has changed
#                |          set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed

AC_SUBST(GVIEWV4L2CORE_LIBRARY_VERSION)

dnl --------------------------------------------------------------------------
dnl check for libgviewv4l2core dependencies
dnl --------------------------------------------------------------------------

PKG_CHECK_MODULES(GVIEWV4L2CORE, [libv4l2 libudev libusb-1.0 libavcodec libpng])
AC_SUBST(GVIEWV4L2CORE_CFLAGS)
AC_SUBST(GVIEWV4L2CORE_LIBS)

dnl --------------------------------------------------------------------------
dnl check internal yuv format (packed yuyv - default) or planar (iyuv) 
dnl --------------------------------------------------------------------------
AC_MSG_CHECKING(if you want to enable iyuv (planar 420) as internal format)
AC_ARG_ENABLE(iyuv, AS_HELP_STRING([--enable-iyuv],
		[enable iyuv (planar 420) as internal format (default: disabled)]),
	[enable_iyuv=yes yuv_format="iyuv (alpha)"],
	[enable_iyuv=no  yuv_format=yuyv])

AC_MSG_RESULT($enable_iyuv)

if test $enable_iyuv = yes; then
	AC_DEFINE(USE_PLANAR_YUV, 1, [set to 1 if iyuv is enabled])
fi

dnl --------------------------------------------------------------------------
dnl Check for avcodec.h installation path
dnl --------------------------------------------------------------------------

dnl Check for avcodec.h directly
dnl if all tests fail must add avcodec.h path to C_INCLUDE_PATH before running configure
CPPFLAGS+="${GVIEWV4L2CORE_CFLAGS}"
AC_CHECK_HEADERS([avcodec.h libavcodec/avcodec.h ffmpeg/avcodec.h])

dnl --------------------------------------------------------------------------
dnl Check if we will use libavcodec or the internal decoder for mjpeg decoding
dnl --------------------------------------------------------------------------
AC_MSG_CHECKING(if you want to use the internal mjpeg decoder over libavcodec )
AC_ARG_ENABLE(builtin-mjpg, AS_HELP_STRING([--enable-builtin-mjpg],
		[enable builtin mjpg decoder support (default: disable)]),
	[enable_builtin_mjpg=yes mjpg_decoder=builtin],
	[enable_builtin_mjpg=no  mjpg_decoder=libavcodec])

AC_MSG_RESULT($enable_builtin_mjpg)


if test $enable_builtin_mjpg = yes; then
	AC_DEFINE(MJPG_BUILTIN, 1, [set to 1 if builtin decoder is enabled])
	AC_MSG_WARN(libavcodec mjpg decoder disabled... the builtin decoder will be used instead.)
fi

dnl -----------------------------------------------
dnl libgviewrender name and version number
dnl -----------------------------------------------

GVIEWRENDER_LIBRARY_NAME=libgviewrender
AC_SUBST(GVIEWRENDER_LIBRARY_NAME)
GVIEWRENDER_LD_NAME=gviewrender
AC_SUBST(GVIEWRENDER_LD_NAME)

#release versioning
GVIEWRENDER_MAJOR_VERSION=1
GVIEWRENDER_MINOR_VERSION=0
GVIEWRENDER_MICRO_VERSION=1

#API version (SONAME)
GVIEWRENDER_API_VERSION=$GVIEWRENDER_MAJOR_VERSION.$GVIEWRENDER_MINOR_VERSION
AC_SUBST(GVIEWRENDER_API_VERSION)

#shared library versioning
GVIEWRENDER_LIBRARY_VERSION=$GVIEWRENDER_MAJOR_VERSION:$GVIEWRENDER_MINOR_VERSION:$GVIEWRENDER_MICRO_VERSION
#
#             current:revision:age
#                |       |     |
#                |       |     +- increment if interfaces have been added
#                |       |        set to zero if interfaces have been removed
#                                  or changed
#                |       +- increment if source code has changed
#                |          set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed

AC_SUBST(GVIEWRENDER_LIBRARY_VERSION)

dnl --------------------------------------------------------------------------
dnl check for libgviewrender dependencies
dnl --------------------------------------------------------------------------

dnl --------------------------------------------------------------------------
dnl Check for prefered sdl version (1 or 2)
dnl --------------------------------------------------------------------------
AC_MSG_CHECKING(if you want to enable sdl2 support)
AC_ARG_ENABLE(sdl2, AS_HELP_STRING([--disable-sdl2],
		[disable sdl2 support (default: enabled)]),
	[enable_sdl2=no],
	[enable_sdl2=yes])

AC_MSG_RESULT($enable_sdl2)

if test $enable_sdl2 = yes; then

	PKG_CHECK_MODULES(GVIEWRENDER, [sdl2 >= 2.0])
	AC_SUBST(GVIEWRENDER_CFLAGS)
	AC_SUBST(GVIEWRENDER_LIBS)

	AC_DEFINE(ENABLE_SDL2, 1, [set to 1 if sdl2 is enabled])
else
	AC_MSG_WARN(sdl2 disabled...  sdl1.2 will be used instead.)
	PKG_CHECK_MODULES(GVIEWRENDER, [sdl >= 1.2.10])
	AC_SUBST(GVIEWRENDER_CFLAGS)
	AC_SUBST(GVIEWRENDER_LIBS)
fi

AM_CONDITIONAL(ENABLE_SDL2, test $enable_sdl2 = yes)

dnl --------------------------------------------------------------------------
dnl Check for gsl (gnu random generator) used by render fx
dnl --------------------------------------------------------------------------
AC_MSG_CHECKING(if you want to enable gsl support)
AC_ARG_ENABLE(gsl, AS_HELP_STRING([--disable-gsl],
		[disable gsl support (default: enabled)]),
	[enable_gsl=no],
	[enable_gsl=yes])

AC_MSG_RESULT($enable_gsl)

if test $enable_gsl = yes; then

	dnl AX_PATH_GSL(
	dnl			1.15,
	dnl			AC_DEFINE(HAS_GSL, 1, [set to 1 if gsl is enabled]),
	dnl			AC_MSG_WARN(libgsl not found... some render fx filters will be disabled.))

	PKG_CHECK_MODULES(GSL, [gsl >= 1.15])
	AC_SUBST(GSL_CFLAGS)
	AC_SUBST(GSL_LIBS)

	AC_DEFINE(HAS_GSL, 1, [set to 1 if gsl is enabled])
else
	AC_MSG_WARN(libgsl disabled... some render fx filters will be disabled.)
fi

dnl -----------------------------------------------
dnl libgviewaudio name and version number
dnl -----------------------------------------------

GVIEWAUDIO_LIBRARY_NAME=libgviewaudio
AC_SUBST(GVIEWAUDIO_LIBRARY_NAME)
GVIEWAUDIO_LD_NAME=gviewaudio
AC_SUBST(GVIEWAUDIO_LD_NAME)

#release versioning
GVIEWAUDIO_MAJOR_VERSION=1
GVIEWAUDIO_MINOR_VERSION=0
GVIEWAUDIO_MICRO_VERSION=1

#API version (SONAME)
GVIEWAUDIO_API_VERSION=$GVIEWAUDIO_MAJOR_VERSION.$GVIEWAUDIO_MINOR_VERSION
AC_SUBST(GVIEWAUDIO_API_VERSION)

#shared library versioning
GVIEWAUDIO_LIBRARY_VERSION=$GVIEWAUDIO_MAJOR_VERSION:$GVIEWAUDIO_MINOR_VERSION:$GVIEWAUDIO_MICRO_VERSION
#
#             current:revision:age
#                |       |     |
#                |       |     +- increment if interfaces have been added
#                |       |        set to zero if interfaces have been removed
#                                  or changed
#                |       +- increment if source code has changed
#                |          set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed

AC_SUBST(GVIEWAUDIO_LIBRARY_VERSION)

dnl --------------------------------------------------------------------------
dnl check for libgviewaudio dependencies
dnl --------------------------------------------------------------------------

PKG_CHECK_MODULES(GVIEWAUDIO, [portaudio-2.0])
AC_SUBST(GVIEWAUDIO_CFLAGS)
AC_SUBST(GVIEWAUDIO_LIBS)

dnl --------------------------------------------------------------------------
dnl Check for pulseaudio
dnl --------------------------------------------------------------------------
AC_MSG_CHECKING(if you want to enable pulse support)
AC_ARG_ENABLE(pulse, AS_HELP_STRING([--disable-pulse],
		[disable pulseaudio support (default: enabled)]),
	[enable_pulse=no],
	[enable_pulse=yes])

AC_MSG_RESULT($enable_pulse)

if test $enable_pulse = yes; then
	PKG_CHECK_MODULES(PULSE, libpulse >= 0.9.15, has_pulse=yes, has_pulse=no)
	AC_SUBST(PULSE_CFLAGS)
	AC_SUBST(PULSE_LIBS)
	if test "$has_pulse" = yes; then
	  AC_DEFINE(HAS_PULSEAUDIO, 1, [set to 1 if pulseaudio installed])
	else
	  AC_MSG_WARN(libpulse missing... pulse support will be disabled.);
	  enable_pulse=no
	fi
fi

AM_CONDITIONAL(HAVE_PULSEAUDIO, test $enable_pulse = yes)

dnl -----------------------------------------------
dnl libgviewencoder name and version number
dnl -----------------------------------------------

GVIEWENCODER_LIBRARY_NAME=libgviewencoder
AC_SUBST(GVIEWENCODER_LIBRARY_NAME)
GVIEWENCODER_LD_NAME=gviewencoder
AC_SUBST(GVIEWENCODER_LD_NAME)

#release versioning
GVIEWENCODER_MAJOR_VERSION=1
GVIEWENCODER_MINOR_VERSION=0
GVIEWENCODER_MICRO_VERSION=1

#API version (SONAME)
GVIEWENCODER_API_VERSION=$GVIEWENCODER_MAJOR_VERSION.$GVIEWENCODER_MINOR_VERSION
AC_SUBST(GVIEWENCODER_API_VERSION)

#shared library versioning
GVIEWENCODER_LIBRARY_VERSION=$GVIEWENCODER_MAJOR_VERSION:$GVIEWENCODER_MINOR_VERSION:$GVIEWENCODER_MICRO_VERSION
#
#             current:revision:age
#                |       |     |
#                |       |     +- increment if interfaces have been added
#                |       |        set to zero if interfaces have been removed
#                                  or changed
#                |       +- increment if source code has changed
#                |          set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed

AC_SUBST(GVIEWENCODER_LIBRARY_VERSION)

dnl --------------------------------------------------------------------------
dnl check for libgviewencoder dependencies
dnl --------------------------------------------------------------------------

PKG_CHECK_MODULES(GVIEWENCODER, [libavcodec, libavutil])
AC_SUBST(GVIEWENCODER_CFLAGS)
AC_SUBST(GVIEWENCODER_LIBS)

dnl --------------------------------------------------------------------------
dnl Check for libavutil/version.h
dnl --------------------------------------------------------------------------

AC_CHECK_HEADERS([libavutil/version.h])

dnl --------------------------------------------------------------------------
dnl check for guvcview dependencies
dnl --------------------------------------------------------------------------

PKG_CHECK_MODULES(GUVCVIEW, [gtk+-3.0 >= 3.0.0 glib-2.0 >= 2.10.0])
AC_SUBST(GUVCVIEW_CFLAGS)
AC_SUBST(GUVCVIEW_LIBS)

AC_CHECK_FUNCS([fpathconf dirfd])

dnl --------------------------------------------------------------------------
dnl set/unset debian menu
dnl --------------------------------------------------------------------------
AC_MSG_CHECKING(if you want to enable debian menu)
AC_ARG_ENABLE(debian-menu, AS_HELP_STRING([--disable-debian-menu],
		[disable debian menu (default: enabled)]),
	[enable_debian_menu=no],
	[enable_debian_menu=yes])

AC_MSG_RESULT($enable_debian_menu)

AM_CONDITIONAL(DEBIAN_MENU, test "$enable_debian_menu" = yes)

dnl --------------------------------------------------------------------------
dnl set/unset desktop file
dnl --------------------------------------------------------------------------
AC_MSG_CHECKING(if you want to enable desktop file)
AC_ARG_ENABLE(desktop, AS_HELP_STRING([--disable-desktop],
		[disable desktop file (default: enabled)]),
	[enable_desktop=no],
	[enable_desktop=yes])

AC_MSG_RESULT($enable_desktop)

AM_CONDITIONAL(DESKTOP, test "$enable_desktop" = yes)

dnl --------------------------------------------------------------------------
dnl Check if the processor stores words with the most significant byte first
dnl (like Motorola and SPARC, unlike Intel and VAX).
dnl --------------------------------------------------------------------------

AC_C_BIGENDIAN

dnl -----------------------------------------------
dnl Generates Makefiles, configuration files and scripts
dnl -----------------------------------------------

AC_CONFIG_FILES([
	Makefile
    pkgconfig/libgviewv4l2core.pc
    pkgconfig/libgviewaudio.pc
    pkgconfig/libgviewrender.pc
    pkgconfig/libgviewencoder.pc
    gview_v4l2core/Makefile
    gview_audio/Makefile
    gview_render/Makefile
    gview_encoder/Makefile
    guvcview/Makefile
    data/Makefile
    data/icons/Makefile
    data/guvcview.desktop.in
    data/guvcview.appdata.xml.in
    data/guvcview.in
    po/Makefile.in
    po/gview_v4l2core/Makefile.in
])
AC_OUTPUT

dnl Output the results
AC_MSG_NOTICE([

  guvcview $VERSION
  ----------------------

  Prefix           : ${prefix}
  Pulseaudio       : ${enable_pulse}
  internal format  : ${yuv_format}
  gsl              : ${enable_gsl}
  sdl2             : ${enable_sdl2}
  mjpg decoder     : ${mjpg_decoder}
  desktop file     : ${enable_desktop}
  debian menu      : ${enable_debian_menu}

])



