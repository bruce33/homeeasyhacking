
#ifndef HomeEasy_h
#define HomeEasy_h

#include "WProgram.h"


/**
 * A class for receiving messages for HomeEasy transmitters.
 * 
 * This class uses digital pin 8 as the input from the receiver.
 * Timer 1 is used for interupts.
 */
class HomeEasy
{
	public:
		
		/**
		 * Constructor
		 */
		HomeEasy();
		
		
		/**
		 * Initialise the system.  This makes the receiver start listening.
		 */
		void init();
		
		
		/**
		 * Register a handler for the simple protocol messages.  The given function will be called
		 * when a message has been received and the data contained within the message will be passed
		 * as parameters.
		 */
		void registerSimpleProtocolHandler(void(*handler)(unsigned int, unsigned int, bool));
		
		
		/**
		 * Register a handler for the advanced protocol messages.  The given function will be called
		 * when a message has been received and the data contained within the message will be passed
		 * as parameters.
		 */
		void registerAdvancedProtocolHandler(void(*handler)(unsigned long, unsigned int, bool, bool));
		
		
		// these should be private rather than static
		static void (*simpleProtocolHandler)(unsigned int, unsigned int, bool);
		static void (*advancedProtocolHandler)(unsigned long, unsigned int, bool, bool);


	private:
		
		int interuptPin;
		
		unsigned int pulseWidth;
		unsigned int latchStage;
		signed int bitCount;
		byte bit;
		byte prevBit;
		
		unsigned long sender;
		unsigned int recipient;
		byte command;
		unsigned int unit;
		bool group;
		
		static void ignoreSimpleProtocol(unsigned int sender, unsigned int recipient, bool on);
		static void ignoreAdvancedProtocol(unsigned long sender, unsigned int recipient, bool on, bool group);
};

#endif
