/*
* CSCI203/803 Assignment 4
*
* Student name: Weicheng Yin
* Subject code:	CSCI203
* Student number: 5009819
* Email ID: wy157
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cfloat>
#include <queue>

using namespace std;

/******global variable******/
char **CITYNAME;
double **DISTANCE;
int CITYNUM;	
const int CHARSIZE = 128;

struct Path{
	int currentNode;
	int nextNode;
	int passNodeNum;		//how many nodes passed
	double lBound;		//current path's lower bound
	double currentCost;		//record the current cost 
	bool isVisited[20];	//mark if the city visited or not 
	int visitedCity[20];		//record the visited city index
	bool operator <(const Path &p)const{
		return lBound > p.lBound;
	}
};

bool readFile(char *nFile, char *dFile){
	ifstream fin;
	//read distance from dis file
	fin.open(dFile);
	if(!fin.good())
		return false;
	fin >> CITYNUM;
	DISTANCE = new double*[CITYNUM];
	for(int i = 0; i < CITYNUM; ++i){
		DISTANCE[i] = new double[CITYNUM];		
	}
	for(int i = 0; i < CITYNUM; ++i){
		for(int j = 0; j < CITYNUM; ++j){
			fin >> DISTANCE[i][j];
		}
	}

	fin.close();
	//read the city name from name file
	fin.open(nFile);
	if(!fin.good())
		return false;
	CITYNAME = new char*[CITYNUM];
	for(int i = 0; i < CITYNUM; ++i){
		CITYNAME[i] = new char[CHARSIZE];
		fin.getline(CITYNAME[i], CHARSIZE);
		// cout << CITYNAME[i] << endl;
	}
	fin.close();

	return true;
}

double greedyAlgorithm(){
	//start from and returnning to Wollongong 
	//iterate for 11 times 
	cout << "1. Greedy algorithm: " << endl;
	int currentCity = 0;		//start from the first city
	bool isVisited[CITYNUM];
	int minCityIndex = -1;
	int lastCityIndex = 0;
	double totalDis = 0;
	isVisited[0] = true;
	int cityIndex[20];
	for(int i = 1; i < CITYNUM; ++i){
		isVisited[i] = false;
	}
	//now iterate CITYNUM + 1 times to find the nearest city each iteration
	cout << "Number of cities:	" << CITYNUM << endl;
	cout << "Tour:" << endl;
	for(int i = 0; i < CITYNUM; ++i){
		// cout << currentCity + 1 << " ";
		cityIndex[i] = currentCity;
		//loop through the coresponding connected city to find the nearest one
		double minDistance = DBL_MAX;
		for(int j = 0; j < CITYNUM; ++j){
			if(DISTANCE[currentCity][j] != 0 && !isVisited[j]){//not the current city
				if(DISTANCE[currentCity][j] < minDistance){
					minDistance = DISTANCE[currentCity][j];					
					minCityIndex = j;					
				}
			}
		}
		isVisited[minCityIndex] = true;
		currentCity = minCityIndex;
		totalDis += minDistance;
		if(i == CITYNUM - 2){		//this is the last city visited
			lastCityIndex = currentCity;
			cityIndex[CITYNUM - 1] = lastCityIndex;
			break;
		}

	}
	for(int i = 0; i < CITYNUM; ++i){
		cout << cityIndex[i] + 1 << " ";
	}
	cout << endl;
	for(int i = 0; i < CITYNUM; ++i){
		cout << CITYNAME[cityIndex[i]] << endl;
	}
	cout << CITYNAME[0] << endl;

	totalDis += DISTANCE[0][lastCityIndex];
	cout << "Total cost: " << totalDis << endl << endl;
	return totalDis;

}

double calLowerBound(Path p){
	//times the current cost with 2
	//cause the lower bound will be counted twice in the following part
	double realCost = p.currentCost * 2;
	double min = DBL_MAX;
	//find the min cost connected to the current node and the next node
	for(int i = 0; i < CITYNUM; ++i){
		if(p.isVisited[i] == 0 && min > DISTANCE[i][p.currentNode]){
			min = DISTANCE[i][p.currentNode];
		}
	}
	realCost += min;
	min = DBL_MAX;
	for(int i = 0; i < CITYNUM; ++i){
		if(p.isVisited[i] == 0 && min > DISTANCE[p.currentNode][i]){
			min = DISTANCE[p.currentNode][i];
		}
	}
	realCost += min;
	//calculate the min 2 cost which is not connecting to the current path
	for(int i = 0; i < CITYNUM; ++i){
		if(p.isVisited[i] == 0){
			min = DBL_MAX;
			for(int j = 0; j < CITYNUM; ++j){
				if(min > DISTANCE[i][j]){
					min = DISTANCE[i][j];
				}
			}
			realCost += min;
			min = DBL_MAX;
			for(int j = 0; j < CITYNUM; ++j){
				if(min > DISTANCE[j][i])
					min = DISTANCE[j][i];
			}
			realCost += min;
		}
	}
	return realCost / 2;
}

//this algorithm will use branch and bound algorithm
void BranchAndBound(double uppBound){
	cout << "2. Branch and bound algorithm (Breath-first):" << endl;
	cout << "Number of cities:	" << CITYNUM << endl;
	//calculate the lower bound
	double lowerBound = 0;
	for(int i = 0; i < CITYNUM; ++i){//loop through city to find the min cost and add together 
		double minDistance = DBL_MAX;
		for(int j = 0; j < CITYNUM; ++j){
			if(DISTANCE[i][j] != 0){//not the city itself
				if(DISTANCE[i][j] < minDistance){
					minDistance = DISTANCE[i][j];
				}
			}
		}
		lowerBound += minDistance;
	}
	cout << "Upper bound:	" << uppBound << endl;
	cout << "Lower bound:	" << lowerBound << endl;

	double realCost = DBL_MAX;
	//use priority queue to determime which path should go first according to the lower bound of this path
	priority_queue<Path> pQueue;
	Path start, end;
	//start from the first node;
	start.currentNode = 0;
	// start.passNodeNum = 1;
	for(int i = 0; i < CITYNUM; ++i){
		start.isVisited[i] = false;
	}
	start.isVisited[0] = true;		//the first node is already visited
	start.lBound = lowerBound;
	start.currentCost = 0;
	start.nextNode = 0;
	start.passNodeNum = 0;
	start.visitedCity[start.passNodeNum] = 0;
	start.passNodeNum = 1;

	pQueue.push(start); 

	while(!pQueue.empty()){//there's still path need to be explore
		Path tmp = pQueue.top();
		pQueue.pop();		//remove this exploring path

		if(tmp.passNodeNum == CITYNUM - 1){
			//now almost find the path except the last node
			int lastCityIndex = 0;
			bool found = false;
			for(int i = 0; i < CITYNUM && !found; ++i){
				if(tmp.isVisited[i] == false){
					lastCityIndex = i;
					found = true;
				}
			}

			double lastCost = tmp.currentCost + DISTANCE[tmp.nextNode][lastCityIndex] + DISTANCE[lastCityIndex][tmp.currentNode];
			Path minCostInQueue = pQueue.top();

			if(lastCost <= minCostInQueue.lBound){		//there's no need to expend other nodes 
				if(lastCost < realCost){
					realCost = lastCost;
					for(int i = 0; i < CITYNUM - 1; ++i){
						end.visitedCity[i] = tmp.visitedCity[i];
					}
					end.visitedCity[CITYNUM - 1] = lastCityIndex;
				}
				break;
			}
			else{
				//otherwise still need to expend other nodes
				//now update the upper bound
				if(lastCost < uppBound)
				{	
					uppBound = lastCost;
				}
				if(lastCost < realCost){
					realCost = lastCost;
					for(int i = 0; i < CITYNUM - 1; ++i){
						end.visitedCity[i] = tmp.visitedCity[i];
					}
					end.visitedCity[CITYNUM - 1] = lastCityIndex;
				}
			}
		}

		Path nextPath;
		//expand the next node 
		for(int i = 0; i < CITYNUM; ++i){
			if(tmp.isVisited[i] == false){
				//visit this node
				nextPath.currentNode = tmp.currentNode;
				//calculate the current cost
				nextPath.currentCost = tmp.currentCost + DISTANCE[tmp.nextNode][i];
				nextPath.nextNode = i;
				//copy all the nodes to the nextPath object
				// nextPath.nodes = tmp.nodes;
				for(int j = 0; j < tmp.passNodeNum; ++j){
					nextPath.visitedCity[j] = tmp.visitedCity[j];
				}
				nextPath.visitedCity[tmp.passNodeNum] = i;		
				nextPath.passNodeNum = tmp.passNodeNum + 1;
				//mark the Cities have been passed
				for(int j = 0; j < CITYNUM; ++j)
					nextPath.isVisited[j] = tmp.isVisited[j];
				nextPath.isVisited[i] = true;

				nextPath.lBound = calLowerBound(nextPath);
				if(nextPath.lBound < uppBound)
					pQueue.push(nextPath);
			}
		}
	}
	cout << "Optimal tour:\n";
	cout << "1 ";
	for(int i = CITYNUM - 1; i > 0; --i){
		cout << end.visitedCity[i] + 1 << " ";
	}
	cout << endl;
	cout << CITYNAME[0] << endl;
	for(int i = CITYNUM - 1; i > 0; --i){
		cout << CITYNAME[end.visitedCity[i]] << endl;
	}
	cout << CITYNAME[0] << endl;
	cout << "Total cost: " << realCost << endl;
}

int main(int argc, char** argv)
{
	if(argc != 3)
		return -1;
	char nameFile[128];
	char disFile[128];
	strcpy(nameFile, argv[2]);
	strcpy(disFile, argv[1]);

	bool readSuccess = readFile(nameFile, disFile);
	if(!readSuccess)
		return -1;
	//Greedy algorithm
	double upperBound = greedyAlgorithm();

	BranchAndBound(upperBound);
	return 0;
}
