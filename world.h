struct level gen_level(int id, int diff, int width, int height)
{
	struct level l;
	l.id       = id;
	l.diff     = diff;
	l.size.x   = width;
	l.size.y   = height;
	l.lcd      = init_pos;
	l.lcd_old  = init_pos;
	return l;
}


int get_level_p(struct level * l, int x, int y)
{
	return (y*l[0].size.x) + x;
}

char get_char(struct level * l, int x, int y)
{
	if(x>=l[0].size.x) return 0;
	if(y>=l[0].size.y) return 0; 
	if(x<0)				return 0; 
	if(y<0)				return 0; 
	return game_map[get_level_p(l,x,y)];
}

int char_dist(struct level * l, int pos, int pos2)
{
	int resultx1 = (pos % l[0].size.x);
	int resulty1 = pos / l[0].size.x;
	int resultx2 = (pos2 % l[0].size.x);
	int resulty2 = pos2 / l[0].size.x;

	int dist = sqrt(((resultx2-resultx1) * (resultx2-resultx1)) + ((resulty2-resulty1) * (resulty2-resulty1)));
	return dist;
}

struct pos get_r_char(char c)
{
	// randomize level generation
	seed_rand();
	struct pos result = {0,0};
	// find the amount of the character
	int num =0;
	for(int i =0; i <levels[cur_level].size.x * levels[cur_level].size.y; i++)
	{
		if(game_map[i] == c) num++;
	}
	int num2 = get_rand(num,1);
	//get the pid of the random character
	num =0;
	for(int i =0; i <levels[cur_level].size.x * levels[cur_level].size.y; i++)
	{
		if(game_map[i] == c) num++;
		if(num == num2)
		{
			num2 =i;
			break;
		}
	}
	if(num2 ==0 ) return result;
	// get pos of pid
	result.x = num2 % levels[cur_level].size.x; 
	result.y = num2 / levels[cur_level].size.x; 

	return result;
}

int flood_char(struct level * l,char c, struct pos p)
{
	//slower code so not to use up memory, since the stack will fill up with a recursive function of this size... we will use something far slower! iteraton
	//check if the character is illegal!
	game_map[get_level_p(l,p.x,p.y)] = c;
	int reset =0;
	for(int i =0; i<(l->size.x * l->size.y); i++)
	{
		reset = 0;
		if(game_map[i] == 0) 
			continue;
		if(game_map[i] == 1)
			continue;
		//check the char below
		if((i + l->size.x) < (l->size.x * l->size.y))
		{
			if((game_map[i + (l->size.x)] == 1) && (game_map[i] == 2))
			{
				game_map[i + (l->size.x)] = 2;
				reset =1;
			}
		}
		// check the char above
		if((i - l->size.x) > (0))
		{
			if((game_map[i - (l->size.x)] == 1) && (game_map[i] == 2))
			{
				game_map[i - (l->size.x)] = 2;
				reset =1;
			}
		}

		if(((i + 1) < (l->size.x * l->size.y)) && ( ((i+1)/l->size.x) == (i/l->size.x)))
		{
			if((game_map[i + 1] == 1) && (game_map[i] == 2))
			{
				game_map[i + 1] = 2;
				reset =1;
			}
		}

		if(((i - 1) < (l->size.x * l->size.y)) && ( ((i-1)/l->size.x) == (i/l->size.x)))
		{
			if((game_map[i - 1] == 1) && (game_map[i] == 2))
			{
				game_map[i - 1] = 2;
				reset =1;
			}
		}
		if(reset ==1)
			i =-1;
	}
	/* print_map(p); */
	return 0;
}

/* return values */
/* 0 = error */
/* 1 = success no other rooms hit */
/* 2 = other room hit, redrawing */
int generate_room(struct level * l, unsigned int room_x, unsigned int room_y, unsigned int width, unsigned int height, char c)
{
	if((room_x + width) > l[0].size.x) return 0;
	if((room_y + height) > l[0].size.y) return 0;
	for(int y = 0; y<height; y++)
	{
		for(int x =0; x<width; x++)
		{
			game_map[get_level_p(&levels[cur_level],room_x+x,room_y+y)] = c;
		}
	}
	return 1;
}

int gen_rand_room(struct level * l)
{
	//seed the random num generation
	seed_rand();
	// generate based off of random width height x and y then build the room
	int w = get_rand(l[0].size.x/10 +2 , 2);
	int h = get_rand(l[0].size.y/8+2 , 2);
	int x = get_rand(l[0].size.x, 0);
	int y = get_rand(l[0].size.x, 0);
	int result = generate_room(l,x,y,w,h,1);
	if(result == 0) gen_rand_room(l);
	return result;
}


int char_iterator_old =0;
int char_iterator_old_old =1;

int generate_path(struct level * l)
{
	//seed random number generation
	seed_rand();
	struct pos p1 = get_r_char(1);
	//set the connecting chars to 2
	flood_char(l,2,p1);

	// get position of the fellows on the other sides of each other
	p1 = get_r_char(2);
	struct pos p2 = get_r_char(1);
	//determine if everything is connected together
	if((p2.x == 0) && (p2.y == 0))
		return 1;
	if((p2.x == 1) && (p2.y == 0))
		return 1;
	
	// generate the paths between them
	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
  int x_sign = (dx > 0) ? 1 : -1;
  int y_sign = (dy > 0) ? 1 : -1;
	char c1 = 1;
	char c2 = 2;

	
	int cursor_x = p1.x;
	int cursor_y = p1.y;
	for(int y =0; y < (dy * y_sign); y++)
	{
		char tmp_char = game_map[get_level_p(l,cursor_x,cursor_y)];
			if((tmp_char != c1) && (tmp_char !=c2))
			{
				game_map[get_level_p(l,cursor_x,cursor_y)] = 3; 
			}
		cursor_y += y_sign;
	}

	for(int x =0; x < (dx * x_sign); x++)
	{
			char tmp_char = game_map[get_level_p(l,cursor_x,cursor_y)];
			if((tmp_char != c1) && (tmp_char !=c2))
			{
				game_map[get_level_p(l,cursor_x,cursor_y)] = 3; 
			}
		cursor_x += x_sign;
	}

	return 0;
}


int normalize_level(struct level * l)
{
	for(int i=0; i < (l->size.x * l->size.y);i++)
	{
		if(game_map[i] != 0)
		{
			game_map[i] = 1;
		}
	}
	return 0;
}

int generate_paths(struct level *l)
{
	while(generate_path(l) == 0)
	{
		normalize_level(l);
	}
	normalize_level(l);
}

/* returns the position of the player in the new level */
struct pos generate_level_structure(int id, int diff)
{
	reset_map();
	struct pos result;
	//dificulty 0 - 1000
	//width 30 - 300
	//height 20 - 200
	int diff_width = pow(2.7, ((float)diff/(float)2) ) + 29;
	int diff_height = pow(2.7, ((float)diff/(float)2) ) + 10;
	/* int diff_width = 10; */ 
	/* int diff_height = 10; */ 
	int diff_mobs = ((double) diff_width * (double) diff_height) * ((double)1.0- ((double) (((double)10.0)/((double) diff + (double) 10.0))));
	int diff_rooms = pow(2.5, ((float)diff/(float)2) ) + 8;

	if(diff_width > map_size_x)
		diff_width = map_size_x;
	if(diff_height > map_size_y)
		diff_height = map_size_y;
	if(diff_mobs > max_mobs)
		diff_mobs = max_mobs;
	num_mobs = diff_mobs;

	levels[id] = gen_level(id,diff, diff_width, diff_height);

	for(int i=0; i < diff_width * diff_height; i++) {
		game_map[i] = 0;
	}

	for(int j=0; j< diff_rooms;j++)
	{
			gen_rand_room(&levels[id]);	
	}

	generate_paths(&levels[id]);
	for(int i =0; i<(diff_width * diff_height); i++)
	{
		if(game_map[i] == 0)
			game_map[i] = '.';
		if(game_map[i] == 1)
			game_map[i] = 'A';
	}

	struct level * l = &levels[id];
	for(int y =0; y < diff_height; y++)
	{
		for(int x =0; x < diff_width; x++)
		{
			if((get_char(l,x,y) == '.'))
			{
				if((get_char(l,x+1,y) > '.') || (get_char(l,x-1,y) > '.') || (get_char(l,x,y+1) > '.') || (get_char(l,x,y-1) > '.'))
				{
					game_map[get_level_p(l,x,y)] = '#';
				}
			}
		}
	}

	for(int i=0; i<(diff_width*diff_height); i++)
	{
		if(game_map[i] == '.')
			game_map[i] = ' ';
		if(game_map[i] == 'A')
			game_map[i] = '.';
	}

	struct pos exit = get_r_char('.');
	game_map[get_level_p(&levels[id],exit.x,exit.y)] = '+';


	// put mobs onto the level
	for(int i =0; i < diff_mobs; i++)
	{
		mobs[i].id =i+1;
		mobs[i].pos_screen = get_r_char('.');
		Serial.print("MOBS --");
		Serial.print("m");
		Serial.print(mobs[i].id);
		Serial.print(" ");
		Serial.print(mobs[i].pos_screen.x);
		Serial.print(",");
		Serial.print(mobs[i].pos_screen.y);
		Serial.print("\n");
	}


}

void next_level(struct character * player)
{
	/* destroy_level(&levels[cur_level]); */
	cur_level +=1;		
	generate_level_structure(cur_level,cur_level);
	player->pos_screen = get_r_char('.');
	levels[cur_level].lcd = player->pos_screen;
	levels[cur_level].lcd.x = 0;
	levels[cur_level].lcd.y = 0;
#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	Serial.print("\nMap size");
	Serial.print(levels[cur_level].size.x);
	Serial.print(",");
	Serial.print(levels[cur_level].size.y);
	Serial.print("\n");
#endif
	print_map(levels[cur_level].lcd);

}
