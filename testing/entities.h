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

void move_character(struct character * player,int x, int y)
{
	player -> pos_screen_old = player[0].pos_screen;
	player[0].pos_screen.x += x;
	player[0].pos_screen.y += y;
}

void move_mob(struct mob * m, int x, int y)
{
	m -> pos_screen.x +=x;
	m -> pos_screen.y +=y;
}

