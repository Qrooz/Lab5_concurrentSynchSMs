/*	Author: Cruz Ramirez
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 9  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *	Create a concurrent sm with two different tasks.
 *
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: Youtube URL> https://www.youtube.com/watch?v=v25oKhO0ITc 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


//START OF TIMER CODE

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;



void TimerOn() {

        TCCR1B = 0x0B;

        OCR1A = 125;

        TIMSK1 = 0x02;

        TCNT1 = 0;

        _avr_timer_cntcurr = _avr_timer_M;

        SREG |= 0x80;
}

void TimerOff(){
        TCCR1B = 0x00;
}

void TimerISR(){
        TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){

        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0){
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}

void TimerSet(unsigned long M){
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}

//END OF TIMER CODE


//TICK FUNCTIONS
//global variables
unsigned char ThreeLEDs_g = 0x00;
unsigned char blinkingLEDs_g = 0x00;
unsigned short i = 0x00; //counter global variable, increment in main after tick fucntions and set zero in if statement in main?


enum LED1_STATES{LED1_Start, LED1_One, LED1_Two, LED1_Three}LED1_STATE;

void TickFct_LED1(){

	switch(LED1_STATE){ //state transitions for first 3 LEDs SM

		case LED1_Start:
			LED1_STATE = LED1_One;
		break;

		case LED1_One:
			if(i >= 1000){
				LED1_STATE = LED1_Two;
			}
			else{
				LED1_STATE = LED1_One;
			}
		break;

		case LED1_Two:
			if(i >= 1000){
				LED1_STATE = LED1_Three;
			}
			else{
				LED1_STATE = LED1_Two;
			}
		break;

		case LED1_Three:
			if(i >= 1000){
				LED1_STATE = LED1_One;
			}
			else{
				LED1_STATE = LED1_Three;
			}
		break;

		default:
		break;
	}

	switch(LED1_STATE){ //state actions for first 3 LEDs SM
	
		case LED1_One:
			ThreeLEDs_g = 0x01;
		break;

		case LED1_Two:
			ThreeLEDs_g = 0x02;
		break;

		case LED1_Three:
			ThreeLEDs_g = 0x04;
		break;

		case LED1_Start:
		default:
		break;

	}

}

//second SM
enum LED2_STATES{LED2_Start, LED2_On, LED2_Off}LED2_STATE;

void TickFct_LED2(){
	
	switch(LED2_STATE){ //state transitions for 2nd SM
		case LED2_Start:
			LED2_STATE = LED2_On;
		break;

		case LED2_On:
			if(i >= 1000){
				LED2_STATE = LED2_Off;
			}
			else{
				LED2_STATE = LED2_On;
			}
		break;

		case LED2_Off:
			if(i >= 1000){
                                LED2_STATE = LED2_On;
                        }
                        else{
                                LED2_STATE = LED2_Off;
                        }
		break;

		default:
		break;

	}

	switch(LED2_STATE){ // state actions for 2nd SM
		
		case LED2_On:
			blinkingLEDs_g = 0x08;
		break;

		case LED2_Off:
			blinkingLEDs_g = 0x00;
		break;

		case LED2_Start:
		default:
		break;
	}
}

//3rd SM for combining global variables and assignment of PORTB

enum COM_STATES{COM_Wait, COM_Update}COM_STATE;

void TickFct_COM(){
	
	switch(COM_STATE){ //state transitions for 3rd SM
		case COM_Wait:
			if(i >= 1000){
				COM_STATE = COM_Update;
			}
			else{
				COM_STATE = COM_Wait;
			}
		break;

		case COM_Update:
			COM_STATE = COM_Wait;
		break;

		default:
		break;
	}

	switch(COM_STATE){ //state actions for 3rd SM
		case COM_Update:
			PORTB = (blinkingLEDs_g | ThreeLEDs_g);
		break;

		case COM_Wait:
		default:
		break;

	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; //set A as input
	DDRB = 0xFF; PORTB = 0x09; //set B as output
    /* Insert your solution below */
    	TimerSet(1); // set time to 1ms tick
	TimerOn();
	
	//assign intial states of tick functions
	LED1_STATE = LED1_Start;
	LED2_STATE = LED2_Start;
	COM_STATE = COM_Wait;
	
	while (1) {

	//call tick functions
	TickFct_LED1();
	TickFct_LED2();
	TickFct_COM();
	
	++i; //increment global counter
	if(i > 1000){ //reset counter after a second
		i = 0;
	}

	
	while(!TimerFlag);
	TimerFlag = 0;


    	}
    return 0;
}
