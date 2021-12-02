
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)


	void setupArduino()
	{
		tft.begin(); 																	// start the lcd library
    Serial.begin(9600); 													// start the arduino serial library
		Serial.println("ARDUINO LOADED!"); 						// print that the arduino loaded 
		IrReceiver.begin(irPin, ENABLE_LED_FEEDBACK); // start the ir library

		//Setting up the pins to be used
		pinMode(interruptPinM1,INPUT_PULLUP);
		pinMode(interruptPinM2,INPUT_PULLUP);
		pinMode(interruptPinM3,INPUT_PULLUP);
		pinMode(interruptPinM4,INPUT_PULLUP);
		//Setting up the interupt pins to be used
		attachInterrupt(digitalPinToInterrupt(interruptPinM1), intPinToMode1, FALLING);
		attachInterrupt(digitalPinToInterrupt(interruptPinM2), intPinToMode2, FALLING);
		attachInterrupt(digitalPinToInterrupt(interruptPinM3), intPinToMode3, FALLING);
		attachInterrupt(digitalPinToInterrupt(interruptPinM4), intPinToMode4, FALLING);

		tft.setRotation(1);                  // rotate the screen so that it looks better
		randomSeed(analogRead(0));           // seed the random number function
		int padding = tft.textWidth("9", 2); // get the width of the text in pixels
		tft.setTextPadding(padding);         // fixes full screen redaws
		tft.setTextFont(2);                  // makes the font larger
}


	

#elif defined(WIN32)
	//dummy function so that I can have a cleaner main
	void setupArduino(){}
#endif

/* A function which is used to do allocation for global vars */
void setupGame()
{
	// allocate memory for the levels to be put into 
	levels = getMalloc(sizeof(struct level) * numLevels);

	//random number generation seeded for windows
  #if defined(WIN32)
		time_t random_time_generator;
		srand((unsigned) time(&random_time_generator));
  #endif

}

/* A function that handles a part of memory managment */
void cleanupGame()
{
	/* deallocate allocated memory */
	free(errorString);
	free(gameMap);
	free(levels);
	free(items);

}
