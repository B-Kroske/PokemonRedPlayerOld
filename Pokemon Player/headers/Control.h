#ifndef CONTROL_H
#define CONTROL_H

#include <windows.h>

class Control {

    public:
        static bool pressA();
		static bool pressB();
		//Press B alot to skip the boring stuff
		static bool mashB(int times);
		static bool pressStart();
		static bool pressSelect();
		static bool dumpRam();
		static bool goUp();
		static bool goDown();
		static bool goLeft();
		static bool goRight();
		static bool go(int dir);

    private:
        static bool pressKey(short vk);
};
#endif