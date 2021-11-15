
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
	char result = levels[cur_level].map[get_level_p(&levels[cur_level],p1.x+p2.x, p1.y+p2.y)];
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
/* 1 - the room/ world where the game takes place is defined and stored as a map "game_map" */
/* 2 - the entities (players, mobs) are recorded to their own map "entities_map" */
/* 3 - the "game_map" and the "entities_map" are both stored into the "render_map" */ 
/* render map is put to the screen, for win32 this is done with the printf, arduino its lcd.print but relative to the screen size */

void init_maps(void)
{
	for(int y =0; y <levels[cur_level].size.y; y++)
	{
		for(int x =0; x <levels[cur_level].size.x; x++)
		{
			int m_pos = (y * levels[cur_level].size.x) + x;
			game_map[m_pos] = levels[cur_level].map[m_pos];
			entities_map[m_pos] = 0;
			render_map[m_pos] = (entities_map[m_pos] == 0) ? game_map[m_pos] : entities_map[m_pos];
		}
	}
}

void get_render_map(void)
{
	int pos =0;
	char output =0;
	for(int y =0; y <levels[cur_level].size.y; y++)
	{
		for(int x =0; x <levels[cur_level].size.x; x++)
		{
			pos = (y * levels[cur_level].size.x) + x;
			output = (entities_map[pos] == 0) ? game_map[pos] : entities_map[pos];
			render_map[y * levels[cur_level].size.x + x] = output;
		}
	}
}

void reset_entity_map(void)
{
	for(int i =0; i < (levels[cur_level].size.x * levels[cur_level].size.y); i++)
	{
		entities_map[i] = 0;
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
void render_character(struct character * player)
{
	/* printf("%i %i",player->pos_screen.x, player->pos_screen.y); */
	entities_map[(player -> pos_screen.y * levels[cur_level].size.x) + player -> pos_screen.x] = player -> tile;
}

void alloc_maps_for_level(struct pos size)
{
	/* printf("%i %i %i %i\n",levels[cur_level].size.x, size.x, levels[cur_level].size.y,size.y); */
	free(game_map);
	int mem_size = sizeof(char) * size.x * size.y;
	game_map     = get_malloc(mem_size);
	entities_map = get_malloc(mem_size);
	render_map   = get_malloc(mem_size);
}








#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
void print_map(struct pos p)
{
	lcd.clear();
	lcd.setCursor(0, 0);
	for(int i =0; i < lcd_width; i++)
	{
		lcd.print(render_map[i+(p.y*lcd_width)]);
	}
	lcd.setCursor(0, 1);
	for(int i =0; i < lcd_width; i++)
	{
		lcd.print(render_map[i+lcd_width+(p.y*lcd_width)]);
	}
	delay(200);
}

#elif defined(WIN32)
void print_aud(struct pos p)
{
	int l_x = levels[cur_level].size.x;
	int l_y = levels[cur_level].size.y;
	int width  = (AUD_WIDTH > l_x)  ? l_x : AUD_WIDTH; // width of level on screen
	int height = (AUD_HEIGHT > l_y) ? l_y : AUD_HEIGHT; // height of level on screen
	struct pos offsets = {AUD_WIDTH - width, AUD_HEIGHT - height};
	struct pos final_pos;
	final_pos.x = ((p.x+width)>l_x) ? l_x-width : p.x;
	final_pos.y = ((p.y+height)>l_y) ? l_y-height : p.y;
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
			putchar(get_char(&levels[cur_level],final_pos.x+x,final_pos.y+y));	
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

	for(int y =0; y <levels[cur_level].size.y; y++)
	{
		for(int x =0; x <levels[cur_level].size.x; x++)
		{
			putchar(render_map[(y*levels[cur_level].size.x) +x]);
			putchar(' ');
		}
		putchar(10);
	}
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
	



