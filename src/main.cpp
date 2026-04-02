#include "bubi.hpp"
//  I2C + LCD configuration
const uint8_t I2C_Addr 			= 0x27;             											// I2C Adresse
const uint8_t LCD_cols 			= 16;               											// LCD Spalten
const uint8_t LCD_rows 			= 2;                											// LCD Zeilen


const rock_t rock = {																									// globale Konstanten
	.leds 			= { GPIO_NUM_18, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_16, GPIO_NUM_17 },
	.btn 				= GPIO_NUM_19,
	.lcdData		= { I2C_Addr, LCD_cols, LCD_rows },
	.longPress 	=	400,
	.prMax 			= 4,
	.hold 			= { 20, 800, 500, 300 },
	.msg 				= { "WAIT     ", "FORWARD ", "BACKWARD", "PINGPONG" }
};

config_t co = {																												// globale Variable - wird in den Klassen als Referenz übergeben
	.mode 			= STAY,
	.ltClick 		= 0,
	.output 		= 0,
	.outstr			= NULL,
	.refresh		= true,
	.click 			= NOCLICK,
	.rPtr 			= &rock
};

Button button(co);  																									// Button-Objekt mit Referenz auf Konfigurationsstruktur
Handler handler(co);  																								// Handler-Objekt mit Referenz auf Konfigurationsstruktur
Show show(co);                                          							// Show-Objekt mit Referenz auf Konfigurationsstruktur
Lcd lcd(co);																													// LCD-Objekt mit Referenz auf Konfigurationsstruktur


void init(config_t &rg) {																							// Alle Pins initialisieren, wird in setup() aufgerufen
	pinMode(rg.rPtr->btn, INPUT_PULLUP);
	for(uint8_t i = 0; i < 8; i++) {
		pinMode(rg.rPtr->leds[i], OUTPUT);
		digitalWrite(rg.rPtr->leds[i], LOW);
	}
	rg.outstr = (char*) malloc(9 * sizeof(char));  											// Speicher für die Binärdarstellung des Musters reservieren
	if(rg.outstr == NULL) {
		Serial.println("Fehler: Kein Speicher verfügbar!");
		while(1);  																												// Endlosschleife, wenn kein Speicher verfügbar ist
	}
	rg.outstr[8] = '\0';  																							// Nullterminator für die String-Ausgabe
}

void setup() {
	Serial.begin(115200);
	delay(1000);  																											// Warte auf Serial-Monitor
	Serial.println("\nBubi gestartet.\nAktueller Modus: " + String(rock.msg[co.mode]));  		
	init(co); 
	lcd.init();																												  // Initialisierung LCD und Anzeige der Startinformationen
}

void loop() {
	static uint32_t nextTime = 0;
	if(millis() > nextTime) {
		nextTime = millis() + co.rPtr->hold[co.mode];  										// Aktualisierungsintervall entsprechend des aktuellen Modus setzen
		co.refresh = true;  																							// Update-Flag setzen, damit die Anzeige aktualisiert wird
	}
	button.update();
	handler.update();
	show.update();
	lcd.update();
	co.refresh = false;  																								// Update-Flag zurücksetzen, um unnötige Updates zu vermeiden
	delay(10);  																												// Kurze Pause, um CPU-Last zu reduzieren
}






