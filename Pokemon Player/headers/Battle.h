#ifndef BATTLE_H
#define BATTLE_H

#include <iostream>
#include <fstream>
#include "../headers/Pokemon.h"
#include "../headers/Control.h"
#include "../headers/Data.h"

class Battle {

    public:
		Battle();
		void fight(Data &dat);

    private:
		void init();
		//The current state of the player's Team
		Pokemon playerTeam[6];
		//Variables to track the state of a battle
		int pkmOut;
		int moveCur;

        char * memBlock;
		bool readRam();
		bool buildTeam();
		bool updatePoke(Pokemon poke);
		bool calculateDamage();
		//A pokmeon's info is stored in a longer format outside of battle. Pass the address of the start of the pokemon's info
		Pokemon loadLongPoke(int addr);
		Pokemon loadShortPoke(int addr);
		//All stats are stored as 2-byte values. Pass the address of the first stat bit
		int loadStat(int addr);
};
#endif