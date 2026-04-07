#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <mutex>

class CheckersGame {
private:
    static const int BOARD_SIZE = 8;
    std::vector<std::vector<int>> board;
    int currentPlayer;
    bool gameOver;
    int winner;
    int selectedRow, selectedCol;
    bool pieceSelected;
    std::mutex gameMutex;
    
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol);
    void checkGameOver();
    std::vector<std::pair<int,int>> getAllValidMoves(int row, int col);
    
public:
    CheckersGame();
    void initGame();
    bool makeMove(int fromRow, int fromCol, int toRow, int toCol);
    void selectPiece(int row, int col);
    void moveSelected(int toRow, int toCol);
    void resetGame();
    
    // Getters
    int getPiece(int row, int col);
    int getCurrentPlayer();
    bool isGameOver();
    int getWinner();
    bool isPieceSelected();
    int getSelectedRow();
    int getSelectedCol();
    
    // JSON для отправки клиенту
    std::string getGameStateJSON();
};

#endif