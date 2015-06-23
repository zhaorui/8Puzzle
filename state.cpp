#include "global.h"
#include "state.h"

//Maps keeps the reached place
map<int,State*>bfsMap;//clean over after getsolution
map<int,State*>dfsMap;//clean over after getsolution
map<int,State*>allPathMap;//clean over by user at any time

queue<State*> StateQueue;//for bfs method,clean over when we got the answer from getsolution
stack<State*> StateStack;//for dfs method,clean over when we got the answer from getsolution


/*
 State member function 
 cause it have complete State,so it only works for
 state which is below or equal 3
 */
State::State(int x,bool isShuffle)
:vect(x*x),N(x),parent(NULL),step(0),moveHistory(State::Z)
{
    for(unsigned int i=0;i<vect.size();i++)
		vect[i] = i+1;
	completeState();
	if(isShuffle)
		shuffleState();
}

State::State(int x,int number)
	:vect(x*x),N(x),id(number),parent(NULL),step(0),moveHistory(State::Z)
{
    int deFactor = (int)pow(10.0,N*N-1);
	for(int i=0;deFactor!=0;deFactor/=10){
		vect[i] = id/deFactor%10;
		i++;
	}
}

bool State::isGoodOrder() const
{
    for(unsigned int i=0;i<vect.size()-1;i++){
		if(vect[i]>=vect[i+1])
			return false;
	}
	return true;
}

void State::shuffleState()
{
	vector<int>::iterator it_end = vect.end();
	it_end--;
	while(1){
		random_shuffle(vect.begin(),it_end);
		if(isReachable()){
			completeState();
			if(!isGoodOrder())
				break;
			
		}
	}
	
}

/*
Description:	This function calculate the reverse few of array to decide
		could the state reach the destination
Return:		if it could return true,otherwise return false
 */
bool State::isReachable() const
{
	int result = 0;
	for(int i=0;i<N*N-1;i++){
		for(int j=0;j<i;j++){
			if(vect[j]>vect[i])
				result++;
		}
	}
	return (result%2)==0;
}

bool State::isAcceptable(int x, int id)
{
    int base = (int)pow(10.0,x*x-1);
    vector<bool> record(x*x-1,false);
    int pos;

    if(id/base==0)
        return false;
    /*check the first 8 position for repetion*/
    for(int deFactor = base,i=0;i<8;deFactor/=10,i++){
        pos = id/deFactor%10;
        if(pos!=0&&pos!=9){
            /*0 and 9 can't appear in first 8 pos*/
            if(record[pos-1]==false)
                record[pos-1] = true;
            else
                return false;
            //record[pos-1]==false?record[pos-1]=true:return false;//record repeat return false;
        }else{
            return false;
        }
    }
    return true;
}

int State::completeState()
{
	int result = 0;
	for(int i=0;i<N*N;i++){
		result*=10;
		result+=vect[i];
	}
	id = result;
	return id;
}

UINT8 State::wayToGo()
{
	UINT8 p = 0x00;
	int pos = vect[N*N-1];
	if(pos<=N*N-N)
		p|=State::U;
	if(pos>N)
		p|=State::D;
	if(pos%N!=1)
		p|=State::R;
	if(pos%N!=0)
		p|=State::L;
	return p;
}

bool State::moveLeft()
{
	UINT8 c = wayToGo();
	if(c&State::L){
		vect[N*N-1]++;
		id++;
		return true;
	}else
		return false;
}

bool State::moveRight()
{
	UINT8 c = wayToGo();
	if(c&State::R){
		vect[N*N-1]--;
		id--;
		return true;
	}else
		return false;
}

bool State::moveUp()
{
	UINT8 c = wayToGo();
	if(c&State::U){
		int pos = vect[N*N-1];
		for(int offset = N-2,k=0;k<N-1;k++)
			swap(vect[pos+offset-k],vect[pos+offset-k-1]);
		vect[N*N-1] = pos+N;
		completeState();
		return true;
	}else
		return false;
}

bool State::moveDown()
{
	UINT8 c = wayToGo();
	if(c&State::D){
		int pos = vect[N*N-1];
		for(int offset = -N-1,k=0;k<N-1;k++)
			swap(vect[pos+offset+k],vect[pos+offset+k+1]);
		vect[N*N-1] = pos-N;
		completeState();
		return true;
	}
	else
		return false;
}

//<----------from this line down to the end is not the member function from State class------------>


void printResult(stack<State>& s)
{
	State* temp = new State(3);
	printf("%-.12s%-.8s%-.4s\n","state","count","move");
	while(!s.empty()){
		*temp = s.top();
		cout<<temp->id<<" : "<<temp->step<<" : ";
		cout<<temp->moveHistory<<endl;
		s.pop();
	}
	delete temp;
}

/*
Description:	clean up the state which is kept on specified map
 */
void cleanMap(map<int,State*>& mymap)
{	
    for(map<int,State*>::iterator it = mymap.begin();
			it!=mymap.end();
			it++)
	delete it->second;
	mymap.clear();
}

void cleanStateQueue()
{
	while(!StateQueue.empty())
		StateQueue.pop();
}

void cleanStack(stack<State>& s)
{
	while(!s.empty())
		s.pop();
}

void reverseStack(stack<State>& st)
{
	stack<State> tmpStack = st;
	State s;
	cleanStack(st);
	while(!tmpStack.empty()){
		s = tmpStack.top();
		tmpStack.pop();
		st.push(s);
	}
}

/*
Description:	This function generate and malloc space for the next state
		which is specified by the current state and the next movement
Return:		It return the next State,which should be delete by user own self
 */
State* genNextState(State& s,State::Way move)
{	
	State *ret = new State(s);
	ret->parent = &s;
	ret->step++;
	switch(move){
		case State::L:
			ret->moveLeft();
			ret->moveHistory = State::L;
			break;
		case State::R:
			ret->moveRight();
			ret->moveHistory = State::R;
			break;
		case State::U:
			ret->moveUp();
			ret->moveHistory = State::U;
			break;
		case State::D:
			ret->moveDown();
			ret->moveHistory = State::D;
			break;
		defalut:return NULL;
	}
	return ret;
}
/*
 solve next bfs step for double thread
 */
State* SolveNextBFSStep(State& s)
{
	UINT8 result = s.wayToGo();
	State* tempState;

	bfsMap.insert(pair<int,State*>(s.id,&s));

	StateQueue.pop();
	
	if(result&State::L){
		tempState = genNextState(s,State::L);
		if(bfsMap.find(tempState->id)==bfsMap.end()){
			/*new state found*/
			StateQueue.push(tempState);
			bfsMap.insert(pair<int,State*>(tempState->id,tempState));
		}else{
			delete tempState;
		}
	}
	if(result&State::R){
		tempState = genNextState(s,State::R);
		if(bfsMap.find(tempState->id)==bfsMap.end()){
			StateQueue.push(tempState);
			bfsMap.insert(pair<int,State*>(tempState->id,tempState));
		}else{
			delete tempState;
		}
	}
	if(result&State::U){
		tempState = genNextState(s,State::U);
		if(bfsMap.find(tempState->id)==bfsMap.end()){
			StateQueue.push(tempState);
			bfsMap.insert(pair<int,State*>(tempState->id,tempState));
		}else{
			delete tempState;
		}
	}
	if(result&State::D){
		tempState = genNextState(s,State::D);
		if(bfsMap.find(tempState->id)==bfsMap.end()){
			StateQueue.push(tempState);
			bfsMap.insert(pair<int,State*>(tempState->id,tempState));
		}else{
			delete tempState;
		}
	}
	if(StateQueue.empty())
		return NULL;
	else{
		tempState = StateQueue.front();
		return tempState;
	}
}


State* SolveNextDFSStep(State& s)
{
	UINT8 result = s.wayToGo();
	State* tempState;
	dfsMap.insert(pair<int,State*>(s.id,&s));

	if(result&State::L){
		tempState = genNextState(s,State::L);
		if(dfsMap.find(tempState->id) == dfsMap.end()){
			/*new State found*/
			return tempState;
		}else{
			delete tempState;
		}

	}
	if(result&State::R){
		tempState = genNextState(s,State::R);
		if(dfsMap.find(tempState->id) == dfsMap.end()){
			/*new State found*/
			return tempState;
		}else{
			delete tempState;
		}
	}
	if(result&State::U){
		tempState = genNextState(s,State::U);
		if(dfsMap.find(tempState->id) == dfsMap.end()){
			/*new State found*/
			return tempState;
		}else{
			delete tempState;
		}
	}
	if(result&State::D){
		tempState = genNextState(s,State::D);
		if(dfsMap.find(tempState->id) == dfsMap.end()){
			/*new State found*/
			return tempState;
		}else{
			delete tempState;
		}
	}
	return NULL;
}

void getSolution(const State& from,const State& to,stack<State>& result,int option)
{
	/*make sure map is empty*/
	cleanMap(bfsMap);
	cleanStateQueue();
	cleanMap(dfsMap);

	State* temp_state = new State(from);
	switch(option){
		case 1:/*bfs method*/
			StateQueue.push(temp_state);
			do{
				temp_state = SolveNextBFSStep(*temp_state);
			}while(temp_state->id!=to.id);
			do{
				result.push(*temp_state);
				temp_state = temp_state->parent;
			}while(temp_state!=NULL);
			cleanMap(bfsMap);
			cleanStateQueue();
			break;
		case 2:/*dfs method*/
			StateStack.push(temp_state);
			do{
				temp_state = SolveNextDFSStep(*temp_state);
				if(temp_state == NULL){
					/*search tho the dead end of some state,then pop out the stack*/
					StateStack.pop();
					temp_state = StateStack.top();
				}else{
					StateStack.push(temp_state);
				}
			}while(temp_state->id!=to.id);
			do{
				//temp_state = StateStack.top();
				result.push(*temp_state);
				temp_state = temp_state->parent;
				//StateStack.pop();
			//}while(!StateStack.empty());
			}while(temp_state!=NULL);
			cleanMap(dfsMap);
			break;
		default:
			break;
	}
}

void getAllBestPath(const State& dst)
{
	cleanMap(bfsMap);
	State* temp_state = new State(dst);
	StateQueue.push(temp_state);
	do{
		temp_state = SolveNextBFSStep(*temp_state);
	}while(!StateQueue.empty());
	allPathMap = bfsMap;
	bfsMap.clear();
}

State* getNextBestState(int stateid)
{
    map<int,State*>::iterator it;
	State *ret;
	it = allPathMap.find(stateid);
	if(it == allPathMap.end()){
	/*
	 process the condition that state dont exist in allPathMap
	 */
		return NULL;
	}
	State *p = it->second;
	ret = p->parent;
	return ret;
}

/*
Hint: Every time we invoke this function should make sure there are value in allPathMap,
other wise invoking doesn't make any sense :)
*/
void getBestWayToStack(int stateid,stack<State>& result)
{
    map<int,State*>::iterator it;
	State *ret;
	it = allPathMap.find(stateid);
	if(it == allPathMap.end()){
        /*This situation may be happend when we input a state that never would reach,
        so we cant find in the allPathMap*/
        cerr<<"Sorry,the stateid doesn't exist! "<<"state id:"<<stateid<<endl;
        return ;
	}
	State *p = it->second;
	while(p!=NULL){
		cout<<p->id<<endl;
		result.push(*p);
		p = p->parent;
	}
	reverseStack(result);
    return ;
}

State::Way oppositeWay(State::Way w)
{
	if(w == State::U)
		w=State::D;
	else if(w == State::D)
		w=State::U;
	else if(w == State::R)
		w=State::L;
	else if(w == State::L)
		w=State::R;
	else
		;
	return w;
}
