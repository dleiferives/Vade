// function that sets level to its default values 
struct level genLevel(int id, int diff, int width, int height)
{
	struct level l;
	l.id       = id;
	l.diff     = diff;
	l.size.x   = width;
	l.size.y   = height;
	l.lcd      = initPos;
	l.lcdOld  = initPos;
	return l;
}

// get integer pos based on level size in gameMap
int getLevelP(struct level * l, int x, int y)
{
	return (y*l[0].size.x) + x;
}

// gets the char in game map for a given x and y
char getChar(struct level * l, int x, int y)
{
	if(x>=l[0].size.x) return 0;
	if(y>=l[0].size.y) return 0; 
	if(x<0)				return 0; 
	if(y<0)				return 0; 
	return gameMap[getLevelP(l,x,y)];
}

// finds the distance between to chars in game map
int charDist(struct level * l, int pos, int pos2)
{
	int resultx1 = (pos % l[0].size.x);
	int resulty1 = pos / l[0].size.x;
	int resultx2 = (pos2 % l[0].size.x);
	int resulty2 = pos2 / l[0].size.x;

	// literally distance formula
	int dist = sqrt(((resultx2-resultx1) * (resultx2-resultx1)) + ((resulty2-resulty1) * (resulty2-resulty1)));
	return dist;
}

// get random character in the map of type c
struct pos getRChar(char c)
{
	// randomize level generation
	seedRand();
	struct pos result = {0,0};
	// find the amount of the character
	int num =0;
	for(int i =0; i <levels[curLevel].size.x * levels[curLevel].size.y; i++)
	{
		if(gameMap[i] == c) num++;
	}
	int num2 = getRand(num,1);
	//get the pid of the random character
	num =0;
	// find the loc of that pid
	for(int i =0; i <levels[curLevel].size.x * levels[curLevel].size.y; i++)
	{
		if(gameMap[i] == c) num++;
		if(num == num2)
		{
			num2 =i;
			break;
		}
	}
	if(num2 ==0 ) return result;
	// get pos of pid
	result.x = num2 % levels[curLevel].size.x; 
	result.y = num2 / levels[curLevel].size.x; 

	return result;
}

// a function that changes all chars at pos that connect to one 
int floodChar(struct level * l,char c, struct pos p)
{
	//slower code so not to use up memory, since the stack will fill up with a recursive function of this size... we will use something far slower! iteraton
	//check if the character is illegal!
	gameMap[getLevelP(l,p.x,p.y)] = c;
	int reset =0;
	// iterate through level 
	for(int i =0; i<(l->size.x * l->size.y); i++)
	{
		reset = 0;
		if(gameMap[i] == 0) 
			continue;
		if(gameMap[i] == 1)
			continue;
		//check the char below
		if((i + l->size.x) < (l->size.x * l->size.y))
		{
			if((gameMap[i + (l->size.x)] == 1) && (gameMap[i] == 2))
			{
				gameMap[i + (l->size.x)] = 2;
				reset =1;
			}
		}
		// check the char above
		if((i - l->size.x) > (0))
		{
			if((gameMap[i - (l->size.x)] == 1) && (gameMap[i] == 2))
			{
				gameMap[i - (l->size.x)] = 2;
				reset =1;
			}
		}
		// check the char to the right
		if(((i + 1) < (l->size.x * l->size.y)) && ( ((i+1)/l->size.x) == (i/l->size.x)))
		{
			if((gameMap[i + 1] == 1) && (gameMap[i] == 2))
			{
				gameMap[i + 1] = 2;
				reset =1;
			}
		}

		// check the char to the left
		if(((i - 1) < (l->size.x * l->size.y)) && ( ((i-1)/l->size.x) == (i/l->size.x)))
		{
			if((gameMap[i - 1] == 1) && (gameMap[i] == 2))
			{
				gameMap[i - 1] = 2;
				reset =1;
			}
		}

		// if any of the chars have been updated, then restart the loop through the level 
		if(reset ==1)
			i =-1;
	}
	/* printMap(p); */
	return 0;
}

/* return values */
/* 0 = error */
/* 1 = success no other rooms hit */
/* 2 = other room hit, redrawing */
int generateRoom(struct level * l, unsigned int roomX, unsigned int roomY, unsigned int width, unsigned int height, char c)
{
	// if too big or too far over
	if((roomX + width) > l[0].size.x) return 0;
	if((roomY + height) > l[0].size.y) return 0;
	for(int y = 0; y<height; y++)
	{
		for(int x =0; x<width; x++)
		{
			gameMap[getLevelP(&levels[curLevel],roomX+x,roomY+y)] = c;
		}
	}
	return 1;
}


// a function that generates a random room
int genRandRoom(struct level * l)
{
	//seed the random num generation
	seedRand();
	// generate based off of random width height x and y then build the room
	int w = getRand(l[0].size.x/10 +2 , 2);
	int h = getRand(l[0].size.y/8+2 , 2);
	int x = getRand(l[0].size.x, 0);
	int y = getRand(l[0].size.x, 0);
	int result = generateRoom(l,x,y,w,h,1);
	// if room errored, try again -- could use up memory... but it should not be called too many times
	if(result == 0) genRandRoom(l);
	return result;
}


// a function that connects disparate rooms
int generatePath(struct level * l)
{
	//seed random number generation
	seedRand();
	struct pos p1 = getRChar(1);
	//set the connecting chars to 2
	floodChar(l,2,p1);

	// get position of the fellows on the other sides of each other
	p1 = getRChar(2);
	struct pos p2 = getRChar(1);
	//determine if everything is connected together
	if((p2.x == 0) && (p2.y == 0))
		return 1;
	if((p2.x == 1) && (p2.y == 0))
		return 1;
	
	// generate the paths between them
	int dx = p2.x - p1.x; // change in x
	int dy = p2.y - p1.y; // change in y
  int xSign = (dx > 0) ? 1 : -1; // abs change x
  int ySign = (dy > 0) ? 1 : -1; // abs change y
	char c1 = 1; // char 1
	char c2 = 2; // char 2

	
	int cursorX = p1.x; // pos p1x -> cursor
	int cursorY = p1.y; // pos p1y -> cursor
	// for the y's
	for(int y =0; y < (dy * ySign); y++)
	{
		// if it is legal
		char tmpChar = gameMap[getLevelP(l,cursorX,cursorY)];
			if((tmpChar != c1) && (tmpChar !=c2))
			{
				// set it to the map
				gameMap[getLevelP(l,cursorX,cursorY)] = 3; 
			}
		cursorY += ySign;
		// iterate on y
	}

	// for the x's
	for(int x =0; x < (dx * xSign); x++)
	{
		// if it is legal
			char tmpChar = gameMap[getLevelP(l,cursorX,cursorY)];
			if((tmpChar != c1) && (tmpChar !=c2))
			{
				// set it to the map
				gameMap[getLevelP(l,cursorX,cursorY)] = 3; 
			}
		cursorX += xSign;
		// iterate on x
	}

	return 0;
}


// a function that resets eveything in map to 0 or 1
int normalizeLevel(struct level * l)
{
	for(int i=0; i < (l->size.x * l->size.y);i++)
	{
		if(gameMap[i] != 0)
		{
			gameMap[i] = 1;
		}
	}
	return 0;
}

// a function that forces a path between all given rooms
int generatePaths(struct level *l)
{
	while(generatePath(l) == 0)
	{
		normalizeLevel(l);
	}
	normalizeLevel(l);
}

/* returns the position of the player in the new level */
struct pos generateLevelStructure(int id, int diff)
{
	// set the map a new
	resetMap();
	struct pos result;
	// these are exponential functions to make the game exponentially harder
	int diffWidth = pow(2.7, ((float)diff/(float)2) ) + 29;
	int diffHeight = pow(2.7, ((float)diff/(float)2) ) + 10;
	int diffMobs = ((double) diffWidth * (double) diffHeight) * ((double)1.0- ((double) (((double)10.0)/((double) diff + (double) 10.0))));
	int diffRooms = pow(2.5, ((float)diff/(float)2) ) + 8;

	// these are locks so that only legal memory is read
	if(diffWidth > mapSizeX)
		diffWidth = mapSizeX;
	if(diffHeight > mapSizeY)
		diffHeight = mapSizeY;
	if(diffMobs > maxMobs)
		diffMobs = maxMobs;
	numMobs = diffMobs;

	// generate the level
	levels[id] = genLevel(id,diff, diffWidth, diffHeight);

	// reset the map once again in level size
	for(int i=0; i < diffWidth * diffHeight; i++) {
		gameMap[i] = 0;
	}

	// generate rooms by diffRooms
	for(int j=0; j< diffRooms;j++)
	{
			genRandRoom(&levels[id]);	
	}

	//generate paths 
	generatePaths(&levels[id]);
	// change it to '.' and 'A' from 0 and 1 so next code runs
	for(int i =0; i<(diffWidth * diffHeight); i++)
	{
		if(gameMap[i] == 0)
			gameMap[i] = '.';
		if(gameMap[i] == 1)
			gameMap[i] = 'A';
	}

	// iterate through level
	struct level * l = &levels[id];
	for(int y =0; y < diffHeight; y++)
	{
		for(int x =0; x < diffWidth; x++)
		{
			if((getChar(l,x,y) == '.'))
			{
				// if empty next to floor put a wall
				if((getChar(l,x+1,y) > '.') || (getChar(l,x-1,y) > '.') || (getChar(l,x,y+1) > '.') || (getChar(l,x,y-1) > '.'))
				{
					gameMap[getLevelP(l,x,y)] = '#';
				}
			}
		}
	}

	// change it to look sleek 
	for(int i=0; i<(diffWidth*diffHeight); i++)
	{
		if(gameMap[i] == '.')
			gameMap[i] = ' ';
		if(gameMap[i] == 'A')
			gameMap[i] = '.';
	}

	// set the exit position of the map to be '+' the trap door
	struct pos exit = getRChar('.');
	gameMap[getLevelP(&levels[id],exit.x,exit.y)] = '+';


	// put mobs onto the level
	for(int i =0; i < diffMobs; i++)
	{
		mobs[i].id =i+1;
		mobs[i].posScreen = getRChar('.');
		if(false)
		{
			Serial.print("MOBS --");
			Serial.print("m");
			Serial.print(mobs[i].id);
			Serial.print(" ");
			Serial.print(mobs[i].posScreen.x);
			Serial.print(",");
			Serial.print(mobs[i].posScreen.y);
			Serial.print("\n");
		}
	}


}

// a function that creates the next level for the player
void nextLevel(struct character * player)
{
	/* destroy_level(&levels[curLevel]); */
	// make the level higher
	curLevel +=1;		
	// generate the level
	generateLevelStructure(curLevel,curLevel);
	// get player pos rand on level on floor
	player->posScreen = getRChar('.');
	levels[curLevel].lcd = player->posScreen;
	//reset lcd pos
	levels[curLevel].lcd.x = 0;
	levels[curLevel].lcd.y = 0;
#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	// print the map size to serial
	Serial.print("\nMap size");
	Serial.print(levels[curLevel].size.x);
	Serial.print(",");
	Serial.print(levels[curLevel].size.y);
	Serial.print("\n");
#endif
	// print the new level to the screen
	printMap(levels[curLevel].lcd);
}
