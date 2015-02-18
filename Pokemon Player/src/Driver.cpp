#include "../headers/Battle.h"
#include "../headers/Data.h"

using namespace std;

int main()
{
	Data dat;
	
	Sleep(5000);
	Battle battleMode(dat);
	battleMode.fight();
	return 0;
}