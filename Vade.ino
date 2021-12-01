#include "util.h"
#include "game.h"
// entities
//render 
// itmes
//combat
#include "world.h"
#include "alloc.h"
#include "input.h"

/* levels -- pointer array of type level */

int _main()
{
	tft.begin();
	setupGame(); // alloc.h
	setupArduino(); // alloc.h
	printMap(levels[curLevel].lcd); //game.h

	//setting up the character
	struct character player = initCharacter;
	player.items[0] =1;
	player.items[1] =0;
	player.tile = '@';
	addItem(&player, 2);
	player.posScreen = getRChar('.');

	// gameplay loop
	while(1)
	{
		oldGameTime = gameTime;
		handleInput(); //input.h
		int tempMode = getMode(); //input.h
		int tempDir = getDir();   //input.h
		

		/* states of the switch statment
		 *
		 * case 1 -- move the screen up down left right
		 * case 2 -- move the character 
		 *  case 0 -- move right
		 *  case 1 -- move left
		 *  case 2 -- move down
		 *  case 3 -- move up
		 * case 3 -- move the cursor (for attacking and reading)
		 * case 4 -- actions! -- does not remove the cursor like case 2 does
		 */
		switch(tempMode)
		{
			case 1:
				/* struct pos tmp_pos = levels[curLevel].lcd; */ 
				if(levels[curLevel].size.y > AUD_HEIGHT)
				{
					if(tempDir == 3) levels[curLevel].lcd.y++;
					if(tempDir == 2) levels[curLevel].lcd.y--;
				}
				if(levels[curLevel].size.x > AUD_WIDTH)
				{
					if(tempDir == 0) levels[curLevel].lcd.x++;
					if(tempDir == 1) levels[curLevel].lcd.x--;
				}
				levels[curLevel].lcd.y = (levels[curLevel].lcd.y<0) ? 0 : levels[curLevel].lcd.y;
				levels[curLevel].lcd.y = (levels[curLevel].lcd.y> levels[curLevel].size.y) ? levels[curLevel].size.y : levels[curLevel].lcd.y;
				levels[curLevel].lcd.x = (levels[curLevel].lcd.x<0) ? 0 : levels[curLevel].lcd.x;
				levels[curLevel].lcd.x = (levels[curLevel].lcd.x> levels[curLevel].size.x ) ? levels[curLevel].size.x : levels[curLevel].lcd.x;
				if(AUD_WIDTH < levels[curLevel].size.x)
				{
					levels[curLevel].lcd.x = (levels[curLevel].lcd.x + AUD_WIDTH > levels[curLevel].size.x ) ? levels[curLevel].size.x - AUD_WIDTH : levels[curLevel].lcd.x;
				}
				if(AUD_HEIGHT< levels[curLevel].size.y)
				{
					levels[curLevel].lcd.y = (levels[curLevel].lcd.y + AUD_HEIGHT > levels[curLevel].size.y ) ? levels[curLevel].size.y - AUD_HEIGHT : levels[curLevel].lcd.y;
				}
				
				/* if((tmp_pos.x != levels[curLevel].lcd.x) || (tmp_pos.y != levels[curLevel].lcd.y)) */
					/* levels[curLevel].lcd_old = tmp_pos; */
				#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
					/* Serial.print(levels[curLevel].lcd.x); */
					/* Serial.print(levels[curLevel].lcd.y); */
				#endif
				if(tempDir != 4)
					printMap(levels[curLevel].lcd);	
				
				break;

			case 2:
				#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
					/* tft.setCursor(0,0); */
					/* tft.print(tempDir); */

					/* Serial.println(tempDir); */
				#endif
				switch(tempDir)
				{
					case 0:
						moveCharacter(&player,1,0);
						break;
					case 1:
						moveCharacter(&player,-1,0);
						break;
					case 2:
						moveCharacter(&player,0,-1);
						break;
					case 3:
						moveCharacter(&player,0,1);
						break;
					case 4:
						break;
				}
				break;

			case 3:
				switch(tempDir)
				{
					case 0:
						moveCursor(1,0);
						break;
					case 1:
						moveCursor(-1,0);
						break;
					case 2:
						moveCursor(0,-1);
						break;
					case 3:
						moveCursor(0,1);
						break;
					case 4:
						break;
					}
				renderCursor();
				break;
			case 4:
				if(tempDir == 3)
				{
					// take action at player	
					characterAction(&player,toPos(0,0));
				}
				if(tempDir == 2)
				{
				  //take action at cursor
					characterAction(&player,toPos(cursorPos.x - player.posScreen.x, cursorPos.y - player.posScreen.y));
				}
				break;
		}
		// runs if the time has advanced. time does not 
		if(gameTime > oldGameTime)
		{
			/* run entities code */
			for(int i =0; i < numMobs; i++)
				mobCrawl(&mobs[i],player.posScreen);
		}

		struct mob * mTemp;
		renderEntities(mTemp, numMobs);
		renderCharacter(&player);
	}
	
	/* deallocate all of it bucco */
	cleanupGame(); // alloc.h

}
/* Makes either windows or arduino code run */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	void setup()
	{
		 _main();
	}

	void loop()
	{
		tft.setCursor(0,0);
		tft.fillScreen(TFT_BLACK);
		tft.print(errorString);
	}


#elif defined(WIN32)
	int main()
	{
		_main();
		return 0;
	}

#endif
