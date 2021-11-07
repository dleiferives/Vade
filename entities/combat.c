#include <stdio.h>
#include <stdlib.h>
struct position
{
	int x;
	int y;
};
struct player
{
	int id;
	struct position pos;
	int hp;
	int damage;
	int range;
	char icon;
};

struct mob
{
	int id;
	struct position po;
	int hp;
	int damage;
	int range;
	char icon;
};

int abs(int a)
{
	a= (a>=0) ? a : -a;
	return a;

int id_dist(struct position p1, struct position p2)
{
	return ( abs(p2.x-p1.x) + abs(p2.y - p1.y));
}


int main()
{



}
