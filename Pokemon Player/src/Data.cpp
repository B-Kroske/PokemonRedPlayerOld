#include "../headers/Data.h"

Data::Data()
{
	//My ordering:
	//Normal, Fire, Water, ELectric, Grass, Ice, Fighting, Poison, Ground, Flying, Psychic
    //Bug Rock Ghost Dragon, NULL
	int tmpCsvConvert[] = {-1, 0, 6, 9, 7, 8, 12, 11, 13, -1, 1, 2, 4, 3, 10, 5, 14};
	int tmpGameConvert[] = {0, 6, 9, 7, 8, 12, -1, 11, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 2, 4, 3, 9, 5, 14};

	//Tracks wherter a move is physical or special
	int tmpPhysSpec[] = {1, 3, 3, 3, 3, 3, 1, 1, 1, 1, 3, 1, 1, 1, 3, 0};

	for(int i = 0;  i < 16; i++)
	{
		csvToMine[i] = tmpCsvConvert[i];
		physOrSpec[i] = tmpPhysSpec[i];
	}
	for(int i = 0;  i < 27; i++)
		gameToMine[i] = tmpGameConvert[i];
	cout << "Working" << endl;
	//Check that the list of the moves was read properly
	if(!initMoves())
	{
		cout << "Couldn't read moves." << endl;
		return;
	}
	cout << "Complete" << endl;
}

bool Data::initMoves()
{
	string line;
	ifstream moveFile("./InfoFiles/MovesCsv1.txt");
	vector<string> elems;
	int moveIndex = 0;

	//Add a blank move
	moveList[moveIndex++] = Move(0, 0, 0, 0);

	//Check if the file is open
	if (moveFile.is_open())
	{
		//Read in all of the moves
		while(getline(moveFile, line))
		{
			//Split the line at commas
			split(line, ',', elems);

			//Create a new move
			moveList[moveIndex++] = Move(StringToNumber(elems[4]), StringToNumber(elems[6]), csvToMine[StringToNumber(elems[3])], StringToNumber(elems[5]));
			
			//Clear the line
			elems.clear();
		}

		return true;
	}
	else cout << "Unable to open move file" << endl;
	return false;
}

Move Data::getMove(int index)
{
	return moveList[index];
}

//Convert the type that the game stores to one that my program uses
int Data::convGameType(int t)
{
	return gameToMine[t];
}

//Does a move target defense or special?
int Data::atkTarg(int t)
{
	return physOrSpec[t];
}

bool Data::sortResults(Result a, Result b)
{
	if(a.maxDamage > b.maxDamage)
		return true;
	else if(a.maxDamage < b.maxDamage)
		return false;
	else
		return a.takeDamage > b.takeDamage;
}

//I copied these off of stack overflow
//http://stackoverflow.com/questions/236129/split-a-string-in-c/236803#236803
vector<string> &Data::split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> Data::split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

//Copied this from cplusplus.com and modified it because I only need ints
//http://www.cplusplus.com/forum/articles/9645/
int Data::StringToNumber ( string &Text )//Text not by const reference so that the function can be used with a 
{                               //character array as argument
	stringstream ss(Text);
	int result;
	return ss >> result ? result : 0;
}
