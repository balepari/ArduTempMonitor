

#include <Chrono.h>
#include <LightChrono.h>

#include <ezTime.h> //for NTP query and sync

#include <LiquidCrystal_I2C.h>	//LCD con interfaccia I2C

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
//#include <NTPClient.h>

#include <DS3231.h>
#include <Wire.h>

#include <OneWire.h>

#include <DallasTemperature.h>

#include <EthernetUdp.h>
#include <EthernetClient.h>
#include <Dhcp.h>
#include <EthernetServer.h>
//#include <Ethernet2.h>
//#include <util.h>
#include <Dns.h>
//#include <Twitter.h>

#include "definitions.h"

void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(115200);
	int error;
	Wire.begin();
	Wire.beginTransmission(0x3F);
	error = Wire.endTransmission();
	if (error == 0)
	{
		Serial.println("LCD Found!");
		lcd.begin(20, 4);
		lcd.setBacklight(255);
		lcd.home();
		lcd.clear();
		lcd.createChar(0, bell);
		lcd.createChar(1, note);
		lcd.createChar(2, clocks);
		lcd.createChar(3, heart);
		lcd.createChar(4, duck);
		lcd.createChar(5, check);
		lcd.createChar(6, cross);
		lcd.createChar(7, retarrow);
		lcd.createChar(8, slash);
	}
	else
	{
		Serial.println("ERROR: LCD not found.");
	}

	// start the Ethernet connection:
	Serial.println("Initialize Ethernet with DHCP:");
	lcd.home();
	lcd.clear();
	lcd.print("Init Eth & SD...");
	lcd.blink();
	if (Ethernet.begin(mac) == 0)
	{
		Serial.println("Failed to configure Ethernet using DHCP");
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("DHCP Eth FAILED!");
	}

	if (Ethernet.hardwareStatus() == EthernetNoHardware)
	{
		Serial.println("Ethernet shield was not found.");
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("Eth Shield MISSING!");
	}

	if (Ethernet.hardwareStatus() == EthernetW5100)
	{
		Serial.println("W5100 Ethernet controller detected.");
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("Found W5100 Module");
	}

	if (Ethernet.hardwareStatus() == EthernetW5200)
	{
		Serial.println("W5200 Ethernet controller detected.");
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("Found W5200 Module");
	}

	if (Ethernet.hardwareStatus() == EthernetW5500)
	{
		Serial.println("W5500 Ethernet controller detected.");
		lcd.noBlink();
		lcd.noCursor();
		lcd.setCursor(0, 2);
		lcd.print("Found W5500 Module");
	}
	delay(2000);

	// print your local IP address:
	Serial.print("My IP address: ");
	for (byte thisByte = 0; thisByte < 4; thisByte++)
	{
		//		 print the value of each byte of the IP address:
		Serial.print(Ethernet.localIP()[thisByte], DEC);
		Serial.print(".");
	}
	lcd.home();
	lcd.clear();
	lcd.noBlink();
	lcd.noCursor();
	lcd.print("IP: ");
	lcd.setCursor(4, 0);
	lcd.print(Ethernet.localIP()[0]);
	lcd.print(".");
	lcd.print(Ethernet.localIP()[1]);
	lcd.print(".");
	lcd.print(Ethernet.localIP()[2]);
	lcd.print(".");
	lcd.print(Ethernet.localIP()[3]);

	Serial.println();

	Serial.print("The DNS server IP address is: ");
	Serial.println(Ethernet.dnsServerIP());

	lcd.setCursor(0, 1);
	lcd.print("DNS: ");
	lcd.setCursor(5, 1);
	lcd.print(Ethernet.dnsServerIP());

	delay(1000);
	//init SDCard and set noLog define
	lcd.setCursor(0, 2);
	lcd.print("Init SDCard...");
	if (!card.init(SPI_HALF_SPEED, chipSelect))	//!SD.begin(chipSelect)
	{
		Serial.println("Card failed, or not present");

		lcd.setCursor(19, 2);
		lcd.printByte(6);
		lcd.setCursor(0, 3);
		lcd.print("No SDCard ");
		lcd.printByte(0x7e);
		lcd.print(" No LOG");
		// don't do anything more:
		//while (1);
	}
	else
	{
		Serial.println("card initialized.");
		lcd.setCursor(19, 2);
		lcd.printByte(5);	//should be a bell symbol
		lcd.setCursor(0, 3);
		lcd.print("Logging to SDCard...");
	}

	delay(3000);

	lcd.clear();
	lcd.setCursor(0, 3);
	lcd.blink();
	lcd.print("Starting up...");
	delay(3000);

	lcd.clear();
	lcd.home();

	lcd.print("Link remote server");
	ethUdp.begin(localPort);
	delay(1500);
	lcd.setCursor(19, 0);
	lcd.printByte(5);

	lcd.setCursor(0, 1);
	lcd.print("Power up sensors");
	sensors.begin();
	delay(1500);
	lcd.setCursor(19, 1);
	lcd.printByte(5);


	lcd.setCursor(0, 2);
	lcd.print("Getting RTC ON");
	orologio.begin();
	delay(1500);
	lcd.setCursor(19, 2);
	lcd.printByte(5);

	lcd.setCursor(0, 3);
	lcd.print("Get time from NTP");
	//	timeClient.begin();
	setDebug(INFO);
	setServer("ntp1.inrim.it");
	waitForSync();

	myTZ.setLocation(F("Europe/Rome"));
	Serial.println(myTZ.dateTime());

	delay(1500);
	lcd.setCursor(19, 3);
	lcd.printByte(5);

	lcd.noBlink();
	lcd.noCursor();
	//lcd.clear();

	// Set sketch compiling time
	//	clock.setDateTime(__DATE__, __TIME__);
	delay(2500);

	//		updateNTP();

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
}

void syncRTCtoNTP()
{
	//	int anno;
	//	int mese;
	//	int giorno;
	//	int ore;
	//	int minuti;
	//	int secondi;

	updateNTP();
	Serial.println("NTP   " + myTZ.dateTime());
	delay(1000);
	Serial.println("Passing to RTC:  " + myTZ.dateTime("Y") + " " + myTZ.dateTime("n") + " " + myTZ.dateTime("j") + " " + myTZ.dateTime("G") + " " + myTZ.dateTime("i") + " " + myTZ.dateTime("s"));
	//	anno = myTZ.dateTime("Y").toInt();
	//	mese = myTZ.dateTime("n").toInt();
	//	giorno = myTZ.dateTime("j").toInt();
	//	ore = myTZ.dateTime("G").toInt();
	//	minuti = myTZ.dateTime("i").toInt();
	//	secondi = myTZ.dateTime("s").toInt();

	orologio.setDateTime(myTZ.dateTime("Y").toInt(), myTZ.dateTime("n").toInt(), myTZ.dateTime("j").toInt(), myTZ.dateTime("G").toInt(), myTZ.dateTime("i").toInt(), myTZ.dateTime("s").toInt());
	//	orologio.setDateTime(anno, mese, giorno, ore, minuti, secondi);
	delay(1000);
	dt = orologio.getDateTime();

	// For leading zero look to DS3231_dateformat example

	Serial.print("GET Raw data: ");
	Serial.print(dt.year);	 Serial.print("/");
	Serial.print(dt.month);	Serial.print("/");
	Serial.print(dt.day);		Serial.print(" ");
	Serial.print(dt.hour);	 Serial.print(":");
	Serial.print(dt.minute); Serial.print(":");
	Serial.print(dt.second); Serial.println("");
}

void printLcdDateTime()
{
	lcd.noBlink();
	lcd.noCursor();
	lcd.setCursor(0, 0);
	if (String(dt.day).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.day);
	}
	else
	{
		lcd.print(dt.day);
	}

	lcd.print("/");

	if (String(dt.month).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.month);
	}
	else
	{
		lcd.print(dt.month);
	}

	lcd.print("/");
	lcd.print(dt.year);

#ifndef printSecond //if second has to not be shown, then shift right clock to upper right
	lcd.setCursor(15, 0);
#endif

#ifdef printSecond
	lcd.setCursor(12, 0);
#endif

	if (String(dt.hour).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.hour);
	}
	else
	{
		lcd.print(dt.hour);
	}

	lcd.print(":");

	if (String(dt.minute).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.minute);
	}
	else
	{
		lcd.print(dt.minute);
	}

#ifdef printSecond
	lcd.print(":");
	if (String(dt.second).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.second);
	}
	else
	{
		lcd.print(dt.second);
	}
#endif
}

void getSensorTemp()
{
	float temp;
	sensors.requestTemperatures(); // Send the command to get temperatures
	Serial.println(sensors.getTempCByIndex(0));
}

void printTempLcd()
{
	lcd.setCursor(0, 1);
	lcd.print("T out: ");
	lcd.print(sensors.getTempCByIndex(0));
}

void getRTCdateTime()
{
	dt = orologio.getDateTime();
}

void aleSignal()
{
	switch (ciclo)
	{
		case 0:
			lcd.setCursor(19, 3);
			lcd.printByte(0xB0);
			ciclo = ciclo + 1;
			break;

		case 1:
			lcd.setCursor(19, 3);
			lcd.printByte(8);
			ciclo = ciclo + 1;
			break;

		case 2:
			lcd.setCursor(19, 3);
			lcd.printByte(0x7C);
			ciclo = ciclo + 1;
			break;

		case 3:
			lcd.setCursor(19, 3);
			lcd.printByte(0x2F);
			ciclo = 0;
			break;

		default:
			ciclo = 0;
	}

	//	lcd.setCursor(19, 3);
	//	lcd.blink();
}

void loop()
{
	syncRTCtoNTP();
	lcd.home();
	lcd.clear();
	getRTCdateTime();
	printLcdDateTime();
	getSensorTemp();
	printTempLcd();

	while (true)
	{

		if (contaSecondi1.hasPassed(60))
		{
			contaSecondi1.restart();
			getRTCdateTime();
			printLcdDateTime();
		}

		if (contaSecondi2.hasPassed(301))
		{
			contaSecondi2.restart();
			getSensorTemp();
			printTempLcd();

		}

		if (contaMillisecondi1.hasPassed(250))
		{
			contaMillisecondi1.restart();
			aleSignal();
		}

		//		Serial.print("Raw data: ");
		//		Serial.print(dt.year);	 Serial.print("-");
		//		Serial.print(dt.month);	Serial.print("-");
		//		Serial.print(dt.day);		Serial.print(" ");
		//		Serial.print(dt.hour);	 Serial.print(":");
		//		Serial.print(dt.minute); Serial.print(":");
		//		Serial.print(dt.second); Serial.println("");

#ifdef GOUDP
		// send a reply, to the IP address and port that sent us the packet we received
		Serial.println("Sending data to remote IP...");
		ethUdp.beginPacket(ip_remoto, 5005);
		temp = sensors.getTempCByIndex(0);
		char tempDallas[0];
		dtostrf(temp, 0, 2, tempDallas);
		ethUdp.write(tempDallas);
		ethUdp.endPacket();
#endif
		//		aleSignal();
		//		delay(1000);
	}
}
