#ifndef HomeEasyDefines_h
#define HomeEasyDefines_h

#include "HomeEasyPinDefines.h"
#include "HomeEasyPortDefines.h"

#define HESTANDARD 0
#define HEMEGA 1

#define HETIMER1 1
#define HETIMER4 4
#define HETIMER5 5

///////////////////////////////
// Only section that needs to be changed...
//
// If you are using a Mega, then HETIMER can be:
//  - HETIMER4
//  - HETIMER5
// Otherwise, it is ignored (it must be HETIMER1)
//
// If you are using a Mega, then HETXPORT may be selected from HEPORTA - HEPORTL
// Otherwise, it may be selected from HEPORTB - HEPORTD
//
// If you are using a Mega with HERXTIMER as HETIMER4, then HETXPIN may be selected from:
//  - PINx0 - PINx7, where x is the port selected for HETXPORT (HEPORTA - HEPORTK)
//  - PINL1 - PINL7, where HEPORTL is the port selected for HETXPORT
// If you are using a Mega with HERXTIMER as HETIMER5, then HETXPIN may be selected from:
//  - PINx0 - PINx7, where x is the port selected for HETXPORT (HEPORTA - HEPORTK)
//  - PINL0, PINL2 - PINL7, where HEPORTL is the port selected for HETXPORT
// Otherwise:
//  - PINx0 - PINx7, where x is the port selected for HETXPORT (HEPORTC, HEPORTD)
//  - PINB1 - PINB7, where HEPORTB is the port selected for HETXPORT
///////////////////////////////

#define HETIMER HETIMER4
#define HETXPORT HEPORTL
#define HETXPIN PINL1

///////////////////////////////
// End of required changes
///////////////////////////////

// The board is a mega
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  // Defaults for an Arduino Mega
  #if HETIMER == HETIMER1 // Set to Standard defaults, replace with Mega defaults
    #undef HETIMER
    #undef HETXPORT
    #undef HETXPIN
  #endif
  #ifndef HETIMER
    #define HETIMER HETIMER4
  #endif
  #ifndef HETXPORT
    #define HETXPORT HEPORTL
  #endif
  #ifndef HETXPIN
    #define HETXPIN PINL6
  #endif
  // RX port and pin
  #define HE_RXPORT PORTL
  #if HETIMER == HETIMER4
    #define HE_RXPIN PINL0
  #elif HETIMER == HETIMER5
    #define HE_RXPIN PINL1
  #endif
  #define HE_RXDDR DDRL
// The board is not a mega
#else
  // Defaults for a standard Arduino
  #ifndef HETIMER
    #define HETIMER HETIMER1
  #elif HETIMER != HETIMER1 // Still set to Mega defaults, replace with Standard defaults
    #undef HETIMER
    #define HETIMER HETIMER1
    #undef HETXPORT
    #undef HETXPIN
  #endif
  #ifndef HETXPORT
    #define HETXPORT HEPORTB
  #endif
  #ifndef HETXPIN
    #define HETXPIN PINB5
  #endif
  // RX port and pin
  #define HE_RXPORT PORTB
  #define HE_RXPIN PINB0
  #define HE_RXDDR DDRB
#endif

// Timers
// Timer1
#if HETIMER == HETIMER1
  #define HE_TCCRA TCCR1A
  #define HE_TCCRB TCCR1B
  #define HE_TIMSK TIMSK1
  #define HE_ICIE ICIE1
  #define HE_OCRA OCR1A
  #define HE_WGM2 WGM12
  #define HE_OCIEA OCIE1A
  #define HE_COMA0 COM1A0
  #define HE_TIMER_CAPT_vect TIMER1_CAPT_vect
  #define HE_TCNT TCNT1
  #define HE_ICR ICR1
  #define HE_ICES ICES1
  #define HE_TIMER_COMPA_vect TIMER1_COMPA_vect
// Timer4
#elif HETIMER == HETIMER4
  #define HE_TCCRA TCCR4A
  #define HE_TCCRB TCCR4B
  #define HE_TIMSK TIMSK4
  #define HE_ICIE ICIE4
  #define HE_OCRA OCR4A
  #define HE_WGM2 WGM42
  #define HE_OCIEA OCIE4A
  #define HE_COMA0 COM4A0
  #define HE_TIMER_CAPT_vect TIMER4_CAPT_vect
  #define HE_TCNT TCNT4
  #define HE_ICR ICR4
  #define HE_ICES ICES4
  #define HE_TIMER_COMPA_vect TIMER4_COMPA_vect
// Timer5
#elif HETIMER == HETIMER5
  #define HE_TCCRA TCCR5A
  #define HE_TCCRB TCCR5B
  #define HE_TIMSK TIMSK5
  #define HE_ICIE ICIE5
  #define HE_OCRA OCR5A
  #define HE_WGM2 WGM52
  #define HE_OCIEA OCIE5A
  #define HE_COMA0 COM5A0
  #define HE_TIMER_CAPT_vect TIMER5_CAPT_vect
  #define HE_TCNT TCNT5
  #define HE_ICR ICR5
  #define HE_ICES ICES5
  #define HE_TIMER_COMPA_vect TIMER5_COMPA_vect
#endif

// TX
// Port A
#if HETXPORT == HEPORTA
  #define HE_TXPORT PORTA
  #define HE_TXDDR DDRA
// Port B
#elif HETXPORT == HEPORTB
  #define HE_TXPORT PORTB
  #define HE_TXDDR DDRB
// Port C
#elif HETXPORT == HEPORTC
  #define HE_TXPORT PORTC
  #define HE_TXDDR DDRC
// Port D
#elif HETXPORT == HEPORTD
  #define HE_TXPORT PORTD
  #define HE_TXDDR DDRD
// Port E
#elif HETXPORT == HEPORTE
  #define HE_TXPORT PORTE
  #define HE_TXDDR DDRE
// Port F
#elif HETXPORT == HEPORTF
  #define HE_TXPORT PORTF
  #define HE_TXDDR DDRF
// Port G
#elif HETXPORT == HEPORTG
  #define HE_TXPORT PORTG
  #define HE_TXDDR DDRG
// Port H
#elif HETXPORT == HEPORTH
  #define HE_TXPORT PORTH
  #define HE_TXDDR DDRH
// Port I
#elif HETXPORT == HEPORTI
  #define HE_TXPORT PORTI
  #define HE_TXDDR DDRI
// Port J
#elif HETXPORT == HEPORTJ
  #define HE_TXPORT PORTJ
  #define HE_TXDDR DDRJ
// Port K
#elif HETXPORT == HEPORTK
  #define HE_TXPORT PORTK
  #define HE_TXDDR DDRK
// Port L
#elif HETXPORT == HEPORTL
  #define HE_TXPORT PORTL
  #define HE_TXDDR DDRL
#endif

// Other definitions
#define TRANSMITTER_MESSAGE_COUNT 5

#define MESSAGE_TYPE_SIMPLE 0
#define MESSAGE_TYPE_ADVANCED 1
#define MESSAGE_TYPE_BBSB2011 2

#endif // HomeEasyDefines_h
