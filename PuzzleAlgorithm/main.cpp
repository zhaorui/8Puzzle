#include "global.h"
#include "state.h"
#include "double.h"

int main(void)
{
	srand((unsigned)time(NULL));
	State *from = new State(3);
	State *to = new State(3,false);//standard state of puzzle
	stack<State> bfsResult;
	stack<State> dfsResult;
	stack<State> quickResult;
	stack<State> doubleEndResult;
//	getSolution(*from,*to,bfsResult,1);
//	getSolution(*from,*to,dfsResult,2);
	doubleEndSearch(*from,*to,doubleEndResult);

//	printResult(bfsResult);
//	printResult(dfsResult);
	cout<<"above done"<<endl;
	printResult(doubleEndResult);
	getchar();
	getAllBestPath(*to);
	while(1){
		int num;
		State *st;
		cin>>num;
		getBestWayToStack(num,quickResult);
		cout<<" - - - - - - -"<<endl;
		/*
		 display from stack
		 */
		while(!quickResult.empty()){
			State s = quickResult.top();
			quickResult.pop();
			cout<<s.id<<endl;
		}
	}

	return 0;
}
