#include "c_h.hpp"
using namespace std;
//rank=row   file=col

void update_variable(char board[9][9],int rank,int drank,int file,int dfile,int color){
   // Determine which piece moved (source square is already empty after makeMove)
    char movedPiece = mystruct.moved;
   //check king move and record current position(for castle)
    if (movedPiece=='K'){
        kingmoved[WHITE]=1;
        king_pos[WHITE][0]=drank;
        king_pos[WHITE][1]=dfile;
    }
    else if(movedPiece=='k'){
        kingmoved[BLACK]=1;
        king_pos[BLACK][0]=drank;
        king_pos[BLACK][1]=dfile;
    }
    //check if rookmoved(for castle)
    else if (movedPiece=='R'){
        if (rank==RANK_1){
            if((file==FILE_A)||(file==FILE_H))
            {
                rookmoved[WHITE][1]=0;
            }
        }
        else{
            rookmoved[WHITE][1]=1;
        }    
    }
    else if (movedPiece=='r'){
        if (rank==RANK_8){
            if((file==FILE_A)||(file==FILE_H))
            {
                rookmoved[BLACK][1]=0;
            }
        }
        else{
            rookmoved[BLACK][1]=1;
        }   
    }
    else if ((movedPiece=='P')&&(rank==RANK_2)){
        // White double-step enables en passant for black on rank 5 capture squares
        if((drank-rank==2)){
            epAvailable = true; epTargetRank = RANK_5; epTargetFile = file;
        } else { epAvailable = false; }
    }
    else if ((movedPiece=='P')&&((rank==RANK_7)&&(drank==RANK_8))){
        char choice;
        if (guiMode) {
            choice = (promotionChoice=='Q'||promotionChoice=='R'||promotionChoice=='B'||promotionChoice=='N') ? promotionChoice : 'Q';
        } else {
            cout<<"enter promotion route:N,R,B,Q:";
            cin>>choice;
            choice = toupper(choice);
            if (!(choice == 'Q' || choice == 'R' || choice == 'B' || choice == 'N')) {
                choice = 'Q';
            }
        }
        board[drank][dfile]=choice;
    }
    else if((movedPiece=='p')&&(rank==RANK_7)){
        // Black double-step enables en passant for white on rank 4 capture squares
        if((rank-drank==2)){
            epAvailable = true; epTargetRank = RANK_4; epTargetFile = file;
        } else { epAvailable = false; }
    }
    else if((movedPiece=='p')&&((rank==RANK_2)&&(drank==RANK_1))){
        char choice;
        if (guiMode) {
            choice = (promotionChoice=='q'||promotionChoice=='r'||promotionChoice=='b'||promotionChoice=='n') ? promotionChoice : 'q';
        } else {
            cout<<"enter promotion route:N,R,B,Q:";
            cin>>choice;
            choice = tolower(choice);
            if (!(choice == 'q' || choice == 'r' || choice == 'b' || choice == 'n')) {
                choice = 'q';
            }
        }
        board[drank][dfile]=choice;
    }  
    // If this move was not a double pawn step, ep window closes
    if (!((movedPiece=='P' && rank==RANK_2 && drank==RANK_4) || (movedPiece=='p' && rank==RANK_7 && drank==RANK_5))) {
        epAvailable = false;
        epTargetRank = 0;
        epTargetFile = 0;
    }
}

void doit(char board[9][9],int rank,int drank,int file,int dfile,int color){
    
    makeMove(board,mystruct,rank,drank,file,dfile); 
    cout<<"222";
    check_king_attack(board,color,king_pos);//currently return nothing only stores boolean of kingattacked
    if(!check_king_attack(board,color,king_pos)){
        update_variable(board,rank,drank,file,dfile,color);
    }
}



void commit_pawn(char board[9][9], int rank, int drank, int file, int dfile, int color) {
    // Forward move
    if (abs(drank-rank)>2){
        cout<<"illegal";
        return;
    }
    if (file == dfile) {
        if (color == WHITE) {
            if (drank - rank == 1 && board[drank][dfile] == '.') {
                doit(board, rank, drank, file, dfile, color);
            }
            else if (rank == RANK_2 && drank - rank == 2 &&
                     board[rank+1][file] == '.' && board[drank][dfile] == '.') {
                doit(board, rank, drank, file, dfile, color);
            }
            else {
                cout << "illegal move\n";
                return;
            }
        }
        else if (color == BLACK) {
            if (rank - drank == 1 && board[drank][dfile] == '.') {
                doit(board, rank, drank, file, dfile, color);
            }
            else if (rank == RANK_7 && rank - drank == 2 &&
                     board[rank-1][file] == '.' && board[drank][dfile] == '.') {
                doit(board, rank, drank, file, dfile, color);
            }
            else {
                cout << "illegal move\n";
                return;
            }
        }
    }
    // Capture move (normal or en passant)
    else if (abs(file - dfile) == 1) {
        // Normal capture if target has opponent
        if ((color == WHITE && drank - rank == 1 && islower(board[drank][dfile])) ||
            (color == BLACK && rank - drank == 1 && isupper(board[drank][dfile]))) {
            doit(board, rank, drank, file, dfile, color);
            return;
        }
        // En passant: capture to empty square matching ep target
        if (color == WHITE && rank == RANK_5 && drank == RANK_6 && board[drank][dfile] == '.') {
            if (epAvailable && epTargetRank == rank && abs(dfile - file) == 1 && dfile == epTargetFile) {
                epCapturedRank = rank; epCapturedFile = dfile; epCapturedPiece = 'p';
                lastWasEnPassant = true;
                board[rank][dfile] = '.';
                doit(board, rank, drank, file, dfile, color);
                epAvailable = false;
                return;
            }
        }
        if (color == BLACK && rank == RANK_4 && drank == RANK_3 && board[drank][dfile] == '.') {
            if (epAvailable && epTargetRank == rank && abs(dfile - file) == 1 && dfile == epTargetFile) {
                epCapturedRank = rank; epCapturedFile = dfile; epCapturedPiece = 'P';
                lastWasEnPassant = true;
                board[rank][dfile] = '.';
                doit(board, rank, drank, file, dfile, color);
                epAvailable = false;
                return;
            }
        }
        cout << "illegal capture\n";
        return;
    }
    else {
        cout << "illegal move\n";
        return;
    }
}


void commit_knight(char board[9][9],int rank,int drank,int file,int dfile,int color){
    if(!((abs(rank - drank) == 2 && abs(file - dfile) == 1) ||
      (abs(rank - drank) == 1 && abs(file - dfile) == 2))) {
        cout<<"illegal move";
        return;
        }
        doit(board, rank, drank, file, dfile, color);
    }

void commit_rook(char board[9][9],int rank,int drank,int file,int dfile,int color){
    if ((rank == drank && file != dfile) || (file == dfile && rank != drank)) {
        // --- Path clearance ---
        if (rank == drank) { // moving along row
            int step = (dfile > file) ? 1 : -1;
            for (int f = file + step; f != dfile; f += step) {
                if (board[rank][f] != '.') {
                    cout << "illegal move - path blocked\n";
                    return;
                }
            }
        }
        else { // moving along column
            int step = (drank > rank) ? 1 : -1;
            for (int r = rank + step; r != drank; r += step) {
                if (board[r][file] != '.') {
                    cout << "illegal move - path blocked\n";
                    return;
                }
            }
        }
        doit(board, rank, drank, file, dfile, color);
    }
    else {
        cout << "illegal move - not a rook move\n";
        return;
    }
}

void commit_bishop(char board[9][9],int rank,int drank,int file,int dfile,int color){

        if(abs(drank - rank) == abs(dfile - file)){
            int rStep = (drank > rank) ? 1 : -1;
            int fStep = (dfile > file) ? 1 : -1;
            int r = rank + rStep;
            int f = file + fStep;

            while (r != drank && f != dfile) {
                if (board[r][f] != '.') {
                    cout << "illegal move - path blocked";
                    return;
                }
                r += rStep;
                f += fStep;
            }
            doit(board, rank, drank, file, dfile, color);
        }
        else{
            cout << "illegal move ";
            return;
        }
}

void commit_king(char board[9][9],int rank,int drank,int file,int dfile,int color){
        // Castling: king moves two squares horizontally
        if (rank==drank && abs(dfile-file)==2) {
            int homeRank = (color==WHITE ? RANK_1 : RANK_8);
            if (rank != homeRank) { cout<<"illegal move"; return; }
            int dir = (dfile>file)? +1 : -1;
            // Squares the king passes through must be empty and not attacked
            int f1 = file + dir; // f or d
            if (board[rank][f1] != '.' || board[rank][dfile] != '.') { cout<<"illegal move"; return; }
            if (isSquareAttacked(board, rank, file, color) ||
                isSquareAttacked(board, rank, f1, color) ||
                isSquareAttacked(board, rank, dfile, color)) { cout<<"illegal move"; return; }
            // Rook must be present on the appropriate corner
            int rookFileFrom = (dir>0 ? FILE_H : FILE_A);
            if (board[rank][rookFileFrom] != (color==WHITE?'R':'r')) { cout<<"illegal move"; return; }
            // Perform king move
            lastWasCastle = true;
            rookFromRank = rank; rookFromFile = rookFileFrom;
            rookToRank = rank; rookToFile = (dir>0 ? FILE_F : FILE_D);
            doit(board, rank, drank, file, dfile, color);
            // Move rook on board (side-effect; undo will restore)
            board[rookToRank][rookToFile] = board[rookFromRank][rookFromFile];
            board[rookFromRank][rookFromFile] = '.';
            return;
        }
        if( (abs(rank-drank)>1)||(abs(file-dfile)>1)){
            cout<<"illegal move";
            return;
        }
        lastWasCastle = false;
        doit(board, rank, drank, file, dfile, color);
    }

void commit_queen(char board[9][9],int rank,int drank,int file,int dfile,int color){
    bool legal = false;
    // --- Rook-like move (same rank or same file) ---
    if ((rank == drank && file != dfile) || (file == dfile && rank != drank)) {
        legal = true;
        // path clearance check (horizontal/vertical)
        if (rank == drank) { // same row
            int step = (dfile > file) ? 1 : -1;
            for (int f = file + step; f != dfile; f += step) {
                if (board[rank][f] != '.') {
                    cout << "illegal move - path blocked\n";
                    return;
                }
            }
        } else { // same column
            int step = (drank > rank) ? 1 : -1;
            for (int r = rank + step; r != drank; r += step) {
                if (board[r][file] != '.') {
                    cout << "illegal move - path blocked\n";
                    return;
                }
            }
        }
    }
    // --- Bishop-like move (diagonal) ---
    else if (abs(rank - drank) == abs(file - dfile)) {
        legal = true;

        // path clearance check (diagonal)
        int rStep = (drank > rank) ? 1 : -1;
        int fStep = (dfile > file) ? 1 : -1;

        int r = rank + rStep, f = file + fStep;
        while (r != drank && f != dfile) {
            if (board[r][f] != '.') {
                cout << "illegal move - path blocked\n";
                return;
            }
            r += rStep;
            f += fStep;
        }
    }
    if (!legal) {
        cout << "illegal move - not a queen move\n";
        return;
    }
    if (legal) {
        doit(board, rank, drank, file, dfile, color);
    }

}

void commit_move(char board[9][9],int rank,int drank,int file,int dfile,int color){
        //drank and d file target destination;
    if((drank==0)||(dfile==0)){
        return;
    }
    if(board[rank][file]=='.'){
        return;
    }
    //check destination occupied by same piece?
    if(board[rank][file]!='.'){
        if ((isupper(board[rank][file]) && isupper(board[drank][dfile])) || (islower(board[rank][file]) && islower(board[drank][dfile]))) {
        cout << "illegal move - can't capture own piece\n";
        return;
        }
    }
    // destination occupancy is validated per-piece; allow captures here
    switch(board[rank][file]){
        case 'p': // Bpawn
            commit_pawn(board,rank,drank,file,dfile,color);
            break;
        case 'n': // Bknight
            commit_knight(board,rank,drank,file,dfile,color);
            break;
        case 'r': // Brook
            commit_rook(board,rank,drank,file,dfile,color);
            break;
        case 'b': // Bbishop
            commit_bishop(board,rank,drank,file,dfile,color);
            break;
        case 'q': // Bqueen
            commit_queen(board,rank,drank,file,dfile,color);
            break;
        case 'k': // Bking
            commit_king(board,rank,drank,file,dfile,color);
            break;
        case 'P': // Wpawn
            commit_pawn(board,rank,drank,file,dfile,color);
            break;
        case 'N': // Wknight
            commit_knight(board,rank,drank,file,dfile,color);
            break;
        case 'R': // Wrook
            commit_rook(board,rank,drank,file,dfile,color);
            break;
        case 'B': // Wbishop
            commit_bishop(board,rank,drank,file,dfile,color);
            break;
        case 'Q': // Wqueen
            commit_queen(board,rank,drank,file,dfile,color);
            break;
        case 'K': // Wking
            commit_king(board,rank,drank,file,dfile,color);
            break;
    }

}