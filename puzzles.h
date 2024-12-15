#include<vector>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "run.h"

using namespace std;


vector<vector<int>> reverse_board(vector<vector<int>> board){
    reverse(board.begin(), board.end());
    return board;
}

struct Puzzle{
    vector<vector<int>> board;
    bool turn;
};

void printPuzzle(const Puzzle& puzzle) {
    std::cout << "Board:" << std::endl;
    for (const auto& row : puzzle.board) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Turn: " << (puzzle.turn ? "true" : "false") << std::endl;
}

std::vector<std::vector<int>> fen_to_board(const std::string& fen) {
    std::unordered_map<char, int> piece_dict = {
        {'r', -4}, {'n', -2}, {'b', -3}, {'q', -5}, {'k', -6}, {'p', -1},
        {'R', 4}, {'N', 2}, {'B', 3}, {'Q', 5}, {'K', 6}, {'P', 1}, {'.', 0}
    };

    std::vector<std::vector<int>> board;
    std::vector<std::string> rows;
    size_t start = 0;

    // Split the FEN string into rows
    for (size_t i = 0; i < fen.size(); ++i) {
        if (fen[i] == '/') {
            rows.push_back(fen.substr(start, i - start));
            start = i + 1;
        }
    }
    rows.push_back(fen.substr(start));

    // Parse each row
    for (const auto& row : rows) {
        std::vector<int> current_row;
        for (char c : row) {
            if (isdigit(c)) {
                int num_blanks = c - '0';
                current_row.insert(current_row.end(), num_blanks, 0);
            } else if (piece_dict.find(c) != piece_dict.end()) {
                current_row.push_back(piece_dict[c]);
            } else {
                throw std::invalid_argument("Invalid FEN string");
            }
        }
        board.push_back(current_row);
    }

    return board;
}

Puzzle get_puzzle_from_fen(string fen){

    vector<vector<int>> board = fen_to_board(fen); 

    // print_board(board);
    return{
        board,
        true
    };

}




State* get_puzzle(int evaluator, string fen){

    Puzzle p;

    p = get_puzzle_from_fen(fen);


    State* puzzle = new State({
        reverse_board(p.board),
        true,
        0,
        0,
        {false,false,false,false},
        "",
        nullptr,
        {}
    });

    puzzle->eval = static_evaluator(*puzzle, evaluator);

    return puzzle;
    
}

