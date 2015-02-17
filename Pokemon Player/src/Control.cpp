#include "../headers/Control.h"

using namespace std;

//This file can press any key, but it is more convenient to 
bool Control::pressKey(short vk)
{
	keybd_event((unsigned char)vk,0,0,0); // key in
	Sleep(100);
	//cout << "TEST: " << GetAsyncKeyState(0x5a) << endl;
	keybd_event((unsigned char)vk,0,KEYEVENTF_KEYUP,0);

	return true;
}

bool Control::pressA()
{
	return pressKey(0x5A);
}

bool Control::pressB()
{
	return pressKey(0x58);
}
bool Control::pressSelect()
{
	return pressKey(0x4F);
}
bool Control::pressStart()
{
	return pressKey(0x50);
}

bool Control::dumpRam()
{
	return pressKey(VK_OEM_PERIOD);
}

//Up, Down, Left, Right (0, 1, 2, 3)
bool Control::go(int dir)
{
	
	switch(dir) {
	case 0:
		return pressKey(0x54);
		break;
	case 1:
		return pressKey(0x55);
		break;
	case 2:
		return pressKey(0x56);
		break;
	case 3:
		return pressKey(0x57);
		break;
	}
	return false;
}

bool Control::goUp()
{
	keybd_event((unsigned char)0x55,0,0,0); // key in
	Sleep(100);
	//cout << "TEST: " << GetAsyncKeyState(0x5a) << endl;
	keybd_event((unsigned char)0x55,0,KEYEVENTF_KEYUP,0);

	return true;
}
bool Control::goDown()
{
	return pressKey(VK_DOWN);
}
bool Control::goLeft()
{
	return pressKey(VK_LEFT);
}

bool Control::goRight()
{
	return pressKey(VK_RIGHT);
}