
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
		// if no item add to inv
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

// get the char that is p2 from p1
char relChar(struct pos p1, struct pos p2)
{
	char result = gameMap[getLevelP(&levels[curLevel],p1.x+p2.x, p1.y+p2.y)];
	putchar(result);
	return result;
}

// move the character rel x and y
void moveCharacter(struct character * player,int x, int y)
{
	// tStep determines if game time increases 
	int tStep =1;
	if((player->posScreen.x + x) >= (levels[curLevel].size.x )) tStep=0;
	if((player->posScreen.x + x) < 0) tStep=0;
	if((player->posScreen.y + y) < 0) tStep=0;
	if((player->posScreen.y + y) >= (levels[curLevel].size.y )) tStep=0;
	// if it seems not extreemly illegal do some more tests 
	if(tStep !=0)
	{
		// check if its a wall though
		if(relChar(player->posScreen,toPos(x,y)) == '#')
		{ 
			tStep = 0;
		}
		// check if its an enemy on the screen -- old but still useful if an error occurs
		if(relChar(player->posScreen,toPos(x,y)) == '0')
		{ 
			tStep = 0;
		}
		// check if the player will move into a monster
		// gets player pos+ rel and sees if its equal to any monsters
		for(int i=0; i < numMobs; i++)
		{
			if((mobs[i].posScreen.x == player->posScreen.x + x) && (mobs[i].posScreen.y == player->posScreen.y + y))
			{
				if(mobs[i].id != 0)
				{
					tStep =0;
				}
			}
		}
		//if everything is legal move him
		if(tStep == 1)
		{
			// update old to current
			player -> posScreenOld = player[0].posScreen;
			player[0].posScreen.x += x; // change cur pos by rel pos
			player[0].posScreen.y += y; // change cur pos by rel pos
			gameTime++; 							  // make time increase -> mobs can move
		}
	}
}

// moves the cursor
void moveCursor(int x, int y)
{
	// tStep deterimes if cursor will move ! does not mean the game time will increase
	int tStep =1;
	if((cursorPos.x + x) >= (levels[curLevel].size.x )) tStep=0;
	if((cursorPos.x + x) < 0) tStep=0;
	if((cursorPos.y + y) < 0) tStep=0;
	if((cursorPos.y + y) >= (levels[curLevel].size.y )) tStep=0;
	// if not off screen...move him
	if(tStep !=0)
	{
		if(tStep == 1)
		{
			cursorPosOld = cursorPos; // set old to cur
			cursorPos.x += x;         // change cur rel
			cursorPos.y += y;         // change cur rel
		}
	}
}

// move a monster on the screen
void moveMob(struct mob * m, int x, int y, struct pos p)
{
	// if the monster is alive
	if(m->id != 0)
	{
		// tStep does not equal time.. rather it determines if the mob can omve
		int tStep =1;
		if((m->posScreen.x + x) >= (levels[curLevel].size.x )) tStep=0;
		if((m->posScreen.x + x) < 0) tStep=0;
		if((m->posScreen.y + y) < 0) tStep=0;
		if((m->posScreen.y + y) >= (levels[curLevel].size.y )) tStep=0;
		// if not off screen check for collision
		if(tStep !=0)
		{
			// check if wall
			if(relChar(m->posScreen,toPos(x,y)) == '#')
			{ 
				tStep = 0;
			}
			// check if exit 
			if(relChar(m->posScreen,toPos(x,y)) == '+')
			{ 
				tStep = 0;
			}
			// check if it is another monster... but more comprehensive
			for(int i=0; i < numMobs; i++)
			{
				// for all mobs check if its position is equal to where this one wants to go
				if((mobs[i].posScreen.x == m->posScreen.x + x) && (mobs[i].posScreen.y == m->posScreen.y + y))
				{
					if(mobs[i].id != 0)
					{
						tStep =0;
					}
				}
			}
			//check if the monster istrying to move into the player
			if((m->posScreen.x +x == p.x)&&(m->posScreen.y+y == p.y ))
			{
				tStep =0;
				// damage the player -- not implemented
			}
			// if all is good, let him move
			if(tStep == 1)
			{
				m-> posScreenOld = m->posScreen; // make old pos cur pos
				m[0].posScreen.x += x;           // change x by rel
				m[0].posScreen.y += y;           // change y by rel
			}
		}
	}
}

// function which makes the mobs move
// "the ai"
void mobCrawl(struct mob * m, struct pos p)
{
	// mp is the mobs pos on screen
	// p is the players pos on screen
	struct pos mp = m->posScreen; 
	int dx = mp.x - p.x; // the dist between player and mob on x
	int dy = mp.y - p.y; // the dist between player and mob on y
	int adx = (dx>0) ? dx : -dx; // the absolute val of dist on x
	int ady = (dy>0) ? dy : -dy; // the absolute val of dist on y

	/* <- Mob detects player in range ->
	 * . . . . . . . . . . . . . . . . .
	 * . . . . . . X X X X X . . . . . .
	 * . . . . . . X X X X X . . . . . .
	 * . . . . . . X X X X X . . . . . .
	 * . . . . . . X X X X X . . . . . .
	 * . . X X X X X X X X X X X X X . .
	 * . . X X X X X X X X X X X X X . . 
	 * . . X X X X X X P X X X X X X . . 
	 * . . X X X X X X X X X X X X X . . 
	 * . . X X X X X X X X X X X X X . . 
	 * . . . . . . X X X X X . . . . . .
	 * . . . . . . X X X X X . . . . . .
	 * . . . . . . X X X X X . . . . . .
	 * . . . . . . X X X X X . . . . . .
	 * . . . . . . . . . . . . . . . . . 
	 */
	if((ady < 3) && (adx < 7))
	{
		moveMob(m,-dx/adx,0,p);
	}
	if((ady < 7) && (adx < 3))
	{
		// move on y axis toward player
		moveMob(m,0,-dy/ady,p);
	}
	else
	{
		// random movment
		if(getRand(1,0) == 0)
		{
			moveMob(m,getRand(1,0),0,p);
		}
		else
		{
			moveMob(m,0,getRand(1,0),p);
		}
	}
}

// function to attack mob if called kills mob under cursor
void attackMob()
{
	//iterate through mobs
	for(int i=0; i < numMobs; i++)
	{
		//check if there is a mob at the pos of the cursor;	
		if((mobs[i].posScreen.x == cursorPos.x) && (mobs[i].posScreen.y == cursorPos.y))
		{
			// if not already dead
			if(mobs[i].id != 0)
			{
				// make dead
				mobs[i].id =0;
				// render it
				renderEntities(mobs,numMobs);
			}
		}
	}
	return;
}

// a function that lets player take characterAction
int characterAction(struct character * player, struct pos p)
{
	// gets the character on the map that is p from player
	char loc = relChar(player->posScreen, p);
	int s_time =1;
	switch(loc)
	{
		// if trapdoor move player down to the next level
		case '+':
			// only happens if player is on trapdoor
			if((p.x == 0) && (p.y == 0))
			{
				s_time =0;
				nextLevel(player);
			}
			break;
			// attacks the ground if there is monster it goes through
		case '.':
			if(sqrt( ((p.x)*(p.x)) + ((p.y)*(p.y))) < 2.5)
			{
				//kill mob there
				attackMob();
			}
			break;
	}
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
/* 1 - the room/ world where the game takes place is defined and stored as a map level map */
/* 2 -  On game rendering entities are put ontop of level map */
/* render map is put to the screen, for win32 this is done with the printf, arduino its lcd.print but relative to the screen size */
void resetMap(void)
{
	for(int i =0; i < (mapSizeX * mapSizeY); i++)
	{
		gameMap[i] = 0;
	}
}


// puts an ascii character into the game map
int renderAscii(char val, struct pos p)
{
	if((p.x >=0) && (p.x < levels[curLevel].size.x) && (p.y >=0) && (p.y < levels[curLevel].size.y))
	{
	  gameMap[(p.y * levels[curLevel].size.x) + p.x] = val;
		return 0;
	}
	if((p.x <0) || (p.x <= levels[curLevel].size.x) || (p.y < 0) || (p.y <=levels[curLevel].size.y)) return 1;
	return 1;
}


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
void renderCharacter(struct character * player)
{
	if(levels[curLevel].lcd.y > player->posScreen.y)
		return 0;
	if((levels[curLevel].lcd.y + AUD_HEIGHT) <= player->posScreen.y)
		return 0;
	// debug 
	if (false)
	{
		Serial.print("LCD > playerL");
		/* if(player > (lcd + level)) no render */ 
		/* if player < lcd no render */
		Serial.print("PLAYER POS ");
		Serial.print(player->posScreen.x);
		Serial.print(" ");
		Serial.print(player->posScreen.y);
		Serial.print("\nLCD POS ");
		Serial.print(levels[curLevel].lcd.x);
		Serial.print(" ");
		Serial.print(levels[curLevel].lcd.y);
		Serial.print("\n");
	}
  tft.setTextColor(TFT_WHITE, TFT_BLACK);


	// sets the old pos of the player back to the map
	tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + player->posScreenOld.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + player->posScreenOld.y)*16));
	// the two lines above were done on paper in lab book
	tft.print(gameMap[getLevelP(&levels[curLevel],player->posScreenOld.x, player->posScreenOld.y)]);


	// sets the cur pos of the player to the map
  tft.setTextColor(TFT_BLUE, TFT_BLACK);

	tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + player->posScreen.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + player->posScreen.y)*16));
	// the two lines above were done on paper in lab book
	// prints to the screen
	tft.print(player->tile);
	// delay to make the game playable by humans ;-;
	delay(300);
}

void renderCursor()
{
	if(levels[curLevel].lcd.y > cursorPos.y)
		return 0;
	if((levels[curLevel].lcd.y + AUD_HEIGHT) <= cursorPos.y)
		return 0;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

	//sets the old pos of the cursor back to the map
	tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + cursorPosOld.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + cursorPosOld.y)*16));
	// the two lines above were done on paper in lab book
	tft.print(gameMap[getLevelP(&levels[curLevel], cursorPosOld.x, cursorPosOld.y)]);

	// sets pos of the cursor to the cursor
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
	tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x +cursorPos.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + cursorPos.y)*16));
	// the two lines above were done on paper in lab book
	
	// prints to the screen
	tft.print(cursorIcon);

	// shorter delay since code takes muuuuch longer 
	delay(50);
}

// a function that renders all of the entities
void renderEntities(struct mob * m, int numMobs)
{
	//iterates through the entities
	for(int i=0; i<numMobs; i++)
	{
		//if the mob is not dead
		if(mobs[i].id != 0)
		{
			// if mob on scree
		if(levels[curLevel].lcd.y > mobs[i].posScreen.y)
			return 0;
		// if mob on screen
		if((levels[curLevel].lcd.y + AUD_HEIGHT) <= mobs[i].posScreen.y)
			return 0;
		
		//old pos of the monster is set back to the map 
		tft.setTextColor(TFT_WHITE, TFT_BLACK);

		tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + mobs[i].posScreenOld.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + mobs[i].posScreenOld.y)*16));
		tft.print(gameMap[getLevelP(&levels[curLevel],mobs[i].posScreenOld.x, mobs[i].posScreenOld.y)]);


		//current pos of the monster is printed as the monster
		tft.setTextColor(TFT_RED);
		tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + mobs[i].posScreen.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + mobs[i].posScreen.y)*16));

		// print the monster icon to the screen at the pos 
		tft.print('0');
		}
	}
}

// the function which prints the map to the lcd with top left corner as p
void printMap(struct pos p)
{
	// fill the screen with blackjd
	// full reset
	tft.fillScreen(TFT_BLACK);
	// reset rotation on the screen
	tft.setRotation(1);
	// LCD library needs length of draw times so start recording
	unsigned long start = micros();
	// set cursor top left
	tft.setCursor(0, 0);
	// set the color to white
  tft.setTextColor(TFT_WHITE); 
	// get level x and y locally for readability
	int l_x = levels[curLevel].size.x;
	int l_y = levels[curLevel].size.y;
	// determine the width being printed to the screen... aka are there borders
	int width  = (AUD_WIDTH > l_x)  ? l_x : AUD_WIDTH; // width of level on screen
	int height = (AUD_HEIGHT > l_y) ? l_y : AUD_HEIGHT; // height of level on screen
	// determines the borders if there are some
	struct pos offsets = {AUD_WIDTH - width, AUD_HEIGHT - height};
	struct pos finalPos;
	// computes the final position of the top left of the lcd in the game map
	finalPos.x = ((p.x+width)>l_x) ? l_x-width : p.x;
	finalPos.x = (finalPos.x < 0) ? 0 : finalPos.x;
	finalPos.y = ((p.y+height)>l_y) ? l_y-height : p.y;
	finalPos.y = (finalPos.y < 0) ? 0 : finalPos.y;
	// the cursor pos on x
	int cursor_print_x =0;
	// the cursor pos on y
	int cursor_print_y = offsets.y/2;
	// put the cursor there
	tft.setCursor(cursor_print_x*8,cursor_print_y*16);
	// set the top left (if there are offsets) to the top left
	mapTL.x = (offsets.x/2) * 8;
	mapTL.y = (offsets.y/2) * 16;
	tft.setTextColor(TFT_WHITE,TFT_BLACK);
	// for height
	for(int y=0; y<height; y++)
	{
		// offset the cursor if neccesary
		cursor_print_x = offsets.x/2;
		// set the cursor to offset norm 0
		tft.setCursor(cursor_print_x*8,cursor_print_y*16);
		// for width
		for(int x=0; x<width; x++)
		{
			// set cursor to pos
			tft.setCursor((cursor_print_x++-1)*8,cursor_print_y*16);
			// print char to screen at pos
			tft.print((char) gameMap[(finalPos.x+x)+((finalPos.y+y) * l_x)]);	
		}
		// increase the cursor y
		cursor_print_y++;
	}
	// return the render time
  return micros() - start;
}


#elif defined(WIN32)
void renderCharacter(struct character * player)
{
	printf("player_pos = %i, %i\n",player->posScreen.x, player->posScreen.y);
}

void renderEntities(struct mob * m, int numMobs)
{
	for(int i=0; i<numMobs; i++)
	{
		printf("mob_%i_pos = %i, %i\n",i,m[i].posScreen.x, m[i].posScreen.y);
	}
}

// arduino lcd screen emulator
void printAud(struct pos p)
{
	// local level size for readability
	int l_x = levels[curLevel].size.x;
	int l_y = levels[curLevel].size.y;
	int width  = (AUD_WIDTH > l_x)  ? l_x : AUD_WIDTH; // width of level on screen
	int height = (AUD_HEIGHT > l_y) ? l_y : AUD_HEIGHT; // height of level on screen
	// screen offsets 
	struct pos offsets = {AUD_WIDTH - width, AUD_HEIGHT - height};
	// final screen offsets
	struct pos finalPos;
	finalPos.x = ((p.x+width)>l_x) ? l_x-width : p.x;
	finalPos.x = (finalPos.x < 0) ? 0 : finalPos.x;
	finalPos.y = ((p.y+height)>l_y) ? l_y-height : p.y;
	finalPos.y = (finalPos.y < 0) ? 0 : finalPos.y;
	printf("Width %i Height %i\n",width,height);
	printf("OWidth %i OHeight %i\n",offsets.x,offsets.y);
	// print the characters rel to the screen offsets
	for(int i=0; i<AUD_WIDTH+2; i++) putchar('-');
	putchar(10);
		for(int oy =0; oy < offsets.y/1; oy++) putchar(10);
	for(int y=0; y<height; y++)
	{
		putchar('|');
			for(int ox =0; ox < offsets.x/2; ox++) putchar(' ');
		for(int x=0; x<width; x++)
		{
			putchar(gameMap[(finalPos.x+x)+((finalPos.y+y) * l_x)]);	
		}
			for(int ox =0; ox < offsets.x/2; ox++) putchar(' ');
				putchar('|');
		putchar(10);
	}
		for(int oy =0; oy < offsets.y/1; oy++) putchar(10);
	for(int i=0; i<AUD_WIDTH+2; i++) putchar('-');
}

void printMap(struct pos p)
{

	printAud(p);
	printf("Mode = %i\n",inputMode);

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

/* <- Some useless functions at the moment... they will not be commented, as they are not used --> */
int add_to_item_list(struct item * item_adding)
{
	if(curGlobalItems < numGlobalItems)
	{
		items[curGlobalItems] = item_adding[0];
		curGlobalItems++;
		return 0;
	}
	else
	{
		numGlobalItems += 100;
		items = (struct item*)realloc(items, numGlobalItems + 100);
		if(items != NULL) puts("ERROR ALLOCATING MORE ITEMS");
		items[curGlobalItems] = item_adding[0];
		curGlobalItems++;
		return 0;
	}
}

struct item item_by_id(int id)
{
	if(id >= numGlobalItems)
	{
		puts("ERROR IN ITEMS, TRYING TO GET A NON ALLOCATED ITEM");	
		return initItem;
	}
	return items[id];
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
 *
 * --------- COMBAT --------
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
	



