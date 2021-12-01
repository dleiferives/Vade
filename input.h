#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	
	int handleInput()
	{
		analogIn.x = 1023 - analogRead(arduinoAnalogX); 
    analogIn.y = analogRead(arduinoAnalogY); 
		IrReceiver.decode();
		return 0;
	}
	
	int getMode()
	{
		return inputMode;	
	}

	int getDir()
	{
		int result =4;
		if(analogIn.y > 800) result =1; 
		if(analogIn.y < 200) result =0;
		if(analogIn.x > 800) result =3;
		if(analogIn.x < 200) result =2;
		if(result != 4)
		{
			Serial.print(analogIn.x);
			Serial.print(" ");
			Serial.print(analogIn.y);
			Serial.print("\n");
		}
		return result;
	}
#elif defined(WIN32)
	char p;
	int handleInput()
	{
		p = getchar();
		if(p == '\n') handleInput();
		if(p == ' ') handleInput();
		return 0;
	}

	int getMode()
	{
		int result = inputMode;
		if(p == '4') inputMode = 4;
		if(p == '2') inputMode = 2;
		if(p == '3') inputMode = 3;
		if(p == '1') inputMode = 1;
		printf("inputMode = %i, p = %c \n",inputMode,p);
		return inputMode;
	}

	int getDir()
	{
		int result =4;
		if(p == 'w') result =2; 
		if(p == 's') result =3;
		if(p == 'a') result =1;
		if(p == 'd') result =0;
		return result;
	}

#endif
