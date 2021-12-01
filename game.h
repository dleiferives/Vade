
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

char relChar(struct pos p1, struct pos p2)
{
	char result = gameMap[getLevelP(&levels[curLevel],p1.x+p2.x, p1.y+p2.y)];
	putchar(result);
	return result;
}

void moveCharacter(struct character * player,int x, int y)
{
	int tStep =1;
	if((player->posScreen.x + x) >= (levels[curLevel].size.x )) tStep=0;
	if((player->posScreen.x + x) < 0) tStep=0;
	if((player->posScreen.y + y) < 0) tStep=0;
	if((player->posScreen.y + y) >= (levels[curLevel].size.y )) tStep=0;
	if(tStep !=0)
	{
		if(relChar(player->posScreen,toPos(x,y)) == '#')
		{ 
			tStep = 0;
		}
		if(relChar(player->posScreen,toPos(x,y)) == '0')
		{ 
			tStep = 0;
		}
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
		if(tStep == 1)
		{
			player -> posScreenOld = player[0].posScreen;
			player[0].posScreen.x += x;
			player[0].posScreen.y += y;
			gameTime++;
		}
	}
}
void moveCursor(int x, int y)
{
	int tStep =1;
	if((cursorPos.x + x) >= (levels[curLevel].size.x )) tStep=0;
	if((cursorPos.x + x) < 0) tStep=0;
	if((cursorPos.y + y) < 0) tStep=0;
	if((cursorPos.y + y) >= (levels[curLevel].size.y )) tStep=0;
	if(tStep !=0)
	{
		if(tStep == 1)
		{
			cursorPosOld = cursorPos;
			cursorPos.x += x;
			cursorPos.y += y;
		}
	}
}

void moveMob(struct mob * m, int x, int y, struct pos p)
{
	if(m->id != 0)
	{
		int tStep =1;
		if((m->posScreen.x + x) >= (levels[curLevel].size.x )) tStep=0;
		if((m->posScreen.x + x) < 0) tStep=0;
		if((m->posScreen.y + y) < 0) tStep=0;
		if((m->posScreen.y + y) >= (levels[curLevel].size.y )) tStep=0;
		if(tStep !=0)
		{
			if(relChar(m->posScreen,toPos(x,y)) == '#')
			{ 
				tStep = 0;
			}
			if(relChar(m->posScreen,toPos(x,y)) == '+')
			{ 
				tStep = 0;
			}
			for(int i=0; i < numMobs; i++)
			{
				if((mobs[i].posScreen.x == m->posScreen.x + x) && (mobs[i].posScreen.y == m->posScreen.y + y))
				{
					if(mobs[i].id != 0)
					{
						tStep =0;
					}
				}
			}
			if((m->posScreen.x +x == p.x)&&(m->posScreen.y+y == p.y ))
				tStep =0;
			if(tStep == 1)
			{
				m-> posScreenOld = m->posScreen;
				m[0].posScreen.x += x;
				m[0].posScreen.y += y;
			}
		}
	}
}

void mobCrawl(struct mob * m, struct pos p)
{
	struct pos mp = m->posScreen;
	// if mob same x dx < 7 mov toward player
	// if mob same y dy < 7 mov toward player
	int dx = mp.x - p.x;
	int dy = mp.y - p.y;
	int adx = (dx>0) ? dx : -dx;
	int ady = (dy>0) ? dy : -dy;

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

void attackMob()
{
	//check if there is a mob at the pos of the cursor;	
	for(int i=0; i < numMobs; i++)
	{
		if((mobs[i].posScreen.x == cursorPos.x) && (mobs[i].posScreen.y == cursorPos.y))
		{
			if(mobs[i].id != 0)
			{
				mobs[i].id =0;
				renderEntities(mobs,numMobs);
			}
		}
	}
	return;
}

int characterAction(struct character * player, struct pos p)
{
	char loc = relChar(player->posScreen, p);
	int s_time =1;
	switch(loc)
	{
		case '+':
			if((p.x == 0) && (p.y == 0))
			{
				s_time =0;
				nextLevel(player);
			}
			break;
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


// SPEED -- could be made faster by passing the pos by value







#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
void renderCharacter(struct character * player)
{
	if(levels[curLevel].lcd.y > player->posScreen.y)
		return 0;
	if((levels[curLevel].lcd.y + AUD_HEIGHT) <= player->posScreen.y)
		return 0;
	/* if(levels[curLevel].lcd.x > player->posScreen.x) */
		/* return 0; */
	/* if((levels[curLevel].lcd.x + AUD_WIDTH) <= player->posScreen.x) */
		/* return 0; */
		/* Serial.print("LCD > playerL"); */
	/* if(player > (lcd + level)) no render */ 
	/* if player < lcd no render */
	/* Serial.print("PLAYER POS "); */
	/* Serial.print(player->posScreen.x); */
	/* Serial.print(" "); */
	/* Serial.print(player->posScreen.y); */
	/* Serial.print("\nLCD POS "); */
	/* Serial.print(levels[curLevel].lcd.x); */
	/* Serial.print(" "); */
	/* Serial.print(levels[curLevel].lcd.y); */
	/* Serial.print("\n"); */
  tft.setTextColor(TFT_WHITE, TFT_BLACK);


	/* tft.setCursor(mapTL.x + (player->posScreenOld.x*8 - 8),mapTL.y + (player->posScreenOld.y*16)); */
	tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + player->posScreenOld.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + player->posScreenOld.y)*16));
	tft.print(gameMap[getLevelP(&levels[curLevel],player->posScreenOld.x, player->posScreenOld.y)]);


  tft.setTextColor(TFT_BLUE, TFT_BLACK);
	tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + player->posScreen.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + player->posScreen.y)*16));
	tft.print(player->tile);

	delay(100);
}

void renderCursor()
{
	if(levels[curLevel].lcd.y > cursorPos.y)
		return 0;
	if((levels[curLevel].lcd.y + AUD_HEIGHT) <= cursorPos.y)
		return 0;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

	tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + cursorPosOld.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + cursorPosOld.y)*16));
	tft.print(gameMap[getLevelP(&levels[curLevel], cursorPosOld.x, cursorPosOld.y)]);


  tft.setTextColor(TFT_GREEN, TFT_BLACK);
	tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x +cursorPos.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + cursorPos.y)*16));
	tft.print(cursorIcon);

	delay(100);
}

void renderEntities(struct mob * m, int numMobs)
{

	for(int i=0; i<numMobs; i++)
	{
		if(mobs[i].id != 0)
		{
		if(levels[curLevel].lcd.y > mobs[i].posScreen.y)
			return 0;
		if((levels[curLevel].lcd.y + AUD_HEIGHT) <= mobs[i].posScreen.y)
			return 0;
		tft.setTextColor(TFT_WHITE, TFT_BLACK);

		tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + mobs[i].posScreenOld.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + mobs[i].posScreenOld.y)*16));
		tft.print(gameMap[getLevelP(&levels[curLevel],mobs[i].posScreenOld.x, mobs[i].posScreenOld.y)]);


		tft.setTextColor(TFT_RED);
		tft.setCursor(mapTL.x + ((-levels[curLevel].lcd.x + mobs[i].posScreen.x)*8 - 8),
			mapTL.y + ((-levels[curLevel].lcd.y + mobs[i].posScreen.y)*16));
		tft.print('0');
		}
	}
}

void printMap(struct pos p)
{
	tft.fillScreen(TFT_BLACK);
	tft.setRotation(1);
	unsigned long start = micros();
	tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE); 
	int l_x = levels[curLevel].size.x;
	int l_y = levels[curLevel].size.y;
	int width  = (AUD_WIDTH > l_x)  ? l_x : AUD_WIDTH; // width of level on screen
	int height = (AUD_HEIGHT > l_y) ? l_y : AUD_HEIGHT; // height of level on screen
	struct pos offsets = {AUD_WIDTH - width, AUD_HEIGHT - height};
	struct pos finalPos;
	finalPos.x = ((p.x+width)>l_x) ? l_x-width : p.x;
	finalPos.x = (finalPos.x < 0) ? 0 : finalPos.x;
	finalPos.y = ((p.y+height)>l_y) ? l_y-height : p.y;
	finalPos.y = (finalPos.y < 0) ? 0 : finalPos.y;
	int cursor_print_x =0;
	int cursor_print_y = offsets.y/2;
	tft.setCursor(cursor_print_x*8,cursor_print_y*16);
	mapTL.x = (offsets.x/2) * 8;
	mapTL.y = (offsets.y/2) * 16;
	tft.setTextColor(TFT_WHITE,TFT_BLACK);
	for(int y=0; y<height; y++)
	{
		cursor_print_x = offsets.x/2;
		tft.setCursor(cursor_print_x*8,cursor_print_y*16);
		for(int x=0; x<width; x++)
		{
			tft.setCursor((cursor_print_x++-1)*8,cursor_print_y*16);
			tft.print((char) gameMap[(finalPos.x+x)+((finalPos.y+y) * l_x)]);	
		}
		cursor_print_y++;
	}
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

void printAud(struct pos p)
{
	int l_x = levels[curLevel].size.x;
	int l_y = levels[curLevel].size.y;
	int width  = (AUD_WIDTH > l_x)  ? l_x : AUD_WIDTH; // width of level on screen
	int height = (AUD_HEIGHT > l_y) ? l_y : AUD_HEIGHT; // height of level on screen
	struct pos offsets = {AUD_WIDTH - width, AUD_HEIGHT - height};
	struct pos finalPos;
	finalPos.x = ((p.x+width)>l_x) ? l_x-width : p.x;
	finalPos.x = (finalPos.x < 0) ? 0 : finalPos.x;
	finalPos.y = ((p.y+height)>l_y) ? l_y-height : p.y;
	finalPos.y = (finalPos.y < 0) ? 0 : finalPos.y;
	printf("Width %i Height %i\n",width,height);
	printf("OWidth %i OHeight %i\n",offsets.x,offsets.y);
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

	/* for(int y =0; y <levels[curLevel].size.y; y++) */
	/* { */
	/* 	for(int x =0; x <levels[curLevel].size.x; x++) */
	/* 	{ */
	/* 		putchar(render_map[(y*levels[curLevel].size.x) +x]); */
	/* 		putchar(' '); */
	/* 	} */
	/* 	putchar(10); */
	/* } */
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
	



