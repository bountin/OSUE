//
//  compress.h
//  osue
//
//  Created by Martin Prebio on 01.11.12.
//  Copyright (c) 2012 Martin Prebio. All rights reserved.
//

#ifndef osue_compress_h
#define osue_compress_h

struct compression_result {
	int uncompressed_size;
	int compressed_size;
};

struct compression_result compress(FILE *, FILE *);

#endif
