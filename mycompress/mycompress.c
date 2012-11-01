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

int main(int argc, const char * argv[])
{
	FILE *input;
	FILE *output;
	
	if (argc == 1) {
		input = stdin;
		output = fopen("Stdin.comp", "w");
		
		struct compression_result result = compress(input, output);
		fprintf(stdout, "%s: %i Zeichen\n%s: %i Zeichen\n", "Stdin", result.uncompressed_size, "Stdin.comp", result.compressed_size);
		
		fclose(output);
		
		return 0;
	}
	
	for (int i = 2; i <= argc; i++) {
		char *file_name_out = (char *) malloc( strlen(argv[i-1]) + strlen(COMPRESSED_SUFFIX) + 1 );
		strcpy(file_name_out, argv[i-1]);
		strcat(file_name_out, COMPRESSED_SUFFIX);
		
		input = fopen(argv[i-1], "r");
		output = fopen(file_name_out, "w");
		
		struct compression_result result = compress(input, output);
		fprintf(stdout, "%s: %i Zeichen\n%s: %i Zeichen\n", argv[i-1], result.uncompressed_size, file_name_out, result.compressed_size);
		
		fclose(input);
		fclose(output);
	}
	
    return 0;
}

