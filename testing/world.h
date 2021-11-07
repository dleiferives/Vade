
struct level gen_level(int id, int diff, int width, int height, int mobs)
{
	struct level l;
	l.id       = id;
	l.diff     = diff;
	l.size.x   = width;
	l.size.y   = height;
	l.lcd      = init_pos;
	l.map      = (char *)malloc(sizeof(char) * l.size.x * l.size.y);
	l.num_mobs = mobs;

	for(int i  = 0; i< (l.size.x * l.size.y); i ++)
		l.map[i] = 0;
	l.mobs     = (struct mob*)malloc(sizeof(struct mob) * mobs);
	return l;
}

void destroy_level(struct level * l)
{
	free(l->mobs);
	free(l->map);
}

void gen_rooms_in_level(struct level *l)
{
	// rand number of rooms
	// fill out area of rectangles with some character possibly +
	// find edgeof those areas, if the room has no empty room.... delete it
	
	//make paths between the rooms... so they are all traversable
	
}



