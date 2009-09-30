
#include "HomeEasy.h"

// the input put for the receiver
int interuptPin = 8;

// variables used for receiving the messages
unsigned int pulseWidth = 0;
unsigned int latchStage = 0;
signed int bitCount = 0;
byte bit = 0;
byte prevBit = 0;

// variables for storing the data recieved
unsigned long sender = 0;
unsigned int recipient = 0;
byte command = 0;
unsigned int unit = 0;
bool group = false;

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
 * Configure the interupt for the receiver.
 */
void HomeEasy::init()
{
	pinMode(interuptPin, INPUT);
	
	// disable PWM (default)
	TCCR1A = 0x00;
	
	// set prescaler to 1/8.  TCNT1 increments every 0.5 micro seconds
	// falling edge used as trigger
	TCCR1B = 0x02;
	
	// enable input capture interrupt for timer 1
	TIMSK1 = _BV(ICIE1);
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
 * The interupt handler.
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
