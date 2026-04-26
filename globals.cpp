#include "c_h.hpp"
using namespace std;
char board[9][9]={'.'};
char PIECES[2][7]={{'R','N','B','K','Q','P'},{'r','n','b','k','q','p'}};
bool kingmoved[2]={0};
bool rookmoved[2][2]={0};
bool kingattacked[2]={0};
int enPassantTargetRank[2][8]={0};//[COLOR] [FILE]= [RANK]
bool epAvailable = false;
int epTargetRank = 0;
int epTargetFile = 0;
int king_pos[2][2]={{1,5},{8,5}};//white and black king
     
Move mystruct;        

// GUI integration defaults
bool guiMode = false;
char promotionChoice = 'Q';
std::vector<std::pair<int,int>> guiMoves;
bool lastWasCastle = false;
int rookFromRank = 0, rookFromFile = 0, rookToRank = 0, rookToFile = 0;
bool lastWasEnPassant = false;
int epCapturedRank = 0, epCapturedFile = 0;
char epCapturedPiece = '.';