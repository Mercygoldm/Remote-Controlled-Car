// ledModule.h
// Module to display decimals and integers on given 8x8 LED Matrix

#ifndef _LEDMODULE_H
#define _LEDMODULE_H_

#include <stdbool.h>
#include <math.h>

//Function called to execute the necessary commands in order to initlialize the LED Matrix and be able to write the values into it
void LedModule_initializeLEDMatrix(void);

//Called to display the double values, separates the left most and right most digits and passes true for isDecimal
void LedModule_prepareDoubleForDisplay(double doubleToBeDisplayed);

//Called to display the int values, separates the left most and right most digits and passes false for isDecimal
void LedModule_prepareIntForDisplay(int intToBeDisplayed);

//Function turns of all the LEDS in the matrix
void LedModule_turnOfAllLEDS(void);

#endif