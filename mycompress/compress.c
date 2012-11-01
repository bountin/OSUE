//
//  compress.c
//  osue
//
//  Created by Martin Prebio on 31.10.12.
//  Copyright (c) 2012 Martin Prebio. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include "compress.h"

struct compression_result compress(FILE *input, FILE *output) {
	char primer;
	int primer_count,
		total_read = 0,
		total_compressed = 0;
	
	while ((primer = fgetc(input)) != EOF) {
		fprintf(output, "%c", primer);
		primer_count = 1;
		
		while (1) {
			char next_char = fgetc(input);
			
			if (next_char == EOF) {
				break;
			} else if (next_char == primer) {
				primer_count += 1;
			} else {
				ungetc(next_char, input);
				break;
			}
		}
		
		fprintf(output, "%i", primer_count);
		total_read += primer_count;
		// One for the primer and the length of the primer_count
		total_compressed += 1 + ceil(log10(primer_count + 1));
	}

	struct compression_result result;
	result.compressed_size = total_compressed;
	result.uncompressed_size = total_read;
	
	return result;
}