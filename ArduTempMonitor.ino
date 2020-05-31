#include <Key.h>
#include <Keypad.h>


#include <Chrono.h>
#include <LightChrono.h>

#include <ezTime.h> //for NTP query and sync

#include <LiquidCrystal_I2C.h> //LCD con interfaccia I2C

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

#include <DS3231M.h>
// #include <DS3231.h>
#include <Wire.h>

#include <OneWire.h>

#include <DallasTemperature.h>

#include <EthernetUdp.h>
#include <EthernetClient.h>
#include <Dhcp.h>
#include <EthernetServer.h>
#include <Ethernet.h>
#include <Dns.h>

#include "definitions.h"
#include "pitches.h"


void setup()
{
	piripi();
	delay(500);
// Open serial communications and wait for port to open:
	Serial.begin(115200);

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
// start the Ethernet connection:
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
//#ifdef Log2Serial
	Serial.println("My IP address: ");
	Serial.print("byte 0: "); Serial.println(Ethernet.localIP()[0]);
	Serial.print("byte 1: "); Serial.println(Ethernet.localIP()[1]);
	Serial.print("byte 2: "); Serial.println(Ethernet.localIP()[2]);
	Serial.print("byte 3: "); Serial.println(Ethernet.localIP()[3]);

/*	for (byte thisByte = 0; thisByte < 4; thisByte++)
   {
   //		 print the value of each byte of the IP address:
   Serial.print(Ethernet.localIP()[thisByte]);
   Serial.print(".");
   }*/
//#endif

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
//init SDCard and set noLog define
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
		Serial.println("############################  SD.begin() FALLITO");
	}
	else
	{
#ifdef Log2Serial
		Serial.println("card initialized.");
#endif
		lcd.setCursor(19, 2);
		lcd.printByte(5); //should be a bell symbol
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
	// orologio.begin();
	while (!orologio1.begin())
	{
		// Initialize RTC communications
		Serial.println("Unable to find DS3231M. Checking again in 3 seconds.");
		delay(3000);
	} // of loop until device is located
	delay(1500);
	lcd.setCursor(19, 2);
	lcd.printByte(5);
	bip();
	delay(100);

	lcd.setCursor(0, 3);
	lcd.print("Get time from NTP");
//	timeClient.begin();
	setDebug(INFO);
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
//lcd.clear();

// Set sketch compiling time
//	clock.setDateTime(__DATE__, __TIME__);
	delay(2500);
#ifdef Log2Serial
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
#endif
	mazalavecia();
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
	printLcdMenu();


	while (true)
	{

		if (contaSecondi1.hasPassed(60))
		{
			contaSecondi1.restart();
			getRTCdateTime();
			printLcdDateTime();
			printLcdMenu();
		}

		if (contaSecondi2.hasPassed(61))
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

		if (contaMillisecondi2.hasPassed(50))
		{
			contaMillisecondi2.restart();
			gestisciTastiera();
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


// void(* Riavvia)(void) = 0;
int wireSetup(int addr)
{
	int error;
	Wire.begin();
	Wire.beginTransmission(addr);
	error = Wire.endTransmission();
	return error;
}

void lcdSetup(int cols, int rows, int becklight) //YES! I named it wrong because there was a reserved word called backlight...OUCH!
{
	lcd.begin(cols, rows);
	lcd.setBacklight(becklight);
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

void piripi()
{
	int melody[] = {NOTE_C4, NOTE_E5, NOTE_G6}; //, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
	int duration = 300; // 500 miliseconds
	for (int thisNote = 0; thisNote < 3; thisNote++)
	{
// pin11 output the voice, every scale is 0.5 sencond
		tone(11, melody[thisNote], duration);
		delay(100);
	}
}

void bip()
{
	tone(11, NOTE_E7, 100);
}

void bop()
{
	tone(11, NOTE_C4, 600);
}

void mazalavecia()
{
	int normale = 500;
	int croma = 200;

	tone(11, NOTE_C5, normale);
	delay(600);
	tone(11, NOTE_G4, croma);
	delay(200);
	tone(11, NOTE_G4, croma);
	delay(300);
	tone(11, NOTE_A4, normale);
	delay(200);
	tone(11, NOTE_G4, normale);
	delay(800);
	tone(11, NOTE_B4, normale);
	delay(400);
	tone(11, NOTE_C5, normale);
	delay(400);
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

	// orologio.setDateTime(myTZ.dateTime("Y").toInt(), myTZ.dateTime("n").toInt(), myTZ.dateTime("j").toInt(), myTZ.dateTime("G").toInt(), myTZ.dateTime("i").toInt(), myTZ.dateTime("s").toInt());
//	orologio.setDateTime(anno, mese, giorno, ore, minuti, secondi);
/*
   orologio.setClockMode(false); //imposta formato 24h
   byte anno = myTZ.dateTime("Y").toInt();
   Serial.print(myTZ.dateTime("Y"));
   Serial.print(" - ");
   Serial.print(myTZ.dateTime("Y").toInt());
   Serial.print(" - ");
   Serial.print((myTZ.dateTime("Y").toInt() / 10 * 16) + (myTZ.dateTime("Y").toInt() % 10)); //(val/10*16) + (val%10)
   Serial.print(" - ");
   Serial.print((((myTZ.dateTime("Y").toInt() / 10 * 16) + (myTZ.dateTime("Y").toInt() % 10)) / 16 * 10) + (((myTZ.dateTime("Y").toInt() / 10 * 16) + (myTZ.dateTime("Y").toInt() % 10)) % 16)); //(val/10*16) + (val%10)
   Serial.print(" - ");
   Serial.println();
   Serial.print(anno); Serial.print(" / "); Serial.print((2020 / 10 * 16) + (2020 % 10)); Serial.println (" / ");

   orologio.setYear(2020);//myTZ.dateTime("Y").toInt());
   orologio.setMonth(myTZ.dateTime("n").toInt());
   orologio.setDate(myTZ.dateTime("j").toInt());
   orologio.setHour(myTZ.dateTime("G").toInt());
   orologio.setMinute(myTZ.dateTime("i").toInt());
   orologio.setSecond(myTZ.dateTime("s").toInt());
   delay(1000);

   dt = RTC.now();//= orologio.getDateTime();

   // For leading zero look to DS3231_dateformat example

   Serial.print("GET Raw data: ");
   Serial.print(dt.year());  Serial.print("/");
   Serial.print(dt.month()); Serial.print("/");
   Serial.print(dt.day());  Serial.print(" ");
   Serial.print(dt.hour());  Serial.print(":");
   Serial.print(dt.minute()); Serial.print(":");
   Serial.print(dt.second()); Serial.println("");
 */

	// DS3231M.adjust(DateTime(2017,8,5,18,19,20));; // Set the RTC to "2017-08-05 18:19:20"
	Serial.println();
	Serial.println();
	Serial.println("orologio1.now(): ");

	DateTime adesso = orologio1.now();

	Serial.print("Year:   "); Serial.println(adesso.year());
	Serial.print("Month:  "); Serial.println(adesso.month());
	Serial.print("Day:    "); Serial.println(adesso.day());
	Serial.print("Hour:   "); Serial.println(adesso.hour());
	Serial.print("Minute: "); Serial.println(adesso.minute());
	Serial.print("Second: "); Serial.println(adesso.second());

	orologio1.adjust(DateTime(myTZ.dateTime("Y").toInt(), myTZ.dateTime("n").toInt(), myTZ.dateTime("j").toInt(), myTZ.dateTime("G").toInt(), myTZ.dateTime("i").toInt(), myTZ.dateTime("s").toInt()));

	adesso = orologio1.now();
	Serial.print("Year:   "); Serial.println(adesso.year());
	Serial.print("Month:  "); Serial.println(adesso.month());
	Serial.print("Day:    "); Serial.println(adesso.day());
	Serial.print("Hour:   "); Serial.println(adesso.hour());
	Serial.print("Minute: "); Serial.println(adesso.minute());
	Serial.print("Second: "); Serial.println(adesso.second());

}

void printLcdDateTime()
{
	lcd.noBlink();
	lcd.noCursor();
	lcd.setCursor(0, 0);
	if (String(dt.day()).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.day());
	}
	else
	{
		lcd.print(dt.day());
	}

	lcd.print("/");

	if (String(dt.month()).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.month());
	}
	else
	{
		lcd.print(dt.month());
	}

	lcd.print("/");
	lcd.print(dt.year());

#ifndef printSecond //if second has to not be shown, then shift right clock to upper right
	lcd.setCursor(15, 0);
#endif

#ifdef printSecond
	lcd.setCursor(12, 0);
#endif

	if (String(dt.hour()).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.hour());
	}
	else
	{
		lcd.print(dt.hour());
	}

	lcd.print(":");

	if (String(dt.minute()).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.minute());
	}
	else
	{
		lcd.print(dt.minute());
	}

#ifdef printSecond
	lcd.print(":");
	if (String(dt.second()).length() == 1)
	{
		lcd.print("0");
		lcd.print(dt.second());
	}
	else
	{
		lcd.print(dt.second());
	}
#endif
}

void getSensorTemp()
{
	float temp;
	sensors.requestTemperatures(); // Send the command to get temperatures
	Serial.println(sensors.getTempCByIndex(0));
/*	fileIP = SD.open("ip.txt", FILE_WRITE);
   if (fileIP)
   {
   fileIP.println(Ethernet.localIP());
   fileIP.close();
   } else
   {
   bop();
   Serial.println("########## FILE IP non aperto!");
   }

   fileDataOra = SD.open("dataora.txt", FILE_WRITE);
   if (fileDataOra)
   {
   fileDataOra.print(dt.year);
   fileDataOra.print(":");
   fileDataOra.print(dt.month);
   fileDataOra.print(":");
   fileDataOra.print(dt.day);
   fileDataOra.print(":");
   fileDataOra.print(dt.hour);
   fileDataOra.print(":");
   fileDataOra.println(dt.minute);
   fileDataOra.close();
   } else
   {
   bop();
   Serial.println("########## FILE DATAORA non aperto!");
   }
   fileTemp = SD.open("temp.txt", FILE_WRITE);

   if (fileTemp)
   {
   fileTemp.println(sensors.getTempCByIndex(0));
   fileTemp.close();
   } else
   {
   bop();
   Serial.println("########## FILE TEMP non aperto!");
   }
 */


}

void printTempLcd()
{
	lcd.setCursor(0, 1);
	lcd.print("T out: ");
	lcd.print(sensors.getTempCByIndex(0));
}

void getRTCdateTime()
{
	// dt = RTC.now();//orologio.getDateTime();
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

void gestisciTastiera()
{
	char customKey = customKeypad.getKey();
/*


 */

	switch (customKey)
	{
	case '1':
		Serial.print("Premuto: ");
		Serial.println(customKey);
		bip();
		contaMillisecondi2.stop();
		mostraContenutoSD_serialPrint();
		Serial.println("di nuovo in gestisciTastiera");
		contaMillisecondi2.resume();
		break;
	case '2':
		contaMillisecondi2.stop();
		Serial.print("Timer contaMillisecondi2 is Running: "); Serial.println(contaMillisecondi2.isRunning());
		Serial.println(Ethernet.localIP());
		Serial.println(Ethernet.localIP()[0]);
		Serial.println(Ethernet.localIP()[1]);
		Serial.println(Ethernet.localIP()[2]);
		Serial.println(Ethernet.localIP()[3]);
		contaMillisecondi2.resume();
		Serial.print("Timer contaMillisecondi2 is Running: "); Serial.println(contaMillisecondi2.isRunning());
		break;
	case '3':
		Serial.print("Premuto: ");
		Serial.println(customKey);
		bip();
		contaMillisecondi2.stop();
		mostraContenutoSD();
		Serial.println("di nuovo in gestisciTastiera");
		contaMillisecondi2.resume();
		break;
	case 'A':
		bip();
		break;
	case 'B':
		bip();
		break;
	case 'D':
		bip();
// Riavvia();
		break;
	case 'C':
		bop();
		break;
	default:
		break;
	}

}


void printLcdMenu()
{
	lcd.setCursor(0, 3);
	lcd.print("A-Menu B-Stat D-Rst");
}

void mostraContenutoSD_serialPrint()
{
//mostra il contenuto dell'SD usando un simbolo se è una directory
	Serial.println("Mostro contenuto SD");
	Serial.println();
	Serial.print("Card type:         ");
	switch (card.type()) {
	case SD_CARD_TYPE_SD1:
		Serial.println("SD1");
		break;
	case SD_CARD_TYPE_SD2:
		Serial.println("SD2");
		break;
	case SD_CARD_TYPE_SDHC:
		Serial.println("SDHC");
		break;
	default:
		Serial.println("Unknown");
	}

	if (!volume.init(card)) {
		Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
		while (1);
	}
// print the type and size of the first FAT-type volume
	uint32_t volumesize;
	Serial.print("Volume type is:    FAT");
	Serial.println(volume.fatType(), DEC);

	volumesize = volume.blocksPerCluster(); // clusters are collections of blocks
	volumesize *= volume.clusterCount(); // we'll have a lot of clusters
	volumesize /= 2; // SD card blocks are always 512 bytes (2 blocks are 1KB)
	Serial.print("Volume size (Kb):  ");
	Serial.println(volumesize);
	Serial.print("Volume size (Mb):  ");
	volumesize /= 1024;
	Serial.println(volumesize);
	Serial.print("Volume size (Gb):  ");
	Serial.println((float)volumesize / 1024.0);

	Serial.println("\nFiles found on the card (name, date and size in bytes): ");
	root.openRoot(volume);

	// list all files in the card with date and size
	root.ls(LS_R | LS_DATE | LS_SIZE);

	Serial.println("FineContenutoSD");

}

void mostraContenutoSD()
{
	pippo = SD.open("/");

	printDirectory(pippo, 0);

}

void printDirectory(File dir, int numTabs)
{
	while (true)
	{
		Serial.println("Dentro a printDirectory");
		File entry =  dir.openNextFile();
		if (!entry)
		{
// no more files
			break;
		}
		for (uint8_t i = 0; i < numTabs; i++)
		{
			Serial.print('\t');
		}
		Serial.print(entry.name());
		if (entry.isDirectory())
		{
			Serial.println("/");
			printDirectory(entry, numTabs + 1);
		}
		else
		{
// files have sizes, directories do not
			Serial.print("\t\t");
			Serial.println(entry.size(), DEC);
		}
		entry.close();
	}
}

void scriviDatiSD()
{
	//scrive i dati sulla SD in 3 file separati: IP.txt, DataOra.txt e Temp.txt per verifica gestione di più file

}
