#include <iostream>
#include <sstream>
#include <ostream>
#include <chrono>
#include <climits>


// When dontMinimizeQueens is not defined,
// the minBishops algorithm will try to find a minimal solution with the queens when no bishops are necessary to find a solution
// will take significantly longer when less queens are necessary.
#define dontMinimizeQueens

//When debug_on is defined, more information will be shown in the terminal about the algorithms minBishops and findNmbSolutions
// - findNmbSolutions prints al solutions that it finds, (Not recommended for chessboard with big sizes, the process will take longer)
// - minBishops prints every new minimal solution that it finds, it shows the working of the algorithm quite well (Recommended to look at it for once).
//#define debug_on

//namespaces
using namespace std::chrono;
using namespace std;

//header of functions
void displayChessBoard(char* board_ptr);                        // Function to display the contents of a chessboard
void displayCoverage();                                         // Used for debugging
void minBishops(int currentQueens, int currentBishops);         // Recursive algorithm to find the minimum amount of bishops for a given amount of queens on a chessboard.
void findNmbSolutions(int currentQueens, int currentBishops);   // Recursive algorithm to find the amount of possible solution
void placePiece(int y, int x, bool queenBishop);               // Function to place a piece on the chessboard
bool removePiece(int y, int x);                                 // Function to remove a piece from the chessboard
void updateCoverage();                                          // Updates the attack lines of pieces is used by boardIsCovered() function
bool boardIsCovered();                                          // Checks if all positions on the chessboard are attacked.
char* copyChessboard(const char* source, int size);             // Function to copy the contents of a chessboard to a new space in memory


// Global constants for better read-ability
const bool bQueen = true;
const bool bBishop = false;

// Global variables:
int M = 0, N = 0; //stores the dimensions of the chessboard.
int queensLimit = INT_MAX, bishopLimit = INT_MAX, pieceLimit = INT_MAX;
char* chessboardPtr = nullptr;             // Points to the chessboard
char* chessboardBestPtr = nullptr;        // Points to the copied chessboard
bool* coverageBoardPtr = nullptr;          // Points to the coverage board, this a board that shows which positions on the chessboard are under attack from pieces.
int nmbSolutions = 0;                       // Keeps track of the amount of solutions


// Functions
void minBishops(int currentQueens, int currentBishops) {
    // A more minimal solution fits the following criteria
    // - 1st priority is less bishops in use, if there are bishops in use we update the bishopLimit to that of current solution.
    // - 2nd priority only applies when there are no bishops in use, we update the queensLimit to see if we can also find a solution for less queens than the given amount

    // Check if further recursion could lead to a more minimal solution.
    // To do this we check for squares which are not yet attacked.
    // If all squares are attacked we check if we have found a new minimal solution
    // If so we update the minimal solution, if all squares are covered we also prevent deeper recursion.

    if (currentQueens + currentBishops >= pieceLimit) {
        // stop recursion, we are are not going get a better result than a previous solution
        return;
    }

    #ifdef dontMinimizeQueens
        if (nmbSolutions > 0) {
            // stop recursion, when we are not minimizing queens
            // our best result has already been reached.
            return;
        }
    #endif

    // check if a solution has been found.
    if (boardIsCovered()) {
        // We have found a solution.
        // Update the best solution
        pieceLimit = currentQueens + currentBishops; // Update the pieceLimit, which limits the amount of further recursion
        queensLimit = currentQueens; //update minimum amount of queens
        bishopLimit = currentBishops; //update minimum amount of bishops

        //save the best layout
        chessboardBestPtr = copyChessboard(chessboardPtr, M * N);

        if (currentBishops == 0) {
            //When we dont minimize the amount of queens,
            // we can stop recursion whenever, we have found a solution when currentBishops are 0
            nmbSolutions = 1;
        }

        #ifdef debug_on
            //when debug is enabled show the proces of minimal solutions
            cout << endl << "Found a solution!:" << endl;
            cout << "Queens: "<< queensLimit << endl;
            cout << "Bishops: "<< bishopLimit << endl;
            cout << "pieceLimit " << pieceLimit << endl;
            cout << endl;
            displayChessBoard(chessboardPtr);
            cout << endl;
            displayCoverage(); //show the board to check that all sqaures are covered.
            cout << endl << endl;
        #endif

        return; //prevent further recursion
    }

    if (currentQueens + currentBishops >= pieceLimit - 1) {
        //adding pieces wont result in shorter solution, so we should quit this loop
        return;
    }

    for (int y = 0; y < M; y++) {
        for (int x = 0; x<N; x++) {
            if (chessboardPtr[x + y * N] == '-') {
                // When there is a square that is not in use we place a queen if we haven't reached the QueenLimit.
                // So that we use the minimum amount of bishops
                // Otherwise we add a bishop.
                if (currentQueens < queensLimit) {
                    placePiece(y, x, bQueen);
                    currentQueens++;
                } else {
                    placePiece(y, x, bBishop);
                    currentBishops++;
                }

                //start the next recursive function, deepening the depth of recursion, until we have found a solution.
                minBishops(currentQueens, currentBishops);

                // we remove the piece we placed in this recursion loop, in this way more solutions can be discovered.
                if(removePiece(y, x)) { //we keep track of the amount of queens and bishop on the chessboard.
                    currentQueens--;
                } else {
                    currentBishops--;
                }
            }
        }
    }
}

//Recursive algorithm to find the amount of possible solution for a given amount of Queens and Bishops.
void findNmbSolutions(int currentQueens, int currentBishops) {
    if (currentQueens + currentBishops > pieceLimit) {
        // stop recursion, we have to many pieces.
        // also alarm the user of this unnecessary recursion happening.
        cerr << "\n\nError: To many pieces on chessboard, quitting this recursion loop!\n\n";
        return;
    }

    if (currentQueens + currentBishops == pieceLimit) {
        if (boardIsCovered()) {
            // we have found a solution
            nmbSolutions++;  // increment amount of solutions

            //save the first solution:
            if (nmbSolutions == 1) {
                chessboardBestPtr = copyChessboard(chessboardPtr, M * N);
            }

            #ifdef debug_on
                //show solution (for debugging reasons):
                cout << "\nSolution " << nmbSolutions << " is:\n";
                displayChessBoard(chessboardPtr);
            #endif
        }
        return;
    } else {// Not all piece are on the board yet so continue adding piece and deepen the level of recursion
        for (int y = 0; y < M; y++) {
            for (int x = 0; x<N; x++) {
                if (chessboardPtr[x + y * N] == '-') {
                    // When there is a square that is not in use we place a queen if we haven't reached the QueenLimit.
                    // So that we use the minimum amount of bishops
                    // Otherwise we add a bishop.
                    if (currentQueens < queensLimit) {
                        placePiece(y, x, bQueen);
                        currentQueens++;
                    } else if (currentBishops < bishopLimit) {
                        placePiece(y, x, bBishop);
                        currentBishops++;
                    } else {
                        // We are out of pieces but we have not found a solution
                        // it should not be possible to reach this state, so notify user if we end up here.
                        cerr << "\n\nError: Out of pieces!\n\n";
                    }

                    //start the next recursive loop, until all pieces are on the board.
                    findNmbSolutions(currentQueens, currentBishops);

                    // We remove the piece we placed in this recursion loop, in this way more solutions can be discovered.
                    // We keep track of the amount of queens and bishop on the chessboard.
                    if(removePiece(y, x)) {
                        currentQueens--;
                    } else {
                        currentBishops--;
                    }
                }
            }
        }
    }
}


// Function to place a piece on the chessboard
void placePiece(int y, int x, bool queenBishop) {
    char piece;
    piece = (queenBishop) ? 'Q' : 'B';
    // decide which piece
    chessboardPtr[x + y * N] = piece;
}


// Function to remove a piece from the chessboard
bool removePiece(int y, int x) {
    char piece = chessboardPtr[x + y * N];
    bool queenBishop;
    if (piece == 'Q') {
        queenBishop = true;
    } else if (piece == 'B') {
        queenBishop = false;
    } else {
        cerr << "\n\nError: You tried removing a non-existing piece!!\n\n";
        return false;  // Return false to indicate failure
    }
    chessboardPtr[x + y * N] = '-';  // remove piece from board
    return queenBishop;
}


// Updates the attack lines of pieces is used by boardIsCovered() function
void updateCoverage() {
    //clean CoverageBoard
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            coverageBoardPtr[x + y * N] = false;
        }
    }

    // mark all the attacked positions of pieces
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            if (chessboardPtr[x + y * N] != '-') { //if not blank space
                // cover the spot of the piece
                coverageBoardPtr[x + y * N] = true;
                // Queen and Bishop both make diagonal line, so we can draw them regardless of which piece it is.
                // Make diagonal line left upwards
                for (int i = 1; (y - i >= 0) && (x - i >= 0); i++) {
                    coverageBoardPtr[(x - i) + (y - i) * N] = true;
                }

                // Make diagonal line right upwards
                for (int i = 1; (y - i >= 0) && (x + i < N); i++) {
                    coverageBoardPtr[(x + i) + (y - i) * N] = true;
                }

                // Make diagonal line left downwards
                for (int i = 1; (y + i < M) && (x - i >= 0); i++) {
                    coverageBoardPtr[(x - i) + (y + i) * N] = true;
                }

                // Make diagonal line right downwards
                for (int i = 1; (y + i < M) && (x + i < N); i++) {
                    coverageBoardPtr[(x + i) + (y + i) * N] = true;
                }

                if (chessboardPtr[x + y * N] == 'Q') {
                    // Also add the horizontal & vertical lines when the piece is a queen
                    // Left horizontal
                    for (int i = 1; (x - i >= 0) && (chessboardPtr[(x - i) + y * N] == '-'); i++) {
                        coverageBoardPtr[(x - i) + y * N] = true;
                    }

                    // Right horizontal
                    for (int i = 1; (x + i < N) && (chessboardPtr[(x + i) + y * N] == '-'); i++) {
                        coverageBoardPtr[(x + i) + y * N] = true;
                    }

                    // Upwards vertical
                    for (int i = 1; (y - i >= 0) && (chessboardPtr[x + (y - i) * N] == '-'); i++) {
                        coverageBoardPtr[x + (y - i) * N] = true;
                    }

                    // Downwards vertical
                    for (int i = 1; (y + i < M) && (chessboardPtr[x + (y + i) * N] == '-'); i++) {
                        coverageBoardPtr[x + (y + i) * N] = true;
                    }
                }
            }
        }
    }
}


//function to check if all position on the chessboard are attacked
bool boardIsCovered() {
    updateCoverage();   //Updates the attack lines of pieces is used by boardIsCovered() function
    bool boardCovered = true; //set variable to be true before proven to be wrong
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            if (!coverageBoardPtr[x + y * N]) {
                boardCovered = false;
                y = M;
                break; //break out of x loop, in combination with y = M, exits the y loop as well.
            }
        }
    }
    return boardCovered;
}


// Function to copy contents of the chessboard
char* copyChessboard(const char* source, int size) {
    char* destination = new char[size];                 //allocates new memory with the same size as the chessboard
    copy(source, source + size, destination);       //copy's the memory from the source to the destination
    return destination;
}


// Function to display the ChessBoard
void displayChessBoard(char* board_ptr) {
    for (int y = 0; y < M; y++) {
        cout << endl;                               //adds a newline for every row of the chessboard
        for (int x = 0; x < N; x++) {
            cout << board_ptr[x + y * N] << "  ";   //adds 2 spaces between each element
        }
    }
    cout << endl;
}


void displayCoverage() {                                        //Updates the attack lines of pieces is used by boardIsCovered() function
    for (int y = 0; y < M; y++) {
        cout << endl;                                           //adds a newline for every row of the chessboard
        for (int x = 0; x < N; x++) {
            cout << coverageBoardPtr[x + y * N] << "  ";       //adds 2 spaces between each element
        }
    }
    cout << endl;
}


int main() {
    int chosenOption = 0;       //stores the users choice, decides which algorithm is used in this program.

    //start screen
    cout << "-------------------------------------------------------" << endl;
    cout << "\t\tChess coverage problem" << endl;
    cout << "-------------------------------------------------------" << endl;

    //ask user to input chessboard size
    string input;
    cout << "Enter the size of the chessboard in the form MxN: " << endl;
    cin >> input;

    // Use a string stream to parse the input string
    istringstream iss(input);
    // Read M
    if (!(iss >> M)) {
        cerr << "Invalid input for M." << endl;
        return 1;
    }

    // Check for 'x'
    char c;
    if (!(iss >> c) || c != 'x') {
        cerr << "Invalid input. Expected 'x' after M." << endl;
        return 1;
    }

    // Read N
    if (!(iss >> N)) {
        cerr << "Invalid input for N." << endl;
        return 1;
    }

    // Print chosen board size
    cout << endl << "Board is an " << M << "x" << N << " board" << endl;

    // Initialize chessboard.
    // Contains chessboard of MxN (Y*X) size,
    chessboardPtr = new char[M * N]; // Keeps track of the location of the pieces
    coverageBoardPtr = new bool[M * N]; // Keeps track of the covered squares;

    // Initialize the chessboard with a default value
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            chessboardPtr[x + y * N] = '-';
            coverageBoardPtr[x + y * N] = false;
        }
    }

    // Ask user if the user wants to find minimum amount of bishops or the amount of solutions to cover a chessboard:
    cout << endl;
    cout << "Choose what you want to find:" << endl;
    cout << "1. Find the number of solution to cover the chessboard.\n"
            "   You enter the amount of queens and bishops " << endl;
    cout << "2. Find the minimum number of bishops necessary to cover the entire chessboard\n"
            "   You enter the amount of queens that may be used." << endl;

    // Ask user to choose an algorithm:
    cout << "\nEnter 1 or 2 and press enter\n\n";
    cin >> chosenOption;

    if ((chosenOption < 1) || (chosenOption > 2)) {
        // User has entered invalid input
        // Notify user and end program
        cerr << "Entered invalid option, ending program!" << endl;
        return 1;
    }

    // Ask the user to enter additional information which is necessary for the algorithm
    cout << "\nEnter the amount of queens: \n";
    cin >> queensLimit;
    cout << "\nqueensLimit is: " << queensLimit << endl;

    if (chosenOption == 1) {
        cout << "\nEnter the amount of bishops: \n";
        cin >> bishopLimit;
        cout << "\nbishopLimit is: " << bishopLimit << endl;
        pieceLimit = queensLimit + bishopLimit; //decide the piece limit
    }

    // Start timer
    high_resolution_clock::time_point start = high_resolution_clock::now(); //start timer
    high_resolution_clock::time_point stop; //initialize stop variable

    switch (chosenOption) {
        case 1:
            // Start the recursive function to find the number of solutions
            findNmbSolutions(0, 0);
            stop = high_resolution_clock::now(); // Stop timer

            // Print result to terminal
            cout << "There are " << nmbSolutions << " Solution(s)." << endl;
            if (nmbSolutions > 0) {     // Only print chessboard solution when there is a solution.
                cout << "1 valid solution is: " <<endl;
                displayChessBoard(chessboardBestPtr);
            }
            break;
        case 2:
            // Run the recursion algorithm to find the minimum amount of bishops
            minBishops(0, 0);
            stop = high_resolution_clock::now(); // Stop timer

            // Show final results
            cout << endl << "Found a solution!:" << endl;
            cout << "Queens: " << queensLimit << endl;
            cout << "Bishops: " << bishopLimit << endl;
            cout << endl;
            displayChessBoard(chessboardBestPtr);
            cout << endl;
            break;
    }

    // Calculate the execution time
    milliseconds executionTime = duration_cast<milliseconds>(stop - start);

    // Display the execution time
    cout << "Execution time was: " << executionTime.count() << " ms" << endl;

    // Free memory
    delete[] chessboardPtr;
    delete[] coverageBoardPtr;
    delete[] chessboardBestPtr;

    return 0;   //End program
}