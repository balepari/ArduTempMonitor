// This is a definition file to handle all the nedded definitions

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
byte mac[] =
{
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

Chrono contaSecondi1(Chrono::SECONDS);
Chrono contaSecondi2(Chrono::SECONDS);
Chrono contaMillisecondi1;


EthernetUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "ntp1.inrim.it", 7200);

uint8_t bell[8] = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
uint8_t note[8] = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
uint8_t clocks[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t duck[8] = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
uint8_t check[8] = {0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};
uint8_t cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
uint8_t retarrow[8] = { 0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};
uint8_t slash[8] = {0x0, 0x10, 0x8, 0x4, 0x2, 0x1, 0x0};

//Enable server UPD transmission
//#define GOUDP

//Enable clock's second show on lcd
//#define printSecond

//Enable Serial logging
//define Log2Serial

int ciclo = 0;

Sd2Card card;
const int chipSelect = 4;	//chipselect SDCard on Arduino Ethernet Shield
