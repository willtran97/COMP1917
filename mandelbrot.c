/*  Will Tran + Chris Lam
 *  20/04/2016
 *  bmpServer.c
 *  1917 serve that 3x3 bmp from lab3 Image activity
 *
 *  Created by Tim Lambert on 02/04/12.
 *  Containing code created by Richard Buckland on 28/01/11.
 *  Copyright 2012 Licensed under Creative Commons SA-BY-NC 3.0. 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "mandelbrot.h"
#include "pixelColor.h"

int waitForConnection (int serverSocket);
int makeServerSocket (int portno);
void serveBMP (int socket, double Xcentre, double Ycentre, int zoom);
static void serveHTML(int socket);
void draw(unsigned char bmpData[], double Xcentre, double Ycentre, int zoom);

int getValuesFromConnectionString(char* connectionString,
	double* x, double* y, int* z);
double readDoubleFromString(char* string);
int readIntFromString(char* string);

#define SIMPLE_SERVER_VERSION 1.0
#define REQUEST_BUFFER_SIZE 1000
#define DEFAULT_PORT 1917
#define NUMBER_OF_PAGES_TO_SERVE 10
// after serving this many pages the server will halt

#define TRUE 1
#define FALSE 0

#define MAX_STEP 256
#define HEIGHT 512
#define WIDTH 512
#define BYTES_PER_PIXEL 3
#define BMP_SIZE HEIGHT*WIDTH*BYTES_PER_PIXEL

int main (int argc, char *argv[]) {
      
   printf ("************************************\n");
   printf ("Starting simple server %f\n", SIMPLE_SERVER_VERSION);
   printf ("Serving bmps since 2012\n");   
   
   int serverSocket = makeServerSocket (DEFAULT_PORT);   
   printf ("Access this server at http://localhost:%d/\n", DEFAULT_PORT);
   printf ("************************************\n");
   
   char request[REQUEST_BUFFER_SIZE];
   
   int numberServed = 0;
   while (numberServed < NUMBER_OF_PAGES_TO_SERVE) {

	   printf("*** So far served %d pages ***\n", numberServed);

	   int connectionSocket = waitForConnection(serverSocket);
	   // wait for a request to be sent from a web browser, open a new
	   // connection for this conversation

	   // read the first line of the request sent by the browser  
	   int bytesRead;
	   bytesRead = read(connectionSocket, request, (sizeof request) - 1);
	   assert(bytesRead >= 0);
	   // were we able to read any data from the connection?

	   // print entire request to the console 
	   printf(" *** Received http request ***\n %s\n", request);

	   //send the browser a simple html page using http
	   printf(" *** Sending http response ***\n");

	   //getting coordinates and zoom level
	   double x, y;
	   int z;
	   if (getValuesFromConnectionString(request, &x, &y, &z) == TRUE) {
		   serveBMP(connectionSocket, x, y, z);
	   }
	   else {
		   serveHTML(connectionSocket);
	   }
	   close(connectionSocket);
	   numberServed++;
   }

   // close the server connection after we are done- keep aust beautiful
   printf ("** shutting down the server **\n");
   close (serverSocket);
   
   return EXIT_SUCCESS; 
}

void serveBMP (int socket, double Xcentre, double Ycentre, int zoom) {
   char* message;
   
   // first send the http response header
   
   // (if you write stings one after another like this on separate
   // lines the c compiler kindly joins them togther for you into
   // one long string)
   message = "HTTP/1.0 200 OK\r\n"
                "Content-Type: image/bmp\r\n"
                "\r\n";
   printf ("about to send=> %s\n", message);
   write (socket, message, strlen (message));
   
   // write bmp header
   unsigned char bmpHeader[] = {
	   0x42, 0x4D, 0x36, 0x00, 0x0C, 0x00, 0x00, 0x00,
	   0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	   0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02,
	   0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	   0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x13, 0x0B,
	   0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x00, 0x00,
	   0x00, 0x00, 0x00, 0x00, 0x00, 0x00
   };
   write(socket, bmpHeader, sizeof(bmpHeader));

   // create and draw array for bmp
   unsigned char bmpData[BMP_SIZE];
   draw(bmpData, Xcentre, Ycentre, zoom);

   write (socket, bmpData, sizeof(bmpData));
}

void draw(unsigned char bmpdData[], double Xcentre, double Ycentre, int zoom) {
	int xdraw;
	int ydraw = 0;

	int row;
	int col;

	double xscale = 0;
	double yscale = 0;

	int i;
	double scale = 1 / (zoom * zoom);

	while (ydraw < HEIGHT) {
		xdraw = 0;
		while (xdraw < WIDTH) {
			//centering 
			xscale = Xcentre - (WIDTH / 2)*scale;
			yscale = Ycentre - (HEIGHT / 2)*scale;
			//current point
			xscale += xdraw*scale;
			yscale += ydraw*scale;

			i = escapeSteps(xscale, yscale);
			row = ydraw*WIDTH*BYTES_PER_PIXEL;
			col = xdraw*BYTES_PER_PIXEL;
			bmpdData[row + col] = stepsToBlue(i);
			bmpdData[row + col + 1] = stepsToGreen(i);
			bmpdData[row + col + 2] = stepsToRed(i);
			xdraw++;
		}
		ydraw++;
	}
}

// start the server listening on the specified port number
int makeServerSocket (int portNumber) { 
   
   // create socket
   int serverSocket = socket (AF_INET, SOCK_STREAM, 0);
   assert (serverSocket >= 0);   
   // error opening socket
   
   // bind socket to listening port
   struct sockaddr_in serverAddress;
   memset ((char *) &serverAddress, 0,sizeof (serverAddress));
   
   serverAddress.sin_family      = AF_INET;
   serverAddress.sin_addr.s_addr = INADDR_ANY;
   serverAddress.sin_port        = htons (portNumber);
   
   // let the server start immediately after a previous shutdown
   int optionValue = 1;
   setsockopt (
      serverSocket,
      SOL_SOCKET,
      SO_REUSEADDR,
      &optionValue, 
      sizeof(int)
   );

   int bindSuccess = 
      bind (
         serverSocket, 
         (struct sockaddr *) &serverAddress,
         sizeof (serverAddress)
      );
   
   assert (bindSuccess >= 0);
   // if this assert fails wait a short while to let the operating 
   // system clear the port before trying again
   
   return serverSocket;
}

// wait for a browser to request a connection,
// returns the socket on which the conversation will take place
int waitForConnection (int serverSocket) {
   // listen for a connection
   const int serverMaxBacklog = 10;
   listen (serverSocket, serverMaxBacklog);
   
   // accept the connection
   struct sockaddr_in clientAddress;
   socklen_t clientLen = sizeof (clientAddress);
   int connectionSocket = 
      accept (
         serverSocket, 
         (struct sockaddr *) &clientAddress, 
         &clientLen
      );
   
   assert (connectionSocket >= 0);
   // error on accept
   
   return (connectionSocket);
}
// viewer
static void serveHTML(int socket) {
	char* message;

	// first send the http response header
	message =
		"HTTP/1.0 200 Found\n"
		"Content-Type: text/html\n"
		"\n";
	printf("about to send=> %s\n", message);
	write(socket, message, strlen(message));

	message =
		"<!DOCTYPE html>\n"
		"<script src=\"http://almondbread.cse.unsw.edu.au/tiles.js\"></script>"
		"\n";
	write(socket, message, strlen(message));
}

int escapeSteps(double x, double y) {
	int steps = 0;
	double calcx = 0;
	double calcy = 0;
	double tempx;
	while (calcx*calcx + calcy*calcy < 4 && steps < MAX_STEP) {
		tempx = (calcx*calcx - calcy*calcy) + x;
		calcy = (2 * calcx*calcy) + y;
		calcx = tempx;
		steps++;
	}
	return steps;
}

// below are functions for getting x, y, z from server
int getValuesFromConnectionString(char* connectionString,
	double* x, double* y, int* z) {
	int index = 0;
	int isXSet = FALSE;
	int isYSet = FALSE;
	int isZSet = FALSE;

	// Skip the GET command and the hostname
	while (connectionString[index] != '/' &&
		connectionString[index] != '\0') {
		index++;
	}

	// Skip to the character after the X
	while (connectionString[index] != 'X' &&
		connectionString[index] != '\0') {
		index++;
	}
	// If it didn't stop on null
	if (connectionString[index] == 'X') {
		isXSet = TRUE;
		index++;
		// Read the X value
		*x = readDoubleFromString(&connectionString[index]);
	}

	// Skip to the character after the Y
	while (connectionString[index] != 'Y' &&
		connectionString[index] != '\0') {
		index++;
	}
	// If it didn't stop on null
	if (connectionString[index] == 'Y') {
		isYSet = TRUE;
		index++;
		// Read the Y value
		*y = readDoubleFromString(&connectionString[index]);
	}


	// Skip to the character after the Z
	while (connectionString[index] != 'Z' &&
		connectionString[index] != '\0') {
		index++;
	}
	// If it didn't stop on null
	if (connectionString[index] == 'Z') {
		isZSet = TRUE;
		index++;
		// Read the Z value
		*z = readIntFromString(&connectionString[index]);
	}

	return isXSet && isYSet && isZSet;
}

double readDoubleFromString(char* str) {
	int index = 0;
	double value = 0;
	int afterDecimalPoint = FALSE;
	int moreThanOneDecimalPoint = FALSE;
	int isNegative = FALSE;
	int digitsAfterDecimalPoint = 0;
	int digitCounter;

	// The value is negative if the first character is a minus
	if (str[index] == '-') {
		isNegative = TRUE;
		index++;
	}

	// Read until end of string or non-numerical character.
	// The first decimal point counts as a numerical character.
	while (str[index] != '\0' && moreThanOneDecimalPoint == FALSE &&
		((str[index] >= '0' && str[index] <= '9') ||
			str[index] == '.'
			)
		) {

		if (str[index] == '.') {
			if (afterDecimalPoint == TRUE) {
				moreThanOneDecimalPoint = TRUE;
			}
			else {
				afterDecimalPoint = TRUE;
			}
		}
		else {
			if (afterDecimalPoint == FALSE) {
				// Shift previous digits left one to add new one
				// to ones column
				value *= 10;
				value += str[index] - '0';
			}
			else {
				// Shift previous digits left before the decimal
				// point to add new one
				// to ones column, then shift all right one to
				// move it after the decimal point
				digitCounter = 0;
				while (digitCounter <= digitsAfterDecimalPoint) {
					value *= 10;
					digitCounter++;
				}

				value += str[index] - '0';

				while (digitCounter > 0) {
					value /= 10;
					digitCounter--;
				}
				digitsAfterDecimalPoint++;
			}
		}

		index++;
	}

	if (isNegative == TRUE) {
		value = -value;
	}

	return value;
}

int readIntFromString(char* str) {
	int index = 0;
	int value = 0;
	int isNegative = FALSE;

	// The value is negative if the first character is a minus
	if (str[index] == '-') {
		isNegative = TRUE;
		index++;
	}

	// Read until end of string or non-numerical character
	while (str[index] != '\0' &&
		((str[index] >= '0' && str[index] <= '9'))
		) {

		// Shift previous digits left one to add new one
		// to ones column
		value *= 10;
		value += str[index] - '0';

		index++;
	}

	if (isNegative == TRUE) {
		value = -value;
	}

	return value;
}

