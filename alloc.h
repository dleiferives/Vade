
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)


	void setup_arduino()
	{
    Serial.begin(9600);
		Serial.println("ARDUINO LOADED!");
		IrReceiver.begin(irPin, ENABLE_LED_FEEDBACK);

		//Setting up the pins to be used
		pinMode(interruptPinM1,INPUT_PULLUP);
		pinMode(interruptPinM2,INPUT_PULLUP);
		pinMode(interruptPinM3,INPUT_PULLUP);
		pinMode(interruptPinM4,INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(interruptPinM1), int_pin_to_mode_1, FALLING);
		attachInterrupt(digitalPinToInterrupt(interruptPinM2), int_pin_to_mode_2, FALLING);
		attachInterrupt(digitalPinToInterrupt(interruptPinM3), int_pin_to_mode_3, FALLING);
		attachInterrupt(digitalPinToInterrupt(interruptPinM4), int_pin_to_mode_4, FALLING);

		tft.begin();
		tft.setRotation(1);
		randomSeed(analogRead(0));
}


	

#elif defined(WIN32)
	void setup_arduino(){}
#endif

void setup_game()
{
	/* main.c || testing.ino */
	error_string = (char *)malloc(sizeof(char) * 100);

	/* world.h */
	levels = (struct level*)malloc(sizeof(struct level) * num_levels);
	/* levels[cur_level] = gen_level(cur_level, 1, GEN_WIDTH, GEN_HEIGHT, GEN_MOBS); */ 
	 generate_level_structure(cur_level, 0);
	/* items.h */
	items = (struct item*)malloc(sizeof(struct item) * num_global_items);
	for(int i =0; i<num_global_items; i++)
	{
		items[i] = init_item;
	}

	/* render.h */
	game_map = (char *) malloc(sizeof(char) * levels[cur_level].size.x * levels[cur_level].size.y);
	entities_map = (char *) malloc(sizeof(char) * levels[cur_level].size.x * levels[cur_level].size.y);
	render_map = (char *) malloc(sizeof(char) * levels[cur_level].size.x * levels[cur_level].size.y);

	/* util.h */
  #if defined(WIN32)
		time_t random_time_generator;
		srand((unsigned) time(&random_time_generator));
  #endif

}

void cleanup_game()
{
	/* main.c || testing.ino */
	free(error_string);
	/* render.h */
	free(game_map);
	free(entities_map);
	free(render_map);
	/* world.h */
	for(int i =0; i <num_levels; i++)
		destroy_level(&levels[i]);
	free(levels);
	/* items.h */
	free(items);

}
