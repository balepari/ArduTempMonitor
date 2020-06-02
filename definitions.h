
/*-----------------------------------------------*/

// This is a definition file to handle all the nedded definitions

#define EZTIME_NETWORK_ENABLE
#define EZTIME_ETHERNET

#define ONE_WIRE_BUS 3

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)	write(args);
#else
#define printByte(args)	print(args,BYTE);
#endif


//Enable server UPD transmission
//#define GOUDP

//Enable clock's second show on lcd
//#define printSecond

//Enable Serial logging
#define Log2Serial