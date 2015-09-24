/* Created By Philip Washy *
 * September 2015          *
 * ptw4@pitt.edu           */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef _WAVHEADER_H
#define _WAVHEADER_H 1

#define DEBUG 1

typedef struct _WAVHEADER {
	/*RIFF header*/
	char     ChunkID[ 4 ];     //should be "RIFF"
	uint32_t ChunkSize;
	char     Format[ 4 ];      //should be "WAVE"

	/*Format subchunk*/
	char     SubChunk1ID[ 4 ]; //should be "fmt "
	uint32_t SubChunk1Size;
	uint16_t AudioFormat;      //LPCM = 1
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;         //= SampleRate * NumChannels * BitsPerSample/8
	uint16_t BlockAlign;       //= NumChannels * BitsPerSample/8
	uint16_t BitsPerSample;    //maybe use BitsPerSample for error checking?

	/*Data subchunk*/
	char     SubChunk2ID[ 4 ]; //should be "data"
	uint32_t SubChunk2Size;
} WAVHEADER;

typedef enum _ENDIANESS { little, big } ENDIANESS;

int getWAVHeader ( char * FileName, WAVHEADER * Header, int ErrorOut ); //given a file name, wavheader struct pointer, and the option for stderr output (0 or !0), this fills the wavheader struct

//check to see if this actually works for conversion
/*
typedef union {
	unsigned char c[ 4 ];
	uint32_t      l;
} NUM32;

typedef union {
	unsigned char c[ 2 ];
	uint16_t      s;
} NUM16;
*/
/* yes this does work, but someone on stackexchange         *
 * who had suggested something similar as the solution      *
 * to a problem was informed that this is technically       *
 * undefined behavior so best just be safe and leave it out */
//also no handling differences in endianess

#endif
