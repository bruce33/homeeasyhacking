/*
 * Example program using the HomeEasy class.
 */

#include "HomeEasy.h"

HomeEasy homeEasy;


/**
 * Set up the serial interface and the HomeEasy class.
 */
void setup()
{
	Serial.begin(9600);
	
	homeEasy = HomeEasy();
	
	homeEasy.registerSimpleProtocolHandler(printSimpleResult);
	homeEasy.registerAdvancedProtocolHandler(printAdvancedResult);
	homeEasy.registerBBSB2011ProtocolHandler(printBBSB2011Result);
	
	homeEasy.init();
}


/**
 * No processing is required in the loop for receiving, so constantly call sendSimpleMessage().
 */
void loop()
{
  sendSimpleMessage();
}


/**
 * Print the details of the advanced protocol message.
 */
void printAdvancedResult(unsigned long sender, unsigned int recipient, bool on, bool group)
{
	Serial.println();

	Serial.println("advanced protocol message");
	
	Serial.print("sender ");
	Serial.println(sender);
	
	Serial.print("recipient ");
	Serial.println(recipient);
	
	Serial.print("on ");
	Serial.println(on);
	
	Serial.print("group ");
	Serial.println(group);
	
	Serial.println();
}


/**
 * Print the details of the simple protocol message.
 */
void printSimpleResult(unsigned int sender, unsigned int recipient, bool on)
{
	Serial.println();

	Serial.println("simple protocol message");
	
	Serial.print("sender ");
	Serial.println(sender);
	
	Serial.print("recipient ");
	Serial.println(recipient);
	
	Serial.print("on ");
	Serial.println(on);
	
	Serial.println();
}

/**
 * Print the details of the BBSB 2011 protocol message.
 */
void printBBSB2011Result(unsigned int sender, unsigned int recipient, bool on, bool group)
{
	Serial.println("bbsb2011 protocol message");
	
	Serial.print("sender ");
	Serial.println(sender);
	
	Serial.print("recipient ");
	Serial.println(recipient);
	
	Serial.print("on ");
	Serial.println(on);
	
	Serial.print("group ");
	Serial.println(group);
	
	Serial.println();
}

/**
 * Gets a simple message from the Serial port and transmits it
 */
void sendSimpleMessage()
{
  unsigned int sender = 0;
  unsigned int recipient = 0;
  unsigned int action = false;

  // Get sender
  Serial.print("Sender (A - P): ");
  while (true)
  {
    if (Serial.available())
    {
      sender = Serial.read() - 65; // - 65 converts 'A' => 0, 'B' => 1, ...
      if (sender > 15)
      {
        Serial.println("Invalid");
        Serial.println();
        return;
      }
      else
      {
        Serial.println(sender + 65, BYTE);
        break;
      }
    }
  }

  // Get recipient
  Serial.print("Recipient (unit number 1 - 16): ");
  while (true)
  {
    if (Serial.available())
    {
      recipient = Serial.read() - 49; // - 49 converts '1' => 0, '2' => 1, ...
       if (recipient > 15)
      {
        Serial.println("Invalid");
        Serial.println();
        return;
      }
      else
      {
        // Get possible 2nd byte
        delay(10); // Allow time for the byte to be transmitted
        if (Serial.available())
        {
          int tempRecipient = Serial.read();
          if (recipient) // If recipient > 0, then the first digit was greater than '1'
          {
            Serial.println("Invalid");
            Serial.println();
            return;
          }
          else
          {
            recipient = 10 + tempRecipient - 49;
          }
        }
        Serial.println(recipient + 1);
        break;
      }
    }
  }

  // Get action
  Serial.print("Action (1 = On, 0 = Off): ");
  while (true)
  {
    if (Serial.available())
    {
      action = Serial.read() - 48; // - 48 converts '0' => 0, '1' => 1
      if (action > 1)
      {
        Serial.println("Invalid");
        Serial.println();
        return;
      }
      else
      {
        Serial.println(action ? "On" : "Off");
        break;
      }
    }
  }

  // Send message
  homeEasy.sendSimpleProtocolMessage(sender, recipient, action);
  Serial.println("Message sent");
  Serial.println();
}
