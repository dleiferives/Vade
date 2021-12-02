#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	
	// a function that handles all input
	int handleInput()
	{
		// joystick x and y
		analogIn.x = 1023 - analogRead(arduinoAnalogX); 
    analogIn.y = analogRead(arduinoAnalogY); 
		// Ir input x and y
		IrReceiver.decode();
		return 0;
	}
	
	// a function that returns the input mode
	int getMode()
	{
		// returns the input mode
		return inputMode;	
	}

	//  a function that returns the direction that the player pressed
	int getDir()
	{
		int result =4;
		// default is four
		if(analogIn.y > 800) result =1; 
		if(analogIn.y < 200) result =0;
		if(analogIn.x > 800) result =3;
		if(analogIn.x < 200) result =2;
		// if not four return that as dir
		if(result != 4)
		{
			Serial.print(analogIn.x);
			Serial.print(" ");
			Serial.print(analogIn.y);
			Serial.print("\n");
		}
		// return it
		return result;
	}
#elif defined(WIN32)
	// p isthe input value
	char p;
	// a function thathandles input
	int handleInput()
	{
		// get input into p
		p = getchar();
		if(p == '\n') handleInput();
		if(p == ' ') handleInput();
		return 0;
	}

	// a function that determines the game mode
	int getMode()
	{
		int result = inputMode;
		// if p is an imput mode, change the input mode 
		if(p == '4') inputMode = 4;
		if(p == '2') inputMode = 2;
		if(p == '3') inputMode = 3;
		if(p == '1') inputMode = 1;
		printf("inputMode = %i, p = %c \n",inputMode,p);
		// return input mode
		return inputMode;
	}

	// a function that gets direction that the player inputs
	int getDir()
	{
		int result =4;
		if(p == 'w') result =2; 
		if(p == 's') result =3;
		if(p == 'a') result =1;
		if(p == 'd') result =0;
		// if p is a dir return it, else dont
		return result;
	}

#endif
