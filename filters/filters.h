/* Created By Philip Washy *
 * October 2015            *
 * ptw4@pitt.edu           */

#include <stdlib.h>
#include <stdio.h>
#include "../wavchunk/wavchunk.h"
#include "../wavheader/wavheader.h"

#ifndef _FILTERS_H
#define _FILTERS_H 1

CHUNK filter( WAVHEADER * info, FILE * sound );

#endif
