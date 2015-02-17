#include "../headers/Pokemon.h"

using namespace std;

Pokemon::Pokemon()
{
	
    name = "Nameless";
    level = 0;
    types[0] = 15;
    types[1] = 15;
    for(int i = 0; i < STAT_COUNT; i++)
    {
        stats[i] = 0;
    }
    for(int i = 0; i < 4; i++)
    {
        moves[i] = 0;
		powPts[i] = 0;
    } 
}

Pokemon::Pokemon(string n, int l, int s[], int t[], int mvs[], int pow[])
{
	
    name = n;
    level = l;
    types[0] = t[0];
    types[1] = t[1];
    for(int i = 0; i < STAT_COUNT; i++)
    {
        stats[i] = s[i];
    }
    for(int i = 0; i < 4; i++)
    {
        moves[i] = mvs[i];
		powPts[i] = pow[i];
    } 
}

double Pokemon::calcWeak(int moveType, Pokemon def)
{
    //Normal, Fire, Water, ELectric, Grass, Ice, Fighting, Poison, Ground, Flying, Psychic
    //Bug Rock Ghost Dragon, NULL
    static int WEAK_CHART[][NUM_TYPES + 1] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0},
        {0, 2, 2, 0, 1, 1, 0, 0, 0, 0, 0, 1, 2, 0, 2, 0},
        {0, 1, 2, 0, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 2, 0},
        {0, 0, 1, 2, 2, 0, 0, 0, 3, 1, 0, 0, 0, 0, 2, 0},
        {0, 2, 1, 0, 2, 0, 0, 2, 1, 2, 0, 2, 1, 0, 2, 0},
        {0, 2, 2, 0, 1, 2, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0},
        {1, 0, 0, 0, 0, 1, 0, 2, 0, 2, 2, 2, 1, 3, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 1, 2, 2, 0, 0},
        {0, 1, 0, 1, 2, 0, 0, 1, 0, 3, 0, 2, 1, 0, 0, 0},
        {0, 0, 0, 2, 1, 0, 1, 0, 0, 0, 0, 1, 2, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0},
        {0, 2, 0, 0, 1, 0, 2, 1, 0, 2, 1, 0, 0, 2, 0, 0},
        {0, 1, 0, 0, 0, 1, 2, 0, 2, 1, 0, 1, 0, 0, 0, 0},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1, 0, 0}, 
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};;

    double multiplier = 1;
    for(int i = 0; i < 2; i++)
    {
        if(WEAK_CHART[moveType][def.types[i]] == 1)
            multiplier *= 2;
        else if(WEAK_CHART[moveType][def.types[i]] == 2)
            multiplier *= .5;
        else if(WEAK_CHART[moveType][def.types[i]] == 3)
            multiplier = 0;
    }

    //cout << multiplier << endl;
    return multiplier;
}

int Pokemon::calcDamage(Pokemon def, int moveNum, bool minDam)
{
    static int PHYS_SPEC[] = {1, 3, 3, 3, 3, 3, 1, 1, 1, 1, 3, 1, 1, 1, 3};

    int damage = ((2 * level) / 5 + 2);
    //Add in the attack stat
//    damage *= stats[PHYS_SPEC[moves[moveNum].type]];
    //damage *= moves[moveNum].pow;
    //Divide by defense
    //damage /= (PHYS_SPEC[moves[moveNum].type] == 1) ? def.stats[2] : def.stats[3];
    damage /= 50;
    //cout << damage << endl;
    if(damage > 997)
        damage = 997;
    damage += 2;
    //if(moves[moveNum].type == types[0] || moves[moveNum].type == types[1])
    {
        damage *= 1.5;
    }
    
    //damage *= calcWeak(moves[moveNum].type, def);
    
    if(damage < 768)
    {
        damage *= (minDam) ? 217 : 255;
        damage /= 255;
    }
    
    return damage;
}

void Pokemon::printInfo()
{
    cout << name << " " << level << endl;
    cout << types[0] << " " << types[1] << endl;
    for(int i = 0; i < STAT_COUNT; i++)
    {
        cout << stats[i] << " ";
    }
	cout << endl;
    for(int i = 0; i < 4; i++)
	{
		cout << powPts[i] << " ";
	}
	cout << endl;
    for(int i = 0; i < 4; i++)
	{
		cout << moves[i] << endl;
	}
    cout << endl << endl;
}
