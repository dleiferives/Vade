#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	
	int handle_input()
	{
		analogIn.x = 1023 - analogRead(arduino_analog_x); 
    analogIn.y = analogRead(arduino_analog_y); 
		IrReceiver.decode();
		return 0;
	}
	
	int get_mode()
	{
		return input_mode;	
	}

	int get_dir()
	{
		int result =4;
		if(analogIn.y > 800) result =1; 
		if(analogIn.y < 200) result =0;
		if(analogIn.x > 800) result =3;
		if(analogIn.x < 200) result =2;
    Serial.print(analogIn.x);
    Serial.print(" ");
    Serial.print(analogIn.y);
    Serial.print("\n");
		return result;
	}
#elif defined(WIN32)
	char p;
	int handle_input()
	{
		p = getchar();
		if(p == '\n') handle_input();
		if(p == ' ') handle_input();
		return 0;
	}

	int get_mode()
	{
		int result = input_mode;
		if(p == '4') input_mode = 4;
		if(p == '2') input_mode = 2;
		if(p == '3') input_mode = 3;
		if(p == '1') input_mode = 1;
		printf("input_mode = %i, p = %c \n",input_mode,p);
		return input_mode;
	}

	int get_dir()
	{
		int result =4;
		if(p == 'w') result =2; 
		if(p == 's') result =3;
		if(p == 'a') result =1;
		if(p == 'd') result =0;
		return result;
	}

#endif
