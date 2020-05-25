#include <Chrono.h>
#include <LightChrono.h>

#include <ezTime.h> //for NTP query and sync

#include <LiquidCrystal_I2C.h>	//LCD con interfaccia I2C

#include <SPI.h>
#include <Ethernet.h>

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

#define ONE_WIRE_BUS 3

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)	write(args);
#else
#define printByte(args)	print(args,BYTE);
#endif

//LiquidCrystal_PCF8574 lcd(0x3F);
LiquidCrystal_I2C lcd(0x3F, 20, 4);

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
	0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};
IPAddress ip_remoto(10, 1, 2, 17);
unsigned int localPort = 5005;			// local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];	// buffer to hold incoming packet,

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
//EthernetClient client;

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP ethUdp;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DS3231 orologio;
RTCDateTime dt;
Timezone myTZ;

Chrono timer1;

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.inrim.it", 7200);

uint8_t bell[8] = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
uint8_t note[8] = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
uint8_t clocks[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t duck[8] = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
uint8_t check[8] = {0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};
uint8_t cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
uint8_t retarrow[8] = { 0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};

//Enable server UPD transmission
//#define GOUDP

//Enable clock's second show on lcd
//#define printSecond

void setup() {
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
	} else
	{
		Serial.println("ERROR: LCD not found.");
	}

	// start the Ethernet connection:
	Serial.println("Initialize Ethernet with DHCP:");
	lcd.home();
	lcd.clear();
	lcd.print("Init Eth DHCP...");
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
	} else
	{
	lcd.print(dt.day);
	}

	lcd.print("/");

	if (String(dt.month).length() == 1)
	{
	lcd.print("0");
	lcd.print(dt.month);
	} else
	{
	lcd.print(dt.month);
	}

	lcd.print("/");
	lcd.print(dt.year);

#ifndef printSecond //if second has to not be shown, then shift right clock to upper right
	//		lcd.print("		 ");
	lcd.setCursor(15, 0);
#endif

#ifdef printSecond
	//		lcd.print("	");
	lcd.setCursor(12, 0);
#endif

	if (String(dt.hour).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.hour);
	} else
	{
		lcd.print(dt.hour);
	}

	lcd.print(":");

	if (String(dt.minute).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.minute);
	} else
	{
		lcd.print(dt.minute);
	}

#ifdef printSecond
	lcd.print(":");
	if (String(dt.second).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.second);
	} else
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
	lcd.setCursor(19,3);
	lcd.blink();
}

void loop() {
	syncRTCtoNTP();
	lcd.home();
	lcd.clear();

	while (true)
	{

		

		getRTCdateTime();
		printLcdDateTime();
		getSensorTemp();
		printTempLcd();

		Serial.print("Raw data: ");
		Serial.print(dt.year);	 Serial.print("-");
		Serial.print(dt.month);	Serial.print("-");
		Serial.print(dt.day);		Serial.print(" ");
		Serial.print(dt.hour);	 Serial.print(":");
		Serial.print(dt.minute); Serial.print(":");
		Serial.print(dt.second); Serial.println("");

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
		aleSignal();
		delay(1000);
	}
}
