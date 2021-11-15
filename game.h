
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

void move_character(struct character * player,int x, int y)
{
	int t_step =1;
	if((player.pos_screen.x + 1) >= (levels[cur_level].size.x )) t_step=0;
	if((player.pos_screen.x - 1) < 0) t_step=0;
	if((player.pos_screen.y - 1) < 0) t_step=0;
	if((player.pos_screen.y + 1) >= (levels[cur_level].size.y )) t_step=0;
	if(t_step !=0)
	{
		if((levels[cur_level].
					map[get_level_p(x+player.pos_screen.x,
													y+player.pos_screen.y)]) 
				   == (' ' || '#'))
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
	entities_map[(player -> pos_screen.y * levels[cur_level].size.x) + player -> pos_screen.x] = player -> tile;
}

void alloc_maps_for_level(struct pos size)
{
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

	



