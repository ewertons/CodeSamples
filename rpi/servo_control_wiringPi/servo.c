// https://learn.sparkfun.com/tutorials/raspberry-gpio/c-wiringpi-example

#include <stdlib.h>    // Used for printf() statements
#include <stdio.h>    // Used for printf() statements
#include <wiringPi.h> // Include WiringPi library!

// Pin number declarations. We're using the Broadcom chip pin numbers.

int main(void)
{
    int pwmPin = 18; // PWM LED - Broadcom pin 18, P1 pin 12
    int pwmValue = 500; // Defines the dutycycle [0-1024]
    int pwmClock = 1920;
    int pwmRange = 200;
    char command[10];
    int value;
	
    wiringPiSetupGpio(); // Initialize wiringPi -- using Broadcom pin numbers

    pinMode(pwmPin, PWM_OUTPUT); // Set PWM LED as PWM output
    // pwmFrequency in Hz = 19.2e6 Hz / pwmClock / pwmRange.

	while (1)
	{
                pwmSetClock(pwmClock);
                pwmSetRange(pwmRange);
		pwmWrite(pwmPin, pwmValue); // PWM LED at bright setting
		printf("\r\nEnter: duty cycle value [0 to 1024] or 'q' to quit: ");
		
		if (scanf("%s %d", &command, &value) != 2 || command[0] == 'q')
		{
			break;
		}
		else if (command[0] == 'c')
		{
			pwmClock = value;
			printf("\r\nClock value set to %d", pwmClock);
		}
		else if (command[0] == 'r')
		{
			pwmRange = value;
			printf("\r\nRange value set to %d", pwmRange);
		}
		else if (command[0] == 'd')
		{
			pwmValue =value;
			printf("\r\nDuty cycle value set to %d", pwmValue);
		}
		else
		{
			break;
		}
	}
	
    return 0;
}

