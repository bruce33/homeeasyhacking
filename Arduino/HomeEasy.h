
#ifndef HomeEasy_h
#define HomeEasy_h

#include "WProgram.h"


/**
 * A class for sending and receiving messages for HomeEasy devices.
 * 
 * This class uses digital pin 8 as the input from the receiver and pin 13 as the output to the
 * transmitter.  Timer 1 is used for interrupts.
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
		
		
		/**
		 * Send a message using the simple protocol.
		 */
		void sendSimpleProtocolMessage(unsigned int sender, unsigned int recipient, bool on);
		
		
		/**
		 * Send a message using the advanced protocol.
		 */
		void sendAdvancedProtocolMessage(unsigned long sender, unsigned int recipient, bool on, bool group);
		
		
		// these should be private rather than static
		static void (*simpleProtocolHandler)(unsigned int, unsigned int, bool);
		static void (*advancedProtocolHandler)(unsigned long, unsigned int, bool, bool);


	private:
		
		void initSending();
		
		static void ignoreSimpleProtocol(unsigned int sender, unsigned int recipient, bool on);
		static void ignoreAdvancedProtocol(unsigned long sender, unsigned int recipient, bool on, bool group);
};

#endif
