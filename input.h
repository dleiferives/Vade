#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	
	int handle_input()
	{
		analogIn.x = analogRead(A0); 
    analogIn.y = analogRead(A1); 
    pA2 = analogRead(A2);
    pA4 = analogRead(A4);
    pA14 = analogRead(A14);
    pA8 = analogRead(A8);

		attachInterrupt(digitalPinToInterrupt(interruptPinM1), callInteruptM1, LOW);
		attachInterrupt(digitalPinToInterrupt(interruptPinM2), callInteruptM2, LOW);
		return 0;
	}
	
	int get_mode()
	{
		if((arduino_toggle ==1) && (pA4>900)) arduino_toggle = 2;
    if((arduino_toggle ==2) && (pA4<900))
		{
			arduino_toggle = 3;
			input_mode =2;
		}
    if((arduino_toggle ==3) && (pA4>900))
		{
			input_mode =1;
			arduino_toggle = 1;
		}
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
