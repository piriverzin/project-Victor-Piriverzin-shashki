#include "game.h"
#include <cmath>
#include <sstream>

CheckersGame::CheckersGame() {
    initGame();
}

void CheckersGame::initGame() {
    std::lock_guard<std::mutex> lock(gameMutex);
    board = std::vector<std::vector<int>>(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    currentPlayer = 1;
    gameOver = false;
    winner = 0;
    pieceSelected = false;
    selectedRow = -1;
    selectedCol = -1;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if ((i + j) % 2 == 1) {
                if (i < 3) board[i][j] = 2;
                else if (i > 4) board[i][j] = 1;
            }
        }
    }
}

bool CheckersGame::isValidMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (gameOver) return false;
    if (toRow < 0 || toRow >= BOARD_SIZE || toCol < 0 || toCol >= BOARD_SIZE) return false;
    
    int piece = board[fromRow][fromCol];
    if (piece == 0) return false;
    if ((piece == 1 || piece == 3) && currentPlayer != 1) return false;
    if ((piece == 2 || piece == 4) && currentPlayer != 2) return false;
    
    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;
    bool isKing = (piece == 3 || piece == 4);
    
    if (!isKing) {
        if (piece == 1 && rowDiff > 0) return false;
        if (piece == 2 && rowDiff < 0) return false;
    }
    
    if (abs(rowDiff) == 1 && abs(colDiff) == 1) {
        return board[toRow][toCol] == 0;
    }
    
    if (abs(rowDiff) == 2 && abs(colDiff) == 2) {
        int midRow = (fromRow + toRow) / 2;
        int midCol = (fromCol + toCol) / 2;
        int midPiece = board[midRow][midCol];
        
        if (midPiece != 0) {
            bool isEnemy = (piece == 1 || piece == 3) ? (midPiece == 2 || midPiece == 4) : (midPiece == 1 || midPiece == 3);
            if (isEnemy && board[toRow][toCol] == 0) {
                return true;
            }
        }
    }
    
    return false;
}

std::vector<std::pair<int,int>> CheckersGame::getAllValidMoves(int row, int col) {
    std::vector<std::pair<int,int>> moves;
    for (int dr = -2; dr <= 2; dr++) {
        for (int dc = -2; dc <= 2; dc++) {
            if (dr == 0 || dc == 0) continue;
            if (abs(dr) != abs(dc)) continue;
            if (isValidMove(row, col, row + dr, col + dc)) {
                moves.push_back({row + dr, col + dc});
            }
        }
    }
    return moves;
}

bool CheckersGame::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    std::lock_guard<std::mutex> lock(gameMutex);
    if (!isValidMove(fromRow, fromCol, toRow, toCol)) return false;
    
    int piece = board[fromRow][fromCol];
    bool isCapture = abs(toRow - fromRow) == 2;
    
    board[toRow][toCol] = piece;
    board[fromRow][fromCol] = 0;
    
    if (isCapture) {
        int midRow = (fromRow + toRow) / 2;
        int midCol = (fromCol + toCol) / 2;
        board[midRow][midCol] = 0;
    }
    
    if ((piece == 1 && toRow == 0) || (piece == 2 && toRow == 7)) {
        board[toRow][toCol] = (piece == 1) ? 3 : 4;
    }
    
    currentPlayer = (currentPlayer == 1) ? 2 : 1;
    checkGameOver();
    pieceSelected = false;
    selectedRow = -1;
    selectedCol = -1;
    
    return true;
}

void CheckersGame::checkGameOver() {
    bool hasWhitePieces = false;
    bool hasBlackPieces = false;
    bool hasWhiteMoves = false;
    bool hasBlackMoves = false;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int piece = board[i][j];
            if (piece == 1 || piece == 3) {
                hasWhitePieces = true;
                if (!getAllValidMoves(i, j).empty()) hasWhiteMoves = true;
            } else if (piece == 2 || piece == 4) {
                hasBlackPieces = true;
                if (!getAllValidMoves(i, j).empty()) hasBlackMoves = true;
            }
        }
    }
    
    if (!hasWhitePieces || (currentPlayer == 1 && !hasWhiteMoves)) {
        gameOver = true;
        winner = 2;
    } else if (!hasBlackPieces || (currentPlayer == 2 && !hasBlackMoves)) {
        gameOver = true;
        winner = 1;
    }
}

void CheckersGame::selectPiece(int row, int col) {
    std::lock_guard<std::mutex> lock(gameMutex);
    if (gameOver) return;
    
    int piece = board[row][col];
    if (piece != 0) {
        bool isCorrectPlayer = (piece == 1 || piece == 3) ? currentPlayer == 1 : currentPlayer == 2;
        if (isCorrectPlayer) {
            selectedRow = row;
            selectedCol = col;
            pieceSelected = true;
        }
    }
}

void CheckersGame::moveSelected(int toRow, int toCol) {
    if (pieceSelected) {
        makeMove(selectedRow, selectedCol, toRow, toCol);
    }
}

void CheckersGame::resetGame() {
    initGame();
}

int CheckersGame::getPiece(int row, int col) {
    std::lock_guard<std::mutex> lock(gameMutex);
    return board[row][col];
}

int CheckersGame::getCurrentPlayer() {
    std::lock_guard<std::mutex> lock(gameMutex);
    return currentPlayer;
}

bool CheckersGame::isGameOver() {
    std::lock_guard<std::mutex> lock(gameMutex);
    return gameOver;
}

int CheckersGame::getWinner() {
    std::lock_guard<std::mutex> lock(gameMutex);
    return winner;
}

bool CheckersGame::isPieceSelected() {
    std::lock_guard<std::mutex> lock(gameMutex);
    return pieceSelected;
}

int CheckersGame::getSelectedRow() {
    std::lock_guard<std::mutex> lock(gameMutex);
    return selectedRow;
}

int CheckersGame::getSelectedCol() {
    std::lock_guard<std::mutex> lock(gameMutex);
    return selectedCol;
}

std::string CheckersGame::getGameStateJSON() {
    std::lock_guard<std::mutex> lock(gameMutex);
    std::stringstream json;
    
    json << "{";
    json << "\"board\":[";
    for (int i = 0; i < BOARD_SIZE; i++) {
        json << "[";
        for (int j = 0; j < BOARD_SIZE; j++) {
            json << board[i][j];
            if (j < BOARD_SIZE - 1) json << ",";
        }
        json << "]";
        if (i < BOARD_SIZE - 1) json << ",";
    }
    json << "],";
    json << "\"currentPlayer\":" << currentPlayer << ",";
    json << "\"gameOver\":" << (gameOver ? "true" : "false") << ",";
    json << "\"winner\":" << winner << ",";
    json << "\"pieceSelected\":" << (pieceSelected ? "true" : "false") << ",";
    json << "\"selectedRow\":" << selectedRow << ",";
    json << "\"selectedCol\":" << selectedCol;
    json << "}";
    
    return json.str();
}