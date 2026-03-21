#ifndef BUBI_HPP
#define BUBI_HPP
#include <Arduino.h>

// Typdefinitionen
typedef struct {
	const uint8_t		leds[8];																						// LED-Pins
	const uint8_t		btn;																								// Button-Pin
	const uint16_t	longPress;																					// Zeit in ms, die für einen Longclick benötigt wird
	const uint8_t 	prMax;																							// Anzahl der Pattern
	const uint16_t 	hold[4];																						// Pausen zwischen den Mustern in ms
	const char 			*msg[4];																						// Ausgabe der Modus-Namen im Serial-Monitor
} rock_t;

typedef enum { NOCLICK = 0, SHORTCLICK, LONGCLICK } clickType_t;			// Klick-Status als Enumeration
typedef enum { STAY = 0, FORWARD, BACKWARD, PING } modeType_t;				// Programm-Modi als Enumeration

typedef struct {
	modeType_t 			mode;																								// aktueller Modus
	uint8_t 				ltClick; 																						// Anzahl der Longclicks	
	uint8_t 				output;																							// aktuelles Ausgangsmuster
	clickType_t 		click;																							// Click-Status
	const rock_t 		*rPtr;																							// Zeiger auf die globale Konstantenstruktur
} config_t;	

// Klassendefinitionen
class Button {																												// Input: Click, Output: Click-Status
	private:
		config_t &rg;
	public:
		Button(config_t &rg) : rg(rg) {};
		void update();
};

class Handler {																												// Input: Click-Status, Output: Modus, Longclick-Zähler, Ausgangsmuster
	private:
		config_t &rg;
		uint32_t nextTime;
		void shortClick();
		void shortLoop();
		void longClick();
		void rol();
		void ror();
		void ping();
	public:
		Handler(config_t &rg) : rg(rg), nextTime(0) {}
		void update();		
};

class Show {																													// Input: Ausgangsmuster, Output: LED-Zustand
	private:
		config_t &rg;
		uint8_t dat;
	public:
		Show(config_t &rg) : rg(rg), dat(0x0F) {};
		void update();
};

// Implementierung der Methoden

void Button::update() {
	static uint32_t nextTime = 0;
	static bool busy = false;
	if(busy) {
		if(millis() < nextTime) return;
		uint8_t c1 = digitalRead(rg.rPtr->btn);
		rg.click = (c1 == LOW) ? LONGCLICK : SHORTCLICK;
		while(digitalRead(rg.rPtr->btn) == LOW);  												// Warte auf Loslassen
		busy = false;
	} else {
		uint8_t c0 = digitalRead(rg.rPtr->btn);
		if(c0 == LOW) {
			busy = true;
			nextTime = millis() + rg.rPtr->longPress;  											// Warte auf möglichen Longclick
			delay(10);  																										// Entprellzeit
		}
		rg.click = NOCLICK;
	}
}	

void Handler::shortClick() {
	uint8_t temp = rg.mode + 1; 
	temp %= rg.rPtr->prMax;  																						// Modulo-Operation, um innerhalb der Anzahl der Pattern zu bleiben	
	Serial.println("Aktueller Modus: " + String(rg.rPtr->msg[temp]));  	// Ausgabe des neuen Modus im Serial-Monitor
	rg.mode = static_cast<modeType_t>(temp);  													// Modulo-Ergebnis in Modus-Enumeration umwandeln
	rg.click = NOCLICK;		
};

void Handler::shortLoop() {
	uint32_t now = millis();
	if(now < nextTime) return;
	nextTime = now + rg.rPtr->hold[rg.mode];
	switch (rg.mode) {
		case STAY: rg.output = 0; break;
		case FORWARD:  ror(); break;
		case BACKWARD: rol(); break;
		case PING: ping(); break;
	}
}

void Handler::longClick() {
	uint8_t temp = rg.ltClick + 1;  																		// Longclick incrementieren
	Serial.println("Long-Click: " + String(temp));											// um delay zu vermeiden, wird der Zähler inkrementiert und nicht direkt im Handler
	rg.ltClick = temp;  																								// Longclick-Zähler zurücksetzen
	rg.click = NOCLICK; 
}

void Handler::update() {
	switch (rg.click) {
		case NOCLICK: break;
		case SHORTCLICK: shortClick(); break;
		case LONGCLICK: longClick(); break;	
	}
	shortLoop();
}

void Handler::rol() {																									// Verschiebt alle Bits um eine Position nach links
	rg.output = (rg.output << 1); if(!rg.output) rg.output = 0b00000001;
}  

void Handler::ror() {																									// Verschiebt alle Bits um eine Position nach rechts			
	rg.output = (rg.output >> 1); if(!rg.output) rg.output = 0b10000000;
}  	

void Handler::ping() {																								// Schiebt die 1 von links nach rechts und zurück, bis das Muster wieder am Anfang ist
	typedef enum { LEFT, RIGHT } direction_t;
	static direction_t dir = LEFT;
	switch (dir) {
		case LEFT: 	if(rg.output == 0b10000000) dir = RIGHT; else rg.output = (rg.output << 1); break;
		case RIGHT: if(rg.output == 0b00000001) dir = LEFT; else rg.output = (rg.output >> 1); break;
	}
}

void Show::update() {
	if(dat != rg.output) {	
		dat = rg.output;
		for(uint8_t i = 0; i < 8; i++) {
			digitalWrite(rg.rPtr->leds[i], (dat >> i) & 0x01);
		}
		if(dat==0) Serial.println("Output: -"); 	
		else {
			char buffer[9];  																							// Puffer für die Binärdarstellung des Musters
			for(uint8_t i = 0; i < 8; i++) {
				buffer[7 - i] = (dat & (0x01 << i)) ? '\x2A' : '\x20';  		// Fülle den Puffer mit '*' oder ' ' entsprechend den Bits des Musters
			}
			buffer[8] = '\0';  																						// Nullterminator für die String-Ausgabe
			Serial.println(buffer);																	
		}											
	}
}

#endif	// BINGO_HPP

