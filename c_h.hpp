/*using 8*8 array for storing chess board*/
#ifndef chess_h
#define chess_h
#include<iostream>
#include<cstdlib>
#include <cmath>
#include <vector>
#include<string>
#include<stdio.h>
#include <stack>
#include <utility>
extern char board[9][9];
extern bool kingmoved[2];
extern bool kingattacked[2];
extern bool rookmoved[2][2];
extern int king_pos[2][2];//white and black king



extern int enPassantTargetRank[2][8];//[COLOR] [FILE]= [RANK]
extern bool epAvailable;
extern int epTargetRank;
extern int epTargetFile;

//enum P{wROOK=1,wBISHOP,wKNIGHT,wKING,wQUEEN,wPAWN,bROOK,bBISHOP,bKNIGHT,bKING,bQUEEN,bPAWN};
extern char PIECES[2][7];
enum File{FILE_NONE=0,FILE_A,FILE_B,FILE_C,FILE_D,FILE_E,FILE_F,FILE_G,FILE_H};//012345678
enum Rank{RANK_NONE=0,RANK_1,RANK_2,RANK_3,RANK_4,RANK_5,RANK_6,RANK_7,RANK_8};
enum COLOR{WHITE=0,BLACK,GRAY};

// GUI integration flags
extern bool guiMode;
extern char promotionChoice; // 'Q','R','B','N' for white; lowercase for black
extern std::vector<std::pair<int,int>> guiMoves; // list of (rank,file) destinations for selected piece
inline void gui_clear_moves(){ guiMoves.clear(); }
inline void gui_add_move(int rank, int file){ if(rank>=RANK_1&&rank<=RANK_8&&file>=FILE_A&&file<=FILE_H) guiMoves.emplace_back(rank,file); }

// Special-move side-effects tracking for proper undo
extern bool lastWasCastle;
extern int rookFromRank, rookFromFile, rookToRank, rookToFile;
extern bool lastWasEnPassant;
extern int epCapturedRank, epCapturedFile;
extern char epCapturedPiece;
extern bool epAvailable;
extern int epTargetRank, epTargetFile; // square the capturing pawn moves to diagonally when en passant


void printboard(char board[9][9]);
bool parseMovein(std::string input,int &rank, int &file) ;//user input to file ,rank
bool parseMoveout(std::string input,int &drank, int &dfile) ;


void moves_generator(char board[9][9],int rank,int file,int color);//generate possible moves
//void next_input(char board[9][9],int rank,int file,int drank,int dfile,int color);
void commit_move(char board[9][9], int rank, int drank, int file, int dfile, int color);
bool isSamePiece(int rank, int file, int color);
bool moveGeneral(char board[9][9],int rank,int file,int color);
//moves decleration
void pawnmove(char board[9][9],int rank,int file,int color);
void knightmove(char board[9][9],int rank,int file,int color);

void RQBDirection(char board[9][9], int rank, int file, int color, int dRank, int dFile);
void bishopMoves(char board[9][9], int rank, int file, int color);
void rookmoves(char board[9][9], int rank,int file, int color);
void queenmoves(char board[9][9], int rank,int dfile, int color);

void kingmoves(char board[9][9], int rank, int file, int color,bool kingmoved[2]);
void kingcastle(char board[9][9], int rank, int file,int color,bool kingmoved[2],bool kingattacked[2]);
bool isSquareAttacked(char board[9][9], int rank, int file, int color);

bool check_king_attack(char board[9][9],int color,int king_pos[2][2]);


struct Move {
    int srank, sfile;
    int sdrank, sdfile;
    char moved;     // piece moved
    char captured;  // piece captured
};
extern Move mystruct;  

void makeMove(char board[9][9], Move &m,int rank,int drank,int file,int dfile);
void undoMove(char board[9][9]);
bool canUndo();
#endif