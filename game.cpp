#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "game.h"

using namespace std;

Game::Game() {
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			board[i][j] = 0;
	
	curr_player = 1;
	moves = 0;
	movesList = "";
	myStatus = false;
	goodProb = 0;
}

void Game::reset() {
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			board[i][j] = 0;
	
	curr_player = 1;
	moves = 0;
	movesList = "";
	myStatus = false;
	goodProb = 0;
}

void Game::Print() {
	for(int i=0; i<2; i++) {
		for(int j=0; j<2; j++)
			cout<<board[i][j]<<"|";
		cout<<board[i][2]<<endl<<"- - -"<<endl;
	}
	for(int j=0; j<2; j++)
		cout<<board[2][j]<<"|";
	cout<<board[2][2]<<endl<<endl;
}


//returns 0: continue, -1: invalid move, else: player that won
int Game::Move(int block, bool print_board) {
	int i, j;
	switch(block) {
		case 0:
			i=0; j=0;
			break;
		case 1:
			i=0; j=1;
			break;
		case 2:
			i=0; j=2;
			break;
		case 3:
			i=1; j=0;
			break;
		case 4:
			i=1; j=1;
			break;
		case 5:
			i=1; j=2;
			break;
		case 6:
			i=2; j=0;
			break;
		case 7:
			i=2; j=1;
			break;
		case 8:
			i=2; j=2;
			break;
		default:
			return -1;
	}
	
	if(board[i][j] != 0)
		return -1;
	
	board[i][j] = curr_player;
	moves++;
	movesList.append(1, ((char) block + '0') );
		
	if(curr_player == 1)
		curr_player = 2;
	else
		curr_player = 1;
	
			
	if(print_board)
		Print();
	
	//temp set status to true
	myStatus = true;
	
	//check if victory occured
	for(int i = 0; i<3; i++)
		if( board[i][0] != 0 && board[i][0] == board[i][1] && board[i][0] == board[i][2] )
			return board[i][0];
		else if (  board[0][i] != 0 && board[0][i] == board[1][i] && board[0][i] == board[2][i] )
			return board[0][i];
	
	if(board[1][1] != 0 && ((board[0][0] == board[2][2] && board[0][0] == board[1][1]) || (board[0][2] == board[2][0] && board[0][2] == board[1][1])) )
		return board[1][1];

	if(moves == 9 ){
		return 3;
	}
	
	//game is still not over
	myStatus = false;
	return 0;
}

int Game::get_player() {
	return curr_player;
}

vector<int> Game::valid_moves() {
	vector<int> array;
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			if(board[i][j] == 0)
				array.push_back(i*3+j);
	
	return array;
}

int Game::fake_move () {
	vector<int> validMove = valid_moves();
	vector<int> win, block;
	
	for( int i=0; i<validMove.size(); i++ ) {
		int check = checkIdeal(MoveId(validMove[i]));
		switch(check) {
			case 1:
				win.push_back(validMove[i]);
				break;
			case 2:
				block.push_back(validMove[i]);
				break;
		}
	}
	
	if(goodProb == 0)
 		return validMove[rand() % validMove.size()];
	
	double chance = (double) (rand() % 100);
	if ( win.size() > 0 && chance < goodProb )
		return win[rand() % win.size()];
	else if (block.size() > 0 && chance < goodProb )
		return block[rand() % block.size()];
	else
		return validMove[rand() % validMove.size()];
}

int Game::checkIdeal ( int* moveId ) {
	int n = moveId[0], m = moveId[1];
	int i = 0, j = 0, k = 0, l = 0;
	if( n == 0 && m == 0) {
		i = checkCase(0,1,0,2);
		j = checkCase(1,0,2,0);
		k = checkCase(1,1,2,2);
	} else if( n == 0 && m == 1) {
		i = checkCase(0,0,2,0);
		j = checkCase(1,1,2,1);
	} else if( n == 0 && m == 2) {
		i = checkCase(0,0,0,1);
		j = checkCase(1,2,2,2);
		k = checkCase(1,1,2,0);
	} else if( n == 1 && m == 0) {
		i = checkCase(0,0,2,0);
		j = checkCase(1,1,1,2);
	} else if( n == 1 && m == 1) {
		i = checkCase(0,1,2,1);
		j = checkCase(1,0,1,2);
		k = checkCase(0,0,2,2);
		l = checkCase(0,2,2,0);
	} else if( n == 1 && m == 2) {
		i = checkCase(0,2,2,2);
		j = checkCase(1,0,1,1);
	} else if( n == 2 && m == 0) {
		i = checkCase(0,0,1,0);
		j = checkCase(2,1,2,2);
		k = checkCase(1,1,0,2);
	} else if( n == 2 && m == 1) {
		i = checkCase(2,0,2,2);
		j = checkCase(0,1,1,1);
	} else if( n == 2 && m == 2) {
		i = checkCase(0,2,1,2);
		j = checkCase(2,0,2,1);
		k = checkCase(0,0,1,1);
	}
	
	if( i == 1 || j == 1 || k == 1 || l == 1 )
		return 1;
	else if ( i == 2 || j == 2 || k == 2 || l == 2)
		return 2;
	
	return 0;
}

int Game::checkCase ( int a, int b, int c, int d ) {
	if( board[a][b] != 0 && board[a][b] == board[c][d]) {
		if( board[a][b] != curr_player)
			return 2;
		else
			return 1;
	}
	
	return 0;
}

int* Game::MoveId ( int moveId ) {
	int moveLoc[2];
	switch(moveId) {
		case 0:
			moveLoc[0] = 0; 
			moveLoc[1] = 0;
			break;
		case 1:
			moveLoc[0]=0; 
			moveLoc[1]=1;
			break;
		case 2:
			moveLoc[0]=0; 
			moveLoc[1]=2;
			break;
		case 3:
			moveLoc[0]=1; 
			moveLoc[1]=0;
			break;
		case 4:
			moveLoc[0]=1; 
			moveLoc[1]=1;
			break;
		case 5:
			moveLoc[0]=1; 
			moveLoc[1]=2;
			break;
		case 6:
			moveLoc[0]=2; 
			moveLoc[1]=0;
			break;
		case 7:
			moveLoc[0]=2; 
			moveLoc[1]=1;
			break;
		case 8:
			moveLoc[0]=2; 
			moveLoc[1]=2;
			break;
	}
	
	return moveLoc;
}
