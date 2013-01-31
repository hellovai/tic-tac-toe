#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

#include "game.h"
#include "reinforce.h"

using namespace std;


/**********************************************************************************
constructors
**********************************************************************************/
void Reinforce::ReinforceError() {
	counter = 0;
	learningRate = 0.5;
	exploreRate = 100.0;
	print = false;
	exploits = 0;
	explores = 0;
	nextUpdate = 100;
	rate = 0.85;
	myTurn = 2;
}

Reinforce::Reinforce(Game* curr_game) {
	game = curr_game;
	counter = 0;
	learningRate = 0.5;
	exploreRate = 100.0;
	print = false;
	fileStore = "config.dat";
	exploits = 0;
	explores = 0;
	nextUpdate = 100;
	rate = 0.85;
	myTurn = 2;
}

Reinforce::Reinforce(Game* curr_game, string filename) {
	game = curr_game;
	fileStore = filename;
	filename.append(".cfg");
	ifstream myfile(filename.c_str());
	if(myfile.is_open()) {
		string temp;
		myfile >> temp >> fileStore >> temp >> print >> temp >> counter >> temp >> learningRate  >> temp >> exploreRate >> temp >> exploits >> temp >> explores >> temp >> nextUpdate >> temp >> rate >> temp >> myTurn;
	} else {
		cout<<"Unable to open file: "<<filename<<"\nSetting Default values..."<<endl;
		ReinforceError();
		sleep(1);
	}
}

/**********************************************************************************
Public Functions
**********************************************************************************/
void Reinforce::setPrint( bool var ) {
	print = var;
}

void Reinforce::write( ) {
	string filename = fileStore;
	filename.append(".cfg");
	ofstream myfile (filename.c_str());
	myfile<<"File "<<fileStore<<endl;
	myfile<<"Print "<<print<<endl;
	myfile<<"MovesMade "<<counter<<endl;
	myfile<<"LearningRate "<<learningRate<<endl;
	myfile<<"ExploreRate "<<exploreRate<<endl;
	myfile<<"Exploits "<<exploits<<endl;
	myfile<<"Explores "<<explores<<endl;
	myfile<<"NextUpdate "<<nextUpdate<<endl;
	myfile<<"Rate "<<rate<<endl;
	myfile<<"Turn "<<myTurn<<endl;
	cout<<"Wrote configuration to "<<fileStore<<endl;
}

void Reinforce::update( ) {
	string moveList = game -> moveList();
	if ( moveList.length() > 2) {
		double newValue = getValue( moveList, true);
		
		string moveListOld = moveList;
		if( (myTurn == 2 && moveList.length() % 2 == 0) || (myTurn == 1 && !(game->status() && moveList.length() % 2 == 0) ) ) {
			moveListOld.erase(moveListOld.end()-1);
		}
		
		moveListOld.erase(moveListOld.end() -1);
		double oldValue = getValue(moveListOld, true);
		int	oldCount = (int) getValue(moveListOld, false);
		oldCount++;
		double updateValue = (double) 1.0/oldCount;
		updateValue = updateValue*( newValue - oldValue );
		updateValue += oldValue;
		updateFile(moveListOld, updateValue, oldCount);
			
		if(print) cout<<"Updating "<<moveListOld<<" to: ("<<newValue<<" - "<<oldValue<<")*"<<(double) 1 / oldCount<<" = "<<updateValue<<endl;
	} else if (print) {
		cout<<"We don't have any moves prior"<<endl;;
	}
}

void Reinforce::stats( ) {
	cout<<"Moves Made: "<< counter<<endl;
	cout<<"Learning Rate: "<<learningRate<<endl;
	cout<<"Explore Chance: "<<exploreRate<<endl;
	cout<<"Exploits: "<<exploits<<endl;
	cout<<"Explores: "<<explores<<endl;
}

int Reinforce::move() {
	int shouldMove;
	if( shouldExplore() )	
		shouldMove = get_explore();
	else 
		shouldMove = get_exploit();
	
	counter++;
	updateRates();
	return shouldMove;
}


/**********************************************************************************
Private Functions
**********************************************************************************/
bool Reinforce::shouldExplore() {
	double choose = (double)(rand() % 100);
	return (choose < exploreRate);
}

//returns the move with the highest value, if multiple moves are there, then just picks one randomly
int Reinforce::get_exploit ( ) {
	if(print) cout<<"Exploiting!"<<endl;
	
	exploits++;
	vector<int> possible = game -> valid_moves();
	double* array = new double[possible.size()];
	string moveList = game -> moveList();
	for( int i = 0; i < possible.size(); i++ ) {
		moveList.append( 1, ((char) possible[i] + '0') );
		array[i] = getValue(moveList, true);
		moveList.erase(moveList.end()-1);
	}
	
	double max = array[0];
	vector<int> index;
	index.push_back(possible[0]);
	
	for(int i =1; i<possible.size(); i++ )
		if( array[i] > max ) {
			index.clear();
			index.push_back(possible[i]);
			max = array[i];
		} else if ( array[i] == max ) {
			index.push_back(possible[i]);
		}
		
	return index[rand() % index.size()];
}

//picks a random move from the move set
int Reinforce::get_explore ( ) {
	if(print) cout<<"Exploring!"<<endl;
	
	explores++;
	vector<int> possible = game->valid_moves();
	
	//we ignore the code below it and define exploring as picking a random move instead of a move except the best moves
	return possible[rand() % possible.size()];
	
// NOT REGARDED for now, could implement soft max after getting the values though
//	double* array = new double[possible.size()];
//	string moveList = game->moveList();
//	for( int i = 0; i < possible.size(); i++ ) {
//		moveList.append( 1, ((char) possible[i] + '0') );
//		array[i] = getValue(moveList, true);
//		moveList.erase(moveList.end() - 1);
//	}
//	
//	double max = array[0];
//	vector<int> index;
//	index.push_back(possible[0]);
//	vector<int> nomax;
//	
//	for(int i =1; i<possible.size(); i++ )
//		if( array[i] > max ) {
//			nomax.insert(nomax.end(), index.begin(), index.end());
//			index.clear();
//			index.push_back(possible[i]);
//			max = array[i];
//		} else if ( array[i] == max ) {
//			index.push_back(possible[i]);
//		} else {
//			nomax.push_back(possible[i]);
//		}
//	
//	if(nomax.size() > 0)
//		return nomax[rand() % nomax.size()];
//	else
//		return index[rand() % index.size()];
}

void Reinforce::updateRates( ) {
	if( (counter % nextUpdate == 0) ||  (counter % (nextUpdate / 1) == 0) )  {
		game -> updateProb();
		learningRate = learningRate * rate;
		exploreRate =  exploreRate * rate;
		if(counter % nextUpdate == 0)
			nextUpdate *= 10;
	}
	
	if( print ) {
		cout<<"Moves Made: "<<counter<<endl;
		cout<<"Learning Rate: "<<learningRate<<"\tExplore Rate: "<<exploreRate<<endl<<endl;
	}
}

void Reinforce::log(int wins, int ties, int losses, int messes) {
	string filename = fileStore;
	filename.append(".log");
	ofstream myfile;
	myfile.open(filename.c_str(), ios::app);
	myfile<<wins<<" "<<ties<<" "<<losses<<" "<<messes<<endl;
	myfile.close();
}

double Reinforce::getValue ( string moves , bool retVal ) {
	ifstream myfile(getName(moves).c_str());
	if(myfile.is_open() ) {
		double val;
		int occur;
		myfile >> val >> occur;
		if(retVal) return val;
		else return occur;
	} else {
		cout<<"Found Missing Case: "<<moves<<endl;
		exit(0);
	}
}

void Reinforce::updateFile( string moves, double value, int count ) {
	string filename = getName(moves);
	ofstream myfile (filename.c_str());
	if(myfile.is_open()) {
		myfile<<value<<" "<<count<<endl;
	} else {
		cout<<"Found Missing Case: "<<moves<<endl;
		exit(0);
	}
}

string Reinforce::getName( string moves ) {
	int sizeArr = ((int) moves.length()/2) + moves.length() % 2;
	char* one = new char[sizeArr];
	char* two = new char[sizeArr - moves.length() % 2];
	int oneCount = 0, twoCount = 0;
	for( int i = 0; i < moves.length(); i++ )
		if(i % 2 == 0 )
			one[oneCount++] = moves[i];
		else
			two[twoCount++] = moves[i];
	
	sort(one, one + sizeArr );
	sort(two, two + sizeArr - moves.length() % 2 );	

	string filename = "./";
	filename.append(fileStore);
	filename.append("/");
	for(int i=0; i<sizeArr; i++)
		filename.append(1, one[i]);
	
	filename.append("_");

	for(int i=0; i<sizeArr - moves.length() % 2 ; i++)
		filename.append(1, two[i]);	

	filename.append(".dat");
	return filename;
}

