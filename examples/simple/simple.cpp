#include "application.h"

#include "log-local.h"

SYSTEM_MODE(MANUAL);

// Use primary serial over USB interface for logging output
SerialLogHandler logHandler;

// Store logs locally for sending later
LogLocalHandler logLocalHandler(1500, LOG_LEVEL_ALL, {
	{ "system", LOG_LEVEL_WARN },
	{ "app", LOG_LEVEL_WARN },
	{ "comm", LOG_LEVEL_WARN }
});

// instantiate some loggers with categories
Logger logSimple("s");
Logger logNetwork("n");
Logger logLogic("l");

uint32_t num = 0;

void setup() {

	Serial.begin(9600);

	// wait for someone to watch
	while(!Serial.available() && millis() < 10000) {};
	Serial.println("Start logging example");
	
	// have to recall
	// this also happens inside the first call to log, so don't have to
	// do this unless you want to send something out really early, like
	// an error that crashed your code
	logLocalHandler.recall();

	// print out the logs saved in the handler
	Serial.printf("LocalLog length: %i", logLocalHandler.logLength());
	Serial.print("\n    ");
	for (uint16_t i = 0; i < logLocalHandler.logLength(); ++i)
	{
		Serial.printf("%c", logLocalHandler.returnValue(i));
		if (logLocalHandler.returnValue(i) == '\n')
		{
			Serial.print("    ");
		}
	}
	Serial.println();

}

void loop() {

	// log something into our file handler
	logSimple.info(String::format("%i", num++));

	// log an error to network
	// in our handler we also have a flag for when an error occurs, so
	// you can choose to take action on it (like send to cloud)
	logNetwork.error("NC");
	Serial.printlnf("Error State %i", logLocalHandler.errorFlagState());
	// you can also clear the flag
	logLocalHandler.errorFlagClear();
	Serial.printlnf("Error State %i", logLocalHandler.errorFlagState());

	// log a warning to logic category
	logLogic.warn("RUN");

	// the log is stored in a uint8_t buffer and so can be printed 
	// out with a for statement
	Serial.printf("LocalLog length: %i", logLocalHandler.logLength());
	Serial.print("\n    ");
	for (uint16_t i = 0; i < logLocalHandler.logLength(); ++i)
	{
		Serial.printf("%c", logLocalHandler.returnValue(i));
		if (logLocalHandler.returnValue(i) == '\n')
		{
			Serial.print("    ");
		}
	}
	Serial.println();

	
	delay(1000);
}