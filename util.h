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
	struct stats compStats; // the stats computed by items
	int items[PLAYER_INVENTORY_SIZE]; //can hold up to 20 items
	struct pos posScreen;            // characte position on the screen
	struct pos posScreenOld;
	struct pos room;                  // the character position within the map
	char tile;
}const initCharacter = {0,initStats, initStats, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, initPos,initPos,initPos,0};

struct mob
{
	int id;
	/* struct stats baseStats; */
  /* struct stats compStats; */
	struct pos posScreen;
	struct pos posScreenOld;
	int nice;
	int ranged;
	char tile;
}const initMob = {0, initPos,initPos,0,0,'0'};

struct cursor
{
 struct pos p;
 char tile;
}const initCursor = {initPos, '+'};

struct level
{
	int id;
	int diff;
	struct pos size;
	struct pos lcd;
	struct pos lcdOld;
};

struct item
{
	struct stats _stats;
	char name[10];
	int discriptionLength;
	char * discription;
}initItem = {{0,0,0,0,0,0,0,0},"Name\0",20,"Item Description\0"};

struct uint12By2 
{ 
	unsigned  x: 12; 
	unsigned  y: 12; 
};

/* <-- gloabl variables --> */
/* items.h */
struct item * items;

int numGlobalItems = 100;
int curGlobalItems = 1;

char * errorString;
int inputMode =1; /* from input.h */

char roomChar = 'A'; // for map generation

int numLevels = 20;
int curLevel  = 0;
struct level * levels;

char gameMap[2400];
int mapSizeX = 60;
int mapSizeY = 40;
struct mob mobs[100];
int maxMobs = 100;
int numMobs = 0;
struct pos mapTL;
int gameFont =1;

unsigned long gameTime =0;// global time
unsigned long oldGameTime=0;  // the previous global time

struct pos cursorPos;
struct pos cursorPosOld;
char cursorIcon = '$';


#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	/* input.h */
	struct uint12By2 analogIn;
	int irPin = 7;
	long int irData;
	int arduinoAnalogX = A1;
	int arduinoAnalogY = A2;
	int arduinoToggle =1;
	const int interruptPinM1 = 19;
	const int interruptPinM2 = 18;
	const int interruptPinM3 = 2;
	const int interruptPinM4 = 3;




#elif defined(WIN32)
	/* input.h */
	char p;
	/* util.h */
	time_t randT;
	void loop()	{};
	
#endif
	int memAlloc =0;

/* <-- global functions --> */

void * getMalloc(unsigned int size)
{
	void *tmp = malloc(size);
	memAlloc+=size;
	if(tmp == NULL)
	{
    #if defined(WIN32)
		printf("Malloc ret NULL ptr : heap = %i",memAlloc);
	  #endif
		errorString = "malloc ret NULL ptr";
		loop();
	}  
	#if defined(WIN32)
	printf("heap = %i",memAlloc);
	#endif
	return tmp;
}

char getChar(struct level *l, int x, int y);

int getLevelP(struct level * l, int x, int y);

void renderEntities(struct mob * m, int numMobs);

void nextLevel(struct character * player);
struct pos toPos(int x, int y)
{
	struct pos result = {x,y};
	return result;
}

struct pos absPosDif(struct pos p1, struct pos p2)
{
	struct pos result = {p1.x-p2.y,p1.x-p2.y};
	result.x = (result.x > 0) ? result.x : -result.x;
	result.y = (result.y > 0) ? result.y : -result.y;
	return result;
}

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	void seedRand()
	{
		randomSeed(analogRead(0));
	}

	int getRand(int max, int min)
	{
		return random(min,max);	
	}
	
	void intPinToMode1()
	{
		inputMode = 1;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM1) == LOW){}
	}

	void intPinToMode2()
	{
		inputMode = 2;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM2) == LOW){}
	}

	void intPinToMode3()
	{
		inputMode = 3;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM3) == LOW){}
	}

	void intPinToMode4()
	{
		inputMode = 4;	
		delayMicroseconds(10);
		while(digitalRead(interruptPinM4) == LOW){}
	}
#elif defined(WIN32)
	void seedRand()
	{
		time_t randTime;
		srand((unsigned) time(&randTime));
	}
 int getRand(int max, int min)
  {
    return (rand() % (max-min)) + min;
  }
#endif




