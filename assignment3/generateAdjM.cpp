/**
 *  This program is provided for CSCI203 Assignment 3
 *
 *     Students may use it or adapt it to generate
 *       adjacency matrices for analysis of MST algorithms
 *
 *  Do not modify the output format
 *
 *  Usage:
 *     To generate a complete graph with random weights:
 *
 *        generateAdjM N 
 *
 *     or, to generate a graph with a lower density
 *
 *        generateAdjM N d
 *
 *        N - number of vertices
 *        d - density  (scale: 40-100) (roughly, used to control edge removal)
 * 
 *  Output:
 *       input.txt - adjacency matrix saved in the working folder
 *
 *                   first line:       N - number of vertices
 *                   following lines:  NxN edge weights
 *
 */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <new>
#include <cstdlib>
using namespace std;

#define WEIGHT_TYPE double

string const OUTPUT_FILENAME = "input.txt";


/******************************************************
 *  Class AdjMatrix 
 *
 *    Example: 
 *
 *         AdjMatrix adjM(N);
 *      
 *         will create an NxN adjacency matrix for 
 *           a complete, undirected, randomly weighted 
 *           graph, stored as a symmetric matrix
 * 
 *           0 - no connection
 *
 ******************************************************/
class AdjMatrix {
  public:
    AdjMatrix(int);
	~AdjMatrix();
	
	void save_to_aFile(string);
	void display_onScreen(void);
	void reduce_density(int);
	
  private:
  	void generateAdjMatrixComplete(void);
    double generateWeight(void);
	int getNumberOfNonzeroEntries(int);
	
	int N;
	WEIGHT_TYPE ** adjM;
};


/******************************************************
 *  The main program that creates an adjacency matrix
 *
 ******************************************************/
int main (int argc, char* argv[]) {
	int N=3, density=100;

	/* parse command line arguments */
	if (argc < 2) {
		cerr << "Usage: generateAdjM NumberOfVertices [densityInPercentage]"<<endl;
		return 1;
	} else {
		N= atoi(argv[1]);
		if (argc == 3 )
			density = atoi(argv[2]);
	}

	/* initialize random seed; for reproductivity */
	srand (203803);

	/* create an adjacency matrix for a complete, undirected, randomly weighted graph*/
	AdjMatrix adjM(N);   
	
    /* reduce density if required */
	if (density < 100 ) {  
		adjM.reduce_density(density);
	}

	/* display on the screen */
	// turn it on only for testing
	adjM.display_onScreen();
	
	/* save the adjacency matrix to the file */
	adjM.save_to_aFile(OUTPUT_FILENAME);
	
	return 0;
}


/******************************************************
 *  Class AdjMatrix implementation
 *
 ******************************************************/
AdjMatrix::AdjMatrix(int N) {
	this->N = N;
	
	/* create 2-d NxN array */
	adjM = new (nothrow) WEIGHT_TYPE*[N];
	for (int i = 0; i < N; i++) {
		adjM[i] = new (nothrow) WEIGHT_TYPE[N]();
	}
	
	/* generate an adjacency matrix */
	generateAdjMatrixComplete();
}

AdjMatrix::~AdjMatrix() {
	/* clear up memory */
	for (int i = 0; i < N; i++) {
		delete [] adjM[i];
	}
	delete [] adjM;
}

/* generate adjacency matrix for a complete, undirected, randomly weighted graph*/
void AdjMatrix::generateAdjMatrixComplete(void) {
		
	//*** int cc=1;  // counter useful only for generating sequential numbers
	for (int i=0; i<N; i++) {
		for (int j=i+1;j<N;j++) {
			adjM[i][j] = generateWeight();
			//*** adjM[i][j] = cc++;  /* generate sequential numbers: useful for testing */
			adjM[j][i] = adjM[i][j];
		}
	}
}

/* save adjacency matrix to a file */
void AdjMatrix::save_to_aFile(string output_filename) {
	
	ofstream myfile;
	// set output format; useful for floating-point weights
	myfile <<setprecision(1);
	myfile << std::fixed;
	myfile.open (output_filename.c_str());

	// save the vertex number first
	myfile<<N<<endl;
	
	// save the matrix row by row
	for (int i=0; i<N; i++) {
		for(int j=0;j<N;j++) {
			myfile<< adjM[i][j] << " ";
		}
		myfile << endl;
	}
	myfile.close();
}

/* diplay the matrix on the screen */
void AdjMatrix::display_onScreen(void) {
	
	// set output format; useful for floating-point weights
	cout <<setprecision(1);
	cout << std::fixed;
	for (int i=0; i<N; i++) {
		for(int j=0;j<N;j++) {
			cout<< adjM[i][j] << " ";
		}
		cout << endl;
	}
}

/* reduce number of edges */
void AdjMatrix::reduce_density(int density) {
	
	// limit the density to a good level
	if(density < 40) {
		density = 40;
	}

	/* remove some edges from some vertices selected randomly: add 0 */
	for (int i=0; i<N; i++) {
		for (int j=i+1;j<N;j++) {
			if( rand()%100 > density) {  // use density scale as percentage
			                             //   of surviving edges 										
				adjM[i][j] = 0;
				adjM[j][i] = adjM[i][j];
			}
		}
	}

	/* 
	 *   For a small graph, make sure it is not too sparse to be 
	 *     disconnected.
	 *   For a large graph, the chance to be disconnected is 
	 *     very low when the density scale is set higher than, 
	 *     say 40. 
	 *   So, this will work for the purpose. 
	 *
	 *   It isn't difficult to verify connectivity but not
	 *     necessary here.
	 *   If there are troubles in your MST program, use higher 
	 *     density or do not reduce density at all.
	 */
	int const VERTEX_DEGREE = 2; // minimal degree of a vertex
	
	for (int i=0; i<N; i++) {
		
		int sum = getNumberOfNonzeroEntries(i);

		/* add some edges back randomly */
		while (sum < VERTEX_DEGREE) {
			
			// add one edge back to a random position
			int m=i;
			while (m==i) {
				m=rand()%N;  // avoid the diagonal entry
			}
			adjM[i][m] = generateWeight();
			adjM[m][i] = adjM[i][m];
			sum++;
		}
	}
}

/* generate a random value in (a, b) */
double AdjMatrix::generateWeight(void) {

	if(N<3) N=3;
	double a=1, b=2*N;   // set a good range for weights
	
	return (a + (rand() / ( RAND_MAX / (b-a) )));
}

/* get the number of non-zero entries in a row */
int AdjMatrix::getNumberOfNonzeroEntries(int i) {

	int sum=0;
	for (int j=0;j<N;j++){
		if (adjM[i][j] != 0){
			sum ++;
		}
	}
	return sum;
}


