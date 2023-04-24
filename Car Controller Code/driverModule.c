#include "driverModule.h"
#include "ledModule.h"
#include "radio.h"

static void DriverModule_goForth(void);
static void DriverModule_turnBack(void);
static void DriverModule_onYourLeft(void);
static void DriverModule_turnRight(void);
static void DriverModule_stopMoving(void);
static void DriverModule_speedUp(void);
static void DriveModule_slowDown(void);
static void DriverModule_setDutyCycle(int cycleToSet, char* filename);

static void DriverModule_setPeriod(char* filename);
static void runCommand(char* command);
static void configPinsForPWM(void);
static void DriverModule_setAPinToZero(char* filename);
static void DriverModule_setAPinToOne(char* filename);
static void DriverModule_setPeriod(char* filename);

static void sleepForMs (long long delayInMs);
static int getVoltage0Reading(char *filename);

// Flags used to set keep the car moving in the same direction it was
// moving before when we speed up and slow down
static bool wasForward = false;
static bool wasBackward = false;

// Thread stuff
static pthread_t DriverModule_tid;
static pthread_attr_t DriverModule_attr;
pthread_mutex_t DriverModule_mutex = PTHREAD_MUTEX_INITIALIZER;
static void* DriverModule_startDrivingTank(void* arg);

// Default dutycycle for the car
static int cycleForForthAndBack = 50;

//Function below was provided by Dr. Brian
static void runCommand(char* command)
{
    //Execute the shell command (output into the pipe)
    FILE *pipe = popen(command, "r");

    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        printf("--> %s", buffer);
    }

    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf("  command:   %s\n", command);
        printf("  exit code: %d\n", exitCode);
    }

    return;
}

//Function configures pins for PWM
static void configPinsForPWM(void)
{
    char* configPin9_21 = "config-pin p9_21 pwm";
    runCommand(configPin9_21);
    
    char* configPin9_22 = "config-pin p9_22 pwm";
    runCommand(configPin9_22);

    char* configPin8_13 = "config-pin p8_13 pwm";
    runCommand(configPin8_13);
    
    char* configPin8_19 = "config-pin p8_19 pwm";
    runCommand(configPin8_19);

}

// Initialize DriverModule and start thread to drive the car
void DriverModule_init(void)
{
    configPinsForPWM();
    LedModule_initializeLEDMatrix();
    Radio_init();
    DriverModule_setPeriod(LEFTMOTOR_A_PERIOD_PATH);
    DriverModule_setPeriod(LEFTMOTOR_B_PERIOD_PATH);
    DriverModule_setPeriod(RIGHTMOTOR_B_PERIOD_PATH);
    DriverModule_setPeriod(RIGHTMOTOR_A_PERIOD_PATH);

    pthread_attr_init(&DriverModule_attr);
    pthread_create(&DriverModule_tid, &DriverModule_attr, DriverModule_startDrivingTank, NULL);
    pthread_join(DriverModule_tid, NULL);

}

// Check the bit patterns set by the LED (Yellow - bit1, Red - bit2, Green - bit3)
// to set the direction of the Chassis
void DriverModule_setDirectionOfTank(int bit1, int bit2, int bit3)
{
    if ((bit1 <= 50) && (bit2 <= 50) && (bit3 >= 1000)) {
        DriverModule_goForth();
        wasBackward = false;
        wasForward = true;
        printf("FORWARD\n");
    } else if ((bit1 >= 1000) && (bit2 <= 50) && (bit3 >= 1000)) {
        DriverModule_onYourLeft();
        printf("LEFT\n");
    } else if ((bit1 <= 50) && (bit2 >= 1000) && (bit3 <= 50)) {
        DriverModule_turnRight();
        printf("RIGHT\n");
    } else if ((bit1 >= 1000) && (bit2 <= 50) && (bit3 <= 50)) {
        DriverModule_turnBack();
        wasForward = false;
        wasBackward = true;
        printf("BACKWARDS\n");
    } else if ((bit1 >= 1000) && (bit2 >= 1000) && (bit3 >= 1000)) {
        DriverModule_stopMoving();
        printf("STOP\n");
    }
}

// Function used to set the speed, increases or decreases as set by the
// LED (Yellow - bit1, Red - bit2, Green - bit3)
void DriverModule_setSpeedOfTank(int bit1, int bit2, int bit3)
{
    if ((bit1 >= 1000) && (bit2 >= 1000) && (bit3 <= 50)) {
        DriverModule_speedUp();
        //printf("SPEED")
    } else if ((bit1 <= 50) && (bit2 >= 1000) && (bit3 >= 1000)) {
        DriveModule_slowDown();
    }
}

// Function inteded to implement a honking sound. Unfortunately didn't have the time
// to set up another LED on the Arduino and test voltage levels
void DriverModule_honkForFun(int bit1, int bit2, int bit3)
{
    if ((bit1 <= 50) && (bit2 <= 50) && (bit3 <= 50)) {
        printf("HONK\n");
    }
}

// Function to clean up and stop threading. 
void DriverModule_cleanup(void)
{
    Radio_cleanup();
    pthread_exit(NULL);
}

// Thread called to start driving the car/tank
static void* DriverModule_startDrivingTank(void *arg) 
{
    while(1) 
    {
        Radio_startPlayingSounds();
        LedModule_prepareIntForDisplay(cycleForForthAndBack);

        //Led bit patterns
        int bit1 = getVoltage0Reading(BIT2);
        int bit2 = getVoltage0Reading(BIT3);
        int bit3 = getVoltage0Reading(BIT4);


        DriverModule_setDirectionOfTank(bit1, bit2, bit3);
        DriverModule_setSpeedOfTank(bit1, bit2, bit3);
        //DriverModule_honkForFun(bit2, bit3, bit4);

        sleepForMs(100);
    }

    return NULL;
}

// Function to set dutycycle for given pin/motor
static void DriverModule_setDutyCycle(int cycleToSet, char* filename)
{
    FILE *daFile = fopen(filename, "w");
    if (daFile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    fprintf(daFile, "%d", cycleToSet*MAX_DUTYCYCLE/100);
    fclose(daFile);
}

// Function provided by Dr. Brian
static void sleepForMs (long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;

    long long delayNs = delayInMs *NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;

    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

// Function provided by Dr. Brian
static int getVoltage0Reading(char *filename)
{
    // Open file
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
    }
    // Get reading
    int a2dReading = 0;
    fscanf(f, "%d", &a2dReading);
    // Close file
    fclose(f);
    return a2dReading;
}

// Function to drive forward, sets necessary enable pins to one and dutycycle to update speed
static void DriverModule_goForth(void)
{
    DriverModule_setAPinToOne(RIGHTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToZero(RIGHTMOTOR_B_ENABLE_PATH);
    DriverModule_setAPinToOne(LEFTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToZero(LEFTMOTOR_B_ENABLE_PATH);

    DriverModule_setDutyCycle(cycleForForthAndBack, RIGHTMOTOR_A_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(cycleForForthAndBack, LEFTMOTOR_A_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, RIGHTMOTOR_B_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, LEFTMOTOR_B_DUTYCYCLE_PATH);
}

// Function to drive backwards, sets necessary enable pins to one and dutycycle to update speed
static void DriverModule_turnBack(void)
{
    DriverModule_setAPinToZero(RIGHTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToOne(RIGHTMOTOR_B_ENABLE_PATH);
    DriverModule_setAPinToZero(LEFTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToOne(LEFTMOTOR_B_ENABLE_PATH);

    DriverModule_setDutyCycle(cycleForForthAndBack, RIGHTMOTOR_B_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(cycleForForthAndBack, LEFTMOTOR_B_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, RIGHTMOTOR_A_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, LEFTMOTOR_A_DUTYCYCLE_PATH);
    
}

// Function to turn the car left, does not allow for speed change due to power issues
static void DriverModule_onYourLeft(void)
{
    DriverModule_setAPinToOne(RIGHTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToZero(RIGHTMOTOR_B_ENABLE_PATH);
    DriverModule_setAPinToOne(LEFTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToZero(LEFTMOTOR_B_ENABLE_PATH);

    DriverModule_setDutyCycle(50, RIGHTMOTOR_A_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(30, LEFTMOTOR_A_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, RIGHTMOTOR_B_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, LEFTMOTOR_B_DUTYCYCLE_PATH);
}

// Function to turn the car right, does not allow for speed change due to power issues
static void DriverModule_turnRight(void)
{
    DriverModule_setAPinToOne(RIGHTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToZero(RIGHTMOTOR_B_ENABLE_PATH);
    DriverModule_setAPinToOne(LEFTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToZero(LEFTMOTOR_B_ENABLE_PATH);

    DriverModule_setDutyCycle(30, RIGHTMOTOR_A_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(50, LEFTMOTOR_A_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, RIGHTMOTOR_B_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, LEFTMOTOR_B_DUTYCYCLE_PATH);
}

// Function to stop our car from moving
static void DriverModule_stopMoving(void)
{
    DriverModule_setAPinToZero(RIGHTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToZero(RIGHTMOTOR_B_ENABLE_PATH);
    DriverModule_setAPinToZero(LEFTMOTOR_A_ENABLE_PATH);
    DriverModule_setAPinToZero(LEFTMOTOR_B_ENABLE_PATH);

    DriverModule_setDutyCycle(0, RIGHTMOTOR_A_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, LEFTMOTOR_A_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, RIGHTMOTOR_B_DUTYCYCLE_PATH);
    DriverModule_setDutyCycle(0, LEFTMOTOR_B_DUTYCYCLE_PATH);

}

// Function to speed up the car by increasing the dutycycle
static void DriverModule_speedUp(void)
{
    if (cycleForForthAndBack < 98) {
        cycleForForthAndBack += 1;
    } else {
        cycleForForthAndBack = 100;
        printf("Minimum Speed.\n");
    }

    if(wasForward && (!(wasBackward))) {
        DriverModule_goForth();
    } else if (wasBackward && (!(wasForward))) {
        DriverModule_turnBack();
    }
}

// Function to slow down the car by decreasing the dutycycle
static void DriveModule_slowDown(void)
{
    if (cycleForForthAndBack > 51) {
        cycleForForthAndBack -= 1;
    } else {
        cycleForForthAndBack = 50;
        printf("Minimum Speed.\n");
    }
}

// Returns current dutycycle
int DriverModule_returnCurrentDutyCycle(void)
{
    return cycleForForthAndBack;
}

// Sets the period of the dutycycle of the given PWM pin
static void DriverModule_setPeriod(char* filename)
{
    FILE *daFile = fopen(filename, "w");
    if (daFile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    fprintf(daFile, "200000");
    fclose(daFile);
}

// Sets the enable of the given pin to low/0
static void DriverModule_setAPinToZero(char* filename)
{
    FILE *daFile = fopen(filename, "w");
    if (daFile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    fprintf(daFile, "0");
    fclose(daFile);
}

// Sets the enable of the given pin to high/1
static void DriverModule_setAPinToOne(char* filename)
{
    FILE *daFile = fopen(filename, "w");
    if (daFile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    fprintf(daFile, "1");
    fclose(daFile);
}

