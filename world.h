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


int getLevelP(struct level * l, int x, int y)
{
	return (y*l[0].size.x) + x;
}

char getChar(struct level * l, int x, int y)
{
	if(x>=l[0].size.x) return 0;
	if(y>=l[0].size.y) return 0; 
	if(x<0)				return 0; 
	if(y<0)				return 0; 
	return gameMap[getLevelP(l,x,y)];
}

int charDist(struct level * l, int pos, int pos2)
{
	int resultx1 = (pos % l[0].size.x);
	int resulty1 = pos / l[0].size.x;
	int resultx2 = (pos2 % l[0].size.x);
	int resulty2 = pos2 / l[0].size.x;

	int dist = sqrt(((resultx2-resultx1) * (resultx2-resultx1)) + ((resulty2-resulty1) * (resulty2-resulty1)));
	return dist;
}

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

int floodChar(struct level * l,char c, struct pos p)
{
	//slower code so not to use up memory, since the stack will fill up with a recursive function of this size... we will use something far slower! iteraton
	//check if the character is illegal!
	gameMap[getLevelP(l,p.x,p.y)] = c;
	int reset =0;
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

		if(((i + 1) < (l->size.x * l->size.y)) && ( ((i+1)/l->size.x) == (i/l->size.x)))
		{
			if((gameMap[i + 1] == 1) && (gameMap[i] == 2))
			{
				gameMap[i + 1] = 2;
				reset =1;
			}
		}

		if(((i - 1) < (l->size.x * l->size.y)) && ( ((i-1)/l->size.x) == (i/l->size.x)))
		{
			if((gameMap[i - 1] == 1) && (gameMap[i] == 2))
			{
				gameMap[i - 1] = 2;
				reset =1;
			}
		}
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
	if(result == 0) genRandRoom(l);
	return result;
}


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
	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
  int xSign = (dx > 0) ? 1 : -1;
  int ySign = (dy > 0) ? 1 : -1;
	char c1 = 1;
	char c2 = 2;

	
	int cursorX = p1.x;
	int cursorY = p1.y;
	for(int y =0; y < (dy * ySign); y++)
	{
		char tmpChar = gameMap[getLevelP(l,cursorX,cursorY)];
			if((tmpChar != c1) && (tmpChar !=c2))
			{
				gameMap[getLevelP(l,cursorX,cursorY)] = 3; 
			}
		cursorY += ySign;
	}

	for(int x =0; x < (dx * xSign); x++)
	{
			char tmpChar = gameMap[getLevelP(l,cursorX,cursorY)];
			if((tmpChar != c1) && (tmpChar !=c2))
			{
				gameMap[getLevelP(l,cursorX,cursorY)] = 3; 
			}
		cursorX += xSign;
	}

	return 0;
}


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
	resetMap();
	struct pos result;
	//dificulty 0 - 1000
	//width 30 - 300
	//height 20 - 200
	int diffWidth = pow(2.7, ((float)diff/(float)2) ) + 29;
	int diffHeight = pow(2.7, ((float)diff/(float)2) ) + 10;
	/* int diffWidth = 10; */ 
	/* int diffHeight = 10; */ 
	int diffMobs = ((double) diffWidth * (double) diffHeight) * ((double)1.0- ((double) (((double)10.0)/((double) diff + (double) 10.0))));
	int diffRooms = pow(2.5, ((float)diff/(float)2) ) + 8;

	if(diffWidth > mapSizeX)
		diffWidth = mapSizeX;
	if(diffHeight > mapSizeY)
		diffHeight = mapSizeY;
	if(diffMobs > maxMobs)
		diffMobs = maxMobs;
	numMobs = diffMobs;

	levels[id] = genLevel(id,diff, diffWidth, diffHeight);

	for(int i=0; i < diffWidth * diffHeight; i++) {
		gameMap[i] = 0;
	}

	for(int j=0; j< diffRooms;j++)
	{
			genRandRoom(&levels[id]);	
	}

	generatePaths(&levels[id]);
	for(int i =0; i<(diffWidth * diffHeight); i++)
	{
		if(gameMap[i] == 0)
			gameMap[i] = '.';
		if(gameMap[i] == 1)
			gameMap[i] = 'A';
	}

	struct level * l = &levels[id];
	for(int y =0; y < diffHeight; y++)
	{
		for(int x =0; x < diffWidth; x++)
		{
			if((getChar(l,x,y) == '.'))
			{
				if((getChar(l,x+1,y) > '.') || (getChar(l,x-1,y) > '.') || (getChar(l,x,y+1) > '.') || (getChar(l,x,y-1) > '.'))
				{
					gameMap[getLevelP(l,x,y)] = '#';
				}
			}
		}
	}

	for(int i=0; i<(diffWidth*diffHeight); i++)
	{
		if(gameMap[i] == '.')
			gameMap[i] = ' ';
		if(gameMap[i] == 'A')
			gameMap[i] = '.';
	}

	struct pos exit = getRChar('.');
	gameMap[getLevelP(&levels[id],exit.x,exit.y)] = '+';


	// put mobs onto the level
	for(int i =0; i < diffMobs; i++)
	{
		mobs[i].id =i+1;
		mobs[i].posScreen = getRChar('.');
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

void nextLevel(struct character * player)
{
	/* destroy_level(&levels[curLevel]); */
	curLevel +=1;		
	generateLevelStructure(curLevel,curLevel);
	player->posScreen = getRChar('.');
	levels[curLevel].lcd = player->posScreen;
	levels[curLevel].lcd.x = 0;
	levels[curLevel].lcd.y = 0;
#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
	Serial.print("\nMap size");
	Serial.print(levels[curLevel].size.x);
	Serial.print(",");
	Serial.print(levels[curLevel].size.y);
	Serial.print("\n");
#endif
	printMap(levels[curLevel].lcd);

}
