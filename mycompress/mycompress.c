//
//  main.c
//  mycompress
//
//  Created by Martin Prebio on 29.10.12.
//  Copyright (c) 2012 Martin Prebio. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compress.h"

#define COMPRESSED_SUFFIX ".comp"

extern int errno;

int main(int argc, const char * argv[])
{
	FILE *input;
	FILE *output;
	
	if (argc == 1) {
		input = stdin;
		output = fopen("Stdin.comp", "w");
		
		if (output == NULL) {
			fprintf(stderr, "%s: fopen ('Stdin.comp') failed (%s)\n", argv[0], strerror(errno));
			return EXIT_FAILURE;
		}

		struct compression_result result = compress(input, output);
		fprintf(stdout, "%s: %i Zeichen\n%s: %i Zeichen\n", "Stdin", result.uncompressed_size, "Stdin.comp", result.compressed_size);
		
		fclose(output);
		
		return EXIT_SUCCESS;
	}
	
	for (int i = 1; i < argc; i++) {
		char file_name_out[strlen(argv[i]) + strlen(COMPRESSED_SUFFIX) + 1];

		strcpy(file_name_out, argv[i]);
		strcat(file_name_out, COMPRESSED_SUFFIX);

		input = fopen(argv[i], "r");
		if (input == NULL) {
			fprintf(stderr, "%s: fopen ('%s') failed: (%s)\n", argv[0], argv[i], strerror(errno));
			return EXIT_FAILURE;
		}
		
		output = fopen(file_name_out, "w");
		if (output == NULL) {
			fprintf(stderr, "fopen ('%s') failed: %s\n", file_name_out, strerror(errno));
			return EXIT_SUCCESS;
		}

		struct compression_result result = compress(input, output);
		fprintf(stdout, "%s: %i Zeichen\n%s: %i Zeichen\n", argv[i], result.uncompressed_size, file_name_out, result.compressed_size);
		
		fclose(input);
		fclose(output);
	}
	
    return 0;
}

