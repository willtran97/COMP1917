/** pixelColor.c
** to print out color intensities based on the steps to escape the Mandelbrot Set
** Created for COMP1917 Task2B
**
** Name: Christopher Shu Chun Lam
** Name: Will Tran
* Date: 18/04/2016
*/

#include <stdio.h>
#include <stdlib.h>
#include "pixelColor.h"
#define MAX 255
#define MIN 0

//RED intensity
unsigned char stepsToRed(int steps) {
	unsigned char intensity = 0;
	if (steps >= MAX) {
		intensity = MAX;
	}
	else if (steps <= MIN) {
		intensity = MIN;
	}
	else if (steps > MIN && steps < MAX) {
		intensity = steps;
	}

	return intensity;
}

//BLUE intensity
unsigned char stepsToBlue(int steps) {
	unsigned char intensity = 0;
	if (steps >= MAX) {
		intensity = MAX;
	}
	else if (steps <= MIN) {
		intensity = MIN;
	}
	else if (steps > MIN && steps < MAX) {
		intensity = steps;
	}

	return intensity;
}

//GREEN intensity
unsigned char stepsToGreen(int steps) {
	unsigned char intensity = 0;
	if (steps >= MAX) {
		intensity = MAX;
	}
	else if (steps <= MIN) {
		intensity = MIN;
	}
	else if (steps > MIN && steps < MAX) {
		intensity = steps;
	}

	return intensity;
}