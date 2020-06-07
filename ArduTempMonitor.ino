#include <Arduino.h>

#include <Key.h>
#include <Keypad.h>

#include <Chrono.h>
#include <LightChrono.h>

#include <ezTime.h> //for NTP query and sync

// #include <LiquidCrystal_I2C.h> //LCD con interfaccia I2C
#include <LiquidCrystal_PCF8574.h>

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

#include <avr/sleep.h>

// #include "definitions.h"
// #include "variables.h"
#include "pitches.h"
// #include "SubAndFunc.h"


/**
 ****************************************
 * 										*
 *       D E F I N I T I O N S			*
 * 										*
 ****************************************
 */

/**
 * Using Ethernet shield from Keyestudio is necessary to foce the ezTime library to not use WiFi and get stuck without finding it
 */
#define EZTIME_NETWORK_ENABLE
#define EZTIME_ETHERNET


/**
 * Defining the pin used to be the 1-Wire Dallas communication port
 */
#define ONE_WIRE_BUS 3


/**
 * in I2C library is necessary to correct behavior of printing to lcd functions
 */
// #if defined(ARDUINO) && ARDUINO >= 100
// #define printByte(args)	write(args);
// #else
// #define printByte(args)	print(args,BYTE);
// #endif


/**
 * Uncomment to have UDP sending message to remote server to be active
 */
//#define GOUDP


/**
 * Uncomment to show second con LCD
 */
//#define printSecond


/**
 * Uncomment to send messages to Serial port (USB) to debug purpose
 */
#define Log2Serial

/**
 ****************************************
 * 										*
 *			V A R I A B L E S 			*
 * 										*
 ****************************************
 */

//! LiquidCrystal_PCF8574 Library for communication with I<SUP>2</SUP>C LCD
/*!
   \param 0x3F is the address on I<SUP>2</SUP>C bus for LCD
   \param 20 is the number of cols of LCD
   \param 4 is the number of rows of LCD
 */
LiquidCrystal_PCF8574 myLcd(0x3F); //, 20, 4);


//! Ethernet Shield require a mac address if not specified by productor
/*!
   \param {xxx} are the six octect forming the mac address written in HEX notation
 */
byte mac[] =
{
	0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

//! IPAddress object to estabilish an ethernet connection
/*!
   \param a,b,c,d are the 4 octect defining the remote IP address to connect to. They must be in DEC format
 */
IPAddress ip_remoto(10, 1, 2, 17);

unsigned int localPort = 5005; /*!< The local port needed to start communications - not sure if it's necessary if I only throw message to remote IP without needing an answer back... SHOULD BE INVESTIGATED */

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; /*!< The packet buffer needed to start communications  */

EthernetUDP ethUdp; /*!< Defining EthernetUDP object needed for communications */

//! Creating object to talk to 1-wire Dallas device
/*!
   \param oneWire the name of the object of type OneWire that create communications
   \param ONE_WIRE_BUS the specifications of what pin should be used to be the 1-Wire bus
 */
OneWire one_Wire (ONE_WIRE_BUS);

//! Creating object on 1-wire Dallas bus to communicate with.
/*!
   \param sensors the name of the object of type DallasTemperature that create communications
   \param &oneWire the specifications of what object handles the 1-Wire bus communications
 */
DallasTemperature dsTemp(&one_Wire);


DS3231M_Class orologio1; /*!< Defining object for RTC Clock. Don't call object 'clock' because that should be a reserved word of the library used in subs or funcitions */

DateTime Tardis; /*!< The structure object that handles data for date and time */
Timezone myTZ; /*!< The structure object that handles timezone data for correct NTP query */

/*!
   Using Chrono library you can create as many timer as you want (the only limit is your arduino memory)
 */
Chrono contaSecondi1(Chrono::SECONDS);
Chrono contaSecondi2(Chrono::SECONDS);
Chrono contaMillisecondi1;
Chrono contaMillisecondi2;


EthernetUDP ntpUDP; /*!< Creating object of type EthernetUPD to communicate with NTP server via UDP */


//! Some special character created to be pun in CGRAM of LCD
/*!
   \param a,b,c,d,e,f,g,h the resulting HEX value of single row cell composing the lcd dot that have to be turned on to draw symbol. My lcd has a character made by 5 dot cols and 8 rows. If a value is not represented it will be set to zero.
 */
int bobina[8] = {0x0, 0x4, 0xa, 0x11, 0x11, 0xa, 0x4, 0x0};
int lowerSx[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1e, 0x0};
int lowerDx[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf, 0x0};
int upperDx[8] = {0x0, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
int upperSx[8] = {0x0, 0x1e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
int check[8] = {0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};
// int cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
int retarrow[8] = { 0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};
int slash[8] = {0x0, 0x10, 0x8, 0x4, 0x2, 0x1, 0x0};



/*!
 * With last version (2.0) of SD library something changed.
 * Also the arduino.cc online documentations didn't show all the member and functions that can be used
 * Not only standard SD command are available, but you can use also the ones imported from library called internally by SD.h
 */
Sd2Card scheda; /*!< Handles the object to talk with SDCard */
SdVolume filesys; /*!< Handles the object to interact with the filesystem on SDCard */
SdFile radice; /*!< Handles the object to work on filesystem of SDVolume type anda accessing them (usually files and dirs) */

File pippo; //RINOMINARE IN QUALCOSA DI PIU' DECENTE !!!

const int chipSelect = 4; /*!< Indicates what pin should be used to be the SDCard chipselect (not only on Arduino Ethernet Shield) */
bool sdPresent = false; /*!< just a simple variable to store if we have SDCard module present or not */


/*!
   Having a numeric kaypad you should define how it's composed so the library can work correctly
 */
const byte ROWS = 4; /*!< four rows */
const byte COLS = 4; /*!< four columns */

//! Define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};

//! define pin connection for keypad
byte rowPins[ROWS] = {23, 25, 27, 29}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {22, 24, 26, 28}; //connect to the column pinouts of the keypad

//! Initialize an instance of class NewKeypad
/*!
   \param customKeypad is the name of your object
   \param makeKeymap(hexaKeys) is the function that creates the image of the keypad
   \param hexaKeys is the matrix representing the character on keypad's key
   \param rowPins is the matrix of pins where the rows are connected
   \param colsPins is the matrix of pins where the cols are connected
   \param ROWS is the variable that indicates how many rows the keypad is composed
   \param COLS is the variable that indicates how many cols the keypad is composed
 */
Keypad tastierino = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//! defines state machine's state
/*!
   0 = on display shows: Date, Time, Temperature reading, aleSignal, recording status, ecc... and Menu
   1 = Shows utility menu with device info, SDCard status, ecc...
   2 = shows SDCard management menu with list SD contnent, erase single file, wipe SDCard
   3 = Shows data acquisition setup to set timing of record, nr of reading in record's timing, if keep only last value or calc mena value
   9 = Arduino "shutdown"
 */
int iStato = 0;
bool bRecordingStatus;
int iAleCiclo = 0;    //indice per l'aleSignal

/**
 ****************************************
 * 										*
 *			P R O T O T Y P E S 		*
 * 										*
 ****************************************
 */



/*! \fn int wireSetup(int addr)
   \brief Start setup of I<SUP>2</SUP>C.
   \param addr The address of device to communicate to.
   \param flags Opening flags.
 */

/*! \fn void lcdSetup(int cols, int rows, int becklight)
   \brief Setup LCD.
   \param cols The number of cols.
   \param rows The number of rows.
   \param becklight The backlight value (named wrong intentionally because backlight is a reserved word). On some lcd che value is indifferent, if >0 backlight on, if 0 backlight off
 */

/*! \fn void piripi(void)
   \brief plays a 3-tone sound
 */

/*! \fn void bip(void)
   \brief plays a bip sound
 */

/*! \fn void bop(void)
   \brief plays a deep bip (a bop) sound
 */

/*! \fn void mazalavecia(void)
   \brief Plays the famous dialect song "maza la vècia"
 */

/*! \fn void syncRTCtoNTP(void)
   \brief Sync RTC DS3231 with NTP server
 */

/*! \fn void printLcdDateTime(void)
   \brief Print Date and Time on LCD
 */

/*! \fn void getSensorTemp(void)
   \brief Retrieve sensor value from DS18B20
 */

/*! \fn void printTempLcd(void)
   \brief Print temperature read on LCD
 */

/*! \fn void getRTCdateTime(void)
   \brief Retrieve Date and Time from RTC DS3231
 */

/*! \fn void aleSignal(void)
   \brief Draws on lower right charter (col=19, row=3) a rotating symbol to demonstrate that program is Alive & Kicking
 */

/*! \fn void gestisciTastiera(void)
   \brief THIS IS ABOUT TO BE REWRITTEN FROM SCRATCH INTEGRATING IN A STATE MACHINE
 */

/*! \fn void printLcdMenu(void)
   \brief Prints the menu on LCD
 */

/*! \fn void mostraContenutoSD_serialPrint(void)
   \brief Dump on Serial the contnent of SDCard
 */

/*! \fn void mostraContenutoSD(void)
   \brief Should show the SDCard contnent on LCD
 */

/*! \fn void printDirectory(File dir, int numTabs)
   \brief Prints recursively what is in dirs
   \param dir is the object that handles access to filesystem
   \param numTabs is the number of tabs to ident data when printing on Serial
 */

/*! \fn void cancellaFileSD(void)
   \brief Delete ricorsively all SDCard contnent (only file by now - futher release have to manage also dirs)
 */

/*! \fn void scriviDatiSD(void)
   \brief Stores on SDCard data
 */

int wireSetup(int addr);
void lcdSetup(int cols, int rows, int becklight, LiquidCrystal_PCF8574 lcd);
void piripi(void);
void bip(void);
void bop(void);
void mazalavecia(void);
void syncRTCtoNTP(Timezone myTZ, DS3231M_Class orologio);
void printLcdDateTime(LiquidCrystal_PCF8574 lcd, DateTime dt);
float getSensorTemp(OneWire oneWire, DallasTemperature sensors);
void printTempLcd(LiquidCrystal_PCF8574 lcd);
DateTime getRTCdateTime(DS3231M_Class orologio);
int aleSignal(int ciclo, LiquidCrystal_PCF8574 lcd);
// void gestisciTastiera(void);
void printLcdMenu(LiquidCrystal_PCF8574 lcd, int stato);
void mostraContenutoSD_serialPrint(Sd2Card card, SdVolume volume, SdFile root);
void mostraContenutoSD(File objFile, int tabs);
void printDirectory(File dir, int numTabs);
void cancellaFileSD(LiquidCrystal_PCF8574 lcd, File objFile);
void scriviDatiSD(void);
void buonanotte(LiquidCrystal_PCF8574 lcd);
int iconarecorder(int step, LiquidCrystal_PCF8574 lcd);

void setup(void);
void loop(void);

/**
 ****************************************
 * 										*
 *			F U N C T I O N S 			*
 * 										*
 ****************************************
 */

int wireSetup(int addr)
{
	int error;
	Wire.begin();
	Wire.beginTransmission(addr);
	error = Wire.endTransmission();
	return error;
}

void lcdSetup(int cols, int rows, int becklight, LiquidCrystal_PCF8574 lcd) //YES! I named it wrong because there was a reserved word called backlight...OUCH!
{
	lcd.begin(cols, rows);
	lcd.setBacklight(becklight);
	lcd.home();
	lcd.clear();
	lcd.createChar(6, bobina);
	lcd.createChar(1, lowerSx);
	lcd.createChar(2, lowerDx);
	lcd.createChar(3, upperDx);
	lcd.createChar(4, upperSx);
	lcd.createChar(5, check);
	lcd.createChar(7, retarrow);
	lcd.createChar(8, slash);
}

void piripi(void)
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

void bip(void)
{
	tone(11, NOTE_E7, 100);
}

void bop(void)
{
	tone(11, NOTE_C4, 600);
}

void mazalavecia(void)
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

void syncRTCtoNTP(Timezone myTZ, DS3231M_Class orologio)
{
	updateNTP();
	#ifdef Log2Serial
		Serial.println("NTP   " + myTZ.dateTime());
	#endif
	delay(1000);
	#ifdef Log2Serial
		Serial.println("Passing to RTC:  " + myTZ.dateTime("Y") + " " + myTZ.dateTime("n") + " " + myTZ.dateTime("j") + " " + myTZ.dateTime("G") + " " + myTZ.dateTime("i") + " " + myTZ.dateTime("s"));
		// DS3231M.adjust(DateTime(2017,8,5,18,19,20));; // Set the RTC to "2017-08-05 18:19:20"
		Serial.println();
		Serial.println();
		Serial.println("orologio.now(): ");
	#endif
	DateTime adesso = orologio.now();
	#ifdef Log2Serial
		Serial.print("Year:   "); Serial.println(adesso.year());
		Serial.print("Month:  "); Serial.println(adesso.month());
		Serial.print("Day:    "); Serial.println(adesso.day());
		Serial.print("Hour:   "); Serial.println(adesso.hour());
		Serial.print("Minute: "); Serial.println(adesso.minute());
		Serial.print("Second: "); Serial.println(adesso.second());
	#endif
	orologio.adjust(DateTime(myTZ.dateTime("Y").toInt(), myTZ.dateTime("n").toInt(), myTZ.dateTime("j").toInt(), myTZ.dateTime("G").toInt(), myTZ.dateTime("i").toInt(), myTZ.dateTime("s").toInt()));

	adesso = orologio.now();
	#ifdef Log2Serial
		Serial.print("Year:   "); Serial.println(adesso.year());
		Serial.print("Month:  "); Serial.println(adesso.month());
		Serial.print("Day:    "); Serial.println(adesso.day());
		Serial.print("Hour:   "); Serial.println(adesso.hour());
		Serial.print("Minute: "); Serial.println(adesso.minute());
		Serial.print("Second: "); Serial.println(adesso.second());
	#endif
}

void printLcdDateTime(LiquidCrystal_PCF8574 lcd, DateTime dt)
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

	lcd.setCursor(15, 0);

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
}

float getSensorTemp(OneWire oneWire, DallasTemperature sensors) //(&oneWire))
{
	float temp;
	sensors.requestTemperatures(); // Send the command to get temperatures
	#ifdef Log2Serial
		Serial.println(sensors.getTempCByIndex(0));
	#endif
	return sensors.getTempCByIndex(0);
}

void printTempLcd(LiquidCrystal_PCF8574 lcd)
{
	lcd.setCursor(0, 1);
	lcd.print("T out: ");
	char tmpTemp[7];
	dtostrf(getSensorTemp(one_Wire, dsTemp), 5, 2, tmpTemp);
	lcd.print(tmpTemp);
}

DateTime getRTCdateTime(DS3231M_Class orologio)
{
	return orologio.now();
}

int aleSignal(int ciclo, LiquidCrystal_PCF8574 lcd)
{
	switch (ciclo)
	{
	case 0:
		lcd.setCursor(19, 3);
		lcd.write(0xB0);
		ciclo = ciclo + 1;
		break;

	case 1:
		lcd.setCursor(19, 3);
		lcd.write(8);
		ciclo = ciclo + 1;
		break;

	case 2:
		lcd.setCursor(19, 3);
		lcd.write(0x7C);
		ciclo = ciclo + 1;
		break;

	case 3:
		lcd.setCursor(19, 3);
		lcd.write(0x2F);
		ciclo = 0;
		break;

	default:
		ciclo = 0;
	}
	return ciclo;
}

void mostraContenutoSD_serialPrint(Sd2Card card, SdVolume volume, SdFile root)
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

void printDirectory(File dir, int numTabs)
{
	while (true)
	{
		// Serial.println("Dentro a printDirectory");
		File entry =  dir.openNextFile();
		if (!entry)
		{
			// no more files
			Serial.println("Niente da visualizzare sulla SD!");
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


void mostraContenutoSD(File objFile, int tabs)
{
	objFile = SD.open("/");

	printDirectory(objFile, tabs);
}

void cancellaFileSD(LiquidCrystal_PCF8574 lcd, File objFile, Keypad customKeypad)
{
	// contaMillisecondi2.stop();
	bip();
	lcd.clear();
	lcd.home();
	lcd.print("1-Eliminare tutto");
	lcd.setCursor(0,1);
	lcd.print("2-Scegli cosa elim.");
	lcd.setCursor(0,2);
	lcd.print("3-Annulla! Annulla!");
	while (true)
	{
		char tastopremuto = customKeypad.getKey();

		Serial.print("tastopremuto vale: ");
		Serial.println(tastopremuto);

		switch (tastopremuto)
		{
		case '1': //cancella tutti i file sulla SD
			bip();
			objFile = SD.open("/");
			while (true)
			{
				File entry =  pippo.openNextFile();
				if (!entry)
				{
					break;
				}
				SD.remove(entry.name());
			}
			lcd.clear();
			lcd.setCursor(7,1);
			lcd.print("FATTO!");
			delay(2000);
			break;
		case '2': //scegli il file da cancellare
			bop();
			lcd.clear();
			lcd.setCursor(0,1);
			lcd.print("FUNZIONE NON IMPLEM.");
			delay(1000);
			break;
		case '3': //Annulla Tutto !!
			bip();
			break;
		}
	}
}

/*void gestisciTastiera(Keypad customKeypad)
   {
   char customKey = customKeypad.getKey();


   switch (customKey)
   {
   case '1':
   Serial.print("Premuto: ");
   Serial.println(customKey);
   bip();
   contaMillisecondi2.stop();
   // mostraContenutoSD_serialPrint();
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
   case '4':
   bip();
   Serial.print("Premuto: ");
   Serial.println(customKey);
   cancellaFileSD();
   Serial.println("di nuovo in gestisciTastiera");
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

   }*/

void scriviDatiSD(void)
{
	//scrive i dati sulla SD in 3 file separati: IP.txt, DataOra.txt e Temp.txt per verifica gestione di più file
}

void buonanotte(LiquidCrystal_PCF8574 lcd)
{
	//esegue lo "shutdown" di arduino impostando il modo sleep e "spegnendo" l'LCD dopo un messaggio
	lcd.clear();
	lcd.setCursor(6,1);
	lcd.print("B Y E");
	lcd.setCursor(7,2);
	lcd.print("B Y E");
	delay(4000);
	lcd.clear();
	lcd.noDisplay();
	lcd.setBacklight(0);
	set_sleep_mode (SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();
}

int iconarecorder(int step, LiquidCrystal_PCF8574 lcd)
{
	switch (step)
	{
	case 0:
		lcd.setCursor(1, 2);
		lcd.write(6);
		lcd.print("  ");
		lcd.setCursor(4, 2);
		lcd.write(6);
		step++;
		break;

	case 1:
		lcd.setCursor(1, 2);
		lcd.write(6);
		lcd.setCursor(2, 2);
		lcd.write(1);
		lcd.print(" ");
		lcd.setCursor(4, 2);
		lcd.write(6);
		step++;
		break;

	case 2:
		lcd.setCursor(1, 2);
		lcd.write(6);
		lcd.setCursor(2, 2);
		lcd.write(1);
		lcd.setCursor(3, 2);
		lcd.write(2);
		lcd.setCursor(4, 2);
		lcd.write(6);
		step++;
		break;

	case 3:
		lcd.setCursor(1, 2);
		lcd.write(6);
		lcd.print(" ");
		lcd.setCursor(3, 2);
		lcd.write(2);
		lcd.setCursor(4, 2);
		lcd.write(6);
		step++;
		break;

	case 4:
		lcd.setCursor(1, 2);
		lcd.write(6);
		lcd.print("  ");
		lcd.setCursor(4, 2);
		lcd.write(6);
		step++;
		break;

	case 5: //bobina - spazio - upperDx - bobina
		lcd.setCursor(1, 2);
		lcd.write(6);
		lcd.print(" ");
		lcd.setCursor(3, 2);
		lcd.write(3);
		lcd.setCursor(4, 2);
		lcd.write(6);
		step++;
		break;

	case 6:
		lcd.setCursor(1, 2);
		lcd.write(6);
		lcd.setCursor(2, 2);
		lcd.write(4);
		lcd.setCursor(3, 2);
		lcd.write(3);
		lcd.setCursor(4, 2);
		lcd.write(6);
		step++;
		break;

	case 7:
		lcd.setCursor(1, 2);
		lcd.write(6);
		lcd.setCursor(2, 2);
		lcd.write(4);
		lcd.setCursor(3, 2);
		lcd.print(" ");
		lcd.setCursor(4, 2);
		lcd.write(6);
		step = 0;
		break;

	default:
		step = 0;
	}
	return step;
}

void printLcdMenu(LiquidCrystal_PCF8574 lcd, int stato)
{
	switch (stato)
	{
	case 0: // on display shows: Date, Time, Temperature reading, aleSignal, recording status, ecc... and Menu
		lcd.setCursor(0, 3);
		lcd.print("A-Menu B-Stat D-Bye!");
		break;

	case 1: // Shows utility menu with device info, SDCard status, ecc...
		break;

	case 2: // shows SDCard management menu with list SD contnent, erase single file, wipe SDCard
		break;

	case 3: // Shows data acquisition setup to set timing of record, nr of reading in record's timing, if keep only last value or calc mena value
		break;

	default:
		stato = 0;
		break;
	}


}

/**
 ****************************************
 * 										*
 *		 M A I N  P R O G R A M 		*
 * 										*
 ****************************************
 */


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
		lcdSetup(20, 4, 255, myLcd);
		myLcd.setBacklight(255);
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

	myLcd.home();
	myLcd.clear();
	myLcd.print("Init Eth & SD...");
	myLcd.blink();
	if (Ethernet.begin(mac) == 0)
	{
		#ifdef Log2Serial
			Serial.println("Failed to configure Ethernet using DHCP");
		#endif
		myLcd.noBlink();
		myLcd.noCursor();
		myLcd.setCursor(0, 2);
		myLcd.print("DHCP Eth FAILED!");
		bop();
	}

	if (Ethernet.hardwareStatus() == EthernetNoHardware)
	{
		#ifdef Log2Serial
			Serial.println("Ethernet shield was not found.");
		#endif
		myLcd.noBlink();
		myLcd.noCursor();
		myLcd.setCursor(0, 2);
		myLcd.print("Eth Shield MISSING!"); //non ho lo shiels quindi NO SDCard
		sdPresent = false;
		bop();
	}

	if (Ethernet.hardwareStatus() == EthernetW5100)
	{
		#ifdef Log2Serial
			Serial.println("W5100 Ethernet controller detected.");
		#endif
		myLcd.noBlink();
		myLcd.noCursor();
		myLcd.setCursor(0, 2);
		myLcd.print("Found W5100 Module");
		sdPresent = true;
		bip();
	}

	if (Ethernet.hardwareStatus() == EthernetW5200)
	{
		#ifdef Log2Serial
			Serial.println("W5200 Ethernet controller detected.");
		#endif
		myLcd.noBlink();
		myLcd.noCursor();
		myLcd.setCursor(0, 2);
		myLcd.print("Found W5200 Module");
		sdPresent = true;
		bip();
	}

	if (Ethernet.hardwareStatus() == EthernetW5500)
	{
		#ifdef Log2Serial
			Serial.println("W5500 Ethernet controller detected.");
		#endif
		myLcd.noBlink();
		myLcd.noCursor();
		myLcd.setCursor(0, 2);
		myLcd.print("Found W5500 Module");
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

	myLcd.home();
	myLcd.clear();
	myLcd.print("IP: ");
	myLcd.setCursor(4, 0);
	myLcd.print(Ethernet.localIP()[0]);
	myLcd.print(".");
	myLcd.print(Ethernet.localIP()[1]);
	myLcd.print(".");
	myLcd.print(Ethernet.localIP()[2]);
	myLcd.print(".");
	myLcd.print(Ethernet.localIP()[3]);
	bip();

	#ifdef Log2Serial
		Serial.println();
		Serial.print("The DNS server IP address is: ");
		Serial.println(Ethernet.dnsServerIP());
	#endif

	myLcd.setCursor(0, 1);
	myLcd.print("DNS: ");
	myLcd.setCursor(5, 1);
	myLcd.print(Ethernet.dnsServerIP());
	bip();

	delay(1000);
	myLcd.setCursor(0, 2);
	myLcd.print("Init SDCard...");
	if (!SD.begin(chipSelect))//!card.init(SPI_HALF_SPEED, chipSelect)
	{

		#ifdef Log2Serial
			Serial.println("Card failed, or not present");
		#endif
		myLcd.setCursor(19, 2);
		myLcd.print(6);
		myLcd.setCursor(0, 3);
		myLcd.print("No SDCard ");
		myLcd.write(0x7e);
		myLcd.print(" No LOG");
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
		myLcd.setCursor(19, 2);
		myLcd.write(5);
		myLcd.setCursor(0, 3);
		myLcd.print("Logging to SDCard...");
		bip();
	}

	delay(3000);


	myLcd.clear();
	myLcd.setCursor(0, 3);
	myLcd.blink();
	myLcd.print("Starting up...");
	tone(11, NOTE_D5, 300);
	delay(100);
	tone(11, NOTE_F7, 600);
	delay(3000);

	myLcd.clear();
	myLcd.home();

	myLcd.print("Link remote server");
	ethUdp.begin(localPort);
	delay(1500);
	myLcd.setCursor(19, 0);
	myLcd.write(5);
	bip();
	delay(100);

	myLcd.setCursor(0, 1);
	myLcd.print("Power up sensors");
	dsTemp.begin();
	delay(1500);
	myLcd.setCursor(19, 1);
	myLcd.write(5);
	bip();
	delay(100);

	myLcd.setCursor(0, 2);
	myLcd.print("Getting RTC ON");
	while (!orologio1.begin())
	{
		// Initialize RTC communications
		#ifdef Log2Serial
			Serial.println("Unable to find DS3231M. Checking again in 3 seconds.");
		#endif
		delay(3000);
	} // of loop until device is located
	delay(1500);
	myLcd.setCursor(19, 2);
	myLcd.write(5);
	bip();
	delay(100);

	myLcd.setCursor(0, 3);
	myLcd.print("Get time from NTP");
	setDebug(INFO); //NTP DEBUG LEVEL
	setServer("ntp1.inrim.it");
	waitForSync();

	myTZ.setLocation(F("Europe/Rome"));
	#ifdef Log2Serial
		Serial.println(myTZ.dateTime());
	#endif

	delay(1500);
	myLcd.setCursor(19, 3);
	myLcd.write(5);
	bip();

	myLcd.noBlink();
	myLcd.noCursor();
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
	syncRTCtoNTP(myTZ, orologio1);
	myLcd.home();

	myLcd.clear();
	Tardis = getRTCdateTime(orologio1);
	printLcdDateTime(myLcd, Tardis);
	getSensorTemp(one_Wire, dsTemp);
	printTempLcd(myLcd);
	while (true)
	{
		switch (iStato)
		{
		case 0: // on display shows: Date, Time, Temperature reading, aleSignal, recording status, ecc... and Menu
			if (bRecordingStatus) //true = devo registrare i dati
			{
				int iCicloIcona = 0;
				if (contaMillisecondi1.hasPassed(250))
				{
					contaMillisecondi1.restart();
					iCicloIcona = iconarecorder(iCicloIcona, myLcd);
				}
			} else
			{
				myLcd.setCursor(1, 2);
				myLcd.print("NO REC");
			}

			if (contaMillisecondi2.hasPassed(200))
			{
				contaMillisecondi2.restart();
				iAleCiclo = aleSignal(iAleCiclo, myLcd);
			}

			if (contaSecondi2.hasPassed(10))
			{
				contaSecondi2.restart();
				Tardis = getRTCdateTime(orologio1);
				printLcdDateTime(myLcd, Tardis);
				getSensorTemp(one_Wire, dsTemp);
				printTempLcd(myLcd);
			}
			printLcdMenu(myLcd, iStato);
			break;

		case 1: // Shows utility menu with device info, SDCard status, ecc...
			break;

		case 2: // shows SDCard management menu with list SD contnent, erase single file, wipe SDCard
			break;

		case 3: // Shows data acquisition setup to set timing of record, nr of reading in record's timing, if keep only last value or calc mena value
			break;

		case 9: // Arduino "shutdown"
			buonanotte(myLcd);
			break;

		default:
			iStato = 0;
			break;
		}
	}
}
/**
 ****************************************
 * 										*
 *				 E N D 					*
 * 										*
 ****************************************
 */

