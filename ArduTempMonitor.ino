#include <Key.h>
#include <Keypad.h>

#include <Chrono.h>
#include <LightChrono.h>

#include <ezTime.h> //for NTP query and sync

#include <LiquidCrystal_I2C.h> //LCD con interfaccia I2C

#include <SPI.h>
#include <SD.h>

#include <DS3231M.h>
#include <Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <EthernetClient.h>
#include <Dhcp.h>
#include <EthernetServer.h>
#include <Dns.h>

#include "definitions.h"
#include "variables.h"
#include "pitches.h"
#include "SubAndFunc.h"


void setup()
{
	piripi();
	delay(500);

#ifdef Log2Serial
	Serial.begin(115200);
#endif
	
	if (wireSetup(0x3F) == 0)
	{
#ifdef Log2Serial
		Serial.println("LCD Found!");
#endif
		lcdSetup(20, 4, 255);
		bip();
	}
	else
	{
#ifdef Log2Serial
		Serial.println("ERROR: LCD not found.");
#endif
		bop();
	}

#ifdef Log2Serial
	Serial.println("Initialize Ethernet with DHCP:");
#endif

	lcd.home();
	lcd.clear();
	lcd.print("Init Eth & SD...");
	lcd.blink();
	if (Ethernet.begin(mac) == 0)
	{
#ifdef Log2Serial
		Serial.println("Failed to configure Ethernet using DHCP");
#endif
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("DHCP Eth FAILED!");
		bop();
	}

	if (Ethernet.hardwareStatus() == EthernetNoHardware)
	{
#ifdef Log2Serial
		Serial.println("Ethernet shield was not found.");
#endif
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("Eth Shield MISSING!"); //non ho lo shiels quindi NO SDCard
		sdPresent = false;
		bop();
	}

	if (Ethernet.hardwareStatus() == EthernetW5100)
	{
#ifdef Log2Serial
		Serial.println("W5100 Ethernet controller detected.");
#endif
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("Found W5100 Module");
		sdPresent = true;
		bip();
	}

	if (Ethernet.hardwareStatus() == EthernetW5200)
	{
#ifdef Log2Serial
		Serial.println("W5200 Ethernet controller detected.");
#endif
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("Found W5200 Module");
		sdPresent = true;
		bip();
	}

	if (Ethernet.hardwareStatus() == EthernetW5500)
	{
#ifdef Log2Serial
		Serial.println("W5500 Ethernet controller detected.");
#endif
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("Found W5500 Module");
		sdPresent = true;
		bip();
	}
	delay(2000);

// print your local IP address:
#ifdef Log2Serial
/*
Per qualche strano motivo la stampa dell'indirizzo IP restituisce un numero sbagliato nel byte0
però l'indirizzo viene acquisito correttamente e se dopo il setup viene visualizzato i valori
dei byte appaiono corretti.
*/

	Serial.println("My IP address: ");
	Serial.print("byte 0: "); Serial.println(Ethernet.localIP()[0]);
	Serial.print("byte 1: "); Serial.println(Ethernet.localIP()[1]);
	Serial.print("byte 2: "); Serial.println(Ethernet.localIP()[2]);
	Serial.print("byte 3: "); Serial.println(Ethernet.localIP()[3]);
#endif

	lcd.home();
	lcd.clear();
	lcd.print("IP: ");
	lcd.setCursor(4, 0);
	lcd.print(Ethernet.localIP()[0]);
	lcd.print(".");
	lcd.print(Ethernet.localIP()[1]);
	lcd.print(".");
	lcd.print(Ethernet.localIP()[2]);
	lcd.print(".");
	lcd.print(Ethernet.localIP()[3]);
	bip();

#ifdef Log2Serial
	Serial.println();
	Serial.print("The DNS server IP address is: ");
	Serial.println(Ethernet.dnsServerIP());
#endif

	lcd.setCursor(0, 1);
	lcd.print("DNS: ");
	lcd.setCursor(5, 1);
	lcd.print(Ethernet.dnsServerIP());
	bip();

	delay(1000);
	lcd.setCursor(0, 2);
	lcd.print("Init SDCard...");
	if (!SD.begin(chipSelect))//!card.init(SPI_HALF_SPEED, chipSelect)
	{

#ifdef Log2Serial
		Serial.println("Card failed, or not present");
#endif
		lcd.setCursor(19, 2);
		lcd.printByte(6);
		lcd.setCursor(0, 3);
		lcd.print("No SDCard ");
		lcd.printByte(0x7e);
		lcd.print(" No LOG");
		bop();
#ifdef Log2Serial		
		Serial.println("############################  SD.begin() FALLITO");
#endif
	}
	else
	{
#ifdef Log2Serial
		Serial.println("card initialized.");
#endif
		lcd.setCursor(19, 2);
		lcd.printByte(5);
		lcd.setCursor(0, 3);
		lcd.print("Logging to SDCard...");
		bip();
	}

	delay(3000);


	lcd.clear();
	lcd.setCursor(0, 3);
	lcd.blink();
	lcd.print("Starting up...");
	tone(11, NOTE_D5, 300);
	delay(100);
	tone(11, NOTE_F7, 600);
	delay(3000);

	lcd.clear();
	lcd.home();

	lcd.print("Link remote server");
	ethUdp.begin(localPort);
	delay(1500);
	lcd.setCursor(19, 0);
	lcd.printByte(5);
	bip();
	delay(100);

	lcd.setCursor(0, 1);
	lcd.print("Power up sensors");
	sensors.begin();
	delay(1500);
	lcd.setCursor(19, 1);
	lcd.printByte(5);
	bip();
	delay(100);

	lcd.setCursor(0, 2);
	lcd.print("Getting RTC ON");
	while (!orologio1.begin())
	{
		// Initialize RTC communications
#ifdef Log2Serial
		Serial.println("Unable to find DS3231M. Checking again in 3 seconds.");
#endif
		delay(3000);
	} // of loop until device is located
	delay(1500);
	lcd.setCursor(19, 2);
	lcd.printByte(5);
	bip();
	delay(100);

	lcd.setCursor(0, 3);
	lcd.print("Get time from NTP");
	setDebug(INFO);	//NTP DEBUG LEVEL
	setServer("ntp1.inrim.it");
	waitForSync();

	myTZ.setLocation(F("Europe/Rome"));
#ifdef Log2Serial
	Serial.println(myTZ.dateTime());
#endif

	delay(1500);
	lcd.setCursor(19, 3);
	lcd.printByte(5);
	bip();

	lcd.noBlink();
	lcd.noCursor();
	delay(2500);

#ifdef Log2Serial
	Serial.println();
	Serial.println("Time in various internet standard formats ...");
	Serial.println();
	Serial.println("ATOM:        " + myTZ.dateTime(ATOM));
	Serial.println("COOKIE:      " + myTZ.dateTime(COOKIE));
	Serial.println("IS8601:      " + myTZ.dateTime(ISO8601));
	Serial.println("RFC822:      " + myTZ.dateTime(RFC822));
	Serial.println("RFC850:      " + myTZ.dateTime(RFC850));
	Serial.println("RFC1036:     " + myTZ.dateTime(RFC1036));
	Serial.println("RFC1123:     " + myTZ.dateTime(RFC1123));
	Serial.println("RFC2822:     " + myTZ.dateTime(RFC2822));
	Serial.println("RFC3339:     " + myTZ.dateTime(RFC3339));
	Serial.println("RFC3339_EXT: " + myTZ.dateTime(RFC3339_EXT));
	Serial.println("RSS:         " + myTZ.dateTime(RSS));
	Serial.println("W3C:         " + myTZ.dateTime(W3C));
	Serial.println();
	Serial.println(" ... and any other format, like \"" + myTZ.dateTime("l ~t~h~e jS ~o~f F Y, g:i A") + "\"");
#endif

	mazalavecia();
}

void loop()
{
	
}


