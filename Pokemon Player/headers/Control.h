#ifndef CONTROL_H
#define CONTROL_H

#include <windows.h>

class Control {

    public:
        static bool pressA();
		static bool pressB();
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