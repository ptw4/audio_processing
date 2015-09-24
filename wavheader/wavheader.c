/* Created By Philip Washy *
 * September 2015          *
 * ptw4@pitt.edu           */

/*TODO:                                                                 *
 *	>change to using a buffered reader for efficiency                  x*
 *	>move file pointer and WAVHEADER into the main method for security x*
 *	>move header stuff into a header file                              x*
 *	>add redundant corruption/error checking for safety                \*
 *	>adjust main method so it can be used as a utility in other exes   x*/

#include "wavheader.h"

uint32_t convertTo32 ( char * Data, ENDIANESS Endian ) {
	if( Endian == little ) {
		return ( Data[ 3 ] | ( Data[ 2 ] << 8 ) | ( Data[ 1 ] << 16 ) | ( Data[ 0 ] << 24 ) );
	} else {
		return ( Data[ 0 ] | ( Data[ 1 ] << 8 ) | ( Data[ 2 ] << 16 ) | ( Data[ 3 ] << 24 ) );
	}
}

uint16_t convertTo16 ( char * Data, ENDIANESS Endian ) {
	if( Endian == little ) {
		return ( Data[ 1 ] | ( Data[ 0 ] << 8 ) );
	} else {
		return ( Data[ 0 ] | ( Data[ 1 ] << 8 ) );
	}
}

int parseHeader ( char * FileName, WAVHEADER * Header ) {
	char * TwoBytes, * FourBytes, * Buffer;
	ENDIANESS Endian = big; //0 = big, 1 = little
	TwoBytes = ( char * )( malloc( sizeof( char ) * 2 ) );
	FourBytes = ( char * )( malloc( sizeof( char ) * 4 ) );
	FILE * AudioPhile = fopen( FileName, "r+" );
	FILE * LogFile = fopen( "log.hex", "w+" );//this always has to be opened and closed cause for some reason
											  //having it in an if that branches the same as any other function
											  //using the ptr means that its undefined for compiling
											  //Static typing is a pain sometimes.
	int i, Chunk2Size;
	Buffer = ( char * )( malloc( sizeof( char ) * 18 ) );
	fgets( Buffer, 19, AudioPhile );

	if( DEBUG ) {
		fwrite( ( const void * )Buffer, 1, 18, LogFile );
	}

	/*RIFF header*/
	for( i = 0; i < 4; i++ ) {
		Header->ChunkID[ i ] = Buffer[ i ];
	}
	if( !( strcmp( Header->ChunkID, "RIFX" ) ) ) {
		Endian = little;
	} else if( strcmp( Header->ChunkID, "RIFF" ) ) {
		return ( 1 );
	}
	for( i = 0; i < 4; i++ ) {
		FourBytes[ i ] = Buffer[ i + 4 ];
	}
	Header->ChunkSize = convertTo32( FourBytes, Endian );
	for( i = 0; i < 4; i++ ) {
		Header->Format[ i ] = Buffer[ i + 8 ];
	}
	if( strcmp( Header->Format, "WAVE" ) ) {
		return ( 2 );
	}


	/*Format Subchunk*/
	for( i = 0; i < 4; i++ ) {
		Header->SubChunk1ID[ i ] = Buffer[ i + 12 ];
	}
	for( i = 0; i < 4; i++ ) {
		FourBytes[ i ] = Buffer[ i + 16 ];
	}
	Header->SubChunk1Size = convertTo32( FourBytes, Endian );
	if( Header->SubChunk1Size < 16 ) {  //handle corrupted data
		return ( 3 );
	}

	free( Buffer ); //Free up the buffer and then grab the next chunk
	Chunk2Size = Header->SubChunk1Size + 8;
	Buffer = ( char * )( malloc( sizeof( char ) * Chunk2Size ) );
	fgets( Buffer, ( Chunk2Size + 1 ), AudioPhile );

	if( DEBUG ) {
		fwrite( ( const void * )Buffer, 1, Chunk2Size, LogFile );
	}

	for( i = 0; i < 2; i++ ) {
		TwoBytes[ i ] = Buffer[ i ];
	}
	Header->AudioFormat = convertTo16( TwoBytes, Endian );
	for( i = 0; i < 2; i++ ) {
		TwoBytes[ i ] = Buffer[ i + 2 ];
	}
	Header->NumChannels = convertTo16( TwoBytes, Endian );
	for( i = 0; i < 4; i++ ) {
		FourBytes[ i ] = Buffer[ i + 4 ];
	}
	Header->SampleRate = convertTo32( FourBytes, Endian );
	for( i = 0; i < 4; i++ ) {
		FourBytes[ i ] = Buffer[ i + 8 ];
	}
	Header->ByteRate = convertTo32( FourBytes, Endian );
	for( i = 0; i < 2; i++ ) {
		TwoBytes[ i ] = Buffer[ i + 12 ];
	}
	Header->BlockAlign = convertTo16( TwoBytes, Endian );
	for( i = 0; i < 2; i++ ) {
		TwoBytes[ i ] = Buffer[ i + 14 ];
	}
	Header->BitsPerSample = convertTo16( TwoBytes, Endian );


	/*Data Subchunk, sans data*/
	for( i = 0; i < 4; i++ ) {
		Header->SubChunk2ID[ i ] = Buffer[ i + Header->SubChunk1Size ];
	}
	for( i = 0; i < 4; i++ ) {
		FourBytes[ i ] = Buffer[ i + Header->SubChunk1Size + 4 ];
	}
	Header->SubChunk2Size = convertTo32( FourBytes, Endian );

	free( Buffer );
	free( TwoBytes );
	free( FourBytes );
	fclose( LogFile );
	fclose( AudioPhile );
	return ( 0 );
}

int getWAVHeader ( char * FileName, WAVHEADER * Header ) {
/*int main ( int argc, char ** argv ) {   This code for testing purposes 
	WAVHEADER * Header = ( WAVHEADER * )( malloc( sizeof( WAVHEADER ) ) );
	char * FileName = "440.wav";
	int ErrorOut = 1;*/
	switch( parseHeader( FileName, Header ) ) {
		case 3:
			if( DEBUG ) {
				fputs( "Corrupted File: Unable to Read", stderr );
			}
			return ( 3 );
		case 2:
			if( DEBUG ) {
				fputs( "Invalid file Type: Not a Wave File", stderr );
			}
			return ( 2 );
		case 1:
			if( DEBUG ) {
				fputs( "Invalid File Type: Not a RIFF or RIFX File", stderr );
			}
			return ( 1 );
		default:
			return ( 0 );
	}
}
