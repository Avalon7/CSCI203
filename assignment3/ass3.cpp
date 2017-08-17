/*
* CSCI203/803 Assignment 3
*
* Student name: Weicheng Yin
* Subject code:	CSCI203
* Student number: 5009819
* Email ID: wy157
* Sources: http://www.geeksforgeeks.org/union-find/ 
* the above soure gives the idea how to detect cycles
*/

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

//this is the node structure to store the two nodes and the cost between them
struct Tree{
	int NodeA;
	int NodeB;
	double cost;
};

/******* Global veriable *******/
int NODENUM = 0;
int MSTNUM = 0;
Tree *tree;
Tree **trees;

//this function will return the total edges for a fully connected graph
int calTotalEdges(){
	int result = (NODENUM * (NODENUM - 1)) / 2;
	return result;
}
//this function will return the cnumber combination
//ex: for node number 4, we need to select 3 edges out of 6 total edges to 
//make a spanning tree
int calCombination(int n){

	int k = NODENUM - 1;
    if (k * 2 > n)  
    	k = n - k;  	//C6 1 is the same with C6 5
    if (k == 0) 
    	return 1;		//C6 0 is always 1
 
    int result = n;
    for(int i = 2; i <= k; ++i ) {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}

int calMaxBin(int num){
	int max = 1;
	for(int i = 0; i < num; ++i){
		max *= 2;
	}
	return max;
}
//this function will convert the decimal to bin and check whether it fits the condition
int* decToBin(int num, int digits){
	int *bits = new int[digits];
	//initialize the bits array with 0;
	for(int i = 0; i < digits; ++i){
		bits[i] = 0;
	}
	int remender = 0;
	int position = digits - 1;
	while(num != 0){
		remender = num % 2;
		num /= 2;
		bits[position] = remender;
		position--;
	}
	return bits;
	
}
//this funciton counts how many 1 bits in bin
bool bitCount(int *array, int digits){
	int oneCount = 0;
	//loop throuth the bits array find if the number of 1 fits the condition
	for(int i = 0; i < digits; ++i){
		if(array[i] == 1)
			oneCount++;
	}
	if(oneCount == (NODENUM - 1)){
		return true;
	}
	return false;
}

void calNumMSTS(){
	for(int i = 0; i < NODENUM - 2; ++i){
		NODENUM *= NODENUM;
	}
}
//this function will the subset of element i
int find(int *parent, int i){
	if(parent[i] == -1)
		return i;		//the element is in this subset
	return find(parent, parent[i]);
}
//Union the two nodes in the same subset
void Union(int *parent, int x, int y){
	int nodeASet = find(parent, x);
	int nodeBSet = find(parent, y);
	parent[nodeASet] = nodeBSet;
}

bool checkST(int *bitArray, Tree *edgeArray, int edgeNum){
	Tree edges[NODENUM - 1];		//when n = 5, it will contains 4 edges
	int node[NODENUM];
	for(int i = 0; i < NODENUM; ++i){//store the connected node
		node[i] = 0;		
	}
	int index = 0;
	for(int i = 0; i < edgeNum; ++i){
		if(bitArray[i] == 1){
			edges[index].NodeA = edgeArray[i].NodeA;
			edges[index].NodeB = edgeArray[i].NodeB;
			index++;
		}
	}
	index = 0;
	
	//now count how many nodes connected by using edges
	for(int i = 0; i < NODENUM - 1; ++i){
		bool checkNodeA = false, checkNodeB = false;
		for(int j = 0; j < NODENUM; ++j){
			if(edges[i].NodeA == node[j])
				checkNodeA = true;
			if(edges[i].NodeB == node[j])
				checkNodeB = true;
		}
		if(!checkNodeA){
			node[index] = edges[i].NodeA;
			index++;
		}
		if(!checkNodeB){
			node[index] = edges[i].NodeB;
			index++;
		}
	}
	for(int i = 0; i < NODENUM; ++i){
		if(node[i] == 0)//there's isoated node
			return false;
	}
	//if there's no conection between the nodes
	for(int i = 0; i < edgeNum; ++i){
		if(bitArray[i] == 1){
			if(edgeArray[i].cost == 0)
				return false;
		}
	}
	//now check whether it contains cycle or not
	//for here i used an Union find algorithm
	//if two element are in the same subset it should contains the cycle
	int parent[NODENUM + 1];
	for(int i = 0; i < NODENUM + 1; ++i){
		parent[i] = -1;		//initialize the parent to -1 indiate that there's no two element in the same subset
	}
	for(int i = 1; i < NODENUM; ++i){
		int nodeA = find(parent, edges[i - 1].NodeA);		//try to find if the nodeA and nodeB are in the same subset
		int nodeB = find(parent, edges[i - 1].NodeB);
		if(nodeA == nodeB)		//this means this should contains the cycle
			return false;
		Union(parent, nodeA, nodeB);		//else unio nodeA with nodeB
	}

	//deal with this case, there's isolated node, but there's isolated edges
	//now check whether it a forest or a tree
	int isSelect[NODENUM];		//this array indicated whether the node is move from node array to selected node array
	int selected[NODENUM];
	int numInNodeSet = NODENUM;
	for(int i = 0; i < NODENUM; ++i){
		if(i == 0){
			isSelect[i] = 1;		//put the first node in the seleted node set
			selected[i] = node[i];	//and mark it is selected
			numInNodeSet--;
		}
		else{
			isSelect[i] = 0;
		}
	}

	return true;
}

double calculateCost(int *bitArray, Tree *edgeArray, int edgeNum){
	double totalCost;
	for(int i = 0; i < edgeNum; ++i){
		if(bitArray[i] == 1){
			totalCost += edgeArray[i].cost;
		}
	}
	return totalCost;
}

void printMST(int *bitArray, Tree *edgeArray, int edgeNum, double totalCost){
	ofstream fout;
	fout.open("output_mst_algoritm1.txt");
	for(int i = 0; i < edgeNum; ++i){
		if(bitArray[i] == 1){
			cout << edgeArray[i].NodeA << " - " << edgeArray[i].NodeB << "  " << edgeArray[i].cost << endl;
			fout << edgeArray[i].NodeA << " - " << edgeArray[i].NodeB << "  " << edgeArray[i].cost << endl;
		}
	}
	cout << totalCost << endl;
	fout << totalCost << endl;
	fout.close();
}	

void createGraph(ifstream &fileIn, double **array){
	for(int i = 0; i < NODENUM; ++i){
		for(int j = 0; j < NODENUM; ++j){
			fileIn >> array[i][j];
		}
	}
}

void prim(double **array){
	Tree *trees = new Tree[NODENUM - 1];		//used to store two nodes and the cost between them
	int nearest[NODENUM];
	double mindist[NODENUM];
	
	//we use 0 to indiate two nodes has no connextion
	//or they have already been connected
	//initialize
	for(int i = 0; i < NODENUM; ++i){
		nearest[i] = 1;
		if(array[i][0] != 0)
			mindist[i] = array[i][0];
		else
			mindist[i] = 9999;
	}
	
	//repeat n - 1 times
	for(int i = 0; i < NODENUM - 1; ++i){
		//for each iteration, find the minimun cost
		double min = 9999;
		int k;
		for(int j = 1; j < NODENUM; ++j){
			if(mindist[j] > 0 && mindist[j] <= min){	//already consider the no connected situation
				min = mindist[j];
				k = j;
			}
		}
		trees[i].NodeA = nearest[k];
		trees[i].NodeB = k + 1;
		trees[i].cost = min;
		mindist[k] = -1;			//mark these two nodes has been connected
		for(int j = 1; j < NODENUM; ++j){
			if(array[j][k] < mindist[j]){
				if(array [j][k] != 0)
					mindist[j] = array[j][k];
				nearest[j] = k + 1;
			}
		}
	}
	ofstream fout;
	fout.open("output_mst_algoritm2.txt");
	double totalCost;
	for(int i = 0; i < NODENUM - 1; ++i){
		cout << trees[i].NodeA << " - " << trees[i].NodeB << "  " << trees[i].cost << endl;
		fout << trees[i].NodeA << " - " << trees[i].NodeB << "  " << trees[i].cost << endl;
		totalCost += trees[i].cost;
	}
	cout << totalCost << endl;
	fout << totalCost << endl;
	fout.close();
	delete [] trees;
}

void bruteForce(double **array){

	int totalEdges = calTotalEdges();		//in case n = 5, totalEdges = 10
	//calculate the combination
	int numCombinaiton = calCombination(totalEdges);	
	//calculate 2^n
	int maxNumBin = calMaxBin(totalEdges);
	//now create dynamic memory to store all the binary
	int **bitSequence;
	bitSequence = new int*[numCombinaiton];
	for(int i = 0; i < numCombinaiton; ++i){
		bitSequence[i] = new int[totalEdges];
	}
	//loop through every number to find all possible combinations
	int index = 0;
	for(int i = 0; i < maxNumBin; ++i){
		int *numInBinary = decToBin(i, totalEdges);	//whether this num in binary fit condition
		bool fits = bitCount(numInBinary, totalEdges);
		if(fits){
			//the number of 1 fits the condition 
			bitSequence[index] = numInBinary;
			index++;
		}
	}
	//now we need the edges
	//in case n = 5, the situation will be like:
	//(1,2),(1,3), (1,4),(1,5),(2,3),(2,4),(2,5),(3,4),(3,5),(4,5)
	Tree edges[totalEdges];
	int position = 0;
	for(int i = 0; i < NODENUM - 1; ++i){
		for(int j = i; j < NODENUM - 1; ++j){
			int num = j;
			edges[position].NodeA = i + 1;
			edges[position].NodeB = num + 2;
			edges[position].cost = array[i][num + 1];
			num++;
			position++;
		} 
	}
	//now we had the set of edges, find the corresponding cost between two edges

	//find the "TREE" -> which is ST
	int totalST = 0;
	int MSTIndex;
	double minCost = 99999;
	for(int i = 0; i < numCombinaiton; ++i){
		bool isST = checkST(bitSequence[i], edges, totalEdges);
		if(isST){
			totalST++;
			//calculate the cost for that spanning tree
			double cost = calculateCost(bitSequence[i], edges, totalEdges);
			if(cost <= minCost){
				minCost = cost;
				MSTIndex = i;
			}
		}
	}
	// cout << "totalST: " << totalST << endl;
	//now output the MST
	printMST(bitSequence[MSTIndex], edges, totalEdges, minCost);
	delete [] bitSequence;
	
}

int main(int argc, char **argv)
{
	ifstream fin;
	double **matrix;		//used to stored the matrix

	fin.open("input.txt");

	if(!fin.good()){
		cout << "file can't be open, program terminated." << endl;
		return -1;
	}

	fin >> NODENUM;	//read the number of the node
	matrix = new double*[NODENUM];
	for(int i = 0; i < NODENUM; ++i){
		matrix[i] = new double[NODENUM];
	}

	createGraph(fin, matrix);
	//now, use prim algorithm to find the MST

	int start_s = clock();
	bruteForce(matrix);
	int stop_s = clock();
	cout << "1. Brute-force algorithm 1: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << " milliseconds\n\n" << endl;
	

	start_s = clock();
	prim(matrix);
	stop_s = clock();
	cout << "2. Prim's algorithm 2 (matrix): " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << " milliseconds" << endl;
	for(int i = 0; i < NODENUM; ++i){
		delete [] matrix[i];
	}
	delete [] matrix;

	return 0;
}
