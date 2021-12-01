
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)


	void setupArduino()
	{
    Serial.begin(9600);
		Serial.println("ARDUINO LOADED!");
		IrReceiver.begin(irPin, ENABLE_LED_FEEDBACK);

		//Setting up the pins to be used
		pinMode(interruptPinM1,INPUT_PULLUP);
		pinMode(interruptPinM2,INPUT_PULLUP);
		pinMode(interruptPinM3,INPUT_PULLUP);
		pinMode(interruptPinM4,INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(interruptPinM1), intPinToMode1, FALLING);
		attachInterrupt(digitalPinToInterrupt(interruptPinM2), intPinToMode2, FALLING);
		attachInterrupt(digitalPinToInterrupt(interruptPinM3), intPinToMode3, FALLING);
		attachInterrupt(digitalPinToInterrupt(interruptPinM4), intPinToMode4, FALLING);

		/* tft.begin(); */
		tft.setRotation(1);
		randomSeed(analogRead(0));
		int padding = tft.textWidth("9", 2); // get the width of the text in pixels
		tft.setTextPadding(padding);
		tft.setTextFont(2);
}


	

#elif defined(WIN32)
	void setupArduino(){}
#endif

void setupGame()
{
	Serial.begin(9600);
#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	Serial.print("GAME SETUP STARTED\n");
#endif
	/* main.c || testing.ino */

	/* world.h */
	levels = getMalloc(sizeof(struct level) * numLevels);
	 generateLevelStructure(curLevel, 0);
#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	Serial.print("LEVEL GENERATED\n");
#endif

	/* items.h */
	/* items = getMalloc(sizeof(struct item) * numGlobalItems); */
	/* for(int i =0; i<numGlobalItems; i++) */
	/* { */
	/* 	items[i] = initItem; */
	/* } */

	/* render.h */

	/* util.h */
  #if defined(WIN32)
		time_t random_time_generator;
		srand((unsigned) time(&random_time_generator));
  #endif

}

void cleanupGame()
{
	/* main.c || testing.ino */
	free(errorString);
	/* render.h */
	free(gameMap);
	/* world.h */
	free(levels);
	/* items.h */
	free(items);

}
