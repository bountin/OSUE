//
//  calculator.h
//  osue
//
//  Created by Martin Prebio on 23.11.12.
//  Copyright (c) 2012 Martin Prebio. All rights reserved.
//

#ifndef osue_calculator_h
#define osue_calculator_h

#define MAX_INPUT_LENGTH 15
#define MAX_RESULT_LENGTH 15

#include <stdarg.h>

#ifdef ENDEBUG
#define DEBUG(...) do { fprintf(stderr, __VA_ARGS__); } while(0)
#else
#define DEBUG(...)
#endif

void bail_out(char *);

#endif
