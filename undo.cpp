#include"c_h.hpp"

using namespace std;
stack<Move> moveHistory;
void makeMove(char board[9][9],Move &m,int rank,int drank,int file,int dfile) {
    m.srank=rank;
    m.sdrank=drank;
    m.sdfile=dfile;
    m.sfile=file;
    m.moved=board[m.srank][m.sfile];
    m.captured=board[m.sdrank][m.sdfile];
    moveHistory.push(m);
    board[m.sdrank][m.sdfile]=m.moved;
    board[m.srank][m.sfile] = '.';
 
}

void undoMove(char board[9][9]) {
    Move m=moveHistory.top();
    moveHistory.pop();

    board[m.srank][m.sfile] = m.moved;
    board[m.sdrank][m.sdfile]= m.captured;
    // Handle special side-effects
    if (lastWasEnPassant) {
        if (epCapturedRank>=RANK_1 && epCapturedRank<=RANK_8 && epCapturedFile>=FILE_A && epCapturedFile<=FILE_H) {
            board[epCapturedRank][epCapturedFile] = epCapturedPiece;
        }
        lastWasEnPassant = false;
        epCapturedPiece = '.';
    }
    if (lastWasCastle) {
        // Move rook back to original square
        if (rookFromRank && rookFromFile && rookToRank && rookToFile) {
            board[rookFromRank][rookFromFile] = board[rookToRank][rookToFile];
            board[rookToRank][rookToFile] = '.';
        }
        lastWasCastle = false;
        rookFromRank = rookFromFile = rookToRank = rookToFile = 0;
    }
}
bool canUndo(){
    return !moveHistory.empty();
}
/*int minimax(char board[9][9], int depth, bool isMaximizing) {
    if (depth == 0) {
        return evaluate(board);  // your evaluation function
    }

    vector<Move> moves = generateMoves(board); // generate all legal moves

    int bestValue = (isMaximizing ? -99999 : 99999);

    for (Move move : moves) {
        makeMove(board, move);              // push move
        int value = minimax(board, depth-1, !isMaximizing);  // recurse
        undoMove(board);                    // pop move (backtrack)

        if (isMaximizing)
            bestValue = std::max(bestValue, value);
        else
            bestValue = std::min(bestValue, value);
    }

    return bestValue;
}
*/