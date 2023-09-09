#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ipc2.h>

#define SOUND_NEED_BUFFER

#include "sound.h"
#include "file.h"

#define MIN(a,b) ((a > b) ? (b) : (a))

static OggVorbis_File vf;
static int current_section;
static char *readbuffer;

void tremor_update(void) {
	int ret;

	if(state == FINISHING)
		return;

	if(sound_channels == 1) {
		while(buffer_samples < buffer_size) {
			ret = ov_read(&vf, (void *) (pcmbufL + buffer_end * 2), 2 * MIN(buffer_size - buffer_samples, buffer_size - buffer_end), &current_section);

			if(ret <= 0) {
				state = FINISHING;
				return;
			}

			buffer_end += ret / 2;
			buffer_end %= buffer_size;
			buffer_samples += ret / 2;
		}

	} else if(sound_channels == 2) {
		int i;
		s16 *srcL, *dstL, *srcR, *dstR;

		while(buffer_samples < buffer_size) {
			ret = ov_read(&vf, readbuffer, 4 * MIN(buffer_size - buffer_samples, buffer_size - buffer_end), &current_section);

			if(ret <= 0) {
				state = FINISHING;
				return;
			}

			srcL = (s16 *) readbuffer;
			dstL = ((s16 *) pcmbufL) + buffer_end;
			srcR = ((s16 *) readbuffer) + 1;
			dstR = ((s16 *) pcmbufR) + buffer_end;

			for(i = 0; i < ret / 2; i += 2) {
				dstL[i/2] = srcL[i];
				dstR[i/2] = srcR[i];
			}

			buffer_end += ret / 4;
			buffer_end %= buffer_size;
			buffer_samples += ret / 4;
		}
	}
}

int tremor_play(struct media *m) {
	vorbis_info *vi;
	char **ptr, *s;
	long len;
	int ret;
	FILE *fp;

	readbuffer = (char *) malloc(buffer_size * 4);

	fp = fopen(m->path, "r");

	format = TREMOR;

	ov_open(fp, &vf, NULL, 0);

	current_section = 0;

	ptr = ov_comment(&vf,-1)->user_comments;
	vi = ov_info(&vf,-1);
	while(*ptr)
		++ptr;
	len = ov_pcm_total(&vf,-1);
	s = ov_comment(&vf,-1)->vendor;

	sound_channels = vi->channels;
	sound_samplerate = vi->rate;
	sound_bps = 2;

	tremor_update();

	return 0;
}

void tremor_stop(void) {
	ov_clear(&vf);
	free(readbuffer);
}

void tremor_flush(void) {
}

u32 tremor_size(void) {
	return ov_pcm_total(&vf, -1);
}

u32 tremor_position(void) {
	return ov_pcm_tell(&vf);
}

void tremor_seek(u32 pos) {
	ov_pcm_seek(&vf, pos);
}
