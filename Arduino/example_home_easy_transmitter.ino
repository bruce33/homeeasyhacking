/*
 * Example program using the HomeEasy class.
 * Extends the receiver program by including a (poorly implemented) transmission
 * routine that accepts input from the Serial Port and transmits simple protocol
 * messages.
 */

#include "HomeEasy.h"

HomeEasy homeEasy;

/**
 * Set up the serial interface and the HomeEasy class.
 */
void setup() {
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
void loop() {
  sendSimpleMessage();
}

/**
 * Print the details of the advanced protocol message.
 */
void printAdvancedResult(unsigned long sender, unsigned int recipient, bool on, bool group) {
  Serial.println("advanced");
  
  Serial.print("s:");
  Serial.println(sender);
  
  Serial.print("r:");
  Serial.println(recipient);
  
  Serial.println(on ? "On" : "Off");
  
  Serial.println(group ? "Group" : "Not group");
  
  Serial.println();
}

/**
 * Print the details of the simple protocol message.
 */
void printSimpleResult(unsigned int sender, unsigned int recipient, bool on) {
  Serial.println("simple");
  
  Serial.print("s:");
  Serial.println(sender);
  
  Serial.print("r:");
  Serial.println(recipient);
  
  Serial.println(on ? "On" : "Off");
  
  Serial.println();
}

/**
 * Print the details of the BBSB 2011 protocol message.
 */
void printBBSB2011Result(unsigned int sender, unsigned int recipient, bool on, bool group) {
  Serial.println("bbsb2011");
  
  Serial.print("s:");
  Serial.println(sender);
  
  Serial.print("r:");
  Serial.println(recipient);
  
  Serial.println(on ? "On" : "Off");
  
  Serial.println(group ? "Group" : "Not group"); // Not sure this part of the library is working as I'd intended at the moment
  
  Serial.println();
}

/**
 * Gets a simple message from the Serial port and transmits it
 */
void sendSimpleMessage() {
  unsigned int sender = 0;
  unsigned int recipient = 100;
  unsigned int action = false;

  // Print instructions
  Serial.println("Please enter your command in the format Sender-Recipient-Command.");
  Serial.println("  The Sender should be a single character A - P.");
  Serial.println("  The Recipient should be one or two digits 1 - 16.");
  Serial.println("  The Command should be a string, OFF or ON.");
  Serial.println("    e.g. H-4-OFF or N-14-ON");
  Serial.print(":> ");

  // Get sender
  while (true) {
    if (Serial.available()) {
      sender = Serial.read() - 65; // - 65 converts 'A' => 0, 'B' => 1, ...
      if (sender > 15) {
        Serial.println("Invalid sender.");
        Serial.println();
        return;
      } else {
        Serial.write(sender + 65);
        Serial.print("-");
        break;
      }
    }
  }

  // Get recipient
  while (true) {
    if (Serial.available()) {
      unsigned int newRecipient = Serial.read();
      if (newRecipient == '-') {
        if (recipient != 100) {
          break;
        }
        continue;
      }
      newRecipient -= 49; // - 49 converts '1' => 0, '2' => 1, ...
      if (newRecipient > 15) {
        Serial.println("Invalid recipient.");
        Serial.println();
        return;
      } else if (recipient != 100) {
        recipient = 10 + newRecipient;
        Serial.write(newRecipient + 49);
      } else {
        recipient = newRecipient;
        Serial.write(newRecipient + 49);
      }
    }
  }
  Serial.print("-");

  // Get action
  unsigned int actionString = 0;
  bool continueActionLoop = true;
  while (continueActionLoop) {
    if (Serial.available()) {
      actionString += Serial.read();
      Serial.println(actionString);
      switch (actionString) {
        case 0:
        case 79:
        case 149:
          continue;
        case 157: // ON
          action = 1;
          continueActionLoop = false;
          break;
        case 219: // OFF
          action = 0;
          continueActionLoop = false;
          break;
        default:
          Serial.println("Invalid command.");
          Serial.println();
          return;
      }
    }
  }
  Serial.println(action ? "ON" : "OFF");

  // Send message
  homeEasy.sendSimpleProtocolMessage(sender, recipient, action);
  Serial.println("Message sent");
  Serial.println();
  Serial.flush();
}
