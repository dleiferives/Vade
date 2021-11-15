#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define DEBUG 0

char * level;
int * level_i;
int level_x = 20;
int level_y = 20;
int rooms = 8;
char room_char = 'A';
int room_val = 1;

void print_level()
{
	for(int i=0; i < level_y; i++)
	{
		for(int j =0; j < level_x; j++)
		{
			putchar(level[(i*level_x) + j]);
		}
		putchar(10);
	}
}

int get_p(int x, int y)
{
	return (y*level_x) + x;
}

int get_rand(int max, int min)
{
	return (rand() % (max-min)) + min;
}

char get_char(int x, int y)
{
	if(x>=level_x) return 0;
	if(y>=level_y) return 0; 
	if(x<0)				return 0; 
	if(y<0)				return 0; 
	return level[get_p(x,y)];
}
int get_var(int x, int y)
{
	if(x>=level_x) return 0;
	if(y>=level_y) return 0; 
	if(x<0)				return 0; 
	if(y<0)				return 0; 
	return level[get_p(x,y)];
}

int char_dist(int pos, int pos2)
{
	int resultx1 = (pos % level_x);
	int resulty1 = pos / level_x;
	int resultx2 = (pos2 % level_x);
	int resulty2 = pos2 / level_x;

	int dist = sqrt(((resultx2-resultx1) * (resultx2-resultx1)) + ((resulty2-resulty1) * (resulty2-resulty1)));
	return dist;
}

int flood_vars(int x, int y, int v)
{
	// is this character illegal?
	if(get_var(x,y) == v )
	{
		return 1;
	}
	if(get_var(x,y) == 0) return 1;
	// no? change it call on surrounding characters
	if(get_var(x,y) != v)
	{
		level[get_p(x,y)] = v;
		flood_vars(x+1,y,v);
		flood_vars(x,y+1,v);
		flood_vars(x-1,y,v);
		flood_vars(x,y-1,v);
	}
	return 0;
}


/* return values */
/* 0 = error */
/* 1 = success no other rooms hit */
/* 2 = other room hit, redrawing */
int generate_room(unsigned int room_x, unsigned int room_y, unsigned int width, unsigned int height, int v)
{
	if((room_x + width) > level_x) return 0;
	if((room_y + height) > level_y) return 0;
	for(int y =room_y; y < height+room_y; y++)
	{
		for(int x =room_x; x < width+room_x; x++)
		{
			int cur = level_i[get_p(x,y)];
			if(cur != 0)
			{
				if(v != 0)
				{
					if(DEBUG) print_level();
					/* clears the memory of this room and its overlaping room */
					generate_room(room_x,room_y,width,height,0);
					if(DEBUG) print_level();
					/* renders this room with the last room's character */
					generate_room(room_x,room_y,width,height,cur);
					if(DEBUG) print_level();
					return 2;
				}
				
			}

			level_i[get_p(x,y)] = v;
		}

	}
	return 1;
}

int gen_rand_room()
{
	int w = get_rand(level_x/10 +2 , 2);
	int h = get_rand(level_y/8+2 , 2);
	int x = get_rand(level_x, 0);
	int y = get_rand(level_x, 0);
	int result = generate_room(x,y,w,h,room_val);
	if(result == 1) room_val++;
	if(result == 0) gen_rand_room();
	return result;
}

int generate_paths(int in_v)
{
	/* cut path to closest thing that is not this value that is greater than 'a' */
	/* find number of vars */
	int var_iterator =0;
	for(int i =0; i < (level_x * level_y); i++)
	{
		if( in_v == level_i[i]) var_iterator++;
	}
	if(var_iterator == 0) return 0;
	int result = var_iterator;
	var_iterator = get_rand(var_iterator,0);
	int var_iterator_2 =0;
	int iterat_var_pos =0;
	for(int i =0; i < (level_x * level_y); i++)
	{
		if( in_v == level_i[i]) var_iterator_2++;
		if(var_iterator_2 == var_iterator)
		{
			iterat_var_pos = i;
			break;
		}
	}
	/* find distance to next closest value > 'a' */
	int low_dist =level_x * level_x;
	int low_dist_pos=0;
	for(int i =0; i < (level_x * level_y); i++)
	{
		if((level_i[i] != level_i[iterat_var_pos]) && (level_i[i] >= 0))
		{
			int temp = char_dist(iterat_var_pos, i);
			if(temp < low_dist)
			{
				low_dist = temp;
				low_dist_pos = i;
			}
		}
	}

	int dx = (iterat_var_pos % level_x) - (low_dist_pos % level_x);
	int dy = (iterat_var_pos / level_x) - (low_dist_pos / level_x);
  int x_sign = (dx > 0) ? 1 : -1;
  int y_sign = (dy > 0) ? 1 : -1;
	
	int cursor_x = low_dist_pos % level_x;
	int cursor_y = low_dist_pos / level_x;
	for(int y =0; y < (dy * y_sign); y++)
	{
		level_i[get_p(cursor_x,cursor_y)] = -1; 
		cursor_y += y_sign;
	}

	for(int x =0; x < (dx * x_sign); x++)
	{
		level_i[get_p(cursor_x,cursor_y)] = -1; 
		cursor_x += x_sign;
	}

	/* generate random number within that */
	return result;
}

int normalize_level(int vars)
{
	int flood_var = vars+1;
	for(int y =0; y < level_y; y++)
	{
		for(int x =0; x < level_x; x++)
		{
			if(level_i[get_p(x,y)] != 0 )
			{
				if(level_i[get_p(x,y)] < vars+1)
				{

				flood_vars(x,y,flood_var);
				flood_var++;

				}
			}
		}
	}

	int max =0;
	for(int i =0; i < (level_x * level_y); i++)
	{
		max = (level_i[i] > max) ? level_i[i] : max;
	}

	print_level();
	putchar(10);
	printf("%i",max);
	for(int i =max; i >= vars+1; i--)
	{
		generate_paths(i);
	}

	char flood_var2 = 10;
	for(int y =0; y < level_y; y++)
	{
		for(int x =0; x < level_x; x++)
		{
			if(level_i[get_p(x,y)] != 0 )
			{
				if((level_i[get_p(x,y)] > vars+1) || (level_i[get_p(x,y)] == -1))
				{

				flood_vars(x,y,flood_var2);
				flood_var2++;

				}
			}
		}
	}

	int max2 =0;
	for(int i =0; i < (level_x * level_y); i++)
	{
		max2 = (level_i[i] > max2) ? level_i[i] : max2;
	}

	if(max2 != 1) normalize_level(max2);

	return 0;
}

int main()
{
	srand(time(0));

	level = malloc(sizeof(char) * level_x*level_y);
	for(int i=0; i < (level_x * level_y); i++)
		level[i] = '.';
	level_i = malloc(sizeof(int) * level_x*level_y);
	for(int i=0; i < (level_x * level_y); i++)
		level_i[i] = 0;

	print_level();


	for(int i= 0; i< rooms; i++)
	{
		gen_rand_room();
		putchar(10);
		/* if(DEBUG)	print_level(); */
	}

	print_level();
	/* flood rooms */

	// flood the corners 
	normalize_level(rooms);
	for(int i=0; i<level_y*level_y; i++)
	{
		if(level_i[i] == 0) level[i] = '.';
		if(level_i[i] == 10) level[i] = 'A';
	}
	putchar(10);
	print_level();
	for(int y =0; y < level_y; y++)
	{
		for(int x =0; x < level_x; x++)
		{
			if((get_char(x,y) == '.'))
			{
				if((get_char(x+1,y) > '.') || (get_char(x-1,y) > '.') || (get_char(x,y+1) > '.') || (get_char(x,y-1) > '.'))
				{
					level[get_p(x,y)] = '#';
				}
			}
			/* if(((x == level_x-1) || (x == 0) || (y == level_y-1) || (y == 0))) */
			/* { */
			/* 	if(level[get_p(x,y)] != '.') level[get_p(x,y)] = '#'; */
			/* } */
		}
	}
	putchar(10);
	print_level();

	free(level);
	
}


