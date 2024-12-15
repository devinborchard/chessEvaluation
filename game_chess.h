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

#include "alpha_beta_chess.h"


using namespace std;

class Vs_AI {
public:
    int evaluation;
    int depth;
    State initial_state;
    bool color;

    State* current_state;
    bool finished = false;

    A_B_Pruning pruning_instance;

    Vs_AI(int d, bool c, int e, State* init) : 
    depth(d), 
    color(c), 
    evaluation(e),
    current_state(init),
    pruning_instance(d, current_state, e) {
        // Initializations can be done directly in the member initializer list
    }

    void ai_turn(vector<State*> next_moves){
        vector<State> state_items{};
        for(State* move : next_moves){
            state_items.push_back(*move);
        }
        State next = pruning_instance.calculate(current_state, state_items);
        current_state = &next;
        cout << next.last_move;
    }

    void human_turn(vector<State*> next_moves){
        
        bool valid_move = false;
        while(!valid_move){
            string move_input;
            getline(std::cin, move_input);
            for(State* move: next_moves){
                if(move->last_move == move_input){
                    current_state = move;
                    valid_move = true;
                    cout << "Move Made:\n";
                    print_state(*current_state);
                }
            }
            if(!valid_move){
                cout << "invalid move input\n";
            }
        }
    }

    bool turn(){
        vector<State*> next_moves = get_all_valid_moves(current_state, evaluation);
        if(next_moves.size() == 0){
            return false;
        }
        if(current_state->turn == color){
            ai_turn(next_moves);
        }
        else{
            human_turn(next_moves);
        }
        return true;
    }
};
