/**
* HomeEasy Library
*
* Usage notes : 
*     By default the library is hooked up to a fixed set of pins (for the benefit of the interrupts).  On your Arduino, you should connect the transmitter data to pin 13 and the receiver data to pin 8 (http://www.arduino.cc/en/Hacking/PinMapping168)
*/
#include "HomeEasy.h"

// for backwards compatibility
#ifndef PINB0
  #define PINB0 PB0
#endif
#ifndef PINB5
  #define PINB5 PB5
#endif

#define TRANSMITTER_MESSAGE_COUNT 5

#define MESSAGE_TYPE_SIMPLE 0
#define MESSAGE_TYPE_ADVANCED 1

// variables used for receiving the messages
unsigned int pulseWidth = 0;
unsigned int latchStage = 0;
signed int bitCount = 0;
byte bit = 0;
byte prevBit = 0;

// variables for storing the data received
unsigned long sender = 0;
unsigned int recipient = 0;
byte command = 0;
bool group = false;

// variables for sending messages
byte messageType;
unsigned int messageCount;

// result handlers
void (*HomeEasy::simpleProtocolHandler)(unsigned int, unsigned int, bool) = NULL;
void (*HomeEasy::advancedProtocolHandler)(unsigned long, unsigned int, bool, bool) = NULL;



/**
 * Constructor
 */
HomeEasy::HomeEasy()
{
}

  	
/**
 * Initialise the system.
 * 
 * Enables the receiving of messages.
 */
void HomeEasy::init()
{
	// ensure the receiver pin is set for input
	DDRB &= ~_BV(PINB0);
	
	// disable PWM (default)
	TCCR1A = 0x00;
	
	// set prescaler to 1/8.  TCNT1 increments every 0.5 micro seconds
	// falling edge used as trigger
	TCCR1B = 0x02;
	
	// enable input capture interrupt for timer 1
	TIMSK1 = _BV(ICIE1);
}


/**
 * Reconfigure the interrupts for sending a message.
 */
void HomeEasy::initSending()
{
	// ensure the transmitter pin is set for output
	DDRB |= _BV(PINB5);
	
	// the value that the timer will count up to before firing the interrupt
	OCR1A = (pulseWidth * 2);

	// toggle OC1A on compare match
	TCCR1A = _BV(COM1A0);

	// CTC mode: top of OCR1A, immediate update of OCR1A, TOV1 flag set on MAX
	TCCR1B |= _BV(WGM12);

	// enable timer interrupt for timer 1, disable input capture interrupt
	TIMSK1 = _BV(OCIE1A);
}


/**
 * Register a handler for the simple protocol messages.
 */
void HomeEasy::registerSimpleProtocolHandler(void(*handler)(unsigned int, unsigned int, bool))
{
	HomeEasy::simpleProtocolHandler = handler;
}


/**
 * Register a handler for the advanced protocol messages.
 */
void HomeEasy::registerAdvancedProtocolHandler(void(*handler)(unsigned long, unsigned int, bool, bool))
{
	HomeEasy::advancedProtocolHandler = handler;
}


/**
 * The input interrupt handler.
 * 
 * This is where the message is received and decoded.
 */
ISR(TIMER1_CAPT_vect)
{
	// reset counter
	TCNT1 = 0;
	
	// get value of input compare register, divide by two to get microseconds
	pulseWidth = (ICR1 / 2);
	
	if(bit_is_clear(TCCR1B, ICES1))
	{	// falling edge was detected, HIGH pulse end
		
		if(latchStage == 1 && pulseWidth > 230 && pulseWidth < 280)
		{	// advanced protocol latch
			
			latchStage = 2;
		}
		else if(latchStage == 3 && (pulseWidth < 150 || pulseWidth > 500))
		{	// advanced protocol data out of timing range
			
			latchStage = 0;
			bitCount = 0;
			
			sender = 0;
			recipient = 0;
		}
		else if(latchStage == 1)
		{	// simple protocol data
			
			bitCount++;
			
			if(pulseWidth > 320 && pulseWidth < 430)
			{
				bit = 0;
			}
			else if(pulseWidth > 1030 && pulseWidth < 1150 && bitCount % 2 == 0)
			{
				bit = 0x08;
			}
			else
			{	// start over if the low pulse was out of range
				
				latchStage = 0;
				bitCount = 0;
				
				sender = 0;
				recipient = 0;
			}
			
			if(bitCount % 2 == 0)
			{
				if(bitCount < 9)
				{
					sender >>= 1;
					sender |= bit;
				}
				else if(bitCount < 17)
				{
					recipient >>= 1;
					recipient |= bit;
				}
				else
				{
					command >>= 1;
					command |= bit;
				}
			}
			
			if(bitCount == 25)
			{	// message is complete
				
				if(command == 14 || command == 6)
				{
					if(HomeEasy::simpleProtocolHandler != NULL)
					{	HomeEasy::simpleProtocolHandler((int)sender, recipient, (command == 14));
					}
				}
				
				latchStage = 0;
				bitCount = 0;
				
				sender = 0;
				recipient = 0;
			}
		}
	}
	else
	{	// raising edge was detected, LOW pulse end
		
		if(latchStage == 0 && pulseWidth > 9480 && pulseWidth < 11500)
		{	// pause between messages
		
			latchStage = 1;
		}
		else if(latchStage == 2 && pulseWidth > 2550 && pulseWidth < 2750)
		{	// advanced protocol latch
			
			latchStage = 3;
		}
		else if(latchStage == 3)
		{	// advanced protocol data
			
			if(pulseWidth > 200 && pulseWidth < 365)
			{
				bit = 0;
			}
			else if(pulseWidth > 1000 && pulseWidth < 1360)
			{
				bit = 1;
			}
			else
			{	// start over if the low pulse was out of range
				latchStage = 0;
				bitCount = 0;
				
				sender = 0;
				recipient = 0;
			}
			
			if(bitCount % 2 == 1)
			{
				if((prevBit ^ bit) == 0)
				{	// must be either 01 or 10, cannot be 00 or 11
					
					latchStage = 0;
					bitCount = -1;
				}
				else if(bitCount < 53)
				{	// first 26 data bits
					
					sender <<= 1;
					sender |= prevBit;
				}
				else if(bitCount == 53)
				{	// 26th data bit
					
					group = prevBit;
				}
				else if(bitCount == 55)
				{	// 27th data bit
					
					command = prevBit;
				}
				else
				{	// last 4 data bits
					
					recipient <<= 1;
					recipient |= prevBit;
				}
			}
			
			prevBit = bit;
			bitCount++;
			
			if(bitCount == 64)
			{	// message is complete
				
				if(HomeEasy::advancedProtocolHandler != NULL)
				{	HomeEasy::advancedProtocolHandler(sender, recipient, (bool)command, group);
				}
				
				sender = 0;
				recipient = 0;
				
				latchStage = 0;
				bitCount = 0;
			}
		}
	}
	
	// toggle bit value to trigger on the other edge
	TCCR1B ^= _BV(ICES1);
}


/**
 * 
 */
void HomeEasy::sendSimpleProtocolMessage(unsigned int s, unsigned int r, bool c)
{
	// disable all interrupts
	TIMSK1 = 0;
	
	// reset variables
	messageCount = 0;
	latchStage = 0;
	bitCount = 0;
	bit = 0;
	prevBit = 0;
	pulseWidth = 10000;
	
	// set data to transmit
	sender = s;
	recipient = r;
	command = (c ? 14 : 6);
	
	// specify encoding
	messageType = MESSAGE_TYPE_SIMPLE;
	
	// start the timer interrupt
	initSending();
}


/**
 * 
 */
void HomeEasy::sendAdvancedProtocolMessage(unsigned long s, unsigned int r, bool c, bool g)
{
	// disable all interrupts
	TIMSK1 = 0;
	
	// reset variables
	messageCount = 0;
	latchStage = 0;
	bitCount = 0;
	bit = 0;
	prevBit = 0;
	pulseWidth = 10000;
	
	// set data to transmit
	sender = s;
	recipient = r;
	command = c;
	group = g;
	
	// specify encoding
	messageType = MESSAGE_TYPE_ADVANCED;
	
	// start the timer interrupt
	initSending();
}


/**
 * The timer interrupt handler.
 * 
 * This is where the message is transmitted.
 * 
 * The timer interrupt is used to wait for the required length of time.  Each call of this
 * function toggles the output and determines the length of the time until the function is
 * called again.
 * 
 * Once the message has been transmitted this class will switch back to receiving.
 */
ISR(TIMER1_COMPA_vect)
{
	if(messageType == MESSAGE_TYPE_SIMPLE)
	{
		if(!prevBit && bitCount != 25)
		{
			PORTB |= _BV(PINB5);
		}
		else
		{
			PORTB &= ~_BV(PINB5);
		}
		
		if(bitCount % 2 == 0)
		{	// every other bit is a zero
			bit = 0;
		}
		else if(bitCount < 8)
		{	// sender
			bit = ((sender & _BV((bitCount - 1) / 2)) != 0);
		}
		else if(bitCount < 16)
		{	// recipient
			bit = ((recipient & _BV((bitCount - 9) / 2)) != 0);
		}
		else if(bitCount < 24)
		{	// command
			bit = ((command & _BV((bitCount - 17) / 2)) != 0);
		}
		
		if(bitCount == 25)
		{	// message finished
			
			bitCount = 0;
			messageCount++;
			
			pulseWidth = 10000;
			
			if(messageCount == 5)
			{	// go back to receiving
				
				messageCount = 0;
				
				TCCR1A = 0x00;
				TCCR1B = 0x02;
				TIMSK1 = _BV(ICIE1);
				
				return;
			}
		}
		else
		{
			if(prevBit && bit || !prevBit && !bit)
			{
				pulseWidth = 375;
			}
			else
			{
				pulseWidth = 1125;
			}
			
			if(prevBit)
			{
				bitCount++;
			}
			
			prevBit = !prevBit;
		}
	}
	else if(messageType == MESSAGE_TYPE_ADVANCED)
	{
		if(!prevBit)
		{
			PORTB |= _BV(PINB5);
		}
		else
		{
			PORTB &= ~_BV(PINB5);
		}
		
		if(!prevBit)
		{
			if(bitCount % 2 == 1 || latchStage == 0)
			{	// every other bit is inverted
				bit = !bit;
			}
			else if(bitCount < 52)
			{	// sender
				bit = (((sender << (bitCount / 2)) & 0x02000000) != 0);
			}
			else if(bitCount < 54)
			{	// group
				bit = group;
			}
			else if(bitCount < 56)
			{	// command
				bit = command;
			}
			else if(bitCount < 64)
			{	// recipient
				bit = ((recipient & _BV(31 - (bitCount / 2))) != 0);
			}
		}
		else
		{
			if(latchStage == 1)
			{
				bitCount++;
			}
		}
		
		if(!prevBit)
		{
			pulseWidth = 235;
		}
		else if(latchStage == 0)
		{
			pulseWidth = 2650;
			
			latchStage = 1;
		}
		else if(bitCount > 64)
		{	// message finished
			
			messageCount++;
			
			pulseWidth = 10000;
			latchStage = 0;
			bitCount = 0;
		}
		else if(bit)
		{
			pulseWidth = 1180;
		}
		else
		{
			pulseWidth = 275;
		}
		
		prevBit = !prevBit;

		if(messageCount == 5)
		{	// go back to receiving
			
			messageCount = 0;
			
			TCCR1A = 0x00;
			TCCR1B = 0x02;
			TIMSK1 = _BV(ICIE1);
			
			return;
		}
	}
	
	// set the next delay
	OCR1A = (pulseWidth * 2);
}
