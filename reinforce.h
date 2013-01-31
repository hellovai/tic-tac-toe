//=================================
// include guard
#ifndef __REINFORCE_H_INCLUDED__
#define __REINFORCE_H_INCLUDED__
//=================================

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Reinforce {
	int counter;
	double learningRate;
	double exploreRate;
	double rate;
	int myTurn;
	
	string fileStore;
	int exploits;
	int explores;
	int nextUpdate;
	
	bool print;
	bool shouldExplore();
	
	int get_explore();
	int get_exploit();
	void updateRates();
	
	double getValue ( string moves , bool retVal );
	void updateFile( string moves, double value, int count );
	string getName( string moves );
	
	Game* game;
	
	public:
		Reinforce( Game* curr_game );
		Reinforce( Game* curr_game, string filename );
		void ReinforceError( );
		void write( );
		void setPrint( bool var );
		int move( );
		void stats( );
		void update( );
		int turn( ) { return myTurn; };
		void log( int wins, int ties, int losses, int messes );
		void change_game ( Game* curr_game ) { game = curr_game; };
};

//=================================
// end guard
#endif
//=================================
