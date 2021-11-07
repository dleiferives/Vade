#include "util.h"
#include "entities.h"
#include "render.h"
#include "world.h"
#include "items.h"
#include "alloc.h"
#include "input.h"


int _main()
{
	setup_game();
	setup_arduino();
	init_maps();
	get_render_map();
	print_map(levels[cur_level].lcd);
	for(int i =0; i<(levels[cur_level].size.x * 16); i++)
	{
		putchar(levels[cur_level].map[i]);
	}


	add_to_item_list(&init_item);

	/* creating the player character */
	struct character player = init_character;
	player.items[0] = 1;
	player.items[1] = 0;

	player.tile = '@';

	add_item(&player, 2);
	player.pos_screen.x = 1;
	player.pos_screen.y = 1;


	/* render loop */
	while(1)
	{
		handle_input();
		reset_entity_map();
		int temp_mode = get_mode();
		int temp_dir = get_dir();
		#if defined(WIN32)
			printf("before_switch x%i y%i old%c cur %c\n",player.pos_screen.x,player.pos_screen.y,player.tile);
		#endif

		switch(temp_mode)
		{
			case 1:
				//render on the screen up/down makes it scroll up or down
				if(temp_dir == 3) levels[cur_level].lcd.y++;
				if(temp_dir == 2) levels[cur_level].lcd.y--;
				levels[cur_level].lcd.y = (levels[cur_level].lcd.y<0) ? 0 : levels[cur_level].lcd.y;
				levels[cur_level].lcd.y = (levels[cur_level].lcd.y>14) ? 14 : levels[cur_level].lcd.y;
				break;
			case 2:
				//move the character / cursor I guess

				#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
					lcd.setCursor(0,0);
					lcd.print(temp_dir);
				#endif
				switch(temp_dir)
				{
					case 0:
						if((player.pos_screen.x + 1) >= (levels[cur_level].size.x )) break;
						move_character(&player,1,0);
						break;
					case 1:
						if((player.pos_screen.x - 1) < 0) break;
						/* player.tile_old = player.tile; */
						/* player.tile = get_char(player.pos_screen.x -1, player.pos_screen.y); */
						move_character(&player,-1,0);
						break;
					case 2:
						if((player.pos_screen.y - 1) < 0) break;
						/* player.tile_old = player.tile; */
						/* player.tile = get_char(player.pos_screen.x, player.pos_screen.y-1); */
						move_character(&player,0,-1);
						break;
					case 3:
						if((player.pos_screen.y + 1) >= (levels[cur_level].size.y )) break;
						/* player.tile_old = player.tile; */
						/* player.tile = get_char(player.pos_screen.x, player.pos_screen.y+1); */
						move_character(&player,0,1);
						break;
					case 4:
						break;
				}
				break;
			case 3:
				//action or cursor or something
				break;
		}

		#if defined(WIN32)
			printf("after_switch x%i y%i old%c cur %c\n",player.pos_screen.x,player.pos_screen.y,player.tile);
		#endif

		render_character(&player);
		get_render_map();	
		print_map(levels[cur_level].lcd);
				
	
		#if defined(__AVT_ATmega1028__) || defined(__AVR_ATmega2560__)
			pA4 = analogRead(A4);
			lcd.setCursor(0,1);
			lcd.print(temp_mode);
			delay(200);
		#endif
		
	}

	free(game_map);


	/* return 0; */
}

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

	void loop(){};
#endif
