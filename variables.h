/**
 * This file contains variables definition
 *
 */

//! LiquidCrystal_I2C Library for communication with I<SUP>2</SUP>C LCD
/*!
  \param 0x3F is the address on I<SUP>2</SUP>C bus for LCD
  \param 20 is the number of cols of LCD
  \param 4 is the number of rows of LCD
 */
LiquidCrystal_I2C myLcd(0x3F, 20, 4);	


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

unsigned int localPort = 5005;	/*!< The local port needed to start communications - not sure if it's necessary if I only throw message to remote IP without needing an answer back... SHOULD BE INVESTIGATED */

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];	/*!< The packet buffer needed to start communications  */

EthernetUDP ethUdp;	/*!< Defining EthernetUDP object needed for communications */

//! Creating object to talk to 1-wire Dallas device
/*!
  \param oneWire the name of the object of type OneWire that create communications
  \param ONE_WIRE_BUS the specifications of what pin should be used to be the 1-Wire bus
 */
OneWire 1_Wire(ONE_WIRE_BUS);

//! Creating object on 1-wire Dallas bus to communicate with.
/*!
  \param sensors the name of the object of type DallasTemperature that create communications
  \param &oneWire the specifications of what object handles the 1-Wire bus communications
 */
DallasTemperature dsTemp(&1_Wire);


DS3231M_Class orologio1;	/*!< Defining object for RTC Clock. Don't call object 'clock' because that should be a reserved word of the library used in subs or funcitions */

DateTime Tardis;	/*!< The structure object that handles data for date and time */
Timezone myTZ;	/*!< The structure object that handles timezone data for correct NTP query */

/*!
  Using Chrono library you can create as many timer as you want (the only limit is your arduino memory)
 */
Chrono contaSecondi1(Chrono::SECONDS);
Chrono contaSecondi2(Chrono::SECONDS);
Chrono contaMillisecondi1;
Chrono contaMillisecondi2;


EthernetUDP ntpUDP;	/*!< Creating object of type EthernetUPD to communicate with NTP server via UDP */


//! Some special character created to be pun in CGRAM of LCD
/*!
  \param a,b,c,d,e,f,g,h the resulting HEX value of single row cell composing the lcd dot that have to be turned on to draw symbol. My lcd has a character made by 5 dot cols and 8 rows. If a value is not represented it will be set to zero.
 */
uint8_t bell[8] = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
uint8_t note[8] = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
uint8_t clocks[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t duck[8] = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
uint8_t check[8] = {0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};
uint8_t cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
uint8_t retarrow[8] = { 0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};
uint8_t slash[8] = {0x0, 0x10, 0x8, 0x4, 0x2, 0x1, 0x0};



int ciclo = 0;	//indice per l'aleSignal


/*!
 * With last version (2.0) of SD library something changed.
 * Also the arduino.cc online documentations didn't show all the member and functions that can be used
 * Not only standard SD command are available, but you can use also the ones imported from library called internally by SD.h
 */
Sd2Card scheda;	/*!< Handles the object to talk with SDCard */
SdVolume filesys;	/*!< Handles the object to interact with the filesystem on SDCard */
SdFile radice;	/*!< Handles the object to work on filesystem of SDVolume type anda accessing them (usually files and dirs) */

File pippo;	//RINOMINARE IN QUALCOSA DI PIU' DECENTE !!!

const int chipSelect = 4; /*!< Indicates what pin should be used to be the SDCard chipselect (not only on Arduino Ethernet Shield) */
bool sdPresent = false;	/*!< just a simple variable to store if we have SDCard module present or not */


/*!
  Having a numeric kaypad you should define how it's composed so the library can work correctly
 */ 
const byte ROWS = 4;	/*!< four rows */
const byte COLS = 4;	/*!< four columns */

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


