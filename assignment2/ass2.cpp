/*
student name: Weicheng Yin
student no: 5009819
this is the file for csci203 ass2
*/


#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
/*****golbal variables*****/

//this is the struct to simulate the actual event
//the size is depending on the file i.e if there r 10 server, 
//the size is also 10
struct Event{
	//Customer customer;		//the customer the server is serving
	double eventTime;			//the happen time for the current event
	int serverNum;				//the index of the server number i.e number 1 indicate server 1 
	double serviceTime;
};

Event event[11];			//the heap for the event, assume there's 10 servers

//the queue store the customer's service time
//and this is the queue for the multiple server and single queue
double queue[500];			
double enqueueTime[500];		//record the customer's enqueue time
int id[11];            //this is the id array used to track who is doing what
int totalServer  = 0;			//the total server number according to the file
int busyServerNum = 0;			//record the busy server num
/***** the following global variable is for the STAT *****/
int totalPeopleServed = 0;		//total people served
double currentTime = 0;
double totalServiceTime;		//total cost of service time
double queue_wait_time = 0;		//cusomer wait in the queue
int queueSize = 0;
double serviceTime = 0;
double nextServiceTime = 0;
double totalQueueTime = 0;
double maxQueueTime = 0;
double lastEventTime = 0;
int maxQueueLength = 0;
int totalMultiQueueLength = 0;
double queueTime[500];
//this is the two dimensional array for the second simulation
double multiQueue[11][500];
double enterMuitQueueTime[11][500];	//record the customer's enqueue time
//this is the array to store the size for each queue in the second simulation
int multiQueueSize[11];

void swap(Event &e1, Event &e2){
	//this functin is ised for swap
	Event tmp = e2;
	e2 = e1;
	e1 = tmp;
}

void siftUp(){
	int child = busyServerNum;
	while(child > 0){//hasnt reach the root
		if(event[(child - 1) / 2].eventTime > event[child].eventTime){
			swap(event[child], event[(child - 1) / 2]);
			child /= 2;
		}
		else	//already be the root of the heap, dont need siftup
			return;
	}
}

void siftDown(){
	if(busyServerNum == 0){//the heap only has one element 
		return;
	}

	int i = 0;
	while(i < busyServerNum){
		int children = i * 2 + 1;
		if(children > busyServerNum)		//the children is overflow bound
			return;			
		if((children + 1) <= busyServerNum){//compare two children of the node
			if(event[children].eventTime > event[children + 1].eventTime)
				children += 1;
		}

		if(event[i].eventTime > event[children].eventTime){
			swap(event[children], event[i]);
		}
		i = children;
	}
}

void enqueue(double serTime, int simulationRound){
	if(simulationRound == 1){//the first simulation, only single queue
		queue[queueSize] = serTime;
		enqueueTime[queueSize] = currentTime;
		// cout << "AT THIS TIME, THE TIME INTO QUEUE: " << enqueueTime[queueSize] << endl;
		queueSize++;
		//find the max queue length
		if(queueSize > maxQueueLength)
			maxQueueLength = queueSize;
	}
	else{//the second simulation find the shortest queue and enqueue
		int smallest = multiQueueSize[1];
		int queueIndex = 1;		
		for(int i = 2; i < totalServer + 1; ++i){//find the smallest queue 
			if(multiQueueSize[i] < smallest){
				smallest = multiQueueSize[i];
				queueIndex = i;
			}
		}
		//now we find the smallest queue, enter that queue
		multiQueue[queueIndex][smallest] = serTime;
		//now record when the customer enter the queue
		enterMuitQueueTime[queueIndex][smallest] = currentTime;
		multiQueueSize[queueIndex]++;
		//now find the max queue length
		//this is in total
		int totalQuelength = 0;	//calculate the total queue length 
		for(int i = 1; i < totalServer + 1; ++i){
			totalQuelength += multiQueueSize[i];
		}
		if(totalQuelength > totalMultiQueueLength)
			totalMultiQueueLength = totalQuelength;
		//this is for each queue
		for(int i = 1; i < totalServer + 1; ++i){
			if(multiQueueSize[i] > maxQueueLength)
				maxQueueLength = multiQueueSize[i];
		}
	}
}

double dequeue(int simulationRound, int serverId){//remove the first service time and return it
	double tmp;
	if(simulationRound == 1){//this is for single queue
		totalQueueTime += (currentTime - enqueueTime[0]);	//this customer will leave, calculate how long he stay in the queue
		if((currentTime - enqueueTime[0]) > maxQueueTime){//find the max queue time
			maxQueueTime = currentTime - enqueueTime[0];
		}
		tmp = queue[0];
		for(int i = 0; i < queueSize - 1; ++i){
			queue[i] = queue[i + 1];
			enqueueTime[i] = enqueueTime[i + 1];
		}
		queueSize--;
	}
	else{//this is for second simulation
		int queueLength = multiQueueSize[serverId];
		tmp = multiQueue[serverId][queueLength];
		//calculate the total queue time for multiple queue
		totalQueueTime += ((currentTime - enterMuitQueueTime[serverId][0]));
		//calculate the max queue time
		if((currentTime - enterMuitQueueTime[serverId][queueLength - 1]) > maxQueueTime)
			maxQueueTime = ((currentTime - enterMuitQueueTime[serverId][0]));
		//dequeue
		for(int i = 0; i < queueLength - 1; ++i){
			multiQueue[serverId][i] = multiQueue[serverId][i + 1];
			enterMuitQueueTime[serverId][i] = enterMuitQueueTime[serverId][i + 1];
		}

		queueLength--;
		multiQueueSize[serverId] = queueLength;
	}
	return tmp;
}

void process_service_end(int simulationRound){
	lastEventTime = currentTime;
	currentTime = event[0].eventTime;		//time = service_end
	double actulServiceTime = 0;
	if(simulationRound == 1){//this is the first simulation
		if(queueSize == 0){//the queue is empty
			Event tmp = event[busyServerNum];
			event[busyServerNum] = event[0];
			event[0] = tmp;
			busyServerNum -= 1;
		}
		else{
			actulServiceTime = dequeue(simulationRound, event[0].serverNum);
			event[0].eventTime = currentTime + actulServiceTime;
			event[0].serviceTime += actulServiceTime;		//calculate the total service inorder to get the idle time
		}
	}
	else{//this is the second round
		//check whether the corresponding queue is empty
		int id = event[0].serverNum;
		if(multiQueueSize[event[0].serverNum] == 0){//the queue is empty
			Event tmp = event[busyServerNum];
			event[busyServerNum] = event[0];
			event[0] = tmp;
			busyServerNum -= 1;
		}
		else{
			actulServiceTime = dequeue(simulationRound, id);
			event[0].eventTime = currentTime + actulServiceTime;
			event[0].serviceTime += actulServiceTime;		//calculate the total service inorder to get the idle time
		}
	}
    totalPeopleServed++;    //increment the total served people
	siftDown();
}

void process_arrival(ifstream &readIntoHeap, int simulationRound){
	lastEventTime = currentTime;
	currentTime = event[0].eventTime;
	readIntoHeap >> event[0].eventTime >> nextServiceTime;
	totalServiceTime += nextServiceTime;
	siftDown();
	if(busyServerNum < totalServer){
		busyServerNum++;
        //make one of the idle server busy
		event[busyServerNum].eventTime = currentTime + serviceTime;
		//add the service time for calculation idle time
		event[busyServerNum].serviceTime += serviceTime;
		siftUp();
	}
	else
		enqueue(serviceTime, simulationRound);
	serviceTime = nextServiceTime;
}

void process_last_arrival(int simulationRound){
	lastEventTime = currentTime;
	currentTime = event[0].eventTime; 
	// remove_arrival_event();		//remove the root element of heap which means the last arrival event
	//swap the first element of the heap with the last element of heap
	if(busyServerNum < totalServer){
		busyServerNum++;
        //make one of the idle server busy
		event[busyServerNum].eventTime = currentTime + serviceTime;
		//add the service time for calculation idle time
		event[busyServerNum].serviceTime += serviceTime;
	
		siftUp();
	}
	else
		enqueue(serviceTime, simulationRound);

	//swap the arrival event with the last event in the heap and remove it
	Event tmp = event[busyServerNum];
	event[busyServerNum] = event[0];
	event[0] = tmp;
	busyServerNum--;
	siftDown();
}


void setSimulation(ifstream &fileIn, int simulationRound){

	currentTime = 0;		//time = 0
	busyServerNum = 0;			//n_busy = 0
	queueSize = 0;			//queue = empty
	totalPeopleServed = 0;	//reset the total server num
	totalServiceTime = 0;	//reset the total service time
	totalQueueTime = 0;		//reset the total queue waiting time
	maxQueueLength = 0;		//reset the max length 

	fileIn >> totalServer;	
	fileIn >> event[0].eventTime >> serviceTime;	//read heap[0], next_service_time

    for(int i = 0; i < totalServer + 1; ++i){//initialize the id array
        id[i] = i;
    }
    //initialize the serverNum from 0 to 10
    for(int i = 0; i < totalServer + 1; ++i){
    	event[i].serverNum = i;
    	event[i].serviceTime = 0;
    }
    if(simulationRound == 2){//initialize the length of the queue
    	for(int i = 0; i < totalServer + 1; ++i){
    		multiQueueSize[i] = 0;
    	}
    }
    totalServiceTime += serviceTime;
}

void simulation(ifstream &fileIn, int simulationRound){

	if(simulationRound == 1){
		cout << "Run the first simulation, it has " << totalServer;
		cout << " servers and 1 queue total" << endl;
	}
	else{
		cout << "Run the second simulation, it has " << totalServer;
		cout << " servers and " << totalServer << " queue total" << endl;
	}

	while(!fileIn.eof() || busyServerNum >= 0){
		long int currentFilePos = fileIn.tellg();
		char testNextChar;
		fileIn >> testNextChar;
		if(!fileIn.fail()){
			fileIn.seekg(currentFilePos);
			if(event[0].serverNum == 0){
				process_arrival(fileIn, simulationRound);
			}
			else{
				process_service_end(simulationRound);
			}
		}
		else{
			if(event[0].serverNum == 0){
				process_last_arrival(simulationRound);
			}
			else{
				process_service_end(simulationRound);
			}
		}
	}
}

void printStat(int simuRound){

	cout << "total service time: " << currentTime << endl;
    cout << "total serverd people: " << totalPeopleServed << endl;
    cout << "Average service time: " << totalServiceTime / totalPeopleServed << endl;
    cout << "Average time a customer spend in queue: " << totalQueueTime / totalPeopleServed << endl;
    cout << "Maximum time a customer spend in queue: " << maxQueueTime << endl;
    cout << "Maximum queue length " << maxQueueLength << endl;
    if(simuRound == 2){
		cout << "The max length of queue in total is: " << totalMultiQueueLength << endl;
    }
    cout << "Average length of queue in any given second is: " << totalQueueTime / currentTime << endl;
    cout << "Total idle time for each server: " << endl;
    for(int i = 0; i < totalServer + 1; ++i){
    	if(event[i].serverNum != 0)
    		cout << "idle for server " << event[i].serverNum << "is: " << currentTime - event[i].serviceTime << endl;
    }

}

int main(){

	char fileName[256];
	cout << "please input the file name: " << endl;
	cin >> fileName;
	/******the first simulation******/
	//initial the server first
	ifstream fin;
	fin.open(fileName);
	while(!fin.good()){
		cout << "file can't be open, enter the file name agian: " << endl;
		cin >> fileName;
		fin.open(fileName);
	}
	setSimulation(fin, 1);
	//run thefirst simulation
	simulation(fin, 1);
	fin.close();
	printStat(1);

    fin.open(fileName);
    //set up the simulation
    setSimulation(fin, 2);
    //run the second simulation
    simulation(fin, 2);
    fin.close();
    printStat(2);

	return 0;
}
