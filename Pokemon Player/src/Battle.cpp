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

		//Update both lead's stats
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

		//Switch
		if(nextMove[0] == 2)
		{
			swap(nextMove[1]);
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
	int bestChoice = 0;
	int bestTank = 0;
	boolean leadSafe;
	Result bestResults[6];

	//Figure out the max damage that each teammember will deal and take
	for(int i = 0; i < teamCount; i++)
	{
		bestResults[i].teamPos = i;
		//Try all moves against the opponent
		for(int j = 0; j < 4; j++)
		{
			Result tmpStorage(i, j, calcDamage(playerTeam[i], enemy, j, false), calcDamage(enemy, playerTeam[i], j, true));
			//cout << tmpStorage[1] << " " << tmpStorage[2] << endl;
			//Update with the best power
			if(bestResults[i].maxDamage < tmpStorage.maxDamage)
			{
				bestResults[i].maxDamage = tmpStorage.maxDamage;
				bestResults[i].move = tmpStorage.move;
			}
			//Update with the max the enemy can do
			if(bestResults[i].takeDamage < tmpStorage.takeDamage)
				bestResults[i].takeDamage = tmpStorage.takeDamage;
		}
	}

	//Can the lead take a hit?
	leadSafe = bestResults[pkmOut].takeDamage < playerTeam[pkmOut].getStat(0);

	//Heal the lead if it is threatened

	//Sort to find the strongest pokemon
	sort(begin(bestResults), end(bestResults), Data::sortResults);

	//Move through the list to find something usable
	while(bestChoice < 6)
	{
		//Can the best choice take the worst hit?
		//*Yes
		if(bestResults[bestChoice].takeDamage < playerTeam[bestResults[bestChoice].teamPos].getStat(0))
		{
			break;
		}
		//No, but it could if it was at full health and the lead can take a hit (I am not considering switching to a tank and then healing the best)
		else if((leadSafe && bestResults[bestChoice].takeDamage < playerTeam[bestChoice].getStat(5)) ||
			(pkmOut == bestResults[bestChoice].teamPos && bestResults[bestChoice].takeDamage < playerTeam[pkmOut].getStat(5)))
		{
			//Heal the best choice
			cout << "Healing" << endl;
			nextMove[0] = 3;
			nextMove[1] = bestResults[bestChoice].teamPos;
			return;
		}
		//Can the next strongest thing take a hit?
		else
		{
			bestChoice++;
		}
	}

	if(bestChoice >= 6)
	{
		cout << "Something went horribly wrong" << endl;
	}

	//If the best choice is currently out, attack
	//If it isn't, switch
	if(bestResults[bestChoice].teamPos == pkmOut)
	{
		cout << "Attacking!" << endl;
		nextMove[0] = 1;
		nextMove[1] = bestResults[bestChoice].move;
	}
	else
	{
		cout << "Switching to " << bestResults[bestChoice].teamPos << endl;
		nextMove[0] = 2;
		nextMove[1] = bestResults[bestChoice].teamPos;
	}
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

void Battle::swap(int i)
{
	//Get to "PKMN"
	Control::goUp();
	Control::goRight();
	Control::pressA();
	Sleep(500);
	//Navigate to the proper pokemon
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
	//Press "A" to select the pokemon and then confirm it
	Control::pressA();
	Control::pressA();
}

void Battle::heal(int i)
{
	//Vitally important that this move is selected properly
	Sleep(2000);
	Control::mashB(5);
	//Get to "Item"
	Control::goDown();
	Control::goLeft();
	Control::pressA();
	Sleep(500);

	//Move to the top of the item list
	for(int j = 0; j < 20; j++)
		Control::goUp();

	//Select that the full heal and say to use it
	Control::pressA();
	Control::pressA();
	Sleep(500);

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
