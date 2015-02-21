#ifndef POKEMON_H
#define POKEMON_H

#include <iostream>
#include <string>
#include <time.h>
#include "Data.h"
class Pokemon {

    public:
		Pokemon();

		//Pokemon's Name (irrelevant), level, stats, types, moves
        Pokemon(std::string n, int l, int s[], int t[], int mvs[], int pow[]);
        //Calculates the weakness/resistance multiplier
        double calcWeak(int moveType);
		int getLvl();
		int getStat(int pos);
		int getMove(int pos);
		bool isSTAB(int type);

        //Calculates the expected damage of a move (minDam is true if we want the min damage, and 
		// false if we want the max (non-crit) damage)
        //int calcDamage(Pokemon def, int moveNum, bool minDam);
        //Prints a pokemon's info
        void printInfo();

    private:
		//The sixth is max HP
        const static int STAT_COUNT = 6;
        const static int NUM_TYPES = 15;
        
        std::string name;
		//HP, Atk, Def, Special, Spd
        int stats[5];
        int types[2];
        int moves[4];
		int powPts[4];
        int level;
};
#endif
