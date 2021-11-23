﻿#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <ctime>

using std::cin;
using std::cout;
using std::string;
using std::ifstream;
using std::ofstream;

const string BOT_ACCOUNT = "Bot";
//CONSTANTS FILE NAME
const string ACCOUNTS_FILE = "accounts.txt";
const string DEFAULT_THREE = "defaultthreethree.txt";
const string DEFAULT_FIVE = "defaultfivefive.txt";
const string DEFAULT_SEVEN = "defaultsevenseven.txt";
const string SPIRAL_THREE = "spiralthreethree.txt";
const string SPIRAL_FIVE = "spiralfivefive.txt";
const string SPIRAL_SEVEN = "spiralsevenseven.txt";

//LIMITATION
const int MAX_PLAYER_PER_MAP = 4;
const int MAX_HORSE_PER_PLAYER = 4;
const int MAX_ROW = 10;
const int MAX_COL = 10;
const int SPACE_BETWEEN_POINT = 2;
const int MAX_NAME_DISPLAY = 9;
const int MID_NAME_DISPLAY = MAX_NAME_DISPLAY / 2;
const int MAX_ACCOUNT = 100;

//DISPLAY CHARACTER
const char DEFAULT_CHAR = '?';
const char BLOCK_CHAR = 'B';
const char UP_CHAR = '^';
const char DOWN_CHAR = 'v';
const char LEFT_CHAR = '<';
const char RIGHT_CHAR = '>';
const char START_CHAR = 'S';
const char FINISH_CHAR = 'F';

bool haveWinner_ = false;
string Winner_ = "?";
string Message_ = "";
string UserInputString_ = "";
int UserInputInt_ = 0;

struct Player
{
	string Username;
	int NumOfHorse;
	int Win;
	int Tie;

	Player(string username = "", int win = 0, int tie = 0, int NumOfHour = MAX_HORSE_PER_PLAYER)
	{
		Username = username;
		Win = win;
		Tie = tie;
	}
};

Player Accounts_[MAX_ACCOUNT];
int AccountCounter_ = 0;

int OnlineID[MAX_ACCOUNT];
int Online = 0;

void loadAccounts()
{
	ifstream file;
	int counter;
	file.open(ACCOUNTS_FILE);
	if (file.fail()) return;
	file >> counter;
	AccountCounter_ = (counter > 100) ? 100 : counter + 1;
	AccountCounter_++;
	string tmp;
	getline(file, tmp);
	for (int i = 1; i < AccountCounter_; ++i)
	{
		Accounts_[i] = Player();
		getline(file, tmp);
		Accounts_[i].Username = tmp;
		file >> Accounts_[i].Win;
		file >> Accounts_[i].Tie;
	}
	file.close();
}

void saveAccounts()
{
	ofstream file;
	file.open(ACCOUNTS_FILE);
	file << AccountCounter_ << '\n';
	for (int i = 1; i < AccountCounter_; ++i)
	{
		file << Accounts_[i].Username << '\n';
		file << Accounts_[i].Win << ' ' << Accounts_[i].Tie << '\n';
	}
	file.close();
}

bool createAccount(string username = "")
{
	cout << "Start create account\n";
	Accounts_[AccountCounter_] = Player();
	if (username == "")
	{
		cout << "Username? ";
		cin >> username;
	}
	for (int i = 0; i < AccountCounter_; ++i)
	{
		if (Accounts_[i].Username == username)
		{
			cout << "This username is exist! Please try another username!\n"; 
			return false;
		}
	}
	Accounts_[AccountCounter_].Username = username;
	cout << "Registed account for "<< username << "!";
	AccountCounter_++;
	saveAccounts();
	return true;
}

bool removeAccount(string username = "")
{
	if (username == "")
	{
		cout << "Please enter the account's username you want to remove! ";
		cin >> username;
	}	
	if (username == "Robot")
	{
		cout << "You cannot remove this account!";
		return false;
	}
	int id = -1;
	for (int i = 0; i < AccountCounter_; ++i)
	{
		if (Accounts_[i].Username == username)
		{
			id = i;
			break;
		}
	}
	if (id == -1)
	{
		cout << "Cannot find that account! Removing account ended!\n";
		return false;
	}
	else
	{
		cout << "Password? ";
		cin >> UserInputString_;
		AccountCounter_--;
		for (int i = id; i < AccountCounter_; ++i)
		{
			Accounts_[i] = Accounts_[i + 1];
		}
		cout << "Removed!\n";
		saveAccounts();
		return true;
	}
}

int login()
{
	cout << "Please type your username ";
	cin >> UserInputString_;
	for (int i = 0; i < AccountCounter_; ++i)
	{
		if (UserInputString_ == Accounts_[i].Username) return i;
	}
	createAccount(UserInputString_);
	return AccountCounter_ - 1;
}

int getDice(int from = 1, int to = 6)
{
	srand(time(NULL));
	return rand() % (to - from + 1) + from;
}

enum CellEffect
{
	BLOCK,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	START,
	FINISH
};

struct Settings
{

};

struct Horse
{
	int PosX;
	int PosY;
	char DisplayID[MAX_NAME_DISPLAY];
	int PlayerID;

	Horse(int playerid = 0, int order = 0, int posx = 0, int posy = 0)
	{
		PosX = posx;
		PosY = posy;
		PlayerID = playerid;
		string tmp = Accounts_[playerid].Username;
		for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
		{
			DisplayID[i] = (i < tmp.size()) ? tmp[i] : ' ';
		}
		DisplayID[MAX_NAME_DISPLAY - 2] = ' ';
		DisplayID[MAX_NAME_DISPLAY - 1] = order + 1 + '0';
	}
};

struct Point 
{
	int X;
	int Y;
	int HorseID;
	CellEffect Effect;
	int ColorCode;
	int OwnerOrder;
	Point(int x = 0, int y = 0, CellEffect effect = BLOCK,int hourseid = -1, int colorcode = 7, int ownerorder = -1)
	{
		Effect = effect;
		X = x;
		Y = y;
		HorseID = hourseid;
		ColorCode = colorcode;
		OwnerOrder = -1;
	}
};

struct Map
{
	int PlayerID[MAX_PLAYER_PER_MAP];
	Point Grid[MAX_ROW][MAX_COL];
	Horse Horses[MAX_PLAYER_PER_MAP * MAX_HORSE_PER_PLAYER];
	int Size;
	int MaxTurn;
	int MaxPlayer;
	int NumOfPlayer;

	Map(int size = 0, int maxturn = 0, int maxplayer = 1)
	{
		MaxTurn = (maxturn > size*size) ? maxturn : size*size;
		Size = size;
		MaxPlayer = maxplayer;
		NumOfPlayer = 0;
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				Grid[i][j].HorseID = -1;
				Grid[i][j].Effect = RIGHT;
			}
		}

		Grid[0][0].Effect = START;
		Grid[Size - 1][Size - 1].Effect = FINISH;

		for (int i = 0; i < MAX_PLAYER_PER_MAP; ++i)
		{
			PlayerID[i] = -1;
		}
	}

	void addAccount (string username)
	{
		for (int i = 0; i < AccountCounter_; ++i)
		{
			if (Accounts_[i].Username == username)
			{
				addAccount(i);
			}
		}
	}

	void addAccount(int id)
	{
		PlayerID[NumOfPlayer] = id;
		NumOfPlayer++;
	}

	void initialize()
	{
		for (int i = 0; i < MAX_PLAYER_PER_MAP * MAX_HORSE_PER_PLAYER; ++i)
		{
			Horses[i] = Horse(PlayerID[i / MAX_HORSE_PER_PLAYER], i % MAX_HORSE_PER_PLAYER);
		}
	}

	void saveMap(string file_name)
	{
		ofstream file;
		file.open(file_name);
		file << Size << '\n';
		for (int i = 0; i < Size; ++i)
		{
			for (int j = 0; j < Size; ++j)
			{
				file << (int)Grid[i][j].Effect << ' ';
			}
			file << '\n';
		}
		file.close();
	}

	void loadMap(string file_name)
	{
		ifstream file;
		int x;
		file.open(file_name);
		file >> Size;
		for (int i = 0; i < Size; ++i)
		{
			for (int j = 0; j < Size; ++j)
			{
				file >> x;
				Grid[i][j].Effect = (CellEffect)x;
			}
		}
		file.close();
	}

	void moveHorse(const int& ID, const int& desX, const int& desY, const int Count)
	{
		if (isValidPath(ID, Horses[ID].PosX, Horses[ID].PosY, desX, desY, Count))
		{
			if (Horses[Grid[desX][desY].HorseID].DisplayID != Horses[ID].DisplayID)
			{
				if (Grid[desX][desY].HorseID != -1)
				{
					string loser = Horses[Grid[desX][desY].HorseID].DisplayID;
					string winner = Horses[ID].DisplayID;
					Horses[Grid[desX][desY].HorseID].PosX = -1;
					Horses[Grid[desX][desY].HorseID].PosY = -1;
					Message_ += loser + " has been kicked by " + winner + '\n';
				}
				Horses[ID].PosX = desX;
				Horses[ID].PosY = desY;
				Grid[desX][desY].HorseID = ID;
				if (Grid[desX][desY].Effect == FINISH)
				{
					haveWinner_ = true;
				}
			}
		}
		else Message_ += "You cannot go to this point. Please try again or end your turn\n"; 
	}

	bool isValidPath(const int &ID, const int& startX, const int& startY, const int& desX, const int& desY, const int Count)
	{
		if (startX < 0 || startX >= Size || startY < 0 || startY >= Size)
			return false;
		if (Grid[desX][desY].HorseID != -1 &&
			Horses[Grid[desX][desY].HorseID].DisplayID == Horses[ID].DisplayID)
			return false;
		else if (!Count && (startX != desX) && (startY != desY)) 
			return false;
		else if (!Count && ((startX == desX) && (startY == desY))) 
			return true;
		else if (Grid[startX][startY].HorseID != -1) 
			return false;
		switch (Grid[startY][startX].Effect)
		{
		case UP:
			return isValidPath(ID, startX - 1, startY, desX, desY, Count - 1);
		case DOWN:
			return isValidPath(ID, startX + 1, startY, desX, desY, Count - 1);
		case LEFT:
			return isValidPath(ID, startX, startY - 1, desX, desY, Count - 1);
		case RIGHT:
			return isValidPath(ID, startX, startY + 1, desX, desY, Count - 1);
		default:
			return false;
		}
	}

	void printHeader()
	{
		const int SIZE = MAX_NAME_DISPLAY + 2;
		char pattern[SIZE];
		for (int i = 0; i < 4; ++i) cout << ' ';
		for (int i = 0; i < SIZE; ++i)
		{
			pattern[i] = ' ';
		}
		for (int i = 1; i <= Size; ++i)
		{
			pattern[(SIZE / 2) - 1] = i / 10 + '0';
			pattern[(SIZE / 2)] = i % 10 + '0';
			for (int j = 0; j < SIZE; ++j) cout << pattern[j];
		}
		cout << '\n';
	}

	void printRowhorizontalLine()
	{
		for (int i = 0; i < 4; ++i) cout << ' ';
		for (int n = 1; n <= Size; ++n)
		{
			cout << ' ';
			const int DISPLAY_SIZE = MAX_NAME_DISPLAY;
			for (int i = 0; i < DISPLAY_SIZE; ++i)
			{
				cout << '-';
			}
			cout << ' ';
		}
		cout << '\n';
	}

	void printBlankVerticalLine()
	{
		for (int i = 0; i < 4; ++i) cout << ' ';
		for (int n = 1; n <= Size; ++n)
		{
			cout << '|';
			for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
			{
				cout << ' ';
			}
			cout << '|';
		}
		cout << '\n';
	}

	char getCharFromEffect(CellEffect effect)
	{
		switch (effect)
		{
		case BLOCK:
			return BLOCK_CHAR;
		case UP:
			return UP_CHAR;
		case DOWN:
			return DOWN_CHAR;
		case LEFT:
			return LEFT_CHAR;
		case RIGHT:
			return RIGHT_CHAR;
		case START:
			return START_CHAR;
		case FINISH:
			return FINISH_CHAR;
		default:
			return DEFAULT_CHAR;
		}
	}

	void printNamedVerticalLine(int order = 0)
	{
		cout << " " << ((order < 10) ? "0" : "") << (order + 1) << " ";
		char show[MAX_NAME_DISPLAY];
		for (int n = 1; n <= Size; ++n)
		{
			if (Grid[order][n - 1].HorseID == -1 || Grid[order][n - 1].Effect == START)
			{
				for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
				{
					show[i] = ' ';
				}
				show[MID_NAME_DISPLAY] = getCharFromEffect(Grid[order][n - 1].Effect);
			}
			else
			{
				for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
				{
					show[i] = Horses[Grid[order][n - 1].HorseID].DisplayID[i];
				}
			}
			cout << '|';
			for (int i = 0; i < MAX_NAME_DISPLAY; ++i)
			{
				cout << show[i];
			}
			cout << '|';
		}
		cout << '\n';
	}

	void printMap()
	{
		printHeader();
		for (int i = 1; i <= Size; ++i)
		{
			printRowhorizontalLine();
			printBlankVerticalLine();
			printNamedVerticalLine(i - 1);
			printBlankVerticalLine();
			printRowhorizontalLine();
		}
	}
};

int main()
{
	Accounts_[0] = Player(BOT_ACCOUNT);
	AccountCounter_++;
	loadAccounts();
	Map test = Map(5);
	test.loadMap(DEFAULT_FIVE);
	test.addAccount("David");
	test.initialize();
	test.moveHorse(0, 0, 1, 1);
	test.printMap();
	/*cout << "Mode:\n";
	cout << "1. Singleplayer\n";
	cout << "2. Multiplayer\n";
	cout << "Please choose the mode that you want to play! ";
	cin >> UserInputInt_;
	switch (UserInputInt_)
	{
	case 1:
		cout << "We are still developing this mode.\n";
		cout << "Please close the app and try again!\n";
		system("pause");
		return 0;
	case 2:

	default:
		break;
	}*/
	return 0;
}