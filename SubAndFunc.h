/**
 * SubAndFunc.h
 */

/*! \file SubAndFunc.h
    \brief File containing prototypes and functions to keem main source more clean as possible

    In first section there will be prototypes of functions and subroutines.
    In second section there will be functions and subroutines code's
*/

#ifndef SubAndFunc_h
#define SubAndFunc_h
#endif

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

/**
 *
 * P R O T O T Y P E S
 *
 */
// int wireSetup(int addr);
// void lcdSetup(int cols, int rows, int becklight);
// void piripi(void);
// void bip(void);
// void bop(void);
// void mazalavecia(void);
// void syncRTCtoNTP(void);
// void printLcdDateTime(void);
// void getSensorTemp(void);
// void printTempLcd(void);
// void getRTCdateTime(void);
// void aleSignal(void);
// void gestisciTastiera(void);
// void printLcdMenu(void);
// void mostraContenutoSD_serialPrint(void);
// void mostraContenutoSD(void);
// void printDirectory(File dir, int numTabs);
// void cancellaFileSD(void);
// void scriviDatiSD(void):


/**
 *
 * S U B S  &  F U N C
 *
 */

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

void syncRTCtoNTP(void)
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

void printLcdDateTime(void)
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

void getSensorTemp(void)
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

void printTempLcd(void)
{
	lcd.setCursor(0, 1);
	lcd.print("T out: ");
	lcd.print(sensors.getTempCByIndex(0));
}

void getRTCdateTime(void)
{
	// dt = RTC.now();//orologio.getDateTime();
	dt = orologio1.now();
}

void aleSignal(void)
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

void mostraContenutoSD_serialPrint(void)
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


void mostraContenutoSD(void)
{
	pippo = SD.open("/");

	printDirectory(pippo, 0);

}

void cancellaFileSD(void)
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
			pippo = SD.open("/");
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

void gestisciTastiera(void)
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

}


void printLcdMenu(void)
{
	lcd.setCursor(0, 3);
	lcd.print("A-Menu B-Stat D-Rst");
}





void scriviDatiSD(void)
{
	//scrive i dati sulla SD in 3 file separati: IP.txt, DataOra.txt e Temp.txt per verifica gestione di più file

}
