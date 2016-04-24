/** pixelColor.c
** to print out color intensities based on the steps to escape the Mandelbrot Set
** Created for COMP1917 Task2B
**
** Name: Christopher Shu Chun Lam
** Name: Will Tran
* Date: 18/04/2016
*/

#define MAX_ITERATIONS 256
#define FIRST_STEP (4 * MAX_ITERATIONS / 5)
#define SECOND_STEP (2 * MAX_ITERATIONS / 5)


unsigned char stepsToBlue(int steps) {
	unsigned char result;
	if (steps == MAX_ITERATIONS) {
		result = 0x00;
	}
	else if (steps > FIRST_STEP) {
		result = 0xFF * steps / MAX_ITERATIONS * 2;
	}
	else {
		result = 0xAA * MAX_ITERATIONS / steps;
	}

	return result;
}
unsigned char stepsToRed(int steps) {
	unsigned char result;
	if (steps == MAX_ITERATIONS) {
		result = 0x00;
	}
	else if (steps > SECOND_STEP) {
		result = 0xCC * steps / MAX_ITERATIONS;
	}
	else {
		result = 0x44 * steps / MAX_ITERATIONS;
	}
	return result;
}
unsigned char stepsToGreen(int steps) {
	unsigned char result;
	if (steps == MAX_ITERATIONS) {
		result = 0x00;
	}
	else if (steps > FIRST_STEP) {
		result = 0xFF * steps / MAX_ITERATIONS * 2;
	}
	else if (steps > SECOND_STEP) {
		result = 0x77 * steps / MAX_ITERATIONS;
	}
	else {
		result = 0x33 * 60 / steps;
	}

	return result;
}