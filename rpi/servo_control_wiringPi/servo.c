// https://learn.sparkfun.com/tutorials/raspberry-gpio/c-wiringpi-example

#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <wiringPi.h> 

int main(void)
{
    int pwmPin = 18; // PWM Pin - Broadcom pin 18, P1 pin 12
    int pwmDutyCycle = 500; // Defines the duty cycle [0-1024]
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
        pwmWrite(pwmPin, pwmDutyCycle);

        printf("\r\nClock=%d, Range=%d, DutyCycle=%d (Frequency=%d Hz)", pwmClock, pwmRange, pwmDutyCycle, 19200000 / pwmClock / pwmRange);

        printf("\r\n\r\nEnter command ('c [0-4095]' for clock; 'r [0-4095]' for range; 'd [1-1024]' for duty cycle; 'q' to quit): ");
       
        memset(command, 0, sizeof(command));
        (void)scanf("%s %d", &command, &value);
 
        if (command[0] == 'q')
        {
            break;
        }
        else if (command[0] == 'c')
        {
            pwmClock = value;
        }
        else if (command[0] == 'r')
        {
            pwmRange = value;
        }
        else if (command[0] == 'd')
        {
            pwmDutyCycle =value;
        }
        else
        {
            printf("Command '%s' not recognized", command);
            break;
        }
    }
    
    return 0;
}

