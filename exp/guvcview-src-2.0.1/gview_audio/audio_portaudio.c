/*******************************************************************************#
#           guvcview              http://guvcview.sourceforge.net               #
#                                                                               #
#           Paulo Assis <pj.assis@gmail.com>                                    #
#           Nobuhiro Iwamatsu <iwamatsu@nigauri.org>                            #
#                             Add UYVY color support(Macbook iSight)            #
#           Flemming Frandsen <dren.dk@gmail.com>                               #
#                             Add VU meter OSD                                  #
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <portaudio.h>
/* support for internationalization - i18n */
#include <locale.h>
#include <libintl.h>

#include "gview.h"
#include "audio.h"
#include "core_time.h"
#include "gviewaudio.h"

extern int verbosity;

static int sample_index = 0;

#define DEFAULT_LATENCY_DURATION 100.0

/*
 * Portaudio record callback
 * args:
 *    inputBuffer - pointer to captured input data (for recording)
 *    outputBuffer - pointer ouput data (for playing - NOT USED)
 *    framesPerBuffer - buffer size
 *    timeInfo - pointer to time data (for timestamping)
 *    statusFlags - stream status
 *    userData - pointer to user data (audio context)
 *
 * asserts:
 *    audio_ctx (userData) is not null
 *
 * returns: error code (0 ok)
 */
static int recordCallback (
	const void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData )
{
	audio_context_t *audio_ctx = (audio_context_t *) userData;

	/*asserts*/
	assert(audio_ctx != NULL);

	int i = 0;

	sample_t *rptr = (sample_t*) inputBuffer;
	sample_t *capture_buff = (sample_t *) audio_ctx->capture_buff;

	unsigned long numSamples = framesPerBuffer * audio_ctx->channels;
	uint64_t frame_length = NSEC_PER_SEC / audio_ctx->samprate; /*in nanosec*/

	PaTime ts_sec = timeInfo->inputBufferAdcTime; /*in seconds (double)*/
	int64_t ts = ts_sec * NSEC_PER_SEC; /*in nanosec (monotonic time)*/
	int64_t buff_ts = 0;

	/*determine the number of samples dropped*/
	if(audio_ctx->last_ts <= 0)
	{
		audio_ctx->last_ts = ts;
	}

	if(statusFlags & paInputOverflow)
	{
		fprintf( stderr, "AUDIO: portaudio buffer overflow\n" );

		int64_t d_ts = ts - audio_ctx->last_ts;
		int n_samples = (d_ts / frame_length) * audio_ctx->channels;
		for( i = 0; i < n_samples; ++i )
		{
			capture_buff[sample_index] = 0;
			sample_index++;

			if(sample_index >= audio_ctx->capture_buff_size)
			{
				audio_fill_buffer(audio_ctx, audio_ctx->last_ts);
				sample_index = 0;
			}
		}

		if(verbosity > 1)
			printf("AUDIO: compensate overflow with %i silence samples\n", n_samples);
	}
	if(statusFlags & paInputUnderflow)
		fprintf( stderr, "AUDIO: portaudio buffer underflow\n" );

	if(sample_index == 0)
	{
		audio_ctx->capture_buff_level[0] = 0;
		audio_ctx->capture_buff_level[1] = 0;
	}

	int chan = 0;
	/*store capture samples*/
	for( i = 0; i < numSamples; ++i )
    {
        capture_buff[sample_index] = inputBuffer ? *rptr++ : 0;
        sample_index++;

		/*store peak value*/
		if(audio_ctx->capture_buff_level[chan] < capture_buff[sample_index])
			audio_ctx->capture_buff_level[chan] = capture_buff[sample_index];
		chan++;
		if(chan >= audio_ctx->channels)
			chan = 0;

        if(sample_index >= audio_ctx->capture_buff_size)
		{
			buff_ts = ts + ( i / audio_ctx->channels ) * frame_length;

			audio_fill_buffer(audio_ctx, buff_ts);

			/*reset*/
			audio_ctx->capture_buff_level[0] = 0;
			audio_ctx->capture_buff_level[1] = 0;
			sample_index = 0;
		}
	}

	audio_ctx->last_ts = ts + (framesPerBuffer * frame_length);

	if(audio_ctx->stream_flag == AUDIO_STRM_OFF )
		return (paComplete); /*capture stopped*/
	else
		return (paContinue); /*still capturing*/
}


/*
 * list audio devices for portaudio api
 * args:
 *    audio_ctx - pointer to audio context data
 * asserts:
 *    audio_ctx is not null
 *
 * returns: error code (0 ok)
 */
static int audio_portaudio_list_devices(audio_context_t *audio_ctx)
{
	int numDevices;
	const PaDeviceInfo *deviceInfo;

	//reset device count
	audio_ctx->num_input_dev = 0;

	numDevices = Pa_GetDeviceCount();
	if( numDevices < 0 )
	{
		printf( "AUDIO: Audio disabled: Pa_CountDevices returned %i\n", numDevices );
	}
	else
	{
		audio_ctx->device = 0;

		int it = 0;
		for( it=0; it < numDevices; it++ )
		{
			deviceInfo = Pa_GetDeviceInfo( it );
			if (verbosity > 0)
				printf( "--------------------------------------- device #%d\n", it );
			/* Mark audio_ctx and API specific default devices*/
			int defaultDisplayed = 0;

			/* with pulse, ALSA is now listed first and doesn't set a API default- 11-2009*/
			if( it == Pa_GetDefaultInputDevice() )
			{
				if (verbosity > 0)
					printf( "[ Default Input" );
				defaultDisplayed = 1;
				audio_ctx->device = audio_ctx->num_input_dev;/*default index in array of input devs*/
			}
			else if( it == Pa_GetHostApiInfo( deviceInfo->hostApi )->defaultInputDevice )
			{
				const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo( deviceInfo->hostApi );
				if (verbosity > 0)
					printf( "[ Default %s Input", hostInfo->name );
				defaultDisplayed = 2;
			}
			/* OUTPUT device doesn't matter for capture*/
			if( it == Pa_GetDefaultOutputDevice() )
			{
			 	if (verbosity > 0)
				{
					printf( (defaultDisplayed ? "," : "[") );
					printf( " Default Output" );
				}
				defaultDisplayed = 3;
			}
			else if( it == Pa_GetHostApiInfo( deviceInfo->hostApi )->defaultOutputDevice )
			{
				const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo( deviceInfo->hostApi );
				if (verbosity > 0)
				{
					printf( (defaultDisplayed ? "," : "[") );
					printf( " Default %s Output", hostInfo->name );/* OSS ALSA etc*/
				}
				defaultDisplayed = 4;
			}

			if( defaultDisplayed!=0 )
				if (verbosity > 0)
					printf( " ]\n" );

			/* print device info fields */
			if (verbosity > 0)
			{
				printf( "Name                     = %s\n", deviceInfo->name );
				printf( "Host API                 = %s\n",  Pa_GetHostApiInfo( deviceInfo->hostApi )->name );
				printf( "Max inputs = %d", deviceInfo->maxInputChannels  );
			}
			/* INPUT devices (if it has input channels it's a capture device)*/
			if (deviceInfo->maxInputChannels > 0)
			{
				audio_ctx->num_input_dev++;
				/*add device to list*/
				audio_ctx->list_devices = realloc(audio_ctx->list_devices, audio_ctx->num_input_dev * sizeof(audio_device_t));
				if(audio_ctx->list_devices == NULL)
				{
					fprintf(stderr,"AUDIO: FATAL memory allocation failure (audio_portaudio_list_devices): %s\n", strerror(errno));
					exit(-1);
				}
				/*fill device data*/
				audio_ctx->list_devices[audio_ctx->num_input_dev-1].id = it;
				strncpy(audio_ctx->list_devices[audio_ctx->num_input_dev-1].name, deviceInfo->name, 511);
				strncpy(audio_ctx->list_devices[audio_ctx->num_input_dev-1].description, deviceInfo->name, 255);
				audio_ctx->list_devices[audio_ctx->num_input_dev-1].channels = deviceInfo->maxInputChannels;
				audio_ctx->list_devices[audio_ctx->num_input_dev-1].samprate = deviceInfo->defaultSampleRate;
				//audio_ctx->list_devices[audio_ctx->num_input_dev-1].Hlatency = deviceInfo->defaultHighInputLatency;
				//audio_ctx->list_devices[audio_ctx->num_input_dev-1].Llatency = deviceInfo->defaultLowInputLatency;
			}
			if (verbosity > 0)
			{
				printf( ", Max outputs = %d\n", deviceInfo->maxOutputChannels  );
				printf( "Def. low input latency   = %8.3f\n", deviceInfo->defaultLowInputLatency  );
				printf( "Def. low output latency  = %8.3f\n", deviceInfo->defaultLowOutputLatency  );
				printf( "Def. high input latency  = %8.3f\n", deviceInfo->defaultHighInputLatency  );
				printf( "Def. high output latency = %8.3f\n", deviceInfo->defaultHighOutputLatency  );
				printf( "Def. sample rate         = %8.2f\n", deviceInfo->defaultSampleRate );
			}

		}

		if (verbosity > 0)
			printf("----------------------------------------------\n");
	}

	/*set defaults*/
	audio_ctx->channels = audio_ctx->list_devices[audio_ctx->device].channels;
	audio_ctx->samprate = audio_ctx->list_devices[audio_ctx->device].samprate;

	return 0;
}

/*
 * init portaudio api
 * args:
 *    none
 *
 * asserts:
 *    none
 *
 * returns: pointer to audio context data
 *     or NULL if error
 */
audio_context_t *audio_init_portaudio()
{
	int pa_error = Pa_Initialize();

	if(pa_error != paNoError)
	{
		fprintf(stderr,"AUDIO: Failed to Initialize Portaudio (Pa_Initialize returned %i)\n", pa_error);
		return NULL;
	}

	audio_context_t *audio_ctx = calloc(1, sizeof(audio_context_t));
	if(audio_ctx == NULL)
	{
		fprintf(stderr,"AUDIO: FATAL memory allocation failure (audio_init_portaudio): %s\n", strerror(errno));
		exit(-1);
	}

	if(audio_portaudio_list_devices(audio_ctx) != 0)
	{
		fprintf(stderr, "AUDIO: Portaudio failed to get audio device list\n");
		free(audio_ctx);
		return NULL;
	}

	audio_ctx->api = AUDIO_PORTAUDIO;

	return audio_ctx;
}

/*
 * start portaudio stream capture
 * args:
 *   audio_ctx - pointer to audio context data
 *
 * asserts:
 *   audio_ctx is not null
 *
 * returns: error code
 */
int audio_start_portaudio(audio_context_t *audio_ctx)
{
	/*assertions*/
	assert(audio_ctx != NULL);

	PaError err = paNoError;
	PaStream *stream = (PaStream *) audio_ctx->stream;

	if(stream)
	{
		if( !(Pa_IsStreamStopped( stream )))
		{
			Pa_AbortStream( stream );
			Pa_CloseStream( stream );
			audio_ctx->stream = NULL;
			stream = audio_ctx->stream;
		}
	}

	PaStreamParameters inputParameters;

	inputParameters.device = audio_ctx->list_devices[audio_ctx->device].id;
	inputParameters.channelCount = audio_ctx->channels;
	inputParameters.sampleFormat = paFloat32; /*sample_t - float*/

	if (Pa_GetDeviceInfo( inputParameters.device ))
		inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
		//inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency;
	else
		inputParameters.suggestedLatency = DEFAULT_LATENCY_DURATION/1000.0;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	/*---------------------------- start recording Audio. ----------------------------- */
	audio_ctx->snd_begintime = ns_time_monotonic();

	audio_ctx->stream_flag = AUDIO_STRM_ON;

	err = Pa_OpenStream(
		&stream,                     /* stream */
		&inputParameters,            /* inputParameters    */
		NULL,                        /* outputParameters   */
		audio_ctx->samprate,         /* sample rate        */
		paFramesPerBufferUnspecified,/* buffer in frames (use API optimal)*/
		paNoFlag,                    /* PaNoFlag - clip and dhiter*/
		recordCallback,              /* sound callback     */
		audio_ctx );                 /* callback userData  */

	if( err == paNoError )
	{
		err = Pa_StartStream( stream );
		audio_ctx->stream = (void *) stream; /* store stream pointer*/
	}

	if( err != paNoError )
	{
		fprintf(stderr, "AUDIO: An error occured while starting the portaudio API\n" );
		fprintf(stderr, "       Error number: %d\n", err );
		fprintf(stderr, "       Error message: %s\n", Pa_GetErrorText( err ) );

		if(stream) Pa_AbortStream( stream );
		audio_ctx->stream_flag = AUDIO_STRM_OFF;

		return(-1);
	}

	const PaStreamInfo* stream_info = Pa_GetStreamInfo (stream);
	if(verbosity > 1)
		printf("AUDIO: latency of %8.3f msec\n", 1000 * stream_info->inputLatency);

	return 0;
}

/*
 * stop portaudio stream capture
 * args:
 *   audio_ctx - pointer to audio context data
 *
 * asserts:
 *   audio_ctx is not null
 *
 * returns: error code
 */
int audio_stop_portaudio(audio_context_t *audio_ctx)
{
	/*assertions*/
	assert(audio_ctx != NULL);

	int ret = 0;
	int err = paNoError;
	audio_ctx->stream_flag = AUDIO_STRM_OFF;

	PaStream *stream = (PaStream *) audio_ctx->stream;

	/*stops and closes the audio stream*/
	if(stream)
	{
		if(Pa_IsStreamActive( stream ) > 0)
		{
			printf("AUDIO: (portaudio) Aborting audio stream\n");
			err = Pa_AbortStream( stream );
		}
		else
		{
			printf("AUDIO: (portaudio) Stoping audio stream\n");
			err = Pa_StopStream( stream );
		}

		if( err != paNoError )
		{
			fprintf(stderr, "AUDIO: (portaudio) An error occured while stoping the audio stream\n" );
			fprintf(stderr, "       Error number: %d\n", err );
			fprintf(stderr, "       Error message: %s\n", Pa_GetErrorText( err ) );
			ret = -1;
		}

		printf("AUDIO: Closing audio stream...\n");
		err = Pa_CloseStream( stream );

		if( err != paNoError )
		{
			fprintf(stderr, "AUDIO: (portaudio) An error occured while closing the audio stream\n" );
			fprintf(stderr, "       Error number: %d\n", err );
			fprintf(stderr, "       Error message: %s\n", Pa_GetErrorText( err ) );
			ret = -1;
		}
	}
	else
	{
		fprintf(stderr, "AUDIO: (portaudio) Invalid stream pointer.\n");
		ret = -2;
	}

	audio_ctx->stream = NULL;

	return ret;
}

/*
 * close and clean audio context for portaudio api
 * args:
 *   audio_ctx - pointer to audio context data
 *
 * asserts:
 *   none
 *
 * returns: none
 */
void audio_close_portaudio(audio_context_t *audio_ctx)
{
	Pa_Terminate();

	if(audio_ctx == NULL)
		return;

	if(audio_ctx->list_devices != NULL);
		free(audio_ctx->list_devices);
	audio_ctx->list_devices = NULL;

	if(audio_ctx->capture_buff)
		free(audio_ctx->capture_buff);

	free(audio_ctx);
}
