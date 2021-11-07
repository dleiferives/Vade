/* <-- library definitions --> */
#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	#include <LiquidCrystal.h>
	
	LiquidCrystal lcd(2,3,4,5,11,12);//12,11,5,4,3,2);
#elif defined(WIN32)
	#include <stdio.h>
  #include <time.h>
#endif
/* <-- compiler definitions --> */
#define PLAYER_INVENTORY_SIZE 20
#define MOB_INV_BASE 1
#define GEN_WIDTH  32
#define GEN_HEIGHT 32
#define GEN_MOBS   16

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
	char * map;
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
int lcd_width  = 16;
int lcd_height = 2;
/* items.h */
struct item * items;

int num_global_items = 100;
int cur_global_items = 1;

char * error_string;
int input_mode =1; /* from input.h */

char * game_map;
char * entities_map;
char * render_map;


int num_levels = 20;
int cur_level  = 0;
struct level * levels;

#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	/* input.h */
	struct uint12By2 analogIn;
	int pA2;
	int pA4;
	int pA14;
	int pA8;
	int pA15;
	int p52;
	int arduino_toggle =1;
	const int interruptPinM1 = 18;
	const int interruptPinM2 = 19;


#elif defined(WIN32)
	/* input.h */
	char p;
	/* util.h */
	time_t rand_t;
#endif

/* <-- global functions --> */

void * get_malloc(unsigned int size)
{
	void *tmp = malloc(size);
	if(tmp == NULL)
	{
		error_string = "malloc ret NULL ptr";
		loop();
	}
	return tmp;
}

template <typename T>
T abs(t val)
{
	if(val < 0) val *= -1;
	return val;
}

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	void callInteruptM2()
	{
		input_mode = 2;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM2) == LOW){}
	}

	void callInteruptM1()
	{
		input_mode = 1;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM1) == LOW){}
	}
#elif defined(WIN32)

long rand_num(int min, int max)
{

}
#endif



