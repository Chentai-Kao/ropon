/*******************************************************************************#
#           guvcview              http://guvcview.sourceforge.net               #
#                                                                               #
#           Paulo Assis <pj.assis@gmail.com>                                    #
#                                                                               #
# This program is free software; you can redistribute it and/or modify          #
# it under the terms of the GNU General Public License as published by          #
# the Free Software Foundation; either version 2 of the License, or             #
# (at your option) any later version.                                           #
#                                                                               #
# This program is distributed in the hope that it will be useful,               #
# but WITHOUT ANY WARRANTY; without even the implied warranty of                #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 #
# GNU General Public License for more details.                                  #
#                                                                               #
# You should have received a copy of the GNU General Public License             #
# along with this program; if not, write to the Free Software                   #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA     #
#                                                                               #
********************************************************************************/

#ifndef GVIEW_H
#define GVIEW_H
#include <inttypes.h>
#include <sys/types.h>
#include <libintl.h>
#include <pthread.h>

/*needed fo PACKAGE definition*/
#include "../config.h"

/* support for internationalization - i18n */
#ifndef _
#  define _(String) dgettext (GETTEXT_PACKAGE, String)
#endif

#ifndef N_
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#define CLEAR_LINE "\x1B[K"

#ifdef WORDS_BIGENDIAN
  #define BIGENDIAN 1
#else
  #define BIGENDIAN 0
#endif

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000LL
#endif

#ifndef USEC_PER_SEC
#define USEC_PER_SEC 1000000LL
#endif

#define ODD(x) ((x%2)?TRUE:FALSE)

#define __THREAD_TYPE pthread_t
#define __THREAD_CREATE(t,f,d) (pthread_create(t,NULL,f,d))
#define __THREAD_JOIN(t) (pthread_join(t, NULL))


#define __MUTEX_TYPE pthread_mutex_t
#define __COND_TYPE pthread_cond_t
#define __STATIC_MUTEX_INIT PTHREAD_MUTEX_INITIALIZER
#define __INIT_MUTEX(m) ( pthread_mutex_init(m, NULL) )
#define __CLOSE_MUTEX(m) ( pthread_mutex_destroy(m) )
#define __LOCK_MUTEX(m) ( pthread_mutex_lock(m) )
#define __UNLOCK_MUTEX(m) ( pthread_mutex_unlock(m) )

#define __INIT_COND(c)  ( pthread_cond_init (c, NULL) )
#define __CLOSE_COND(c) ( pthread_cond_destroy(c) )
#define __COND_BCAST(c) ( pthread_cond_broadcast(c) )
#define __COND_TIMED_WAIT(c,m,t) ( pthread_cond_timedwait(c,m,t) )

/*next index of ring buffer with size elements*/
#define NEXT_IND(ind,size) ind++;if(ind>=size) ind=0
/*previous index of ring buffer with size elements*/
//#define PREV_IND(ind,size) ind--;if(ind<0) ind=size-1

typedef char* pchar;

/* 0 is device default*/
static const int stdSampleRates[] =
{
	0, 8000,  9600, 11025, 12000,
	16000, 22050, 24000,
	32000, 44100, 48000,
	88200, 96000,
	-1   /* Negative terminated list. */
};

#define DHT_SIZE 432

/*clip value between 0 and 255*/
#define CLIP(value) (uint8_t)(((value)>0xFF)?0xff:(((value)<0)?0:(value)))

/*MAX macro - gets the bigger value*/
#ifndef MAX
#define MAX(a,b) (((a) < (b)) ? (b) : (a))
#endif

/* On Screen Display flags*/
#define OSD_METER  (1<<0)

#endif

