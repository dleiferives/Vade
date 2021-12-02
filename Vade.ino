/* Dylan Leifer-Ives -- Vade
 * EE-151 Janine's section
 * A small rougelike game that is meant to be played on the Arduino Mega
 * Inspired by NetHack
 */

#include "util.h"   // global variables & when neccesary functions & libraries
#include "game.h"   // functions to handle rendering movment and combat
#include "world.h"  // functions to setup game world
#include "alloc.h"  // functions to setup and cleanup the game
#include "input.h"  // Handles player input

/* Main function -- called from either windows or arduino */
int _main()
{
	setupGame();                    		 // all of the game setup functions    -- alloc.h
	setupArduino();                			 // all of the arduion setup functions -- alloc.h
  generateLevelStructure(curLevel, 0); // generate the first level for the game 
	while(1)
	{ handleInput(); 
		if(getDir() != 4) break;
	}  // pause execution while waiting for input
	printMap(levels[curLevel].lcd); 		 //game.h

	//setting up the character
	struct character player = initCharacter;
	player.items[0] =1;
	player.items[1] =0;
	player.tile = '@';
	add_item(&player, 2);
	player.posScreen = getRChar('.');

	// gameplay loop
	while(1)
	{
		// mob movment is determined by ticks
		// here we check if the game has ticked forward in time
		oldGameTime = gameTime;
		handleInput(); // gets the input from the buttons/ IR -- input.h
		int tempMode = getMode(); // gets mode button -- input.h
		int tempDir = getDir();   // gets dir on joystick or ir -- input.h
		
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
			/* Move the screen around on the level */
			case 1:
				/* if the screen can move, change the pos of top left Y*/
				if(levels[curLevel].size.y > AUD_HEIGHT)
				{
					if(tempDir == 3) levels[curLevel].lcd.y++;
					if(tempDir == 2) levels[curLevel].lcd.y--;
				}
				/* if the screen can move, change the pos of the top left X */
				if(levels[curLevel].size.x > AUD_WIDTH)
				{
					if(tempDir == 0) levels[curLevel].lcd.x++;
					if(tempDir == 1) levels[curLevel].lcd.x--;
				}
				/* locking the lcd to the lcd size so we dont get phanotm player movement */
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
				
				/* if the player has done a directional movment... update the screen accordingly */
				if(tempDir != 4)
					printMap(levels[curLevel].lcd);	
				
				break;
			/* Move the player */
			case 2:
			/* Quite literally based on the direction, move the character relative to itself */
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
			/* move the cursor relative to itself and then render it */
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
			/* the actions the player can take */
			/* should be a switch but right now there are only two possibilities */
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
			// Run the entities code
			for(int i =0; i < numMobs; i++)
				mobCrawl(&mobs[i],player.posScreen);
		}

		// render the entities
		struct mob * mTemp;
		renderEntities(mTemp, numMobs);
		// render the character
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
		//error screen
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
