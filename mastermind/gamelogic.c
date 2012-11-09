//
//  gamelogic.c
//  osue
//
//  Created by Martin Prebio on 09.11.12.
//  Copyright (c) 2012 Martin Prebio. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>

uint16_t get_next_tipp(void)
{
	uint16_t selected_colors = 0x0002;
	
	// Hier kann ihre Spiellogik stehen
	
	/*************** PARITY STUFF ****************/
	uint16_t parity = 0;
	for (int i = 0; i < 15; i++) {
		parity ^= (selected_colors % (1 << (i+1))) >> i;
	}
	
	// Just to be sure that parity bit is unset
	selected_colors <<= 1;
	selected_colors >>= 1;
	
	parity <<= 15;
	
	return selected_colors | parity;
}

void save_result(uint8_t result)
{
	int red, white;
	
	red = result & 7;
	white = (result >> 3) & 7;
	
	// Hier kann Ihre Spiellogik stehen
}