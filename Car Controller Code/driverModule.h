#ifndef _DRIVERMODULE_H_
#define _DRIVERMODULE_H_

#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Enable, dutycycle and period paths for left motor turning forward
#define LEFTMOTOR_A_ENABLE_PATH "/dev/bone/pwm/2/a/enable"
#define LEFTMOTOR_A_DUTYCYCLE_PATH "/dev/bone/pwm/2/a/duty_cycle"
#define LEFTMOTOR_A_PERIOD_PATH "/dev/bone/pwm/2/a/period"

// Enable, dutycycle and period paths for left motor turning backwards
#define LEFTMOTOR_B_ENABLE_PATH "/dev/bone/pwm/2/b/enable"
#define LEFTMOTOR_B_DUTYCYCLE_PATH "/dev/bone/pwm/2/b/duty_cycle"
#define LEFTMOTOR_B_PERIOD_PATH "/dev/bone/pwm/2/b/period"

// Enable, dutycycle and period paths for right motor turning forward
#define RIGHTMOTOR_A_ENABLE_PATH "/dev/bone/pwm/0/a/enable"
#define RIGHTMOTOR_A_DUTYCYCLE_PATH "/dev/bone/pwm/0/a/duty_cycle"
#define RIGHTMOTOR_A_PERIOD_PATH "/dev/bone/pwm/0/a/period"

// Enable, dutycycle and period paths for right motor turning backward
#define RIGHTMOTOR_B_ENABLE_PATH "/dev/bone/pwm/0/b/enable"
#define RIGHTMOTOR_B_DUTYCYCLE_PATH "/dev/bone/pwm/0/b/duty_cycle"
#define RIGHTMOTOR_B_PERIOD_PATH "/dev/bone/pwm/0/b/period"

// Honk file
#define HONK_FILE "beatbox-wave-files/mixkit-car-horn-718.wav"

// Dutycycle constants
#define DEFAULT_DUTYCYCLE 80000
#define MIN_TURN_DUTYCYCLE 105000
#define MAX_TURN_DUTYCYCLE 180000
#define MAX_DUTYCYCLE 200000

// Reading AIN pins to get input values from Arduino LEDs
#define BIT1 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define BIT2 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define BIT3 "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define BIT4 "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"

void DriverModule_init(void);

void DriverModule_setDirectionOfTank(int bit1, int bit2, int bit3);

void DriverModule_setSpeedOfTank(int bit1, int bit2, int bit3);

void DriverModule_honkForFun(int bit1, int bit2, int bit3);

int DriverModule_returnCurrentDutyCycle(void);

void DriverModule_cleanup(void);


#endif