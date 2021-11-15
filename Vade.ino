#include "util.h"
#include "game.h"
// entities
//render 
// itmes
#include "world.h"
#include "alloc.h"
#include "input.h"

/* levels -- pointer array of type level */

int _main()
{
	//Setting up the game
	setup_game(); // alloc.h
	setup_arduino(); // alloc.h
	init_maps();     // game.h
	get_render_map();// game.h 
	print_map(levels[cur_level].lcd); //game.h

	//setting up the character
	struct character player = init_character;
	player.items[0] =1;
	player.items[1] =0;
	player.tile = '@';
	add_item(&player, 2);
	player.pos_screen = get_rand_pos_of_char('.');

	// gameplay loop
	while(1)
	{
		old_g_time = g_time;
		handle_input(); //input.h
		reset_entity_map(); // game.h -- render
		int temp_mode = get_mode(); //input.h
		int temp_dir = get_dir();   //input.h
		
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
		switch(temp_mode)
		{
			case 1:
				if(temp_dir == 3) levels[cur_level].lcd.y++;
				if(temp_dir == 2) levels[cur_level].lcd.y--;
				levels[cur_level].lcd.y = (levels[cur_level].lcd.y<0) ? 0 : levels[cur_level].lcd.y;
				levels[cur_level].lcd.y = (levels[cur_level].lcd.y>14) ? 14 : levels[cur_level].lcd.y;
				break;

			case 2:
				#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
					lcd.setCursor(0,0);
					lcd.print(temp_dir);
				#endif
				switch(temp_dir)
				{
					case 0:
						move_character(&player,1,0);
						break;
					case 1:
						move_character(&player,-1,0);
						break;
					case 2:
						move_character(&player,0,-1);
						break;
					case 3:
						move_character(&player,0,1);
						break;
					case 4:
						break;
				}
				break;

			case 3:
			//cursors n stuff
				break;
			case 4:
				if(temp_dir == 3)
				{
					// take action at player	
					character_action(&player,to_pos(0,0));
				}
				if(temp_dir == 2)
				{
					next_level(&player);

				}
				break;
					reset_entity_map();
		}
		// runs if the time has advanced. time does not 
		if(g_time > old_g_time)
		{
			/* run entities code */
		}

		render_character(&player);
		get_render_map();
		print_map(levels[cur_level].lcd);

	}
	
	/* deallocate all of it bucco */
	cleanup_game(); // alloc.h

}
/* Makes either windows or arduino code run */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	void setup()
	{
		 _main();
	}

	void loop()
	{
		lcd.setCursor(0,0);
		lcd.print(error_string);
	}


#elif defined(WIN32)
	int main()
	{
		_main();
		return 0;
	}

#endif
