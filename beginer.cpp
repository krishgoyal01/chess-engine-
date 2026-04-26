#include"c_h.hpp"
using namespace std;

void printboard(char board[9][9]){
    for (int rank = RANK_8; rank>RANK_NONE; rank--) {
    for (int file = FILE_A; file <= FILE_H; file++) {
    cout<<board[rank][file]<<" ";
 }
 cout<<endl;
}
}

bool parseMovein(std::string input,int &rank, int &file) {

    file = input[0]- 'a' + 1;   // 'a'→1 ... 'h'→8
    rank = input[1] - '0';       // '1'→1 ... '8'→8

    return (file >= 1 && file <= 8 &&
            rank >= 1 && rank <= 8 );
}
bool parseMoveout(std::string input,int &drank, int &dfile) {
    dfile = input[0] - 'a' + 1;
    drank = input[1] - '0';

    return ( dfile >= 1 && dfile <= 8 &&
            drank >= 1 && drank <= 8);
}
bool tryMove(char board[9][9], int rank, int file, int drank, int dfile, int color) {
    char piece = board[rank][file];
    if (piece == '.' || (color == WHITE && islower(piece)) || (color == BLACK && isupper(piece))) {
        return false; // not your piece
    }
    // save board
    Move backup;
    makeMove(board, backup, rank, drank, file, dfile);
    // check if own king is in check
    if (check_king_attack(board,color,king_pos )) {
        undoMove(board);
        return false; // illegal move
   }
    return true;
}

int main(){
    //initialize
for (int rank = RANK_8; rank>RANK_NONE; rank--) {
    for (int file = FILE_A; file <=FILE_H; file++) {
        if((rank==RANK_7)){
            board[rank][file]=PIECES[1][5];//black pawns
        }
        else if((rank==RANK_2)){
            board[rank][file]=PIECES[0][5];//white pawns
        }
        else if((rank==RANK_1)){
            if((file==FILE_A)||(file==FILE_H)){
                board[rank][file]=PIECES[0][0];//b rook
            }
            else if((file==FILE_B)||(file==FILE_G)){
                board[rank][file]=PIECES[0][1];//knight
            }
            else if((file==FILE_C)||(file==FILE_F)){
                board[rank][file]=PIECES[0][2];//BISHOP
            }
            else if((file==FILE_D)){
                board[rank][file]=PIECES[0][4];//QUEEN;
            }
            else if((file==FILE_E)){
                board[rank][file]=PIECES[0][3];//KING;
            }
        }
        else if((rank==RANK_8)){
        if((file==FILE_A)||(file==FILE_H)){
            board[rank][file]=PIECES[1][0];// rook
        }
        else if((file==FILE_B)||(file==FILE_G)){
            board[rank][file]=PIECES[1][1];//knight
        }
        else if((file==FILE_C)||(file==FILE_F)){
            board[rank][file]=PIECES[1][2];//BISHOP
        }
        else if((file==FILE_D)){
            board[rank][file]=PIECES[1][4];//QUEEN;
        }
        else if((file==FILE_E)){
            board[rank][file]=PIECES[1][3];//KING;
        }
    }
        else{
            board[rank][file]='.';
        }
    
 }
 }

int turn = WHITE; // start with White

while (true) {
    printboard(board);
    cout << (turn == WHITE ? "White" : "Black") << "'s piece file+rank(like e2): ";
    std::string smove;
    int cr,cf;//current rank and file,
    cin >> smove;
    if ((smove[0]=='0')&&(smove[1]=='0')) break;
    // parse move (next step)
    
    if (!parseMovein(smove,cr, cf)) {//check piece move is contained in board
        cout << "Invalid input format. Try again.\n";
        continue;
    }
    if(((turn==WHITE)&&(islower(board[cr][cf])))|| ((turn==BLACK)&&(isupper(board[cr][cf])))   ){
        cout << "Illegal move. Try again.\n";
        continue;
    }
    moves_generator(board,cr,cf,turn);//possible  legal moves
    std::string tmove;
    int dr, df;//target rank and file
    
    cout<< "target: file+rank(like e2): ";
    cin >> tmove;
    if (!parseMoveout(tmove,dr, df)) {//check piece move is contained in board
        cout << "Invalid input format. Try again.\n";
        continue;
    }


    // validate and execute move
    if (!tryMove(board,cr,cf,dr,df,turn)) {//it checks for correct turns turns and king in check
     cout << "Illegal move. Try again.\n";
        continue;
    }
   // cout<<cr<<endl<<cf<<endl<<dr<<endl<<df<<endl;
    commit_move(board,cr,dr,cf,df,turn);
    cout<<"want to undo?(y/n) :";
    char choice;
    cin>>choice;
    if(choice=='y'){
        undoMove(board);
        continue;}
    // switch player
    turn = (turn == WHITE ? BLACK : WHITE);//alternate between 2 players
}
}
   /*
   for(int rank=RANK_8;rank>=RANK_1;rank--){
        for(int file=FILE_A;file<=FILE_H;file++){
            cout<<
        }
    }*/