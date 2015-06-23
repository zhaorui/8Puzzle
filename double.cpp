#ifdef LINUX_SYS
#include "state.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//for double end search ,multithread
int retCommonID = 0;//for double end search

/*
Description:	if user choose to use double end search , we should create two thread,thread1 stands for bfs way
		and this function is for inserting state which is found by bfs algorithm
Parameter:	It's the state you want to insert.
Return:		return 0 if insert operation is successful,else return positive value which
		is the id of state,it means we got the common value from two map,now we got a solution
		from double end search!
 */
int InsertForThread1(State& s)
{
	pthread_mutex_lock(&mutex);
	bfsMap.insert(pair<int,State*>(s.id,&s));
	if(dfsMap.find(s.id)!=dfsMap.end()){
		pthread_mutex_unlock(&mutex);
		return s.id;
	}
	pthread_mutex_unlock(&mutex);
	return 0;
}

int InsertForThread2(State& s)
{
	pthread_mutex_lock(&mutex);
	dfsMap.insert(pair<int,State*>(s.id,&s));
	if(bfsMap.find(s.id)!=bfsMap.end()){
		pthread_mutex_unlock(&mutex);
		return s.id;
	}
	pthread_mutex_unlock(&mutex);
	return 0;
}


/*
Description:	This function is used to calculate the next state for bfs or dfs
		method.
 */
State* SolveNextBFSStepForThread1(State& s,int *commonState)
{
	UINT8 result = s.wayToGo();
	State* tempState;

	if((*commonState = InsertForThread1(s))!=0){
		/*got the common state*/
		return NULL;
	}

	StateQueue.pop();

	if(result&State::L){
		tempState = genNextState(s,State::L);
		if(bfsMap.find(tempState->id)==bfsMap.end()){
			/*new state found*/
			StateQueue.push(tempState);
			if((*commonState = InsertForThread1(s))!=0){
				/*got the common state*/
				return NULL;
			}
		}else{
			delete tempState;
		}
	}
	if(result&State::R){
		tempState = genNextState(s,State::R);
		if(bfsMap.find(tempState->id)==bfsMap.end()){
			StateQueue.push(tempState);
			if((*commonState = InsertForThread1(s))!=0){
				/*got the common state*/
				return NULL;
			}
		}else{
			delete tempState;
		}
	}
	if(result&State::U){
		tempState = genNextState(s,State::U);
		if(bfsMap.find(tempState->id)==bfsMap.end()){
			StateQueue.push(tempState);
			if((*commonState = InsertForThread1(s))!=0){
				/*got the common state*/
				return NULL;
			}
		}else{
			delete tempState;
		}
	}
	if(result&State::D){
		tempState = genNextState(s,State::D);
		if(bfsMap.find(tempState->id)==bfsMap.end()){
			StateQueue.push(tempState);
			if((*commonState = InsertForThread1(s))!=0){
				/*got the common state*/
				return NULL;
			}
		}else{
			delete tempState;
		}
	}

	tempState = StateQueue.front();
	return tempState;
}


State* SolveNextDFSStepForThread2(State& s,int *commonState)
{
	UINT8 result = s.wayToGo();
	State* tempState;
	if((*commonState = InsertForThread2(s))!=0){
		/*got the common state*/
		return NULL;
	}

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

typedef struct
{
	State& st;
	stack<State>& s;
}Para;

/*bfs end for "123456789", to */
void* Thread1(void *arg)
{
	Para *p = (Para*)arg;
	State* temp_state = new State(p->st);
	StateQueue.push(temp_state);
	do{
		temp_state = SolveNextBFSStepForThread1(*temp_state,&retCommonID);
	}while(retCommonID==0);
	State* commonst = bfsMap.find(retCommonID)->second;
	do{
		p->s.push(*commonst);
		commonst = commonst->parent;
	}while(commonst!=NULL);
}

/*from*/
void* Thread2(void *arg)
{
	Para *p = (Para*)arg;
	State* temp_state = new State(p->st);
	StateStack.push(temp_state);
	do{
		temp_state = SolveNextDFSStepForThread2(*temp_state,&retCommonID);
		if(temp_state == NULL){
			/*search tho the dead end of some state,then pop out the stack*/
			StateStack.pop();
			temp_state = StateStack.top();
		}else{
			StateStack.push(temp_state);
		}
	}while(retCommonID==0);
	State* commonst = dfsMap.find(retCommonID)->second;
	do{
		p->s.push(*commonst);
		commonst = commonst->parent;
	}while(commonst!=NULL);
}

void doubleEndSearch(State& from,State& to,stack<State>& result)
{
	cleanMap(bfsMap);
	cleanMap(dfsMap);
	cleanStateQueue();

	pthread_t tid1,tid2;
	stack<State> stackForBFS;
	stack<State> stackForDFS;

	Para bfsPara = {to,stackForBFS};
	Para dfsPara = {from,stackForDFS};

	pthread_create(&tid1,NULL,Thread1,&bfsPara);
	pthread_create(&tid2,NULL,Thread2,&dfsPara);
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);

	while(!stackForBFS.empty()){
		result.push(stackForBFS.top());
		stackForBFS.pop();
	}

	reverseStack(stackForDFS);

	while(!stackForDFS.empty()){
		result.push(stackForDFS.top());
		stackForDFS.pop();
	}
	
	cleanMap(bfsMap);
	cleanMap(dfsMap);
	cleanStateQueue();
}
#endif
