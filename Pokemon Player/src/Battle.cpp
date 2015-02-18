#include "../headers/Battle.h"

using namespace std;

Battle::Battle(Data &d)
{
	pkmOut = 0;
	moveCur = 0;
	//Save the reference to the data
	dat = d;
}

void Battle::fight()
{
	//Reset the state variables
	pkmOut = 0;
	moveCur = 0;

	//Get the ram and rebuild the Player's team
	readRam();
	buildTeam();

	playerTeam[0].showInfo();
	enemy.showInfo();

	cout << calcDamage(playerTeam[0], enemy, 0, false) << " - " << calcDamage(playerTeam[0], enemy, 0, true) << endl;
	cout << calcDamage(enemy, playerTeam[0], 0, false) << " - " << calcDamage(enemy, playerTeam[0], 0, true) << endl;
}
bool Battle::readRam()
{
	Control::dumpRam();

	streampos size;
	memBlock = new char [8192];

	ifstream file ("./Emulator/cgb_wram.bin", ios::in|ios::binary);
	if (file.is_open())
	{
		//file.seekg (0, ios::beg);
		file.read (memBlock, 8192);
		file.close();

		cout << "RAM loaded" << endl;
	}
	else cout << "Unable to open file" << endl;

	return true;
}

int Battle::loadStat(int addr)
{
	return (((int)memBlock[addr]) << 8) + (int)memBlock[addr + 1];
}

Pokemon Battle::loadLongPoke(int addr)
{
	//Load the stats from RAM based on their offset from the starting address
	int stats[] = {loadStat(addr + 1), loadStat(addr + 36), loadStat(addr + 38), loadStat(addr + 40), loadStat(addr + 42)};
	int types[] = {dat.convGameType(memBlock[addr + 5]), (memBlock[addr + 5] == memBlock[addr + 6]) ? dat.convGameType(memBlock[addr + 6]) : 15};
	int moves[4] = {memBlock[addr + 8], memBlock[addr + 9], memBlock[addr + 10],memBlock[addr + 11]};
	int pp[4] = {memBlock[addr + 29], memBlock[addr + 30], memBlock[addr + 31], memBlock[addr + 32]};

	//Create the pokemon based on the stats
	Pokemon newPoke("Name", memBlock[addr + 33], stats, types, moves, pp);

	return newPoke;
}

Pokemon Battle::loadShortPoke(int addr)
{
	//Load the stats from RAM based on their offset from the starting address
	int stats[] = {loadStat(addr + 12), loadStat(addr + 28), loadStat(addr + 30), loadStat(addr + 32), loadStat(addr + 34)};
 	int types[] = {dat.convGameType(memBlock[addr + 16]), (memBlock[addr + 16] == memBlock[addr + 17]) ? dat.convGameType(memBlock[addr + 17]) : 15};
	int moves[] = {memBlock[addr + 19], memBlock[addr + 20], memBlock[addr + 21],memBlock[addr + 22]};
	
	//If this is the user's pokemon, it has limited pp, but if a pokemon is the 
	//computer's it effectively has infinite PP. However, we still read the values
	//where PP should be. I deal with moves having 0 PP in the damage functions
	int pp[] = {memBlock[addr + 36], memBlock[addr + 37], memBlock[addr + 38], memBlock[addr + 39]};


	//Create the pokemon based on the stats
	Pokemon newPoke("Name", memBlock[addr + 25], stats, types, moves, pp);

	return newPoke;
}

double Battle::calcDamage(Pokemon atk, Pokemon def, int move, bool isMax)
{
	Move mv = dat.getMove(atk.getMove(move));
    double damage = (int)((2 * atk.getLvl()) / 5 + 2);
    //Multiply by the attack or special stat
    damage *= atk.getStat(dat.atkTarg(mv.type));
	//Multiply by the move's power
    damage *= mv.pow;
    //Divide by defense
    damage /= (dat.atkTarg(mv.type) == 1) ? def.getStat(2) : def.getStat(3);
	damage = (int) damage;
    damage /= 50;
	damage = (int) damage;
    //cout << damage << endl;
    if(damage > 997)
        damage = 997;
    damage += 2;
    

	//Take STAB into account
    damage *= (atk.isSTAB(mv.type)) ? 1.5 : 1.0;
    
    //Is the target weak to the move?
    damage *= def.calcWeak(mv.type);
    
    if(damage < 768)
    {
        damage *= (isMax) ? 255 : 217;
        damage /= 255;
    }
    
    return damage;
}

bool Battle::buildTeam()
{
	//Load the fist enemy
	enemy = loadShortPoke(0x0FDA);

	//Load the entire team of pokemon
	Pokemon team[6];
	int teamMem = (char) memBlock[0x1163];
	for(int i = 1; i < teamMem; i++)
	{
		playerTeam[i] = loadLongPoke(0x116B + (44 * i));
	}

	return true;
}

//Updates a pokemon with its current stat values
bool Battle::updatePoke(int pos)
{
	if(pos >= 0)
		playerTeam[pos] = loadShortPoke(0x1009);
	else
		enemy = loadShortPoke(0xCFDA);
	return true;
}
