#include<string>

#include "game_chess.h"


using namespace std;



int main(int argc, char *argv[]) {

  int depth = stoi(argv[1]);
  int evaluation = stoi(argv[2]);

  string fen = argv[3];

  //remvove the turn for the game state, we are assuming all tests are white to move
  fen = fen.substr(0, fen.length() - 2);

  State* init = get_puzzle(evaluation, fen);

  Vs_AI game(depth, true, evaluation, init);
  bool still_going = true;

  still_going = game.turn();



  return 0;
}