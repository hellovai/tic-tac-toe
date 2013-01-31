#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib> 

#include "game.h"
#include "reinforce.h"


using namespace std;

int get_artificial_move( );

void testbed(Reinforce * agent_play);

int gameCounter, wins = 0, losses = 0, ties = 0, MAXGAMES = 2000, messes = 0, TESTGAME = 1000;
Game game;

bool gamePrint = false;
bool agentPrint = false;
bool enableHuman = false;
bool testbedtrack = true;

Reinforce agent1 (&game, "first");
Reinforce agent2 (&game, "second");

int main(int argc, char* argv[]){
	if(argc == 2)
		MAXGAMES = atoi(argv[1]);
	
	srand ( time(NULL) );
	if(enableHuman) gamePrint = true;
	agent1.setPrint(agentPrint);
	agent2.setPrint(agentPrint);
	
    for(gameCounter = 0; gameCounter < MAXGAMES ; gameCounter++ ) {
    	game.reset();
    	cout<<"STARTING GAME: "<<gameCounter<<endl;
    	if(gamePrint) game.Print();
		bool play = true;
		int x, val;
		while(play) {
			if(game.get_player() == agent1.turn() )
				x = agent1.move();
			else //if(game.get_player() == agent2.turn() )
				x = agent2.move();
				
			val = game.Move(x, gamePrint);
			
			while(val == -1 && enableHuman) {
				cout<<"Invalid move! Player "<<game.get_player()<<" move: ";
				cin>>x;
				if( x == 100 ) {
					enableHuman = false;
					gamePrint = false;
					x = game.fake_move();
				}
				val = game.Move(x, gamePrint);
			}
			
			if(	val == -1 ) {
					cout<<"WTH HOW DID THIS HAPPEN? "<<endl;
					cout<<"Wanted to move: "<<x<<endl;
					game.Print();
					exit(0);				
			}

			if(val != 0)
				play = false;
			
			//we update the file if the agent made a move or if win / lose condition
			if( game.get_player() != agent1.turn() || game.status() )
				agent1.update();
			if( game.get_player() != agent2.turn() || game.status() )
				agent2.update();
			
		}
		
		if (val == agent1.turn() )
			wins++;
		else if (val == 3)
			ties++;
		else if (val == -1)
			messes++;
		else
			losses++;
				
		cout<<"\tWinner was: "<<val<<endl;
		if(gameCounter % 10 == 0 && testbedtrack) {
			testbed(&agent1);
			testbed(&agent2);
		}
	}
	
	cout<<endl<<"========STATS========="<<endl;
	cout<<"Total Games : "<<wins+losses+messes+ties<<endl;
	cout<<"Wins Player 1: "<<wins<<endl;
	cout<<"Wins Player 2: "<<losses<<endl;
	cout<<"Ties: "<<ties<<endl;
	cout<<"Mess Ups: "<<messes<<endl;
	agent1.stats();
	agent1.write();
	agent2.stats();
	agent2.write();
}

void testbed(Reinforce * agent_play) {
	Game temp;
	agent_play->change_game(&temp);
	int t_wins = 0, t_losses = 0, t_ties = 0, t_messes = 0;
	for(int test_game = 0; test_game<=TESTGAME; test_game++) {
		temp.reset();
		bool play = true;
		int x, val;
		while(play) {
			if( temp.get_player() == agent_play->turn() ) {
				x = agent_play->move();
			} else {
				x = temp.fake_move();
			}
			
			val = temp.Move(x, false);
			
			if(val != 0)
				play = false;
			
		}
		
		if ( val == agent_play->turn() )
			t_wins++;
		else if (val == 3)
			t_ties++;
		else if (val == -1) {
			t_messes++;
			sleep(1);
		}
		else
			t_losses++;
	}
	
	
	agent_play->log(t_wins, t_ties, t_losses, t_messes);
	agent_play->change_game(&game);
}
