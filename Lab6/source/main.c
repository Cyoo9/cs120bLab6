/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

void TimerOff() { TCCR1B = 0x00; }
void TimerISR() { TimerFlag = 1; }

ISR(TIMER1_COMPA_vect) { 
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States {Start, One, Two, Three, Four, Press,  Release}  state;

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF;
	PORTB = 0x00;
	DDRA = 0x00;
	PORTA = 0xFF;

	TimerSet(300);
	TimerOn();
	
	unsigned char pressed = 0;	
	while(1) {
		switch(state) {
			case Start:
				PORTB = 0x00;
				state = One;
				break;
			case One:
				if((~PINA & 0x01) == 1 && pressed == 0) { 
					state = Press; 
					
				}
				else if((~PINA & 0x01) == 1 && pressed == 1) { 
					state = Two;
			       	}
				else { 
					state = Two; 
					pressed = 0;
				}
				break;
			case Two:
				if((~PINA & 0x01) == 1 && pressed == 0) { 
					state = Press;
				      
				}
				else if((~PINA & 0x01) == 1 && pressed == 1) { 
					state = Three; 
				}
				else {
					state = Three;
					pressed = 0;
				}
				break;
			case Three:
				if((~PINA & 0x01) == 1 && pressed == 0) {
					state = Press; 
		
				}
				else if((~PINA & 0x01) == 1 && pressed == 1)
			      	{ 
					state = Four;
			       	}
				else {
					state = Four;
					pressed = 0;
				}
				break;
			case Four:
				if((~PINA & 0x01) == 1 && pressed == 0) { 
					state = Press; 
		
				}
				else if((~PINA & 0x01) == 1 && pressed == 1) {
				       	state = One;
			       	}
				else {
					state = One;
					pressed = 0;
				}	
				break;
			case Press:
				if((~PINA & 0x01) == 1) { 
					state = Press; 
				}
				else { state = Release; }
				break;
			case Release:
				if((~PINA & 0x01) == 1) { 
					state = One; 
					pressed = 1;
				}
				else { state = Release; }
				break;
			default:
				break;
		}
		switch(state) {
			case One:
				PORTB = 0x01;
				break;
			case Two:
				PORTB = 0x02;
				break;
			case Three:
				PORTB = 0x04;
				break;
			case Four:
				PORTB = 0x02;
				break;
			default:
				break;
		}
					
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
