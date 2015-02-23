#include "../headers/Battle.h"

using namespace std;

Battle::Battle(Data &d)
{
	pkmOut = 0;
	currMove = 0;
	currSelPoke = 0;
	itemPos = 0;
	//Save the reference to the data
	dat = d;
	//Create the memory chunk that will be read into
	memBlock = new char[8192];
	//Should never be accessed before being initialized, but just in case
	for(int i = 0; i < 2; i++)
		nextMove[i] = 0;
}

Battle::~Battle()
{
	delete[] memBlock;
}


void Battle::fight()
{
	//Reset the state variables
	pkmOut = 0;
	currMove = 0;
	currSelPoke = 0;
	itemPos = 0;

	//Get the ram and build the Player's team
	readRam();
	buildTeam();

	playerTeam[0].printInfo();
	enemy.printInfo();

	while(((int) memBlock[0x1057]) > 0)
	{
		//Read the current state of the battle
		readRam();

		//Update bost lead's stats
		updatePoke(0);
		updatePoke(-1);
		//Calculate the best action
		determineAction();

		cout << nextMove[0] << " " << nextMove[1] << endl;
		//Take the action
		if(nextMove[0] == 1)
		{
			//Attack function
			attack(nextMove[1]);
		}

		//Heal
		if(nextMove[0] == 3)
		{
			heal(nextMove[1]);
		}
		//Take some time to let the attack play out
		Sleep(5000);
		//Mash "B" to get by the boring words (carful to not have the cursor in a text document)
		Control::mashB(6);
	}
	//Talk to everyone after the battle
	Control::mashB(10);
}

bool Battle::readRam()
{
	Control::dumpRam();
	ifstream file ("./Emulator/cgb_wram.bin", ios::in|ios::binary);
	if (file.is_open())
	{
		//file.seekg (0, ios::beg);
		file.read (memBlock, 8192);
		file.close();

		//cout << "RAM loaded" << endl;
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
	int stats[] = {loadStat(addr + 1), loadStat(addr + 36), loadStat(addr + 38), loadStat(addr + 40), loadStat(addr + 42), loadStat(addr + 34)};

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
	int stats[] = {loadStat(addr + 12), loadStat(addr + 28), loadStat(addr + 30), loadStat(addr + 32), loadStat(addr + 34), loadStat(addr + 26)};

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
	if(mv.pow == 0) return 0;

	//cout << "Move being checked " << move << " Name: " << atk.getMove(move) << endl;
	//cout << "Pow " << mv.pow << endl;
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
    
	//Return % damage
	//cout << damage / def.getStat(0) << endl;
	//return damage / def.getStat(0);
    return damage;
}

//Determine the best move to take at a specific point.
//1 = attack, 2 = switch, 3 = heal
void Battle::determineAction()
{
	double bestResults[6][3] = {{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}};

	double tmpStorage[3];

	for(int i = 0; i < teamCount; i++)
	{
		//Try all moves against the opponent
		for(int j = 0; j < 4; j++)
		{
			tmpStorage[0] = j;
			tmpStorage[1] = calcDamage(playerTeam[i], enemy, j, false);
			//Calculate the damage the player would take from the jth move.
			tmpStorage[2] = calcDamage(enemy, playerTeam[i], j, true);
			//cout << tmpStorage[1] << " " << tmpStorage[2] << endl;
			//Update with the best power
			if(bestResults[i][1] < tmpStorage[1])
			{
				bestResults[i][0] = tmpStorage[0];
				bestResults[i][1] = tmpStorage[1];
			}
			//Update with the max the enemy can do
			if(bestResults[i][2] < tmpStorage[2])
				bestResults[i][2] = tmpStorage[2];
		}
	}

	//Assume we will attack with the strongest move on our current lead
	nextMove[0] = 1;
	nextMove[1] = (int)bestResults[pkmOut][0];

	cout << "MAX TAKE: " << bestResults[0][2] << endl;
	cout << "Remaining HP: " << playerTeam[pkmOut].getStat(0) << endl << endl;

	//Will the pokemon survive a hit?
	if(bestResults[pkmOut][2] >= playerTeam[pkmOut].getStat(0))
	{
		//It will drop in one more hit
		cout << "DANGER" << endl;
		//Could it survive at full health?
		//If no, switch
		if(playerTeam[pkmOut].getStat(5) < bestResults[pkmOut][2])
		{
			//Reset which move is highlighted
			cout << "SWITCH!!!" << endl;
			nextMove[0] = 2;
		}
		//If yes, heal
		else
		{
			cout << "HEAL!!!" << endl;
			nextMove[0] = 3;
			nextMove[1] = 0;
		}
		
	}
	
	//		Heal or switch to one that has the best damage/survivability ratio (assuming it can take a hit).

	cout << "nextMove " << nextMove[0] << " " << nextMove[1] << endl;
}

void Battle::attack(int i)
{
	//Get to "Fight"
	Control::goUp();
	Control::goLeft();
	Control::pressA();
	//Navigate to the proper move
	while(currMove > i)
	{
		Control::goUp();
		currMove--;
	}
	while(currMove < i)
	{
		Control::goDown();
		currMove++;
	}
	//Press "A"
	Control::pressA();
}

void Battle::heal(int i)
{
	//Vitally important that this move is selected properly
	Sleep(3000);
	Control::mashB(5);
	cout << "Healing..." << endl;
	//Get to "Item"
	Control::goDown();
	Control::goLeft();
	Control::pressA();
	
	//Move to the top of the item list
	for(int j = 0; j < 20; j++)
		Control::goUp();

	cout << "using the item" << endl;

	//Select that the full heal and say to use it
	Control::pressA();
	Control::pressA();

	//Select the i'th pokemon
	while(currSelPoke > i)
	{
		Control::goUp();
		currSelPoke--;
	}
	while(currSelPoke < i)
	{
		Control::goDown();
		currSelPoke++;
	}
	//Press "A" to heal
	Control::pressA();
	//Press "B" a couple times
	Control::mashB(3);
}

bool Battle::buildTeam()
{
	//Load the fist enemy
	enemy = loadShortPoke(0x0FDA);

	//Load the entire team of pokemon
	teamCount = memBlock[0x1163];
	for(int i = 0; i < teamCount; i++)
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
		enemy = loadShortPoke(0x0FDA);
	return true;
}
