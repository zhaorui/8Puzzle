#ifndef STATE_H
#define STATE_H
#include "global.h"

typedef unsigned char UINT8;
class State;

//Maps keeps the reached place
extern map<int,State*>bfsMap;//clean over after getsolution
extern map<int,State*>dfsMap;//clean over after getsolution
extern map<int,State*>allPathMap;//clean over by user at any time
extern queue<State*> StateQueue;//for bfs method,clean over when we got the answer from getsolution
extern stack<State*> StateStack;//for dfs method,clean over when we got the answer from getsolution

/*
Description:	use choiceable method to solve problem of "get path from one state to another state"
Parameter:	const State& : stands for the current state which is expected to be solved
 		stack<State>& : keeps the result path we got from this function
		int option : it's choice which is chosen by user
 			1 --> use "BFS" way to find the answer
			2 --> ..."DFS"...
			3 --> Double end Search , which use both of above
 */
void getSolution(const State& from,const State& to,stack<State>& result,int option=1);

/*
Description:	print the result we already got to the terminal
Parameter:	stack<State>& is the stack which keeps the answer
 */
void printResult(stack<State>& );

/*
Function Name: getAllBestPath
Description;	Use bfs method,and use "123456789" state as dst,we can easily fill
		the map which contain all the state that original state can reach
		each element in the map have best path to the target state
Parameter:	dst : is the destination of the game
		allPath : include all case that have the best answer to the target

Function Name: getNextBestState
Description:	Presumably we already fill the best way map,then use this function we
        can easily got what we want(the best path),only if we have the id number
        of the state
Parameter:	the id of the state
return :	if the id exist in map then return the pointer
        else return NULL
 */
void getAllBestPath(const State& dst);
State* getNextBestState(int idofstate);
void getBestWayToStack(int stateid,stack<State>& result);


void cleanMap(map<int, State *> &mymap);
void cleanStateQueue();
void cleanStack(stack<State>& s);
void reverseStack(stack<State>& st);

class State
{
	public:
		State(int x=3,bool isShuffle=true);
		State(int x,int number);
		void shuffleState();
		UINT8 wayToGo();
		bool moveLeft();
		bool moveRight();
		bool moveUp();
		bool moveDown();
        bool isGoodOrder()const;
        bool isReachable() const;
        static bool isAcceptable(int x,int id) ;//is the id can be a state id
	public:
		enum Way {U = 0x01,D = 0x02,R = 0x04,L = 0x08,Z = 0x00};
		int N;
		int id;/*The id value of state*/
		vector<int> vect;
		State* parent;
		State::Way moveHistory;
		int step;
    private:
		int completeState();

};

State* genNextState(State& s,State::Way move);//How to declare State::Way outside
State::Way oppositeWay(State::Way);
#endif
