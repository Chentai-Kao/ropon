## Process this file with automake to produce Makefile.in

# Public headers
h_sources = gviewrender.h

c_sources = render.c \
			render_fx.c \
			render_osd_vu_meter.c

if ENABLE_SDL2
c_sources += render_sdl2.c
else
c_sources += render_sdl1.c
endif

#Install the headers in a versioned directory - guvcvideo-x.x/libgviewrender:
library_includedir=$(includedir)/$(PACKAGE)-$(PACKAGE_RELEASE)/$(GVIEWRENDER_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

#AM_CFLAGS =

lib_LTLIBRARIES= libgviewrender.la

libgviewrender_la_SOURCES= $(h_sources) $(c_sources)

libgviewrender_la_CFLAGS = $(GVIEWRENDER_CFLAGS) \
			$(GSL_CFLAGS) \
			$(PTHREAD_CFLAGS) \
			-I$(top_srcdir) \
			-I$(top_srcdir)/includes

libgviewrender_la_LIBADD= $(GVIEWRENDER_LIBS) $(GSL_LIBS)

libgviewrender_la_LDFLAGS= -version-info $(GVIEWRENDER_LIBRARY_VERSION) -release $(GVIEWRENDER_API_VERSION)


