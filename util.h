/* <-- library definitions --> */
#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
 #include <TFT_HX8357.h> // Hardware-specific library
 #include <IRremote.h>
  // Invoke library
  TFT_HX8357 tft = TFT_HX8357();	
#elif defined(WIN32)
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
  #include <time.h>
#endif
/* <-- compiler definitions --> */
#define PLAYER_INVENTORY_SIZE 20
#define MOB_INV_BASE 1
#define AUD_WIDTH  60 
#define AUD_HEIGHT 40 

/* <-- struct definitions --> */
struct pos
{
	int x;
	int y;
}const init_pos = {0,0};


struct pos def_pos(int width, int height)
{
	struct pos result;
	result.x = width;
	result.y = height;
	return result;
}

struct stats
{
	int level;
	int xp;
	float health;
	float stamina;
	float inteligence;
	float charisma;
	float speed;
	float defense;
}const init_stats = {0,0,0,0,0,0,0,0};

struct character 
{
	int id;
	struct stats base_stats;
	struct stats comp_stats; // the stats computed by items
	int items[PLAYER_INVENTORY_SIZE]; //can hold up to 20 items
	struct pos pos_screen;            // characte position on the screen
	struct pos pos_screen_old;
	struct pos room;                  // the character position within the map
	char tile;
}const init_character = {0,init_stats, init_stats, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, init_pos,init_pos,init_pos,0};

struct mob
{
	int id;
	struct stats base_stats;
  struct stats comp_stats;
	struct pos pos_screen;
	int items[5];
	int nice;
	int ranged;
	char tile;
}const init_mob = {0,init_stats, init_stats, init_pos, {0,0,0,0,0},0,0,0};

struct cursor
{
 struct pos p;
 char tile;
}const init_cursor = {init_pos, '+'};

struct level
{
	int id;
	int diff;
	int num_mobs;
	struct mob * mobs;
	struct pos size;
	struct pos lcd;
};

struct item
{
	struct stats _stats;
	char name[10];
	int discription_length;
	char * discription;
}init_item = {{0,0,0,0,0,0,0,0},"Name\0",20,"Item Description\0"};

struct uint12By2 
{ 
	unsigned  x: 12; 
	unsigned  y: 12; 
};

/* <-- gloabl variables --> */
int lcd_width  = 60;
int lcd_height = 40;
/* items.h */
struct item * items;

int num_global_items = 100;
int cur_global_items = 1;

char * error_string;
int input_mode =1; /* from input.h */

char room_char = 'A'; // for map generation

int num_levels = 20;
int cur_level  = 0;
struct level * levels;

char * game_map;
int map_size_x = 80;
int map_size_y = 80;
struct pos map_tl;
int game_font =1;

unsigned long g_time =0;// global time
unsigned long old_g_time=0;  // the previous global time

struct pos cursor_pos;
char cursor_icon = '$';

#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	/* input.h */
	struct uint12By2 analogIn;
	int irPin = 7;
	long int ir_data;
	int arduino_analog_x = A1;
	int arduino_analog_y = A2;
	int arduino_toggle =1;
	const int interruptPinM1 = 19;
	const int interruptPinM2 = 18;
	const int interruptPinM3 = 2;
	const int interruptPinM4 = 3;




#elif defined(WIN32)
	/* input.h */
	char p;
	/* util.h */
	time_t rand_t;
	void loop()	{};
#endif

/* <-- global functions --> */

void * get_malloc(unsigned int size)
{
	void *tmp = malloc(size);
	if(tmp == NULL)
	{
    #if defined(WIN32)
		printf("Malloc ret NULL ptr");
	  #endif
		error_string = "malloc ret NULL ptr";
		loop();
	}
	return tmp;
}

char get_char(struct level *l, int x, int y);

int get_level_p(struct level * l, int x, int y);

void next_level(struct character * player);
struct pos to_pos(int x, int y)
{
	struct pos result = {x,y};
	return result;
}
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	void seed_rand()
	{
		randomSeed(analogRead(0));
	}
	int get_rand(int max, int min)
	{
		return random(min,max);	
	}
	
	void int_pin_to_mode_1()
	{
		input_mode = 1;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM1) == LOW){}
	}
	void int_pin_to_mode_2()
	{
		input_mode = 2;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM2) == LOW){}
	}
	void int_pin_to_mode_3()
	{
		input_mode = 3;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM3) == LOW){}
	}
	void int_pin_to_mode_4()
	{
		input_mode = 4;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM4) == LOW){}
	}
#elif defined(WIN32)
	void seed_rand()
{
	time_t rand_time;
	srand((unsigned) time(&rand_time));
}
 int get_rand(int max, int min)
  {
    return (rand() % (max-min)) + min;
  }
#endif




