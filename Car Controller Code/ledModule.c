#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include "ledModule.h"

#define I2C_DEVICE_ADDRESS 0x70

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

static int LedModule_initI2cBus(char* bus, int address);
static void LedModule_writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static void LedModule_leftAndRightDigitsToDisplay(int leftMostDigit, int rightMostDigit, bool isDecimal);
static void LedModule_createFrameToDisplay(unsigned int leftDigitArray[8], unsigned int rightDigitArray[8]);
static void LedModule_displayFrame(unsigned int arrayToDisplay[8]);
static void runCommand(char* command);

static unsigned int ledFrame[8] = {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E};

static unsigned int arrayOfNumbers[10][8] = {
    {0x00, 0x01, 0x82, 0x82, 0x82, 0x82, 0x82, 0x01}, //#0
    {0x00, 0x83, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01}, //#1
    {0x00, 0x83, 0x02, 0x02, 0x01, 0x80, 0x82, 0x01}, //#2
    {0x00, 0x03, 0x80, 0x80, 0x03, 0x80, 0x80, 0x03}, //#3
    {0x00, 0x80, 0x80, 0x83, 0x82, 0x82, 0x81, 0x80}, //#4
    {0x00, 0x01, 0x82, 0x80, 0x83, 0x02, 0x02, 0x83}, //#5
    {0x00, 0x01, 0x82, 0x82, 0x03, 0x02, 0x02, 0x81}, //#6
    {0x00, 0x01, 0x01, 0x01, 0x80, 0x80, 0x80, 0x83}, //#7
    {0x00, 0x01, 0x82, 0x82, 0x01, 0x82, 0x82, 0x01}, //#8
    {0x00, 0x01, 0x82, 0x80, 0x81, 0x82, 0x82, 0x01}};//#9


//Following three functions provided by Dr. Brian
static void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
        break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

static int LedModule_initI2cBus(char* bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

static void LedModule_writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
    exit(1);
    }
}

//Function called to execute the necessary commands in order to initlialize the LED Matrix and be able to write the values into it
void LedModule_initializeLEDMatrix(void)
{
    char* command1 = "config-pin p9.18 i2c";
	runCommand(command1);
	char* command2 = "config-pin p9.17 i2c";
	runCommand(command2);
	
	FILE *gpio61Direction = fopen("/sys/class/gpio/gpio61/direction", "w");
    if (gpio61Direction == NULL) {
        printf("ERROR: Unable to open gpio61 direction file.\n");
    }
    fprintf(gpio61Direction, "out");
    fclose(gpio61Direction);

	FILE *gpio61value = fopen("/sys/class/gpio/gpio61/value", "w");
    if (gpio61value == NULL) {
        printf("ERROR: Unable to open gpio61 value file.\n");
    }
    fprintf(gpio61value, "1");
    fclose(gpio61value);

	FILE *gpio44Direction = fopen("/sys/class/gpio/gpio44/direction", "w");
    if (gpio44Direction == NULL) {
        printf("ERROR: Unable to open gpio44 direction file.\n");
    }
    fprintf(gpio44Direction, "out");
    fclose(gpio44Direction);

	FILE *gpio44value = fopen("/sys/class/gpio/gpio44/value", "w");
    if (gpio44value == NULL) {
        printf("ERROR: Unable to open export file.\n");
    }
    fprintf(gpio44value, "1");
    fclose(gpio44value);

	char* command3 = "i2cset -y 1 0x70 0x21 0x00";
	runCommand(command3);
	char* command4 = "i2cset -y 1 0x70 0x81 0x00";
	runCommand(command4);
}

//Called to display the double values, separates the left most and right most digits and passes true for isDecimal
void LedModule_prepareDoubleForDisplay(double doubleToBeDisplayed)
{
    int leftMostDigit = doubleToBeDisplayed / 1;
    double rightDigit = (doubleToBeDisplayed - leftMostDigit)*10;
    int rightMostDigit = rightDigit + 1e-9;

    LedModule_leftAndRightDigitsToDisplay(leftMostDigit, rightMostDigit, true);
}

//Called to display the int values, separates the left most and right most digits and passes false for isDecimal
void LedModule_prepareIntForDisplay(int intToBeDisplayed)
{
    int leftMostDigit = intToBeDisplayed / 10;
    int rightMostDigit = intToBeDisplayed % 10;

    LedModule_leftAndRightDigitsToDisplay(leftMostDigit, rightMostDigit, false);
}

//Takes in the left and right digits and matches them to corresponding array for each number and passes those arrays to create the frame
static void LedModule_leftAndRightDigitsToDisplay(int leftMostDigit, int rightMostDigit, bool isDecimal)
{
    int numbers[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned int leftDigitArray[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned int rightDigitArray[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    for (int i = 0; i < 10; i++)
    {
        if (leftMostDigit == numbers[i])
        {
            for (int j = 0; j < 8; j++)
            {
                leftDigitArray[j] = (arrayOfNumbers[numbers[i]][j] << 5) | (arrayOfNumbers[numbers[i]][j] >> 3);
            }
            if (isDecimal)
            {
                leftDigitArray[0] = 0x04;
            }
        }

        if (rightMostDigit == numbers[i])
        {
            for (int j = 0; j < 8; j++)
            {
                rightDigitArray[j] = arrayOfNumbers[numbers[i]][j];
            }
            if (isDecimal)
            {
                rightDigitArray[0] = 0x04;
            }            
        }
    }

    LedModule_createFrameToDisplay(leftDigitArray, rightDigitArray);
    
}

//Function takes in the two left and right digit arrays and ors the two arrays to create one final one
static void LedModule_createFrameToDisplay(unsigned int leftDigitArray[8], unsigned int rightDigitArray[8])
{
    unsigned int digitsArray[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < 8; i++)
    {
        digitsArray[i] = leftDigitArray[i] | rightDigitArray[i];
    }
    
    LedModule_displayFrame(digitsArray);
}

//Function takes in the final array and displays it onto matrix
static void LedModule_displayFrame(unsigned int arrayToDisplay[8])
{
    int i2cFileDesc = LedModule_initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

    for (int i = 0; i < 8; i++)
    {
        LedModule_writeI2cReg(i2cFileDesc, ledFrame[i], arrayToDisplay[i]);
    }
}

//Function turns of all the LEDS in the matrix
void LedModule_turnOfAllLEDS(void)
{
    int i2cFileDesc = LedModule_initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

    for (int i = 0; i < 8; i++)
    {
        LedModule_writeI2cReg(i2cFileDesc, ledFrame[i], 0x00);
    }
}

