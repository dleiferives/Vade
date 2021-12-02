/* <-- library definitions --> */
#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
 #include <TFT_HX8357.h> // Hardware-specific library for lcd
 #include <IRremote.h>   // for ir remote
 #include <avr/pgmspace.h>
 #include "vade_icon.h"
  // Invoke library
  TFT_HX8357 tft = TFT_HX8357();	

#elif defined(WIN32)
	/* Windows includes */
	#include <stdio.h>  // to print to console
	#include <stdlib.h> // for allocation and deallocation
	#include <math.h>   // for sqrt for dist
  #include <time.h>   // for rand
#endif

/* <-- compiler definitions --> */
#define PLAYER_INVENTORY_SIZE 20
#define MOB_INV_BASE 1
#define AUD_WIDTH  60 
#define AUD_HEIGHT 20 

/* <-- struct definitions --> */
struct pos
{
	int x;
	int y;
}const initPos = {0,0};


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
}const initStats = {0,0,0,0,0,0,0,0};

struct character 
{
	int id;
	struct stats baseStats;
	struct stats compStats;           // the stats computed by items
	int items[PLAYER_INVENTORY_SIZE]; //can hold up to 20 items
	struct pos posScreen;             // characte position on the screen
	struct pos posScreenOld;          // the characters previous position on the screen
	struct pos room;                  // the character position within the map
	char tile;                        // the tile for the player
}const initCharacter = {0,initStats, initStats, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, initPos,initPos,initPos,0};

struct mob
{
	int id;                  // mob id
	struct pos posScreen;    // mob position onthe screen
	struct pos posScreenOld; // mob previous position on the screen
	int nice;                // not implemented "nice mobs"
	int ranged;              // not implememnted ranged mobs
	char tile;               // the tile for the mob
}const initMob = {0, initPos,initPos,0,0,'0'};

// not used currenly, requires a refactor to be implemented
struct cursor
{
 struct pos p;
 char tile;
}const initCursor = {initPos, '+'};

struct level
{
	int id;           // the id of the level
	int diff;         // the dificulty of the level
	struct pos size;  // the size of the level
	struct pos lcd;   // the position on the screen
	struct pos lcdOld;// the old position on the screen
};

// not currenly used, but fully implemented
struct item
{
	struct stats _stats;   // the stats of the item
	char name[10];         // the name ofthe item
	int discriptionLength; // the description of the item
	char * discription;    // pointer to description string
}initItem = {{0,0,0,0,0,0,0,0},"Name\0",20,"Item Description\0"};

// 12 bit int for mem storage ... didnt really use ngl
struct uint12By2 
{ 
	unsigned  x: 12; 
	unsigned  y: 12; 
};

/* <-- gloabl variables --> */
// global item list
struct item * items;
int numGlobalItems = 100;
int curGlobalItems = 1;

// Error string printed on error string
char * errorString;

// the input mode, used to do diff things
int inputMode =1; /* from input.h */
// the default map char... not currenly useful
char roomChar = 'A'; // for map generation

// the number of levels that are allocated. 
int numLevels = 20;
int curLevel  = 0; // the level you currenly reside upon
struct level * levels; // global array of the levels

char gameMap[4400];   // personal heap. where the game map is stored
int mapSizeX = 60;    // max x width
int mapSizeY = 40;    // max y height
struct mob mobs[100]; // the number of mobs
int maxMobs = 100;    // int 
int numMobs = 0;      // the current number of mobs
struct pos mapTL;     // the top left of the map
int gameFont =1;      // the font used by the game

unsigned long gameTime =0;// global time
unsigned long oldGameTime=0;  // the previous global time

struct pos cursorPos;    // the position of the cursor on the screen 
struct pos cursorPosOld; // the old position of the cursor 
char cursorIcon = 'X';   // the icon of the cursor


#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	/* input.h */
	struct uint12By2 analogIn;     // joystick 
	int irPin = 7;                 // ir
	int arduinoAnalogX = A1;       // joystick pin1
	int arduinoAnalogY = A2;       // joystick pin2
	int arduinoToggle =1;          // is it goin tho
	const int interruptPinM1 = 19; // interupt pin 1
	const int interruptPinM2 = 18; // interupt pin 2
	const int interruptPinM3 = 2;  // interupt pin 3
	const int interruptPinM4 = 3;  // interupt pin 4




#elif defined(WIN32)
	/* input.h */
	char p;
	/* util.h */
	time_t randT;
	void loop()	{};
	
#endif
	int memAlloc =0;

/* <-- global functions --> */

/* A relatively safe malloc function, if there is a null pointer pulls
 * up the error string instead of continuing to run */
void * getMalloc(unsigned int size)
{
	//tmp void pointer to hold cur malloc
	void *tmp = malloc(size);
	memAlloc+=size;
	// check if pointer is null
	if(tmp == NULL)
	{
		// print error
    #if defined(WIN32)
		printf("Malloc ret NULL ptr : heap = %i",memAlloc);
	  #endif
		errorString = "malloc ret NULL ptr";
		loop();
	}  
	#if defined(WIN32)
	// print size of heap on windows
	printf("heap = %i",memAlloc);
	#endif
	// if its not null return
	return tmp;
}

/* gloabl definitions since use was before definition */
char getChar(struct level *l, int x, int y);
int getLevelP(struct level * l, int x, int y);
void renderEntities(struct mob * m, int numMobs);
void nextLevel(struct character * player);

// integers to pos 
struct pos toPos(int x, int y)
{
	struct pos result = {x,y};
	return result;
}

// the |diff of positions|
struct pos absPosDif(struct pos p1, struct pos p2)
{
	struct pos result = {p1.x-p2.y,p1.x-p2.y};
	result.x = (result.x > 0) ? result.x : -result.x;
	result.y = (result.y > 0) ? result.y : -result.y;
	return result;
}


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	// seed random num gen
	void seedRand()
	{
		randomSeed(analogRead(0));
	}

	// universal random number func
	int getRand(int max, int min)
	{
		return random(min,max);	
	}
	
	// interupt for mode 1 -- changes game to mode 1
	void intPinToMode1()
	{
		inputMode = 1;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM1) == LOW){}
	}

	// interupt for mode 2 -- changes game to mode 2
	void intPinToMode2()
	{
		inputMode = 2;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM2) == LOW){}
	}

	// interupt for mode 3 -- changes game to mode 3
	void intPinToMode3()
	{
		inputMode = 3;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM3) == LOW){}
	}

	// interupt for mode 4 -- changes game to mode 4
	void intPinToMode4()
	{
		inputMode = 4;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM4) == LOW){}
	}

	// render game startup icon
	void drawIcon(const unsigned short* icon, int16_t xf, int16_t yf, int8_t width, int8_t height, int8_t scaler)
	{
		for(int y =0 ; y<height; y++)
		{
			for(int x =0; x<width; x++)
			{
				for(int i=0; i<scaler; i++)
				{
					for(int j=0; j<scaler; j++)
					{
						tft.drawPixel(xf+(x*scaler)+i,yf+(y*scaler)+j,pgm_read_word(&icon[y * width + x]));
					}
				}
			}
		}
	}
#elif defined(WIN32)
	// universal randomseed func
	void seedRand()
	{
		time_t randTime;
		srand((unsigned) time(&randTime));
	}
// universal random function
 int getRand(int max, int min)
  {
    return (rand() % (max-min)) + min;
  }
#endif




