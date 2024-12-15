#include <stdio.h>
#include<iostream>
#include<string>
#include<vector>
#include <cstring>

#include "util.h"


using namespace std;

// white is postive, black is negative
// pawn = 1
// knight = 2
// bishop = 3
// rook = 4
// queen = 5
// king = 6

vector<State*> get_all_valid_moves(State*, int);
bool in_check(bool, State*, int);

double static_evaluator(State state, int evaluator){
  

  vector<State*> next_moves = get_all_valid_moves(&state, -1);

  if(next_moves.size() == 0){
        bool black_in_check = in_check(false, &state, -1);
        bool white_in_check = in_check(true, &state, -1);


        if(!state.turn and black_in_check){
          return 10000;
        }
        else if(state.turn and white_in_check){
          return -10000;
        }
        else{
          return 0;
        }
    }

    for(State* child : next_moves){
      delete child;
    }

 
  vector<int> materials = get_material_count(state.board);
  int material_differential = materials[0] - materials[1];

  if(evaluator == 0){
    return material_differential;
  }
  if(evaluator == 1){
    int white_moves = 0;
    int black_moves = 0;
    
    for(int i = 0; i < 8; i++){
      for(int j = 0; j < 8; j++){
        int piece = state.board[i][j];
        if(piece > 0){
          int white_move_count = get_spaces_controlled(piece, true, state.board, {j,i}).size();
          white_moves = white_moves + white_move_count;
        }
        else if(piece < 0){
          int black_move_count = get_spaces_controlled(piece, false, state.board, {j,i}).size();
          black_moves = black_moves + black_move_count;
        }
      }
    }

    int move_differential = white_moves - black_moves;
    return material_differential + (move_differential / 2);
  }     

  if(evaluator == 2){


    bool black_in_check = in_check(false, &state, -1);
    bool white_in_check = in_check(true, &state, -1);

    int weakend = 0;
    if(white_in_check and state.turn) weakend = -50; 
    if(black_in_check and !state.turn) weakend = 50;

    return material_differential + weakend;
  } 
        
  return 0;
}

vector<State*> get_piece_moves(int piece, bool side, State* state, Location l, int evaluation){


  vector<State*> move_states{};
  Location start_l = l;
  int type = abs(piece);
  vector<Location> new_moves;
  string move_string;

  vector<vector<int>> new_board = state->board;

  string extra = "";


  // ROOK
  if(type == 4){
    for(int i = 0 ; i < 4; i++){
      vector<Location> new_moves1 = straight_moves(i, new_board, l, side);
      new_moves.insert(new_moves.end(), new_moves1.begin(), new_moves1.end());
    }
  }

  // BISHOP
  if(type == 3){
    for(int i = 0 ; i < 4; i++){
      vector<Location> new_moves1 = diagonal_moves(i, new_board, l, side);
      new_moves.insert(new_moves.end(), new_moves1.begin(), new_moves1.end());
    }
  }

  // KNIGHT
  if(type == 2){
    new_moves = knight_moves(new_board, l, side);
  }

  // QUEEN
  if(type == 5){
    for(int i = 0 ; i < 4; i++){
      vector<Location> new_moves1 = diagonal_moves(i, new_board, l, side);
      new_moves.insert(new_moves.end(), new_moves1.begin(), new_moves1.end());
    }
    for(int i = 0 ; i < 4; i++){
      vector<Location> new_moves1 = straight_moves(i, new_board, l, side);
      new_moves.insert(new_moves.end(), new_moves1.begin(), new_moves1.end());
    }
  }
  
  // KING
  if(type == 6){
    new_moves = king_moves(state->board, l, side);
  }

  // PAWN
  if(type == 1){
    vector<Location> all_pawn_moves = pawn_moves(piece, new_board, l, side);

    for(Location move: all_pawn_moves){

      if((side and move.y < 7) or (!side and move.y > 0)){
        new_moves.push_back(move);
      }
    }
  }


  for(Location valid_location : new_moves){
 
    vector<vector<int>> state_board = new_board;
    state_board[start_l.y][start_l.x] = 0;
    state_board[valid_location.y][valid_location.x] = piece;
    State new_state = {
      state_board,
      !state->turn,
      state->count + 1,
      0,
      state->castles,
      make_move_string(piece, valid_location.x, valid_location.y),
      state
    };
    if(evaluation >= 0){
      new_state.eval = static_evaluator(new_state, evaluation);
    }
    

    move_states.push_back(new State(new_state));
    
  }

  return move_states;

}

//check if side is in check based on current state
bool in_check(bool side, State* state, int evaluation){

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      int piece = state->board[i][j];
      if((side and piece < 0) or (!side and piece > 0)){
        vector<State*> piece_moves = get_piece_moves(piece, !side, state, {j,i}, -1);
        for(State* move : piece_moves){

          if(!find_king(move, side)){
            for(State* move : piece_moves){
              delete move;
            }
            return true;
          }
        }
        for(State* move : piece_moves){
          delete move;
        }
      }
    }
  }
  return false;
}

vector<State*> get_all_valid_moves(State* state, int evaluation){
  vector<State*> moves ={};
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      int piece = state->board[i][j];
      if((state->turn and piece > 0) or (!state->turn and piece < 0)){

        vector<State*> piece_moves = get_piece_moves(piece, state->turn, state, {j,i}, evaluation);


        for(State* move : piece_moves){
          if(!in_check(state->turn,move, evaluation)){
            moves.push_back(move);
          }
        }
      }
    }
  }  

  return moves;
}
