/**
 * SubAndFunc.h
 */

/*! \file SubAndFunc.h
    \brief File containing prototypes and functions to keem main source more clean as possible

    In first section there will be prototypes of functions and subroutines.
    In second section there will be functions and subroutines code's
*/

#ifndef SUBANDFUNC_H
#define SUBANDFUNC_H
#include <Arduino.h>

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
int wireSetup(int addr);
void lcdSetup(int cols, int rows, int becklight, LiquidCrystal_I2C lcd);
void piripi(void);
void bip(void);
void bop(void);
void mazalavecia(void);
void syncRTCtoNTP(Timezone myTZ, DS3231M_Class orologio);
void printLcdDateTime(LiquidCrystal_I2C lcd, DateTime dt);
float getSensorTemp(OneWire oneWire, DallasTemperature sensors(&oneWire));
void printTempLcd(LiquidCrystal_I2C lcd);
DateTime getRTCdateTime(DS3231M_Class orologio);
void aleSignal(int ciclo, LiquidCrystal_I2C lcd);
// void gestisciTastiera(void);
void printLcdMenu(LiquidCrystal_I2C lcd);
void mostraContenutoSD_serialPrint(Sd2Card card, SdVolume volume, SdFile root);
void mostraContenutoSD(File objFile, int tabs);
void printDirectory(File dir, int numTabs);
void cancellaFileSD(LiquidCrystal_I2C lcd, File objFile);
void scriviDatiSD(void):

#endif