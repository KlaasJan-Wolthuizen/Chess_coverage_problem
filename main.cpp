#include <iostream>
#include <sstream>
#include <ostream>
#include <chrono>

using namespace std::chrono;
using namespace std;

//definitions for better read-ability
#define bQueen true
#define bBishop false

//headers
void displayChessBoard(char* board_ptr);
void displayCoverage();
void MinBishops(int CurrentQueens, int CurrentBishops); // recursive algorithm to find the minimum amount of bishops for a given amount of queens on a chessboard.
void findNmbSolutions(int CurrentQueens, int CurrentBishops); //Recursive algorithm to find the ammount of possible solution
void PlacePiece(int y, int x, bool Queen_Bishop);
bool RemovePiece(int y, int x);
void UpdateCoverage();
bool boardIsCovered();
char* copyChessboard(const char* source, int size);


//Global variables:
int M = 0, N = 0; //stores the dimensions of the chessboard.
int QueensLimit = INT_MAX, BishopLimit = INT_MAX, PieceLimit = INT_MAX;
char* Chessboard_ptr = nullptr;
char* Chessboard_best_ptr = nullptr;
bool* CoverageBoard_ptr = nullptr;
int nmbSolutions = 0;
int ChosenOption = 0;

//functions
void MinBishops(int CurrentQueens, int CurrentBishops) {
    // A more minimal solution fits the following criteria
    // - 1st priority is less bishops in use, if there are bishops in use we update the BishopLimit to that of current solution.
    // - 2nd priority only applies when there are no bishops in use, we update the queensLimit to see if we can also find a solution for less queens than the given amount

    // Check if further recursion could lead to a more minimal solution.
    // To do this we check for squares which are not yet attacked.
    // If all squares are attacked we check if we have found a new minimal solution
    // If so we update the minimal solution, if all squares are covered we also prevent deeper recursion.

    if (CurrentQueens + CurrentBishops >= PieceLimit) {
        //stop recursion, we are are not going get a better result than a previous solution
        return;
    }

    //check if a solution has been found.
    if (boardIsCovered()) {
        // we have found a solution.
        //Update the best solution, and the PieceLimit, which limits the amount of further recursion
        PieceLimit = CurrentQueens + CurrentBishops;
        QueensLimit = CurrentQueens; //update minimum amount of queens
        BishopLimit = CurrentBishops; //update minimum amount of bishops

        //save the best layout
        Chessboard_best_ptr = copyChessboard(Chessboard_ptr, M * N);

//        cout << endl << "Found a solution!:" << endl;
//        cout << "Queens: "<< QueensLimit << endl;
//        cout << "Bishops: "<< BishopLimit << endl;
//        cout << "PieceLimit " << PieceLimit << endl;
//        cout << endl;
//        displayChessBoard(Chessboard_ptr);
//        cout << endl;
//        displayCoverage(); //show the board to check that all sqaures are covered.
//        cout << endl << endl;
        return; //prevent further recursion
    }

    if (CurrentQueens + CurrentBishops >= PieceLimit-1) {
        //adding pieces wont result in shorter solution, so we should quit this loop
        return;
    }

    for (int y = 0; y < M; y++) {
        for (int x = 0; x<N; x++) {
            if (Chessboard_ptr[x + y * N] == '-') {
                // When there is a square that is not in use we place a queen if we haven't reached the QueenLimit.
                // So that we use the minimum amount of bishops
                // Otherwise we add a bishop.
                if (CurrentQueens < QueensLimit) {
                    PlacePiece(y, x, bQueen);
                    CurrentQueens++;
                } else {
                    PlacePiece(y, x, bBishop);
                    CurrentBishops++;
                }

                MinBishops(CurrentQueens, CurrentBishops);

                // we remove the piece we placed in this recursion loop, in this way more solutions can be discovered.
                if(RemovePiece(y, x)) { //we keep track of the ammount of queens and bishop on the chessboard.
                    CurrentQueens--;
                } else {
                    CurrentBishops--;
                }
            }
        }
    }
}

//Recursive algorithm to find the ammount of possible solution for a given amount of Queens and Bishops.
void findNmbSolutions(int CurrentQueens, int CurrentBishops) {
    if (CurrentQueens + CurrentBishops > PieceLimit) {
        // stop recursion, we have to many pieces.
        // also alarm the user of this unnecessary recursion happening.
        cout << "TO MANY PIECES ON CHESSBOARD, QUITTING THIS RECURSION LOOP"<< endl;
        return;
    }

    if (CurrentQueens + CurrentBishops == PieceLimit) {
        if (boardIsCovered()) {
            // we have found a solution
            nmbSolutions++;  //increment amount of solutions

            //save the first solution:
            if (nmbSolutions == 1) {
                Chessboard_best_ptr = copyChessboard(Chessboard_ptr, M * N);
            }

            //show solution (for debugging reasons):
//            cout << "\nSolution " << nmbSolutions << " is:\n";
//            displayChessBoard(Chessboard_ptr);
        } else {
            ;//this is not a solution, quit this loop and try another one
        }
        return;
    } else {//Not all piece are on the board yet so continue adding piece and deepen the level of recursion
        for (int y = 0; y < M; y++) {
            for (int x = 0; x<N; x++) {
                if (Chessboard_ptr[x + y * N] == '-') {
                    // When there is a square that is not in use we place a queen if we haven't reached the QueenLimit.
                    // So that we use the minimum amount of bishops
                    // Otherwise we add a bishop.
                    if (CurrentQueens < QueensLimit) {
                        PlacePiece(y, x, bQueen);
                        CurrentQueens++;
                    } else if (CurrentBishops < BishopLimit) {
                        PlacePiece(y, x, bBishop);
                        CurrentBishops++;
                    } else {
                        //we are out of pieces but we have not found a solution
                        // it should not be possible to reach this state, so notify user if we end up here.
                        cout << "OUT OF PIECES" << endl;
                    }

                    findNmbSolutions(CurrentQueens, CurrentBishops);

                    // we remove the piece we placed in this recursion loop, in this way more solutions can be discovered.
                    if(RemovePiece(y, x)) { //we keep track of the ammount of queens and bishop on the chessboard.
                        CurrentQueens--;
                    } else {
                        CurrentBishops--;
                    }
                }
            }
        }
    }
}


void PlacePiece(int y, int x, bool Queen_Bishop) {
    char Piece = '-';
    Piece = (Queen_Bishop) ? 'Q' : 'B';
    // decide which piece
    Chessboard_ptr[x + y * N] = Piece;
    UpdateCoverage();
}


bool RemovePiece(int y, int x) {
    char Piece = Chessboard_ptr[x + y * N];
    bool Queen_Bishop;
    if (Piece == 'Q') {
        Queen_Bishop = true;
    } else if (Piece == 'B') {
        Queen_Bishop = false;
    } else {
        cout << "\n\nError! You tried removing a non-existing piece!!\n\n";
        return false;  // Return false to indicate failure
    }
    Chessboard_ptr[x + y * N] = '-';  // remove piece from board
    //UpdateCoverage();
    return Queen_Bishop;
}

void UpdateCoverage() {
    //clean CoverageBoard
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            CoverageBoard_ptr[x + y * N] = false;
        }
    }

    //mark all the covered positions
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            if (Chessboard_ptr[x + y * N] != '-') { //if not blank space
                //cover the spot of the piece
                CoverageBoard_ptr[x + y * N] = true;
                //Queen and Bishop both make diagonal line, so we can draw them regardless of which piece it is.
                // make diagonal line left upwards
                for (int i = 1; (y - i >= 0) && (x - i >= 0); i++) {
                    CoverageBoard_ptr[(x - i) + (y - i) * N] = true;
                }

                // make diagonal line right upwards
                for (int i = 1; (y - i >= 0) && (x + i < N); i++) {
                    CoverageBoard_ptr[(x + i) + (y - i) * N] = true;
                }

                // make diagonal line left downwards
                for (int i = 1; (y + i < M) && (x - i >= 0); i++) {
                    CoverageBoard_ptr[(x - i) + (y + i) * N] = true;
                }

                // make diagonal line right downwards
                for (int i = 1; (y + i < M) && (x + i < N); i++) {
                    CoverageBoard_ptr[(x + i) + (y + i) * N] = true;
                }

                if (Chessboard_ptr[x + y * N] == 'Q') {
                    //Also add the horizontal & vertical lines when the piece is a queen
                    // left horizontal
                    for (int i = 1; (x - i >= 0) && (Chessboard_ptr[(x - i) + y * N] == '-'); i++) {
                        CoverageBoard_ptr[(x - i) + y * N] = true;
                    }

                    // right horizontal
                    for (int i = 1; (x + i < N) && (Chessboard_ptr[(x + i) + y * N] == '-'); i++) {
                        CoverageBoard_ptr[(x + i) + y * N] = true;
                    }

                    // upwards vertical
                    for (int i = 1; (y - i >= 0) && (Chessboard_ptr[x + (y - i) * N] == '-'); i++) {
                        CoverageBoard_ptr[x + (y - i) * N] = true;
                    }

                    // downwards vertical
                    for (int i = 1; (y + i < M) && (Chessboard_ptr[x + (y + i) * N] == '-'); i++) {
                        CoverageBoard_ptr[x + (y + i) * N] = true;
                    }
                }
            }
        }
    }
}

bool boardIsCovered() {
    bool board_covered = true;

    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            if (!CoverageBoard_ptr[x + y * N]) {
                board_covered = false;
                y = M;
                break;
            }
        }
    }
    return board_covered;
}


char* copyChessboard(const char* source, int size) {
    char* destination = new char[size];
    copy(source, source + size, destination);
    return destination;
}

//function to display the ChessBoard
void displayChessBoard(char* board_ptr) {
    for (int y = 0; y < M; y++) {
        cout << endl;
        for (int x = 0; x < N; x++) {
            cout << board_ptr[x + y * N] << "  ";
        }
    }
    cout << endl;
}

void displayCoverage() {
    for (int y = 0; y < M; y++) {
        cout << endl;
        for (int x = 0; x < N; x++) {
            cout << CoverageBoard_ptr[x + y * N] << "  ";
        }
    }
    cout << endl;
}


int main() {
    //start screen
    cout << "--------------------------------------------" << endl;
    cout << "\t\tChess coverage problem" << endl;
    cout << "--------------------------------------------" << endl;

    //ask user to input chessboard size
    string input;
    cout << "Enter the size of the chessboard in the form MxN: " << endl;
    cin >> input;

    // Use a stringstream to parse the input string
    istringstream iss(input);

    // Read M
    if (!(iss >> M)) {
        cerr << "Invalid input for M." << endl;
        return 1;
    }

    // Check for 'x'
    char x;
    if (!(iss >> x) || x != 'x') {
        cerr << "Invalid input. Expected 'x' after M." << endl;
        return 1;
    }

    // Read N
    if (!(iss >> N)) {
        cerr << "Invalid input for N." << endl;
        return 1;
    }

    //print chosen board size
    cout << endl << "Board is an " << M << "x" << N << " board" << endl;

    //cout << typeid(M).name() << endl; // returns i for integers

    //Initialize chessboard.
    //Contains chessboard of MxN (Y*X) size,
    Chessboard_ptr = new char[M * N]; //Keeps track of the location of the pieces
    CoverageBoard_ptr = new bool[M * N]; //keeps track of the covered squares;

    // Initialize the chessboard with a default value (Could maybe be done with memset as well...)
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            Chessboard_ptr[x + y * N] = '-';
            CoverageBoard_ptr[x + y * N] = false;
        }
    }

    //cout ask user if the user wants to find minimum ammount of bishops or the amount of solutions to cover a chessboard:
    cout << endl;
    cout << "Choose what you want to find:" << endl;
    cout << "1. Find the number of solution to cover the chessboard.\n"
            "   You enter the amount of Queens and bishops " << endl;
    cout << "2. Find the minimum number of bishops necessary to cover the entire chessboard\n"
            "   You enter the amount of queens that may be used." << endl;

    //ask user to choose an algorithm:
    cout << "\nEnter 1 or 2 and press enter\n";
    cin >> ChosenOption;

    if ((ChosenOption < 1) || (ChosenOption > 2)) {
        // user has entered invalid input
        // notify user and end program
        cout << "Entered invalid option, ending program!" << endl;
        return -1;
    }

    //ask the user to enter additional information which is neccesary for the algorithm
    cout << "\nEnter the amount of Queens: \n";
    cin >> QueensLimit;
    cout << "QueensLimit is: " << QueensLimit << endl;

    if (ChosenOption == 1) {
        cout << "\nEnter the amount of Bishops: \n";
        cin >> BishopLimit;
        cout << "BishopLimit is: " << BishopLimit << endl;
        PieceLimit = QueensLimit+BishopLimit; //decide the piece limit
    }

    //start timer
    auto start = high_resolution_clock::now(); //start timer
    auto stop = high_resolution_clock::now(); //initialize stop variable

    switch (ChosenOption) {
        case 1:
            findNmbSolutions(0, 0);
            stop = high_resolution_clock::now(); //stop timer
            cout << "There are " << nmbSolutions << " Solution(s)." << endl;
            if (nmbSolutions > 0) {
                cout << "This is 1 valid solution: " <<endl;
                displayChessBoard(Chessboard_best_ptr);
            }

            break;
        case 2:
            MinBishops(0, 0); //run the recursion algorithm
            stop = high_resolution_clock::now(); //stop timer

            //show final results
            cout << endl << "Found a solution!:" << endl;
            cout << "Queens: "<< QueensLimit << endl;
            cout << "Bishops: "<< BishopLimit << endl;
            cout << endl;
            displayChessBoard(Chessboard_best_ptr);
            cout << endl;
            break;
    }

    auto ExecutionTime = duration_cast<milliseconds>(stop - start);

    cout << "Execution time was: " << ExecutionTime.count() << " ms" << endl;

    //free memory
    delete[] Chessboard_ptr;
    delete[] CoverageBoard_ptr;
    delete[] Chessboard_best_ptr;

    return 0;
}