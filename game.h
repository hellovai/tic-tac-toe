//=================================
// include guard
#ifndef __GAME_H_INCLUDED__
#define __GAME_H_INCLUDED__
//=================================

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Game {
	int board[3][3];
	int curr_player;
	int moves;
	string movesList;
	//bool check_state();
	bool myStatus;
	int checkIdeal ( int* moveId );
	int checkCase ( int a, int b, int c, int d );
	int* MoveId ( int moveId );
	double goodProb;
	
	public:
		Game();
		int Move(int block, bool print_board);
		void Print();
		int get_player();
		vector<int> valid_moves();
		void reset();
		string moveList() {return movesList; };
		bool status() {return myStatus; }
		int fake_move();
		void updateProb() { if ( goodProb < 100 ) goodProb *= 1.25; else goodProb = 100; }
};

//=================================
// end guard
#endif
//=================================
