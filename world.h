struct level gen_level(int id, int diff, int width, int height, int mobs)
{
	struct level l;
	l.id       = id;
	l.diff     = diff;
	l.size.x   = width;
	l.size.y   = height;
	l.lcd      = init_pos;
	l.map      = (char *)malloc(sizeof(char) * l.size.x * l.size.y);
	l.num_mobs = mobs;

	for(int i  = 0; i< (l.size.x * l.size.y); i ++)
		l.map[i] = 0;
	l.mobs     = (struct mob*)malloc(sizeof(struct mob) * mobs);
	return l;
}

void destroy_level(struct level * l)
{
	free(l->mobs);
	free(l->map);
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
	return l->map[get_level_p(l,x,y)];
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

int flood_char(struct level * l, int x, int y, char c)
{
	// is this character illegal?
	if(get_char(l,x,y) == c )
	{
		return 1;
	}
	if(get_char(l,x,y) == '.') return 1;
	if(get_char(l,x,y) == 0) return 1;
	// no? change it call on surrounding characters
	if(get_char(l,x,y) != c)
	{
		l[0].map[get_level_p(l,x,y)] = c;
		flood_char(l,x+1,y,c);
		flood_char(l,x,y+1,c);
		flood_char(l,x-1,y,c);
		flood_char(l,x,y-1,c);
	}
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
	for(int y =room_y; y < height+room_y; y++)
	{
		for(int x =room_x; x < width+room_x; x++)
		{
			char cur = l[0].map[get_level_p(l,x,y)];
			if(cur != '.')
			{
				if(c != '.')
				{
					/* clears the memory of this room and its overlaping room */
					generate_room(l,room_x,room_y,width,height,'.');
					/* renders this room with the last room's character */
					generate_room(l,room_x,room_y,width,height,cur);
					return 2;
				}
			}
			l[0].map[get_level_p(l,x,y)] = c;
		}
	}
	return 1;
}

int gen_rand_room(struct level * l)
{
	int w = get_rand(l[0].size.x/10 +2 , 2);
	int h = get_rand(l[0].size.y/8+2 , 2);
	int x = get_rand(l[0].size.x, 0);
	int y = get_rand(l[0].size.x, 0);
	int result = generate_room(l,x,y,w,h,room_char);
	if(result == 1) room_char++;
	if(result == 0) gen_rand_room(l);
	return result;
}

int char_iterator_old =0;
int char_iterator_old_old =1;
int generate_paths(struct level * l, char in_c)
{
	seed_rand();
	/* cut path to closest thing that is not this value that is greater than 'a' */
	/* find number of character */
	int char_iterator =0;
	for(int i =0; i < (l[0].size.x * l[0].size.y); i++)
	{
		if( in_c == l[0].map[i]) char_iterator++;
	}
	if(char_iterator == 0) return 0;
	/* printf("a%i",char_iterator); */
	char_iterator = get_rand(char_iterator,0);
	int char_iterator_2 =0;
	int iterat_char_pos =0;
	for(int i =0; i < (l[0].size.x* l[0].size.y); i++)
	{
		if( in_c == l[0].map[i]) char_iterator_2++;
		if(char_iterator_2 == char_iterator)
		{
			iterat_char_pos = i;
			break;
		}
	}
	/* find distance to next closest value > 'a' */
	int low_dist =l[0].size.x * l[0].size.x;
	int low_dist_pos=0;
	for(int i =0; i < (l[0].size.x * l[0].size.y); i++)
	{
		if((((l[0].map[i] >= 'a') && (l[0].map[i] <= 'z')) || ((l[0].map[i]>='A') && (l[0].map[i] <='Z')) ) && (l[0].map[i] != l[0].map[iterat_char_pos]))
		{
			int temp = char_dist(l,iterat_char_pos, i);
			if(temp < low_dist)
			{
				low_dist = temp;
				low_dist_pos = i;
			}
		}
	}

	int dx = (iterat_char_pos % l[0].size.x) - (low_dist_pos % l[0].size.x);
	int dy = (iterat_char_pos / l[0].size.x) - (low_dist_pos / l[0].size.x);
  int x_sign = (dx > 0) ? 1 : -1;
  int y_sign = (dy > 0) ? 1 : -1;
	char c1 = l[0].map[iterat_char_pos];
	char c2 = l[0].map[low_dist_pos];

	
	int cursor_x = low_dist_pos % l[0].size.x;
	int cursor_y = low_dist_pos / l[0].size.x;
	for(int y =0; y < (dy * y_sign); y++)
	{
		char tmp_char = l[0].map[get_level_p(l,cursor_x,cursor_y)];
			if((tmp_char != c1) && (tmp_char !=c2))
			{
				l[0].map[get_level_p(l,cursor_x,cursor_y)] = '*'; 
			}
		cursor_y += y_sign;
	}

	for(int x =0; x < (dx * x_sign); x++)
	{
		char tmp_char = l[0].map[get_level_p(l,cursor_x,cursor_y)];
			if((tmp_char != c1) && (tmp_char !=c2))
			{
				l[0].map[get_level_p(l,cursor_x,cursor_y)] = '*'; 
			}
		cursor_x += x_sign;
	}
}


int normalize_level(struct level * l)
{
	puts("-1");
	char flood_var = 'a';
	puts("-2");
	for(int y =0; y < l[0].size.y; y++)
	{
		for(int x =0; x < l[0].size.x; x++)
		{
			if(l[0].map[get_level_p(l,x,y)] != '.' )
			{
				if(l[0].map[get_level_p(l,x,y)] < 'a')
				{

				flood_char(l,x,y,flood_var);
				flood_var++;

				}
			}
		}
	}

	puts("-2");
	char max =0;
	for(int i =0; i < (l[0].size.x * l[0].size.y); i++)
	{
		max = (l[0].map[i] > max) ? l[0].map[i] : max;
	}

	puts("-3");
	for(int i =max; i >= 'a'; i--)
	{
		generate_paths(l,i);
	}

	puts("-4");
	char flood_var2 = 'A';
	for(int y =0; y < l[0].size.y; y++)
	{
		for(int x =0; x < l[0].size.x; x++)
		{
			if(l[0].map[get_level_p(l,x,y)] != '.' )
			{
				if((l[0].map[get_level_p(l,x,y)] > 'a') || (l[0].map[get_level_p(l,x,y)] == '+'))
				{

				flood_char(l,x,y,flood_var2);
				flood_var2++;

				}
			}
		}
	}
	char max2 =0;
	puts("-5");
	for(int i =0; i < (l[0].size.x * l[0].size.y); i++)
	{
		max2 = (l[0].map[i] > max2) ? l[0].map[i] : max2;
	}

	puts("-8");
	if((max2 != 'A') && (max2 != 'a'))
	{
		for(int y =0; y< levels[cur_level].size.y; y++)
		{
			for(int x =0; x< levels[cur_level].size.x; x++)
			{
				putchar(levels[cur_level].map[get_level_p(&levels[cur_level],x,y)]);
			}
			putchar(10);
		}
		printf("%i\n",max2);
		normalize_level(l);
	}

	return 0;
}

/* returns the position of the player in the new level */
struct pos generate_level_structure(int id, int diff)
{
	struct pos result;
	//dificulty 0 - 1000
	//width 30 - 300
	//height 20 - 200
	puts("1");
	int diff_width = pow(2.7, ((float)diff/(float)2) ) + 19;
	int diff_height = pow(2.7, ((float)diff/(float)2) )+ 19;
	int diff_mobs = ((double) diff_width * (double) diff_height) * ((double)1.0- ((double) (((double)10.0)/((double) diff + (double) 10.0))));
	int diff_rooms = pow(2.5, ((float)diff/(float)2) ) + 8;
	levels[id] = gen_level(id,diff, diff_width, diff_height, diff_mobs);
	puts("2");
	for(int i=0; i < diff_width * diff_height; i++)
		levels[id].map[i] = '.';
	puts("3");
	seed_rand();

	for(int i=0;i<diff_rooms; i++)
	{
		gen_rand_room(&levels[id]);	
	}
	puts("4");
	normalize_level(&levels[id]);
	struct level * l = &levels[id];
	puts("5");
	for(int y =0; y < diff_height; y++)
	{
		for(int x =0; x < diff_width; x++)
		{
			if((get_char(l,x,y) == '.'))
			{
				if((get_char(l,x+1,y) > '.') || (get_char(l,x-1,y) > '.') || (get_char(l,x,y+1) > '.') || (get_char(l,x,y-1) > '.'))
				{
					levels[id].map[get_level_p(l,x,y)] = '#';
				}
			}
		}
	}
	int floor_tiles=0;
	for(int i=0; i<(diff_width*diff_height); i++)
	{
		if(levels[id].map[i] == '.') levels[id].map[i] = ' ';
		if(levels[id].map[i] == 'A')
		{
			levels[id].map[i] = '.';
			floor_tiles++;
		}
	}
	floor_tiles = get_rand(floor_tiles,0);
	int floor_tiles2 =0;
	for(int i=0; i<(diff_width*diff_height); i++)
	{
		if(levels[id].map[i] == '.')
		{
			floor_tiles2++;
		}
		if(floor_tiles2 == floor_tiles)
		{
			levels[id].map[i] = '+';
			break;
		}
	}
}

struct pos get_rand_pos_of_char(char c)
{
	struct pos result;
	int num =0;
	for(int i =0; i <levels[cur_level].size.x * levels[cur_level].size.y; i++)
	{
		if(levels[cur_level].map[i] == c) num++;
	}
	seed_rand();
	int num2 = get_rand(num,1);
	num =0;
	for(int i =0; i <levels[cur_level].size.x * levels[cur_level].size.y; i++)
	{
		if(levels[cur_level].map[i] == c) num++;
		if(num == num2)
		{
			num2 =i;
			break;
		}
	}
	if(num2 ==0 ) return result;
	result.x = num2 % levels[cur_level].size.x; 
	result.y = num2 / levels[cur_level].size.x; 

	return result;
}

void next_level(struct character * player)
{
	/* destroy_level(&levels[cur_level]); */
	cur_level +=1;		
	puts("1");
	generate_level_structure(cur_level,cur_level);
	puts("2");
	player->pos_screen.x = 1;
	player->pos_screen.y = 1;
	puts("3");
	alloc_maps_for_level(levels[cur_level].size);
	puts("4");
	init_maps();
	puts("5");
	get_render_map();
		print_map(levels[cur_level].lcd);
}
