#include <iostream>
#include <conio.h>
using namespace std;

void main() 
{
	int c;
	while (1) 
	{
		c = _getch();
		if (c >= 48 && c <= 57)
			break;
		cout << c << endl;
	}
}