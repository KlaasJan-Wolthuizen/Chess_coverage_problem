#include <iostream>
#include <ostream>
#include <chrono>

using namespace std::chrono;
using namespace std;

//definitions for better read-ability
#define bQueen true
#define bBishop false

//headers....
void displayChessBoard();
void displayCoverage();
void MinBishops(int CurrentQueens, int CurrentBishops);

//bool isAttacked(int y, int x);
void PlacePiece(int y, int x, bool Queen_Bishop);
bool RemovePiece(int y, int x);
void UpdateCoverage();


//Global variables:
int M = 0, N = 0; //stores the dimensions of the chessboard.
int QueensLimit = INT_MAX, BishopLimit = INT_MAX;
char *Chessboard_ptr = nullptr;
bool *CoverageBoard_ptr = nullptr;
//char * Chessboard_res_ptr = nullptr;
//uint8_t * Coverageboard_res_ptr = nullptr;

int PieceLimit = INT_MAX;
//int depth = 0;
//int step = 0;

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

    if (board_covered) {
        // we have found a solution.
        //Update the best solution, and the PieceLimit, which limits the amount of further recursion
        PieceLimit = CurrentQueens + CurrentBishops;
        QueensLimit = CurrentQueens; //update minimum amount of queens
        BishopLimit = CurrentBishops; //update minimum amount of bishops

        cout << endl << "Found a solution!:" << endl;
        cout << "Queens: "<< QueensLimit << endl;
        cout << "Bishops: "<< BishopLimit << endl;
        cout << "PieceLimit " << PieceLimit << endl;
        cout << endl;
        displayChessBoard(); //show position of pieces
        cout << endl;
        displayCoverage(); //show the board to check that all sqaures are covered.
        cout << endl << endl;
        return; //prevent further recursion
    }

    for (int y = 0; y < M; y++) {
        for (int x = 0; x<N; x++) {
            if (Chessboard_ptr[x + y * N] == '-') {
                // When there is a square that is not under attack we place a queen if we haven't reached the QueenLimit.
                // So that we use the minimum amount of bishops
                // Otherwise we add a bishop.

                if (CurrentQueens+CurrentBishops >= PieceLimit-1) {
                    //adding pieces wont result in shorter solution, so we should quit this loop;
                    return;
                }

                if (CurrentQueens < QueensLimit) {
                    PlacePiece(y, x, bQueen);
                    CurrentQueens++;
                } else {
                    PlacePiece(y, x, bBishop);
                    CurrentBishops++;
                }


                MinBishops(CurrentQueens, CurrentBishops);

                // we remove the piece we placed in this recursion loop, in this way more solutions can be discovered.
                if(RemovePiece(y, x)) {
                    CurrentQueens--;
                } else {
                    CurrentBishops--;
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
    UpdateCoverage();
    return Queen_Bishop;
}

void UpdateCoverage() {
    //clean CoverageBoard
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            CoverageBoard_ptr[x + y * N] = false;
        }
    }

    //notedown covered positions
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


//function to display the ChessBoard
void displayChessBoard() {
    for (int y = 0; y < M; y++) {
        cout << endl;
        for (int x = 0; x < N; x++) {
            cout << Chessboard_ptr[x + y * N] << " ";
        }
    }
    cout << endl;
}

void displayCoverage() {
    for (int y = 0; y < M; y++) {
        cout << endl;
        for (int x = 0; x < N; x++) {
            cout << CoverageBoard_ptr[x + y * N] << " ";
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
    cout << "Enter the size of the chess board in het form MxN: ";
    cin >> M >> N;

    //print chosen board size
    cout << endl << "Board is an " << M << "x" << N << " board" << endl;
    //cout << typeid(M).name() << endl; // returns i for integers

    //Initialize chessboard.
    //Contains chessboard of MxN (Y*X) size,
    Chessboard_ptr = new char[M * N]; //Keeps track of the location of the pieces
    CoverageBoard_ptr = new bool[M * N]; //keeps track of the covered squares;

//    Chessboard_res_ptr = new char[M * N]; //Keeps track of the location of the pieces
//    Coverageboard_res_ptr = new bool [M * N]; //keeps track of the covered squares;

    // Initialize the chessboard with a default value (Could maybe be done with memset as well...)
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            Chessboard_ptr[x + y * N] = '-';
            CoverageBoard_ptr[x + y * N] = false;
        }
    }

    cout << "Enter the amount of Queens: \n";
    cin >> QueensLimit;
    cout << "QueensLimit is: " << QueensLimit << endl;

    //start timer
    auto start = high_resolution_clock::now();
    MinBishops(0, 0); //run the recursion algorithm
    auto stop = high_resolution_clock::now();
    auto ExecutionTime = duration_cast<milliseconds>(stop - start);

    cout << "Execution time is: " << ExecutionTime.count() << "ms" << endl;


    delete[] Chessboard_ptr;
    delete[] CoverageBoard_ptr;

    return 0;
}