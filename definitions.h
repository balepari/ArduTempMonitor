/**
 * This file contains definition
 *
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
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)	write(args);
#else
#define printByte(args)	print(args,BYTE);
#endif


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