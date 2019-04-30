#include "LPC11xx.h"
#include <stdio.h>
#include <rt_misc.h>
#define PRESCALE (48000-1) //48000 PCLK clock cycles to increment TC by 1 

/* Import external functions from Serial.c file                               */
extern void SER_init (void);

void configureGPIO()
{
	//Enable CLKOUT
	/*
	LPC_IOCON->PIO0_1 &= ~0x3F; // Select clkout function for P0.1
	LPC_IOCON->PIO0_1 |= 0x01;
	LPC_SYSCON->CLKOUTCLKSEL = 0x00; // IRC: 0x00 System osci: 0x01 WTD: 0x02 Main clk: 0x03
	LPC_SYSCON->CLKOUTUEN = 0x01; // Update clock
	LPC_SYSCON->CLKOUTUEN = 0x00; // Toggle update register once
	LPC_SYSCON->CLKOUTUEN = 0x01;
	while ( !(LPC_SYSCON->CLKOUTUEN & 0x01) ); // Wait until updated
	LPC_SYSCON->CLKOUTDIV = 1; // Divided by 255
	*/

	//set port 0_7 to output (high current drain in LPC1114)
    LPC_GPIO0->DIR |= (1<<7);				// input button
		LPC_GPIO1->DIR &= (0xFFFFFFFE); // output power control coffee machine
		LPC_SYSCON->SYSAHBCLKCTRL     |= (1<<6);    //enable clock GPIO (sec 3.5.14)
}

void ledOn()
{
	LPC_GPIO0->DATA &= ~(1<<7);
}

void ledOff()
{						 
	LPC_GPIO0->DATA |= (1<<7);
}


void initTimer0(void)
{
	/*Assuming CCLK (System Clock) = 48Mhz (set by startup code)*/
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9); //Enable 32Bit Timer0 Clock
	LPC_TMR32B0->CTCR = 0x0;
	LPC_TMR32B0->PR = PRESCALE; //Increment LPC_TMR32B0->TC at every 47999+1 clock cycles
	//48000 clock cycles @48Mhz = 1 mS
	LPC_TMR32B0->TCR = 0x02; //Reset Timer
}

void delayMS(unsigned int milliseconds) //Using Timer0
{
	LPC_TMR32B0->TCR = 0x02; //Reset Timer
	LPC_TMR32B0->TCR = 0x01; //Enable timer

	while(LPC_TMR32B0->TC < milliseconds); //wait until timer counter reaches the desired delay

	LPC_TMR32B0->TCR = 0x00; //Disable timer
}

int main()
{
	SER_init();
	configureGPIO();
	//SystemInit(); //called by Startup Code before main(), hence no need to call again.
	//initTimer0(); //Initialize Timer0
	
	while(1){
		//delayMS(5000); //5 Second Delay
		//ledOff();
		if(((LPC_GPIO1->DATA)&1)== 0){
			ledOff();
		}
		else {
			ledOn();
		} 
		//printf("Led On, Iteration \n");
		//while(1);
	}
	}
