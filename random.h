#ifndef __RANDOM_H
#define __RANDOM_H

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h> // Only for rand()...hate using it but...meh
#include <time.h>
#include "debug.h"
#include "mt.h"

/**
 * KEEP ON TOP
 *
 * 64-bit version of MWC.
 * Credit: http://www.velocityreviews.com/forums/t673657-64-bit-kiss-rngs.html
 *
 * Modified so its not a static (rands are supposed to be random :D)
 *
 * Returns 64-bit integer (unsigned) of a random number.
 * Seeds are generated by reading data from /dev/urandom
 **/

#define MWC (t=(x<<58)+c, c=(x>>6), x+=t, c+=(x<t), x)
#define XSH ( y^=(y<<13), y^=(y>>17), y^=(y<<43) )
#define CNG ( z=k*z+g )
#define KISS(c,k,g,x,y,z,t) (MWC+XSH+CNG)

uint64_t getrand(uint64_t c, uint64_t k, uint64_t g, uint64_t x, uint64_t y, uint64_t z){
        uint64_t t;
//D(("c = %d, k = %d, g = %d, x = %d, y = %d, z = %d", c, k, g, x, y, z));
        return KISS(c, k, g, x, y, z, t);
}

/**
 * /KEEP ON TOP
 **/

/**
 * numbdigits()
 * number:	The number to evaluate	[in]
 *
 * Returns the number of digits found in a number.
 **/
uint64_t numdigits(uint64_t number){
	uint64_t tmp = number;

	uint64_t i = 0;

	if(number < 0)
		tmp = -tmp;

	while(tmp){
		tmp /= 10;
		i++;
	}

	return i;
}

/**
 * URandom()
 * bytes:	Amount of bytes to read from device	[in]
 * buff:	Buffer to hold retrieved bytes		[out]
 *
 * Reads x bytes from /dev/urandom and returns it in buff.
 * !! buff is NOT allocated in function, MUST be allocated before use.
 *
 * Returns number of bytes read.
 **/
uint64_t URandom(int bytes, char *buff){
	FILE *fp;
	int len = 0;
	char c;

	fp = fopen("/dev/urandom", "rb");

	if(!fp){
		printf("Unable to open /dev/urandom!\n");
		return 0;
	}

	while(len < bytes){
		c = fgetc(fp);

		if((c >= ' ') && (c <= '~')){
			buff[len] = c;

			len++;
		}
	}

	fclose(fp);

	return len;
}

/**
 * sized_num()
 * digits:	The amount of digits to ensure the final result has.	[in]
 *
 * Loops through until a number is atleast (digits) long.
 **/
uint64_t sized_num(int digits){
	uint64_t val = 2;
	uint64_t tmp = 0;
	uint64_t curd = 0; // Current digits

	time_t tt;
	struct tm *ti;

	time(&tt);
	ti = localtime(&tt);

	srand(time(NULL));
	int r = rand();// % 57 + 65;

	uint64_t s, m, h, y, x, z;
	s = ti->tm_sec;
	m = ti->tm_min;
	h = ti->tm_hour;
	y = ti->tm_yday;
	x = (s + m + h + y) / digits;
	z = ((s + m + h + y) * digits) * r;

	uint64_t init[4] = { getrand(s,m,h,y,x,z), getrand(s,m,h,y,x,z), getrand(s,m,h,y,x,z), getrand(s,m,h,y,x,z) };
	init_by_array64(init, 4);

	while(curd <= digits){
		val *= genrand64_int64();
		curd += numdigits(val);
	}

//	while(curd <= digits){
//		tmp = getrand(1, 2, 3, 4, 5, 6);
//		val = tmp * tmp; //val * tmp;
//		curd = numdigits(val);
//	}
D(("val = %"PRIu64" was determined of digits %"PRIu64, val, numdigits(val)));

	return val;
}

#endif
