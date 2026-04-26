#include "c_h.hpp"
using namespace std;
//rank=row   file=col
//possible move


void possible_move(int file ,int rank,int x,int y){
        cout<<"possible moves:"<<char('a'+file-1)<<"_"<<rank<<"to "<<char('a'+file-1+x)<<"_"<<rank+y;
        cout<<endl;
        if (guiMode) {
            int drank = rank + y;
            int dfile = file + x;
            gui_add_move(drank, dfile);
        }
}

bool check_king_attack(char board[9][9],int color,int king_pos[2][2]){
    int rank=king_pos[color][0];
    int file=king_pos[color][1];
    kingattacked[color]=0;
    if(isSquareAttacked(board, rank, file, color)){
        kingattacked[color]=1;
        return true;
    }

    return false;
}
//generate possible moves
void moves_generator(char board[9][9],int rank,int file,int color){
    if(board[rank][file]=='.'){
        cout << "Invalid input format. Try again.\n";
        return;
    }
    char temp=tolower(board[rank][file]);
    if(temp=='p'){
        pawnmove(board,rank,file,color);
    }
    else if(temp=='n'){
        knightmove(board,rank,file,color);
    }
    else if(temp=='b'){
        bishopMoves(board,rank,file,color);
    }
    else if(temp=='r'){
        rookmoves(board,rank,file,color);
    }
    else if(temp=='q'){
        queenmoves(board,rank,file,color);
    }
    else if(temp=='k'){
        kingmoves(board,rank,file,color,kingmoved);
    }
}
//to commit move

bool isSamePiece(int rank, int file, int color){
    if(color == WHITE) return isupper(board[rank][file]);
    else return islower(board[rank][file]);
}
bool isOpponentPiece(int rank, int file, int color){
    if (board[rank][file]=='K'){
        kingattacked[WHITE]=1;
    }
    else if(board[rank][file]=='k'){
        kingmoved[BLACK]=1;
    }if(color == WHITE) {      
        return islower(board[rank][file]);}
    else{
        return isupper(board[rank][file]);}
}
bool moveGeneral(char board[9][9],int rank,int file,int color){
    if((rank>RANK_8)||(rank<RANK_1)||(file<FILE_A)||(file>FILE_H)){
        return false;
    }
    if(isOpponentPiece(rank,file,color)){
        cout<<"capture at "<<char('a'+file-1)<<"_"<<rank<<endl;
        if (guiMode) { gui_add_move(rank, file); }
        return false;
    }
    if(isSamePiece(rank,file,color)){
        return false;
    }
    cout<<"possible move to "<<char('a'+file-1)<<"_"<<rank<<endl;
    if (guiMode) { gui_add_move(rank, file); }
    return true;
}

void pawnmove(char board[9][9],int rank,int file,int color){
    if(color==WHITE){
        //starting 2 steps move for white
    if (rank==RANK_2){
        if (board[rank+1][file]=='.'){
            if (board[rank+2][file]=='.'){
                possible_move(file,rank,0,2);
                }
        }
    }
    //TO MOVE 1 forward white
    if ((rank<RANK_8)&&board[rank+1][file]=='.'){
        possible_move(file ,rank,0,1);
        }

    //to capture black pieces
    if((file<FILE_H) &&(rank<RANK_8)&&(board[rank+1][file+1]!='.')&& (islower(board[rank+1][file+1]))){
        possible_move(file ,rank,1,1);//right diagonal
    }
    if((file>FILE_A) &&(rank<RANK_8)&&(board[rank+1][file-1]!='.')&&(islower(board[rank+1][file-1]))){
        possible_move(file ,rank,-1,1);//left diagonal
    }
        //en passant capture
    if((rank==RANK_5)){
        // White can capture a black pawn that just double-stepped (adjacent 'p')
        if(file<FILE_H && board[rank][file+1]=='p' && epAvailable && epTargetRank==rank && epTargetFile==file+1){
            possible_move(file ,rank,1,1);
        }
        if(file>FILE_A && board[rank][file-1]=='p' && epAvailable && epTargetRank==rank && epTargetFile==file-1){
            possible_move(file ,rank,-1,1);
        }
    }
    }
    
    else if ((color==BLACK)){
        //two move starting black
        if((rank==RANK_7) ){
        if (board[rank-1][file]=='.'){
            if (board[rank-2][file]=='.'){
                possible_move(file ,rank,0,-2);
                }
        }
        }
        if ((rank>RANK_1)&&board[rank-1][file]=='.'){
            possible_move(file ,rank,0,-1);//TO MOVE 1 forward black
        }

            //capture white
            if((file<FILE_H)&&(rank>RANK_1)&&(board[rank-1][file+1]!='.')&& (isupper(board[rank-1][file+1]))){
                possible_move(file ,rank,1,-1);
            }
            if((file>FILE_A) &&(rank>RANK_1)&&(board[rank-1][file-1]!='.')&&(isupper(board[rank-1][file-1]))){
                possible_move(file ,rank,-1,-1);
            }
            //capture en passant
            if((rank==RANK_4)){
            if(file<FILE_H && board[rank][file+1]=='P' && epAvailable && epTargetRank==rank && epTargetFile==file+1){
                possible_move(file ,rank,1,-1);
            }
            if(file>FILE_A && board[rank][file-1]=='P' && epAvailable && epTargetRank==rank && epTargetFile==file-1){
                possible_move(file ,rank,-1,-1);
            }
        }
            //promotion

    }

    
}

void knightmove(char board[9][9],int rank,int file,int color){
        //row,col
        //north east(2,1)(1,2)
        if((rank<RANK_7)&&(file<FILE_H)&&((board[rank+2][file+1]=='.')||(isOpponentPiece(rank+2,file+1,color)))){
            possible_move(file ,rank,1,2);}
        if((rank<RANK_8)&&(file<FILE_G)&&((board[rank+1][file+2]=='.')||(isOpponentPiece(rank+1,file+2,color)))){
            possible_move(file ,rank,2,1);
            }
        //south east(-1,2)(-2,1)
        if((rank>RANK_1)&&(file<FILE_G)&&((board[rank-1][file+2]=='.')||(isOpponentPiece(rank-1,file+2,color)))){
            possible_move(file ,rank,2,-1);}
        if((rank>RANK_2)&&(file<FILE_H)&&((board[rank-2][file+1]=='.')||(isOpponentPiece(rank-2,file+1,color)))){
            possible_move(file ,rank,1,-2);}
        //south west(-2,-1)(-1,-2)
        if((rank>RANK_2)&&(file>FILE_A)&&((board[rank-2][file-1]=='.')||(isOpponentPiece(rank-2,file-1,color)))){
            possible_move(file ,rank,-1,-2);
        }
        if((rank>RANK_1)&&(file>FILE_B)&&((board[rank-1][file-2]=='.')||(isOpponentPiece(rank-1,file-2,color)))){
            possible_move(file ,rank,-2,-1);}
        //north west(1,-2)(2,-1)
        if((rank<RANK_8)&&(file>FILE_B)&&((board[rank+1][file-2]=='.')||(isOpponentPiece(rank+1,file-2,color)))){
            possible_move(file ,rank,-2,+1);}
        if((rank<RANK_7)&&(file>FILE_A)&&((board[rank+2][file-1]=='.')||(isOpponentPiece(rank+2,file-1,color)))){
            possible_move(file ,rank,-1,2);}
    
}
void RQBDirection(char board[9][9], int rank, int file, int color, int dRank, int dFile) {
    int newRank = rank + dRank;
    int newFile = file + dFile;

    if (moveGeneral(board, newRank, newFile, color)) {
        RQBDirection(board, newRank, newFile, color, dRank, dFile);
    }
}
void bishopMoves(char board[9][9], int rank, int file, int color) {
    RQBDirection(board, rank, file, color, +1, +1); // NE
    RQBDirection(board, rank, file, color, -1, -1); // SW
    RQBDirection(board, rank, file, color, +1, -1); // NW
    RQBDirection(board, rank, file, color, -1, +1); // SE
    


}
void rookmoves(char board[9][9], int rank, int file, int color){
    RQBDirection(board, rank, file, color, +1, 0); // N
    RQBDirection(board, rank, file, color, -1, 0); // S
    RQBDirection(board, rank, file, color, 0, -1); // W
    RQBDirection(board, rank, file, color, 0, +1); // E

 
}
void queenmoves(char board[9][9], int rank, int file, int color){
    rookmoves( board, rank, file, color);
    bishopMoves(board, rank, file, color);

}


//check
void kingmoves(char board[9][9], int rank, int file, int color,bool kingmoved[2]){
   int moves[8][2] = {
        {+1, 0}, {-1, 0},  // up, down
        {0, +1}, {0, -1},  // right, left
        {+1, +1}, {+1, -1}, // up-right, up-left
        {-1, +1}, {-1, -1}  // down-right, down-left
    };
    for(int i=0;i<8;i++){
        moveGeneral(board, rank+moves[i][0], file+moves[i][1], color);
    }
    kingcastle(board,rank, file, color, kingmoved,kingattacked);
/*8   cout<<"perform castle?(y/n):";
    char yn;
    cin>>yn;
    
   
        if(yn){
        kingmoved[color]=1;
        if(dfile==FILE_G){commit_move(board,rank,drank,file+3,dfile-1,color);}//RIGHT SIDE rook
        if(dfile==FILE_A){commit_move(board,rank,drank,file-2,dfile+1,color);}//RIGHT SIDE ROOK
    }
    */
}

//above code contains simple moves for pawn,knight,bishop,king,queen
//code for castling,check

bool isSquareAttacked(char board[9][9], int rank, int file, int color) {
    int opponent = 1 - color;
    // --- Knights ---
    int knightMoves[8][2] = {{+2,+1},{+2,-1},{-2,+1},{-2,-1},{+1,+2},{+1,-2},{-1,+2},{-1,-2}};
    for (auto &m : knightMoves) {
        int r = rank + m[0], f = file + m[1];
        if (r>=RANK_1 && r<=RANK_8 && f>=FILE_A && f<=FILE_H) {
            if ((opponent==WHITE && board[r][f]=='n') ||(opponent==BLACK && board[r][f]=='N'))
                return true;
        }
    }

    // --- Pawns ---
    if (opponent == BLACK) { // black pawns attack downwards
        if ((rank+1<=RANK_8 && file-1>=FILE_A  && board[rank+1][file-1]=='p') ||
            (rank+1<=RANK_8 && file+1<=FILE_H && board[rank+1][file+1]=='p'))
            return true;
    } 
    else  if (opponent == WHITE){ // white pawns attack upwards
        if ((rank-1>=RANK_1 && file-1>=FILE_A  && board[rank-1][file-1]=='P') ||
            (rank-1>=RANK_1 && file+1<=FILE_H && board[rank-1][file+1]=='P'))
            return true;
    }

    // --- Sliding pieces (rook/queen) ---
    int rookDirs[4][2]={{+1,0},{-1,0},{0,+1},{0,-1}};
    for (auto &d : rookDirs) {
        int r=rank, f=file;
        while (true) {
            r+=d[0]; f+=d[1];
            if (r<RANK_1||r>RANK_8||f<FILE_A||f>FILE_H) break;
            if (board[r][f] != '.') {
                if ((opponent==BLACK && (board[r][f]=='r'||board[r][f]=='q')) ||
                    (opponent==WHITE && (board[r][f]=='R'||board[r][f]=='Q')))
                    return true;
                break;
            }
        }
    }
    // --- Sliding pieces (bishop/queen) ---
    int bishopDirs[4][2]={{+1,+1},{+1,-1},{-1,+1},{-1,-1}};
    for (auto &d : bishopDirs) {
        int r=rank, f=file;
        while (true) {
            r+=d[0]; f+=d[1];
            if (r<RANK_1||r>RANK_8||f<FILE_A||f>FILE_H) break;
            if (board[r][f] != '.') {
                if ((opponent==BLACK && (board[r][f]=='b'||board[r][f]=='q')) ||
                    (opponent==WHITE && (board[r][f]=='B'||board[r][f]=='Q')))
                    return true;
                break;
            }
        }
    }
    // --- Opponent king (edge case) ---
    int kingMoves[8][2]={{+1,0},{-1,0},{0,+1},{0,-1},{+1,+1},{+1,-1},{-1,+1},{-1,-1}};
    for (auto &m : kingMoves) {
        int r=rank+m[0], f=file+m[1];
        if (r>=RANK_1 && r<=RANK_8 && f>=FILE_A && f<=FILE_H) {
            if ((opponent==BLACK && board[r][f]=='k') ||
                (opponent==WHITE && board[r][f]=='K'))
                return true;
        }
    }
    return false;
}



void kingcastle(char board[9][9], int rank, int file,int color,bool kingmoved[2],bool kingattacked[2]){ 
    if (kingattacked[color]){
         return; } 
    if (kingmoved[color]){ 
        return; } 
    int x = (color == 0 ? 1 : 8); 
    if(rank!=x){
        //current rank 
        return; } 
        if(file!=5){ return; } 
        //checking rook doesnt move,sq empty and not in check 
        //queen side
        if (board[x][1] != (color==0 ? 'R' : 'r')) return;  // queenside rook 
        if (!rookmoved[color][0]) {
    if (board[x][2]=='.' && board[x][3]=='.' && board[x][4]=='.') {
        if (!isSquareAttacked(board, x, 5, color) &&!isSquareAttacked(board, x, 4, color) &&   // d-square
            !isSquareAttacked(board, x, 3, color)) {   // c-square
            cout<<"castle move to: c_"<<x<<endl;
            if (guiMode) { gui_add_move(x, FILE_C); }
        }
    }
}
// Kingside
if (board[x][8] != (color==0 ? 'R' : 'r')) return;  // kingside rook
if (!rookmoved[color][1]) {
    if (board[x][6]=='.' && board[x][7]=='.') {
        if (!isSquareAttacked(board, x, 5, color) &&!isSquareAttacked(board, x, 6, color) &&   // f-square
            !isSquareAttacked(board, x, 7, color)) {   // g-square
            cout<<"castle move to: g_"<<x<<endl;
            if (guiMode) { gui_add_move(x, FILE_G); }
        }
    }
}
}