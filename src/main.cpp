#include "bubi.hpp"

const rock_t rock = {																							// globale Konfigurationsstruktur mit allen relevanten Parametern
	.leds 			= { GPIO_NUM_18, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_16, GPIO_NUM_17 },
	.btn 				= GPIO_NUM_19,
	.longPress 	=	400,
	.prMax 			= 4,
	.hold 			= { 20, 1000, 300, 200 },
	.msg 				= { "WAIT", "FORWARD", "BACKWARD", "PINGPONG" }
};

config_t co = {																										// globale Konfigurationsstruktur mit allen relevanten Parametern
	.mode 			= STAY,
	.ltClick 		= 0,
	.output 		= 0,
	.click 			= NOCLICK,
	.rPtr 			= &rock
};

Button button(co);  																							// Button-Objekt mit Referenz auf Konfigurationsstruktur
Handler handler(co);  																						// Handler-Objekt mit Referenz auf Konfigurationsstruktur
Show show(co);                                          					// Show-Objekt mit Referenz auf Konfigurationsstruktur

void init(const rock_t *rPtr) {																		// Alle Pins initialisieren, wird in setup() aufgerufen
	pinMode(rPtr->btn, INPUT_PULLUP);
	for(uint8_t i = 0; i < 8; i++) {
		pinMode(rPtr->leds[i], OUTPUT);
		digitalWrite(rPtr->leds[i], LOW);
	}
}

void setup() {
	Serial.begin(115200);
	delay(1000);  																									// Warte auf Serial-Monitor
	Serial.println("\nBubi gestartet.\nAktueller Modus: " + String(rock.msg[co.mode]));  		// Ausgabe des Startmodus im Serial-Monitor
	init(&rock);  																									// Initialisierung der Pins
}

void loop() {
	button.update();
	handler.update();
	show.update();
	delay(10);  																										// Kurze Pause, um CPU-Last zu reduzieren
}






