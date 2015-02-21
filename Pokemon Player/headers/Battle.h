#ifndef BATTLE_H
#define BATTLE_H

#include <iostream>
#include <fstream>
#include "../headers/Pokemon.h"
#include "../headers/Control.h"
#include "../headers/Data.h"

class Battle {

    public:
		Battle(Data &d);
		void fight();

    private:
		void init();
		//The current state of the player's Team
		Pokemon playerTeam[6];
		int teamCount;
		int nextMove[2];

		//The opponent
		Pokemon enemy;

		//Variables to track the state of a battle
		int pkmOut;
		int currMove;
		int itemPos;
		int currSelPoke;

		//Storage for various things that may be useful
		Data dat;
        char * memBlock;

		bool readRam();
		bool buildTeam();
		//Determines the best action to take on a specific turn. Sets the values in "nextMove".
		void determineAction();
		//Attack with the i'th move
		void attack(int i);

		//Updates the i'th position pokemon on the player's team. If pos equals -1, update the opposing pokemon.
		bool updatePoke(int pos);
		double calcDamage(Pokemon atk, Pokemon def, int move, bool isMax);
		//A pokmeon's info is stored in a longer format outside of battle. Pass the address of the start of the pokemon's info
		Pokemon loadLongPoke(int addr);
		Pokemon loadShortPoke(int addr);
		//All stats are stored as 2-byte values. Pass the address of the first stat bit
		int loadStat(int addr);
};
#endif