
/*
 *
 *
 *
 *
 *
 *
 *
 *
 * ---------------- ENTITIES ------------------------ 
 *
 *
 *
 *
 *
 *
 */

/* a function that takes the player and the id of the item that is desired to be added, if it cannot be added returns NULL */
int add_item(struct character * player, int item_id)
{
	int iterator =0;
	while(iterator < PLAYER_INVENTORY_SIZE)
	{
		if(player[0].items[iterator] == 0)
		{
			player[0].items[iterator] = item_id;
			break;
		}
		iterator++;
		if(iterator == PLAYER_INVENTORY_SIZE)
		{
			// inventory is full
			return 0;
		}
	}
	return 1;
}

char rel_char(struct pos p1, struct pos p2)
{
	char result = game_map[get_level_p(&levels[cur_level],p1.x+p2.x, p1.y+p2.y)];
	putchar(result);
	return result;
}

void move_character(struct character * player,int x, int y)
{
	int t_step =1;
	if((player->pos_screen.x + x) >= (levels[cur_level].size.x )) t_step=0;
	if((player->pos_screen.x + x) < 0) t_step=0;
	if((player->pos_screen.y + y) < 0) t_step=0;
	if((player->pos_screen.y + y) >= (levels[cur_level].size.y )) t_step=0;
	if(t_step !=0)
	{
		if(rel_char(player->pos_screen,to_pos(x,y)) == '#')
		{ 
			t_step = 0;
		}
		if(t_step == 1)
		{
			player -> pos_screen_old = player[0].pos_screen;
			player[0].pos_screen.x += x;
			player[0].pos_screen.y += y;
			g_time++;
		}
	}
}

void move_mob(struct mob * m, int x, int y)
{
	m -> pos_screen.x +=x;
	m -> pos_screen.y +=y;
}

int character_action(struct character * player, struct pos p)
{
	char loc = rel_char(player->pos_screen, p);
	int s_time =1;
	switch(loc)
	{
		case '+':
			if((p.x == 0) && (p.y == 0))
			{
				s_time =0;
				next_level(player);
			}
			break;
	}
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 * ----------------- RENDER --------------- 
 *
 *
 *
 *
 *
 *
 *
 */
/* how the rendering ruitine works ... */
/* 1 - the room/ world where the game takes place is defined and stored as a map level map */
/* 2 -  On game rendering entities are put ontop of level map */
/* render map is put to the screen, for win32 this is done with the printf, arduino its lcd.print but relative to the screen size */
void reset_map(void)
{
	for(int i =0; i < (map_size_x * map_size_y); i++)
	{
		game_map[i] = 0;
	}
}



int render_ascii(char val, struct pos p)
{
	if((p.x >=0) && (p.x < levels[cur_level].size.x) && (p.y >=0) && (p.y < levels[cur_level].size.y))
	{
	  game_map[(p.y * levels[cur_level].size.x) + p.x] = val;
		return 0;
	}
	if((p.x <0) || (p.x <= levels[cur_level].size.x) || (p.y < 0) || (p.y <=levels[cur_level].size.y)) return 1;
	return 1;
}


// SPEED -- could be made faster by passing the pos by value







#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
void render_character(struct character * player)
{
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
	tft.setCursor(map_tl.x + (player->pos_screen.x*8),map_tl.y + (player->pos_screen.y*8));
	tft.print(player->tile);
}

void render_entities(struct mob * m, int num_mobs)
{
	for(int i=0; i<num_mobs; i++)
	{
		tft.setTextColor(TFT_RED, TFT_BLACK);
		tft.setCursor(map_tl.x + (m[i].pos_screen.x*8),map_tl.y + (m[i].pos_screen.y*8));
		tft.print(m[i].tile);
	}
}

void print_map(struct pos p)
{
	tft.fillScreen(TFT_BLACK);
	unsigned long start = micros();
	tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE); 
	tft.setTextSize(1);
  tft.println("Hello World!");
	int l_x = levels[cur_level].size.x;
	int l_y = levels[cur_level].size.y;
	int width  = (AUD_WIDTH > l_x)  ? l_x : AUD_WIDTH; // width of level on screen
	int height = (AUD_HEIGHT > l_y) ? l_y : AUD_HEIGHT; // height of level on screen
	struct pos offsets = {AUD_WIDTH - width, AUD_HEIGHT - height};
	struct pos final_pos;
	final_pos.x = ((p.x+width)>l_x) ? l_x-width : p.x;
	final_pos.x = (final_pos.x < 0) ? 0 : final_pos.x;
	final_pos.y = ((p.y+height)>l_y) ? l_y-height : p.y;
	final_pos.y = (final_pos.y < 0) ? 0 : final_pos.y;
	int cursor_print_x =0;
	int cursor_print_y = offsets.y/2;
	tft.setCursor(cursor_print_x*8,cursor_print_y*8);
	map_tl.x = (offsets.x/2) * 8;
	map_tl.y = (offsets.y/2) * 8;
	tft.setTextColor(TFT_WHITE,TFT_BLACK);
	for(int y=0; y<height; y++)
	{
			cursor_print_x = offsets.x/2;
			tft.setCursor(cursor_print_x*8,cursor_print_y*8);
		for(int x=0; x<width; x++)
		{
			tft.print((char) game_map[(final_pos.x+x)+((final_pos.y+y) * l_x)]);	
		}
			cursor_print_y++;
	}
  return micros() - start;
}

#elif defined(WIN32)
void render_character(struct character * player)
{
	printf("player_pos = %i, %i\n",player->pos_screen.x, player->pos_screen.y);
}

void render_entities(struct mob * m, int num_mobs)
{
	for(int i=0; i<num_mobs; i++)
	{
		printf("mob_%i_pos = %i, %i\n",i,player->pos_screen.x, player->pos_screen.y);
	}
}

void print_aud(struct pos p)
{
	int l_x = levels[cur_level].size.x;
	int l_y = levels[cur_level].size.y;
	int width  = (AUD_WIDTH > l_x)  ? l_x : AUD_WIDTH; // width of level on screen
	int height = (AUD_HEIGHT > l_y) ? l_y : AUD_HEIGHT; // height of level on screen
	struct pos offsets = {AUD_WIDTH - width, AUD_HEIGHT - height};
	struct pos final_pos;
	final_pos.x = ((p.x+width)>l_x) ? l_x-width : p.x;
	final_pos.x = (final_pos.x < 0) ? 0 : final_pos.x;
	final_pos.y = ((p.y+height)>l_y) ? l_y-height : p.y;
	final_pos.y = (final_pos.y < 0) ? 0 : final_pos.y;
	printf("Width %i Height %i\n",width,height);
	printf("OWidth %i OHeight %i\n",offsets.x,offsets.y);
	for(int i=0; i<AUD_WIDTH+2; i++) putchar('-');
	putchar(10);
		for(int oy =0; oy < offsets.y/1; oy++) putchar(10);
	for(int y=0; y<height; y++)
	{
		putchar('|');
			for(int ox =0; ox < offsets.x/2; ox++) putchar(' ');
		for(int x=0; x<width; x++)
		{
			putchar(render_map[(final_pos.x+x)+((final_pos.y+y) * l_x)]);	
		}
			for(int ox =0; ox < offsets.x/2; ox++) putchar(' ');
				putchar('|');
		putchar(10);
	}
		for(int oy =0; oy < offsets.y/1; oy++) putchar(10);
	for(int i=0; i<AUD_WIDTH+2; i++) putchar('-');
}

void print_map(struct pos p)
{

	/* for(int y =0; y <levels[cur_level].size.y; y++) */
	/* { */
	/* 	for(int x =0; x <levels[cur_level].size.x; x++) */
	/* 	{ */
	/* 		putchar(render_map[(y*levels[cur_level].size.x) +x]); */
	/* 		putchar(' '); */
	/* 	} */
	/* 	putchar(10); */
	/* } */
	print_aud(p);
	printf("Mode = %i\n",input_mode);

}

#endif

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *------------------ ITEMS ------------------
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

int add_to_item_list(struct item * item_adding)
{
	if(cur_global_items < num_global_items)
	{
		items[cur_global_items] = item_adding[0];
		cur_global_items++;
		return 0;
	}
	else
	{
		num_global_items += 100;
		items = (struct item*)realloc(items, num_global_items + 100);
		if(items != NULL) puts("ERROR ALLOCATING MORE ITEMS");
		items[cur_global_items] = item_adding[0];
		cur_global_items++;
		return 0;
	}
}

struct item item_by_id(int id)
{
	if(id >= num_global_items)
	{
		puts("ERROR IN ITEMS, TRYING TO GET A NON ALLOCATED ITEM");	
		return init_item;
	}
	return items[id];
}


/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * --------- COMBAT --------
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
	



