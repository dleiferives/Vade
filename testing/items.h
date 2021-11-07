
int add_to_item_list(struct item * item_adding)
{
	if(cur_global_items < num_global_items)
	{
		items[cur_global_items] = item_adding[0];
		cur_global_items++;
		return 0;
	}
	else
	{
		num_global_items += 100;
		items = (struct item*)realloc(items, num_global_items + 100);
		if(items != NULL) puts("ERROR ALLOCATING MORE ITEMS");
		items[cur_global_items] = item_adding[0];
		cur_global_items++;
		return 0;
	}
}

struct item item_by_id(int id)
{
	if(id >= num_global_items)
	{
		puts("ERROR IN ITEMS, TRYING TO GET A NON ALLOCATED ITEM");	
		return init_item;
	}
	return items[id];
}

	



