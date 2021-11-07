#include <stdio.h>
#include <math.h>


int main()
{
	printf("{");
	for(int i =0; i < 2048; i++)
	{
		double cur_pos = (((double) i*2.0*M_PI)/((double) 2048.0));
		double value = sin(cur_pos);
		printf("%lf,",value);
	}
	printf("}");
}

