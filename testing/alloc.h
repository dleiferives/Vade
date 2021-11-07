#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)


	void setup_arduino()
	{
    Serial.begin(9600);
		pinMode(A0, INPUT); 
		pinMode(A1, INPUT); 
		pinMode(A2, INPUT);
		pinMode(A4, INPUT);
		pinMode(A8, INPUT);
		pinMode(A9 , INPUT);
		pinMode(A14, INPUT);
		pinMode(A15, INPUT);
		pinMode(52, OUTPUT);

		lcd.begin(16,2);


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
	levels[cur_level] = gen_level(cur_level, 1, GEN_WIDTH, GEN_HEIGHT, GEN_MOBS); 
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
		srand((unsigned) time(&t));
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
