#include "c_h.hpp"
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
using namespace Gdiplus;
#include <string>
#include <algorithm>

// Simple Win32 GUI to visualize and play using existing engine logic

static const int kSquareSize = 72; // 8 * 72 = 576
static const int kBoardOffset = 16; // padding around board
static int gSelectedRank = 0; // 1..8
static int gSelectedFile = 0; // 1..8
static int gTurn = WHITE;
static ULONG_PTR gGdiplusToken = 0;
static Image* gSprite = nullptr; // chess_pieces.png
static HWND gMainWnd = nullptr;
static const UINT CMD_UNDO = 1001;

// Forward decls
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DrawBoard(HDC hdc, RECT client);
void DrawPieces(HDC hdc);
void DrawMoveHints(HDC hdc);
void InitStartingPosition();
bool TryAndCommitMove(int srank, int sfile, int drank, int dfile, int color);
char PromptPromotion(HWND hwnd, int color, int rank, int file);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Init GDI+
    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&gGdiplusToken, &gdiplusStartupInput, nullptr) != Ok) {
        return 1;
    }
    InitStartingPosition();

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "BegChessGUI";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    int windowSize = kBoardOffset * 2 + kSquareSize * 8;
    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, "Beginner Chess - GUI",
        WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME),
        CW_USEDEFAULT, CW_USEDEFAULT, windowSize + 16, windowSize + 39,
        nullptr, nullptr, hInstance, nullptr
    );
    gMainWnd = hwnd;
    // Menu with Undo
    HMENU hMenu = CreateMenu();
    HMENU hGame = CreatePopupMenu();
    AppendMenu(hGame, MF_STRING, CMD_UNDO, "Undo\tCtrl+Z");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hGame, "Game");
    SetMenu(hwnd, hMenu);

    // Load sprite sheet (PNG)
    gSprite = Image::FromFile(L"chess_pieces.png");
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // Cleanup
    if (gSprite) { delete gSprite; gSprite = nullptr; }
    if (gGdiplusToken) { GdiplusShutdown(gGdiplusToken); gGdiplusToken = 0; }
    return (int)msg.wParam;
}

// Board rank/file to pixel
static inline void SquareToRect(int rank, int file, RECT &r) {
    int left = kBoardOffset + (file - 1) * kSquareSize;
    int top = kBoardOffset + (8 - rank) * kSquareSize;
    r.left = left;
    r.top = top;
    r.right = left + kSquareSize;
    r.bottom = top + kSquareSize;
}

// Pixel to board rank/file; returns true if on board
static inline bool PointToSquare(int x, int y, int &rank, int &file) {
    x -= kBoardOffset;
    y -= kBoardOffset;
    if (x < 0 || y < 0) return false;
    int f = x / kSquareSize + 1;
    int rFromTop = y / kSquareSize; // 0..7 top to bottom
    if (f < 1 || f > 8 || rFromTop < 0 || rFromTop > 7) return false;
    int r = 8 - rFromTop; // convert to 8..1
    rank = r;
    file = f;
    return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT client;
        GetClientRect(hwnd, &client);
        DrawBoard(hdc, client);
        DrawPieces(hdc);
        DrawMoveHints(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        int r, f;
        if (!PointToSquare(x, y, r, f)) {
            if (gSelectedRank != 0) {
                gSelectedRank = gSelectedFile = 0;
                gui_clear_moves();
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            break;
        }

        if (gSelectedRank == 0) {
            // First click: select a piece of current side
            char piece = board[r][f];
            if (piece == '.') break;
            if ((gTurn == WHITE && islower(piece)) || (gTurn == BLACK && isupper(piece))) break;
            gSelectedRank = r;
            gSelectedFile = f;
            guiMode = true;
            gui_clear_moves();
            moves_generator(board, gSelectedRank, gSelectedFile, gTurn);
        } else {
            // Second click: attempt move
            if (gSelectedRank == r && gSelectedFile == f) {
                gSelectedRank = gSelectedFile = 0;
                gui_clear_moves();
                InvalidateRect(hwnd, nullptr, FALSE);
                break;
            }
            // If clicking another own piece, switch selection instead of attempting a move
            char targetPiece = board[r][f];
            if (targetPiece != '.' && ((gTurn == WHITE && isupper(targetPiece)) || (gTurn == BLACK && islower(targetPiece)))) {
                gSelectedRank = r;
                gSelectedFile = f;
                gui_clear_moves();
                moves_generator(board, gSelectedRank, gSelectedFile, gTurn);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (TryAndCommitMove(gSelectedRank, gSelectedFile, r, f, gTurn)) {
                gTurn = (gTurn == WHITE ? BLACK : WHITE);
                // Update title for turn and check status
                char title[128];
                bool inCheck = kingattacked[gTurn];
                sprintf(title, "Beginner Chess - GUI  [%s%s]", gTurn==WHITE?"White":"Black", inCheck?" in check":"");
                SetWindowText(hwnd, title);
                // Clear selection after a successful move
                gSelectedRank = gSelectedFile = 0;
                gui_clear_moves();
            } else {
                // Keep selection so the user can try another target
                // Regenerate move hints in case internal state changed minimally
                gui_clear_moves();
                moves_generator(board, gSelectedRank, gSelectedFile, gTurn);
            }
        }
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_RBUTTONDOWN: {
        if (gSelectedRank != 0) {
            gSelectedRank = gSelectedFile = 0;
            gui_clear_moves();
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }
    case WM_KEYDOWN: {
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
            return 0;
        }
        if ((wParam == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000)) || wParam == 'U') {
            // Undo last move if available
            if (canUndo()) {
                undoMove(board);
                gTurn = (gTurn == WHITE ? BLACK : WHITE);
                gSelectedRank = gSelectedFile = 0;
                gui_clear_moves();
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        return 0;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
            case CMD_UNDO:
                if (canUndo()) {
                    undoMove(board);
                    gTurn = (gTurn == WHITE ? BLACK : WHITE);
                    gSelectedRank = gSelectedFile = 0;
                    gui_clear_moves();
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void DrawBoard(HDC hdc, RECT) {
    HBRUSH light = CreateSolidBrush(RGB(240, 217, 181));
    HBRUSH dark  = CreateSolidBrush(RGB(181, 136, 99));
    HBRUSH sel   = CreateSolidBrush(RGB(120, 180, 120));

    for (int r = 1; r <= 8; ++r) {
        for (int f = 1; f <= 8; ++f) {
            RECT sq; SquareToRect(r, f, sq);
            bool isSel = (r == gSelectedRank && f == gSelectedFile);
            HBRUSH b = isSel ? sel : (((r + f) % 2 == 0) ? light : dark);
            FillRect(hdc, &sq, b);
        }
    }

    DeleteObject(light);
    DeleteObject(dark);
    DeleteObject(sel);
}

void DrawPieces(HDC hdc) {
    if (!gSprite || gSprite->GetLastStatus() != Ok) {
        return; // sprite not available
    }

    Graphics gfx(hdc);
    gfx.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    const int cellW = 200;
    const int cellH = 200;

    auto pieceToCol = [&](char p) -> int {
        // Sprite columns: 0=K,1=Q,2=R,3=B,4=N,5=P
        switch (tolower(p)) {
            case 'k': return 0;
            case 'q': return 1;
            case 'r': return 2;
            case 'b': return 3;
            case 'n': return 4;
            case 'p': return 5;
        }
        return -1;
    };

    for (int r = 1; r <= 8; ++r) {
        for (int f = 1; f <= 8; ++f) {
            char p = board[r][f];
            if (p == '.') continue;
            int col = pieceToCol(p);
            if (col < 0) continue;
            int row = isupper(p) ? 0 : 1; // white row 0 (bright), black row 1

            RECT sq; SquareToRect(r, f, sq);
            int destW = sq.right - sq.left;
            int destH = sq.bottom - sq.top;

            Rect dest(sq.left, sq.top, destW, destH);
            Rect src(col * cellW, row * cellH, cellW, cellH);
            gfx.DrawImage(gSprite, dest, src.X, src.Y, src.Width, src.Height, UnitPixel);
        }
    }
}

void DrawMoveHints(HDC hdc) {
    if (guiMoves.empty()) return;
    Graphics gfx(hdc);
    SolidBrush moveBrush(Color(160, 30, 144, 255));   // blue for empty-square moves
    SolidBrush capBrush(Color(180, 220, 20, 60));     // red for captures
    for (auto &rc : guiMoves) {
        int r = rc.first;
        int f = rc.second;
        RECT sq; SquareToRect(r, f, sq);
        int w = sq.right - sq.left;
        int h = sq.bottom - sq.top;
        int d = std::min(w, h) / 3;
        int cx = sq.left + w/2 - d/2;
        int cy = sq.top + h/2 - d/2;
        bool isCapture = (board[r][f] != '.');
        gfx.FillEllipse(isCapture ? &capBrush : &moveBrush, Rect(cx, cy, d, d));
    }
}

void InitStartingPosition() {
    // Clear
    for (int r = RANK_1; r <= RANK_8; ++r) {
        for (int f = FILE_A; f <= FILE_H; ++f) {
            board[r][f] = '.';
        }
    }
    // Pawns
    for (int f = FILE_A; f <= FILE_H; ++f) {
        board[RANK_2][f] = PIECES[WHITE][5];
        board[RANK_7][f] = PIECES[BLACK][5];
    }
    // Back ranks
    board[RANK_1][FILE_A] = PIECES[WHITE][0];
    board[RANK_1][FILE_H] = PIECES[WHITE][0];
    board[RANK_1][FILE_B] = PIECES[WHITE][1];
    board[RANK_1][FILE_G] = PIECES[WHITE][1];
    board[RANK_1][FILE_C] = PIECES[WHITE][2];
    board[RANK_1][FILE_F] = PIECES[WHITE][2];
    board[RANK_1][FILE_D] = PIECES[WHITE][4];
    board[RANK_1][FILE_E] = PIECES[WHITE][3];

    board[RANK_8][FILE_A] = PIECES[BLACK][0];
    board[RANK_8][FILE_H] = PIECES[BLACK][0];
    board[RANK_8][FILE_B] = PIECES[BLACK][1];
    board[RANK_8][FILE_G] = PIECES[BLACK][1];
    board[RANK_8][FILE_C] = PIECES[BLACK][2];
    board[RANK_8][FILE_F] = PIECES[BLACK][2];
    board[RANK_8][FILE_D] = PIECES[BLACK][4];
    board[RANK_8][FILE_E] = PIECES[BLACK][3];

    // Reset state
    kingmoved[WHITE] = kingmoved[BLACK] = 0;
    rookmoved[WHITE][0] = rookmoved[WHITE][1] = 0;
    rookmoved[BLACK][0] = rookmoved[BLACK][1] = 0;
    kingattacked[WHITE] = kingattacked[BLACK] = 0;
    enPassantTargetRank[WHITE][0] = enPassantTargetRank[BLACK][0] = 0; // not comprehensive; ok for start
    king_pos[WHITE][0] = RANK_1; king_pos[WHITE][1] = FILE_E;
    king_pos[BLACK][0] = RANK_8; king_pos[BLACK][1] = FILE_E;
    gTurn = WHITE;
}

bool TryAndCommitMove(int srank, int sfile, int drank, int dfile, int color) {
    char piece = board[srank][sfile];
    if (piece == '.') return false;
    if ((color == WHITE && islower(piece)) || (color == BLACK && isupper(piece))) return false;

    // Handle promotion choice for GUI before commit
    guiMode = true;
    if (piece == 'P' && drank == RANK_8) {
        promotionChoice = PromptPromotion(gMainWnd, WHITE, drank, dfile);
        if (!(promotionChoice=='Q'||promotionChoice=='R'||promotionChoice=='B'||promotionChoice=='N')) promotionChoice = 'Q';
    }
    if (piece == 'p' && drank == RANK_1) {
        promotionChoice = PromptPromotion(gMainWnd, BLACK, drank, dfile);
        if (!(promotionChoice=='q'||promotionChoice=='r'||promotionChoice=='b'||promotionChoice=='n')) promotionChoice = 'q';
    }

    // Save state and test legality via existing helpers
    Move backup;
    makeMove(board, backup, srank, drank, sfile, dfile);
    bool illegal = check_king_attack(board, color, king_pos);
    undoMove(board);
    if (illegal) return false;

    // Snapshot before
    char beforeSrc = board[srank][sfile];
    char beforeDst = board[drank][dfile];

    // Now commit via engine (it does detailed per-piece legality)
    commit_move(board, srank, drank, sfile, dfile, color);

    // Detect if move actually happened
    bool moved = (board[srank][sfile] != beforeSrc) || (board[drank][dfile] != beforeDst);
    return moved;
}

char PromptPromotion(HWND hwnd, int color, int rank, int file) {
    // Show a small popup menu near the target square with Q/R/B/N
    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_STRING, 1, color==WHITE ? "Queen (Q)"  : "queen (q)");
    AppendMenu(menu, MF_STRING, 2, color==WHITE ? "Rook (R)"   : "rook (r)");
    AppendMenu(menu, MF_STRING, 3, color==WHITE ? "Bishop (B)" : "bishop (b)");
    AppendMenu(menu, MF_STRING, 4, color==WHITE ? "Knight (N)" : "knight (n)");

    // Popup near the destination square center
    RECT sq; SquareToRect(rank, file, sq);
    POINT pt = { sq.left + (sq.right - sq.left)/2, sq.top + (sq.bottom - sq.top)/2 };
    ClientToScreen(gMainWnd, &pt);

    UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd ? hwnd : GetForegroundWindow(), nullptr);
    DestroyMenu(menu);

    if (color==WHITE) {
        switch (cmd) { case 1: return 'Q'; case 2: return 'R'; case 3: return 'B'; case 4: return 'N'; default: return 'Q'; }
    } else {
        switch (cmd) { case 1: return 'q'; case 2: return 'r'; case 3: return 'b'; case 4: return 'n'; default: return 'q'; }
    }
}


