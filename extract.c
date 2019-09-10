/*
** Extract OGG/Vorbis soundtrack from Hotline Miami
** of Humble Indie Bundle #8
**
** Coypright (C) 2013 Ernst Friedrich Jonathan Wonneberger <info@jonwon.de>
** Copyright (C) 2013 Sebastian Pipping <sebastian@pipping.org>
**
** Licensed under GPL v3 or later
*/
#define _BSD_SOURCE
#include <endian.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#define WAD_PARENT_FILENAME  "hotlinemiami_v1.0.9a-Linux_28-05-2013.tar.gz"
#define WAD_FILENAME  "hlm2_music_desktop.wad"
#define WAD_EXPECTED_SIZE_BYTES_A  488239033
#define WAD_EXPECTED_SIZE_BYTES_B  497702478

// OGG header offsets from
// https://en.wikipedia.org/wiki/Ogg#Page_structure
#define CAPTURE_PATTERN  "OggS"
#define OFFSET_PAGE_SEQUENCE_NUMBER  18
#define OFFSET_PAGE_SEGMENT_COUNT  26
#define OFFSET_PAGE_SEGMENTS  27


const char * find_capture(const char * haystack, size_t len_hackstack, const char * needle, size_t len_needle) {
	const char * first = haystack;
	for (;;) {
		const size_t bytes_more_to_look_in = len_hackstack - (first - haystack);
		first = memchr(first, needle[0], bytes_more_to_look_in);
		if (first == NULL) {
			return NULL;
		}

		if (strncmp(first, needle, len_needle) == 0) {
			return first;
		} else {
			first++;
		}
	}
}

void extract_file(const char * container, const char * file_start, const char * file_end, const char * file_name) {
	const uint32_t offset_start = file_start - container;
	const uint32_t offset_end = file_end - container;
	const uint32_t size_bytes = file_end - file_start;

	const int fd = open(file_name, O_WRONLY|O_CREAT|O_EXCL, 0700);
	if (fd == -1) {
		fprintf(stderr, "ERROR: Could not open file \"%s\" for writing (with flags O_CREAT|O_EXCL): %s (error %d).\n",
				file_name, strerror(errno), errno);
		return;
	}
	const ssize_t bytes_written = write(fd, file_start, size_bytes);
	if (bytes_written == size_bytes) {
		printf("File \"%s\" (offset %u to %u, size %u bytes) extracted.\n",
				file_name, offset_start, offset_end, size_bytes);
	} else if (bytes_written < 1) {
		fprintf(stderr, "ERROR: Could not write to file \"%s\".\n", file_name);
	} else {
		fprintf(stderr, "ERROR: File \"%s\" left at incomplete state (%d bytes to write, %zu bytes written).\n",
				file_name, size_bytes, bytes_written);
	}
	close(fd);
}

int main() {
	// $ strings hlm2_music_desktop.wad | grep -Eo '^.+\.ogg' | sed 's|^Music/|| | sed -e 's/\(.*\)/"\1",/' > files.txt'
	const char * const FILE_NAMES[] = {
"Abyss.ogg",
"AbyssIntro.ogg",
"AcidSpit.ogg",
"Around.ogg",
"beams.ogg",
"Benjamin.ogg",
"BlackTar.ogg",
"Blizzard.ogg",
"Bloodline.ogg",
"BurningCoals.ogg",
"Decade.ogg",
"Delay.ogg",
"Detection.ogg",
"Disturbance.ogg",
"Divide.ogg",
"Dubmood.ogg",
"Dust.ogg",
"evil.ogg",
"Fahkeet.ogg",
"Frantic.ogg",
"FutureClub.ogg",
"Ghost.ogg",
"GuidedMeditation.ogg",
"Hideout.ogg",
"HollywoodHeights.ogg",
"Hotline3.ogg",
"HotlineTheme.ogg",
"Interlude.ogg",
"Java.ogg",
"KeepCalm.ogg",
"LePerv.ogg",
"MiamiJam.ogg",
"MsMinnie.ogg",
"NARC.ogg",
"NewWave.ogg",
"Pursuit.ogg",
"Quixotic.ogg",
"Remorse.ogg",
"Richard.ogg",
"RollerMobster.ogg",
"Rumble.ogg",
"Run.ogg",
"Rust.ogg",
"Sexualizer.ogg",
"SheMeditates.ogg",
"SlumLord.ogg",
"Technoir.ogg",
"Videodrome.ogg",
"voyager.ogg",
"WayHome.ogg",
"WouJuno.ogg",
"YouAreTheBlood.ogg",
"ZebraBackground.ogg",
"Abyss.ogg",
"AbyssIntro.ogg",
"AcidSpit.ogg",
"Around.ogg",
"beams.ogg",
"Benjamin.ogg",
"BlackTar.ogg",
"Blizzard.ogg",
"Bloodline.ogg",
"BurningCoals.ogg",
"Decade.ogg",
"Delay.ogg",
"Detection.ogg",
"Disturbance.ogg",
"Divide.ogg",
"Dubmood.ogg",
"Dust.ogg",
"evil.ogg",
"Fahkeet.ogg",
"Frantic.ogg",
"FutureClub.ogg",
"Ghost.ogg",
"GuidedMeditation.ogg",
"Hideout.ogg",
"HollywoodHeights.ogg",
"Hotline3.ogg",
"HotlineTheme.ogg",
"Interlude.ogg",
"Java.ogg",
"KeepCalm.ogg",
"LePerv.ogg",
"MiamiJam.ogg",
"MsMinnie.ogg",
"NARC.ogg",
"NewWave.ogg",
"Pursuit.ogg",
"Quixotic.ogg",
"Remorse.ogg",
"Richard.ogg",
"RollerMobster.ogg",
"Rumble.ogg",
"Run.ogg",
"Rust.ogg",
"Sexualizer.ogg",
"SheMeditates.ogg",
"SlumLord.ogg",
"Technoir.ogg",
"Videodrome.ogg",
"voyager.ogg",
"WayHome.ogg",
"WouJuno.ogg",
"YouAreTheBlood.ogg",
"ZebraBackground.ogg"
	};

	const int fd = open(WAD_FILENAME, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "ERROR: Could not open file \"%s\".\n", WAD_FILENAME);
		return EXIT_FAILURE;
	}

	struct stat props;
	fstat(fd, &props);

	if ((props.st_size != WAD_EXPECTED_SIZE_BYTES_A)
			&& (props.st_size != WAD_EXPECTED_SIZE_BYTES_B)) {
		fprintf(stderr, "WARNING: Input file is neither %d nor %d bytes in size.  Ideally, grab \"%s\".\n",
				WAD_EXPECTED_SIZE_BYTES_A, WAD_EXPECTED_SIZE_BYTES_B,
				WAD_FILENAME, WAD_PARENT_FILENAME);
		//return 1;
	}

	char * const content = malloc(props.st_size + 1);
	if (! content) {
		fprintf(stderr, "ERROR: Out of memory.\n");
		return 1;
	}
	const ssize_t bytes_read = read(fd, content, props.st_size);
	content[bytes_read] = '\0';

	const char * first = content;
	unsigned int files_found = 0;

	const char * current_file_start = NULL;
	const char * current_file_end = NULL;

	for (;;) {
		const char * haystack = first;
		const size_t haystack_len = bytes_read - (haystack - content);
		first = find_capture(first, haystack_len, CAPTURE_PATTERN, sizeof(CAPTURE_PATTERN) - 1);
		if (first == NULL) {
			break;
		}

		const uint32_t page_sequence_number = le32toh(*(uint32_t *)(first + OFFSET_PAGE_SEQUENCE_NUMBER));
		if (page_sequence_number == 0) {
			if (files_found > 0) {
				extract_file(content, current_file_start, current_file_end, FILE_NAMES[files_found - 1]);
			}
			current_file_start = first;
			files_found++;
		}
		const uint8_t page_segment_count = *(uint8_t *)(first + OFFSET_PAGE_SEGMENT_COUNT);

		size_t page_segment_index = 0;
		uint32_t size_of_all_pages_bytes = 0;
		for (; page_segment_index < page_segment_count; page_segment_index++) {
			const uint8_t page_size_byts = *(uint8_t *)(first + OFFSET_PAGE_SEGMENTS + page_segment_index);
			size_of_all_pages_bytes += page_size_byts;
		}
		const uint32_t size_from_this_page_bytes = OFFSET_PAGE_SEGMENTS + page_segment_count + size_of_all_pages_bytes;

		current_file_end = first + size_from_this_page_bytes;
		first += size_from_this_page_bytes;
	}

	if(files_found)
		extract_file(content, current_file_start, current_file_end, FILE_NAMES[files_found - 1]);
	close(fd);

	free(content);
	return EXIT_SUCCESS;
}
