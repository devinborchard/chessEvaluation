#include <stdio.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<tuple>
#include<vector>
#include <map>
#include <limits>
#include<queue>
#include <cstring>
#include <cmath>
#include <random>
#include <math.h>
#include <unordered_map>

using namespace std;

struct Location{
  int x;
  int y;
};

struct Castles{
  bool w_castle_l;
  bool w_castle_s;
  bool b_castle_l;
  bool b_castle_s;
};

struct State{
  vector<vector<int>> board;
  bool turn;
  int count;

  double eval;

  Castles castles;

  string last_move;
  State* last_state;
  vector<State*> next_moves;
};

vector<int> get_material_count(vector<vector<int>> board){
  int white_material = 0;
  int black_material = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){

      int piece = board[i][j];
      
      if(piece != 0 and piece != 6 and piece != -6){
        bool white = true;
        if(piece < 0){
          white = false;
          piece = piece * -1;
        }

        if(piece == 2){
          piece = 3;
        }
        else if(piece == 4){
          piece = 5;
        }
        else if(piece == 5){
          piece = 9;
        }

        if(white){
          white_material = white_material + piece;
        }
        else{
          black_material = black_material + piece;
        }

      }
    }
  }
  return {white_material, black_material};
}


bool find_king(State* state, bool side){
  bool w_king = false;
  bool b_king = false;

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      int piece = state->board[i][j];
      if(piece == -6){b_king = true;}
      if(piece == 6){w_king = true;}
    }
  }
  if(w_king and side){
    return true;
  }
  if(b_king and !side){
    return true;
  }
  return false;
}

bool find_kings(State* state){
  bool w_king = false;
  bool b_king = false;

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      int piece = state->board[i][j];
      if(piece == -6){b_king = true;}
      if(piece == 6){w_king = true;}
    }
  }
  if(w_king and b_king){
    return true;
  }
  return false;
}




bool in_bounds(Location l){
  if(l.y > 7 or l.y < 0 or l.x > 7 or l.x < 0){
    return false;
  }
  return true;
}

string make_move_string(int piece, int x, int y){
  int abs_piece = abs(piece);
  string piece_letter;
  if(abs_piece == 1) piece_letter = "p";
  if(abs_piece == 2) piece_letter = "n";
  if(abs_piece == 3) piece_letter = "b";
  if(abs_piece == 4) piece_letter = "r";
  if(abs_piece == 6) piece_letter = "k";
  if(abs_piece == 5) piece_letter = "q";
  // if(piece == 0) piece_letter = "_";
  string row = "";
  if(x == 0) row = "a";
  if(x == 1) row = "b";
  if(x == 2) row = "c";
  if(x == 3) row = "d";
  if(x == 4) row = "e";
  if(x == 5) row = "f";
  if(x == 6) row = "g";
  if(x == 7) row = "h";
  return piece_letter + row + to_string(y+1);
}

vector<Location> pawn_moves(int direction, vector<vector<int>> board, Location l, bool turn){
  // cout << "PAWN MOVES "<<direction << "\n";
  // cout << " AT: " << l.x << "," << l.y << "\n";
  vector<Location> moves{};

  // determine if a pawn has moved from the start space for the initial double move option
  bool has_moved = false;
  int initial_y;
  if(turn){
    initial_y = 1;
  }else{
    initial_y = 6;
  }
  if((l.y>initial_y and turn) or (l.y<initial_y and !turn)){
    has_moved = true;
  }

  // cout << "HAS MOVED: " << has_moved << "\n";

  l.y = l.y + direction;
  // cout << " pawn check for " << l.x << "," << l.y << "\n";

  bool in = in_bounds(l);


  if(in){

    // basic moves
    if(board[l.y][l.x] == 0){
      moves.push_back(l);
    }


    // initial double move
    if(!has_moved and board[l.y][l.x] == 0){
      l.y = l.y + direction;
      if(board[l.y][l.x] == 0){
        moves.push_back(l);
      }
      l.y = l.y - direction;
    }
    // cout << "DONE IT\n";

    // capture moves
    l.x = l.x + 1;
    if(in_bounds(l)){
      if(turn and board[l.y][l.x] < 0 or !turn and board[l.y][l.x] > 0){
        moves.push_back(l);
      }
    }
    l.x = l.x - 2;
    if(in_bounds(l)){
      if(turn and board[l.y][l.x] < 0 or !turn and board[l.y][l.x] > 0){
        moves.push_back(l);
      }
    }
  }
  

  return moves;
}

vector<Location> king_moves(vector<vector<int>> board, Location l, bool turn){
  vector<Location> translations = {{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1}, {-1,-1},{1,0}};
  // vector<Location> translations = {{1,0},{-1,0}};
  vector<Location> moves{};

  for(Location translation : translations){
    int new_x = l.x + translation.x;
    int new_y = l.y + translation.y;
    if(new_y <= 7 and new_y >= 0 and new_x <= 7 and new_x >= 0){
      if((board[new_y][new_x] == 0) or (turn and board[new_y][new_x] < 0) or (!turn and board[new_y][new_x] > 0)){
        moves.push_back({new_x, new_y});
      }
    }
  }

  return moves;

}

vector<Location> knight_moves(vector<vector<int>> board, Location l, bool turn){
  vector<Location> translations = {{1,2},{1,-2},{-1,2},{-1,-2}, {2,1},{2,-1},{-2,1}, {-2,-1}};
  vector<Location> moves{};

  for(Location translation : translations){
    int new_x = l.x + translation.x;
    int new_y = l.y + translation.y;
    // cout << "CHECKING: " << new_x << ", " << new_y << "\n";
    if(new_y <= 7 and new_y >= 0 and new_x <= 7 and new_x >= 0){
      // cout << "CHECKING: " << new_x << ", " << new_y << "\n";      
      if((board[new_y][new_x] == 0) or (turn and board[new_y][new_x] < 0) or (!turn and board[new_y][new_x] > 0)){
        moves.push_back({new_x, new_y});
      }
    }
  }

  return moves;

}

vector<Location> straight_moves(int direction, vector<vector<int>> board, Location l, bool turn){
  bool done = false;

  vector<Location> moves{};

  while(!done){
    if(direction == 0){l.y = l.y + 1;}
    if(direction == 1){l.x = l.x + 1;}
    if(direction == 2){l.y = l.y - 1;}
    if(direction == 3){l.x = l.x - 1;}

    if(l.y > 7 or l.y < 0 or l.x > 7 or l.x < 0){
      done = true;
    }else{
      // cout << "TURN: " << turn<<"\n";
      // cout << "checking location: "  << l.x << ", " << l.y << "\n";
      // cout << "PIECE AT LOCATION: " <<board[l.y][l.x]<<"\n";
      if(board[l.y][l.x] == 0){
        // cout << "ROOK CAN MOVE TO EMPTY SPACE: " << l.x << ", " << l.y << "\n";
        moves.push_back(l);
      }

      else if((turn and board[l.y][l.x] < 0) or (!turn and board[l.y][l.x] > 0)){
        // cout << "ROOK CAN MOVE TO OCCUPIED SPACE: " << l.x << ", " << l.y << "\n";
        moves.push_back(l);
        done = true;
      }

      else{
        done = true;
      }
    }
  }

  return moves;

}

vector<Location> diagonal_moves(int direction, vector<vector<int>> board, Location l, bool turn){
  bool done = false;

  vector<Location> moves{};

  while(!done){
    if(direction == 0){l.y = l.y + 1; l.x = l.x + 1;}
    if(direction == 1){l.y = l.y + 1; l.x = l.x - 1;}
    if(direction == 2){l.y = l.y - 1; l.x = l.x + 1;}
    if(direction == 3){l.y = l.y - 1; l.x = l.x - 1;}

    if(l.y > 7 or l.y < 0 or l.x > 7 or l.x < 0){
      done = true;
    }else{
      if(board[l.y][l.x] == 0){
        moves.push_back(l);
      }

      else if((turn and board[l.y][l.x] < 0) or (!turn and board[l.y][l.x] > 0)){
        moves.push_back(l);
        done = true;
      }

      else{
        done = true;
      }
    }
  }

  return moves;

}

vector<Location> get_spaces_controlled(int piece, bool turn, vector<vector<int>> board, Location l){
  Location start_l = l;
  int type = abs(piece);
  vector<Location> new_moves;
  string move_string;

  string extra = "";

  // ROOK
  if(type == 4){
    for(int i = 0 ; i < 4; i++){
      vector<Location> new_moves1 = straight_moves(i, board, l, turn);
      new_moves.insert(new_moves.end(), new_moves1.begin(), new_moves1.end());
    }
  }

  // BISHOP
  if(type == 3){
    for(int i = 0 ; i < 4; i++){
      vector<Location> new_moves1 = diagonal_moves(i, board, l, turn);
      new_moves.insert(new_moves.end(), new_moves1.begin(), new_moves1.end());
    }
  }

  // KNIGHT
  if(type == 2){
    new_moves = knight_moves(board, l, turn);
  }

  // QUEEN
  if(type == 5){
    for(int i = 0 ; i < 4; i++){
      vector<Location> new_moves1 = diagonal_moves(i, board, l, turn);
      new_moves.insert(new_moves.end(), new_moves1.begin(), new_moves1.end());
    }
    for(int i = 0 ; i < 4; i++){
      vector<Location> new_moves1 = straight_moves(i, board, l, turn);
      new_moves.insert(new_moves.end(), new_moves1.begin(), new_moves1.end());
    }
  }
  
  // KING
  if(type == 6){
    new_moves = king_moves(board, l, turn);
  }

  return new_moves;
}

void print_board(vector<vector<int>> board){

  cout << "Board ------------\n";

  vector<vector<int>> reverse_board = {};
  for(int i = 0; i < 8; i++){
    reverse_board.push_back(board.back());
    board.pop_back();
  }

  for(int i = 0; i < 8; i++){
    cout << "|";
    for(int j = 0; j < 8; j++){
      int value = reverse_board[i][j];
      if(value < 0){
        cout << reverse_board[i][j];
      }else{
        cout << " " <<reverse_board[i][j];
      }
      cout << "|";
    }
    cout << "\n";
  }
  cout << "\n";
}

void print_state(State state){
  cout << "Players turn: " << state.turn << "\n";
  cout << "Previous move: "<< state.last_move<< "\n";
  cout << "Evaluation: " << state.eval << "\n";
  cout << "count: " << state.count << "\n";
  if(state.last_state){
    cout << "Prev prev move: " << state.last_state->last_move<<"\n";
  }
  // cout << "White can castle: "<< state.w_castle << "\n";
  // cout << "Black can castle: "<< state.b_castle << "\n";

  vector<vector<int>> reverse_board = {};
  for(int i = 0; i < 8; i++){
    reverse_board.push_back(state.board.back());
    state.board.pop_back();
  }
  for(int i = 0; i < 8; i++){
    cout << " __";
  }
  cout << "\n";
  for(int i = 0; i < 8; i++){
    cout << "|";
    for(int j = 0; j < 8; j++){
      int value = reverse_board[i][j];
      int piece = abs(value);
      if(value >= 0){
        cout << "_";
      }
      else if (value < 0){
        cout << "-";
      }
      if(piece == 1) cout << "P";
      if(piece == 2) cout << "N";
      if(piece == 3) cout << "B";
      if(piece == 4) cout << "R";
      if(piece == 6) cout << "K";
      if(piece == 5) cout << "Q";
      if(piece == 0) cout << "_";
      

      cout << "|";
    }
    cout << "\n";
  }
  cout << "\n";
}

vector<vector<int>> parse2DArrayString(const string& arrayString) {
    vector<vector<int>> array2D;

    istringstream iss(arrayString);
    string token;

    vector<int> row;
    bool newRow = false;  // Flag to indicate if we're starting a new row
    while (getline(iss, token, ',')) {
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");
        if (start != string::npos && end != string::npos) {
            token = token.substr(start, end - start + 1);
        } else {
            continue;  // Skip empty tokens
        }

        if (token.find("{") != string::npos) {
            // If it's not the start of the input string, push the current row into the 2D array
            // newRow = true;  // Start a new row
            token.erase(0, 1);
        }

        if (token.find("}") != string::npos) {
            // Push the current row into the 2D array
            newRow = true;  // Start a new row
            token.erase(token.size()-1);

        }

        int num;
        istringstream(token) >> num;

        if (newRow) {
            row.push_back(num);
            array2D.push_back(row);
            row.clear();
            newRow = false;
        } else {
            row.push_back(num);
        }
    }

    if (!row.empty()) {
        array2D.push_back(row);
    }

    return array2D;
}
