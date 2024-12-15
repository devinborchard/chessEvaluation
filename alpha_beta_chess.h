#include <stdio.h>
#include<fstream>
#include<sstream>
#include<string>
#include<tuple>
#include<vector>
#include <cstring>
#include <cmath>
#include <random>
#include <math.h>
#include <iostream>
#include <queue>
#include "puzzles.h"
#include <limits>

using namespace std;
int counter = 0;

double get_max(double d1, double d2){
    if(d1 > d2) return d1;
    return d2;
}
double get_min(double d1, double d2){
    if(d1 < d2) return d1;
    return d2;
}

class A_B_Pruning {
public:
    int depth;
    State* initial_state;
    int evaluation;

    A_B_Pruning(int d, State* n, int e) : depth(d), initial_state(n), evaluation(e) {
        srand((unsigned int)time(NULL));
    }

    struct PruneObj{
        double eval;
        State child;
    };

    PruneObj alpha_beta_search(State* s, double alpha, double beta, int running_depth, bool maximizingPlayer) {
        if(s->eval == -10000){
            double new_eval = -10000+depth;
            s->eval = new_eval;
            return {new_eval, *s};
        }
        if(s-> eval == 10000){
            double new_eval = 10000-depth;
            s->eval = new_eval;
            return {new_eval, *s};
        }
        if(running_depth == 0){
            return {s->eval, *s};
        }
        vector<State*> children = get_all_valid_moves(s, evaluation);
        if(children.size() == 0 and s->eval == 0){
            return {0, *s};
        }
        PruneObj return_vals;

        double value = -numeric_limits<double>::infinity();
        if(!maximizingPlayer) value = numeric_limits<double>::infinity();
        State best_child;

        if(maximizingPlayer){
            for(State* child : children){
                PruneObj evals = alpha_beta_search(child, alpha, beta, running_depth-1, child->turn);
                if (evals.eval > value) {
                    value = evals.eval;
                    best_child = *child;
                }
                alpha = get_max(alpha, evals.eval);
                if(beta <= alpha){
                    break;
                }
            }
            return_vals = {value, best_child};

        }else{
            State best_child;
            for(State* child : children){
                PruneObj evals = alpha_beta_search(child, alpha, beta, running_depth-1, child->turn);
                if (evals.eval < value) {
                    value = evals.eval;
                    best_child = *child;
                }
                beta = get_min(beta, evals.eval);
                if(beta <= alpha){
                    break;
                }
            }

            return_vals = {value, best_child};
        }

        for(State* child : children){
            delete child;
        }
        counter = counter + 1;
        return return_vals;
    }

    State calculate(State* initialState, vector<State> children) {

        double alpha = -numeric_limits<double>::infinity();
        double beta = numeric_limits<double>::infinity();

        PruneObj result = alpha_beta_search(initial_state, alpha, beta, depth, initial_state->turn);

        return result.child;
    }
};