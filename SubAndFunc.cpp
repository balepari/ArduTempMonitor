/**
 *
 * S U B S  &  F U N C
 *
 */

#include "SubAndFunc.h"

int wireSetup(int addr)
{
	int error;
	Wire.begin();
	Wire.beginTransmission(addr);
	error = Wire.endTransmission();
	return error;
}

void lcdSetup(int cols, int rows, int becklight, LiquidCrystal_I2C lcd) //YES! I named it wrong because there was a reserved word called backlight...OUCH!
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

void printLcdDateTime(LiquidCrystal_I2C lcd, DateTime dt)
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

float getSensorTemp(OneWire oneWire, DallasTemperature sensors(&oneWire))
{
	float temp;
	sensors.requestTemperatures(); // Send the command to get temperatures
 #ifdef Log2Serial
	Serial.println(sensors.getTempCByIndex(0));
 #endif
	return sensors.getTempCByIndex(0);
}

void printTempLcd(LiquidCrystal_I2C lcd)
{
	lcd.setCursor(0, 1);
	lcd.print("T out: ");
	lcd.print(getSensorTemp);
}

DateTime getRTCdateTime(DS3231M_Class orologio)
{
	return dt = orologio.now();
}

void aleSignal(int ciclo, LiquidCrystal_I2C lcd)
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

void cancellaFileSD(LiquidCrystal_I2C lcd, File objFile)
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


void printLcdMenu(LiquidCrystal_I2C lcd)
{
	lcd.setCursor(0, 3);
	lcd.print("A-Menu B-Stat D-Rst");
}





void scriviDatiSD(void)
{
	//scrive i dati sulla SD in 3 file separati: IP.txt, DataOra.txt e Temp.txt per verifica gestione di più file

}
