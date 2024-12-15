from subprocess import Popen, PIPE, TimeoutExpired
from stockfish import test, read_board, string_to_board
from puzzle_builder import fen_to_board
import os
from datetime import datetime


def translate_move(move,fen):
    start = move[0:2]
    end = move[2:4]
    # print("START: ", start)
    # print("END: ", end)

    fen = fen[0:len(fen)-2]

    board = fen_to_board(fen)

    board = board[::-1]

    board_x = 0
    if(start[0] == 'a'): 
        board_x = 0
    if(start[0] == 'b'): 
        board_x = 1
    if(start[0] == 'c'): 
        board_x = 2
    if(start[0] == 'd'): 
        board_x = 3
    if(start[0] == 'e'): 
        board_x = 4
    if(start[0] == 'f'): 
        board_x = 5
    if(start[0] == 'g'): 
        board_x = 6
    if(start[0] == 'h'): 
        board_x = 7
    board_y = int(start[1]) -1

    start_piece = board[board_y][board_x]
    # print("START PIECE: ", start_piece)
    # print("START x: ", board_x)
    # print("START y: ", board_y)
    abs_piece = abs(start_piece)
    piece_letter = ""
    if(abs_piece == 1): piece_letter = "p"
    if(abs_piece == 2): piece_letter = "n"
    if(abs_piece == 3): piece_letter = "b"
    if(abs_piece == 4): piece_letter = "r"
    if(abs_piece == 6): piece_letter = "k"
    if(abs_piece == 5): piece_letter = "q"

    return piece_letter + end

def write_results(moves, fen, path, depth, eval):
    board = fen_to_board(fen[:-2])
    board_string = ""
    for row in board:
        for char in row:
            board_string = board_string + "|"
            if char >= 0:
                board_string = board_string + "_"
            board_string = board_string + str(char)
        board_string = board_string + "|\n"

    ai_move = moves[0]
    
    fish_move = moves[1]
    path_fen = fen.replace("/","_",7)
    exists = os.path.isfile(path+"/"+path_fen+".txt")
    results = ""
    if not exists:
        results = fen + "\n\n" + board_string
    results = results + "\n\nEvaluator: " + str(eval)
    results = results + "\nDepth: " + str(depth)
    if ai_move == "...":
        ai_move = "DNF"
    results = results + "\nAI Move: " + ai_move + "\nStock Move: " + fish_move
    results = results + "\nCorrect: " + str(ai_move == fish_move)
    
    f = open(path+"/"+path_fen+".txt", "a")
    f.write(results)
    f.close()

def run_puzzle(depth, evaluator, fen):

    correct_move = test(fen)
    translated_move = translate_move(correct_move,fen)

    timeout = 600
    params = [str(depth), str(evaluator), fen]

    cmd = " ".join(["a.exe"] + params)
    ai_move = "..."
    try: 
        process = Popen(cmd, stdin=PIPE, stdout=PIPE, stderr=PIPE, shell=True, text = True)
        ai_move, _ = process.communicate(timeout=timeout)
        if process.returncode is None:
            process.kill()
            ai_move = "DNF"

    ##########

        return [ai_move, translated_move]
        
    except TimeoutExpired:
            # Timeout occurred
            return[ai_move,translated_move]
            # return None, None, f"Process exceeded timeout of {timeout} seconds and was killed."
    except Exception as e:
        # Other exceptions
        return[ai_move,translated_move]


def main():
    path = "fen.txt"
    fens = ""
    with open(path) as file1:
        fens = fens + file1.read()

    fen_list = fens.split("\n")

    now = datetime.now()
    date_time = now.strftime("%m_%d_%Y_%H_%M_%S")
    os.mkdir("test_results/"+date_time)

    for fen in fen_list:
    # for i in range(2):
        # fen = fen_list[i]
        if  len(fen) > 0 and fen[0] != '#':
            print("FEN: ", fen)
            for eval in range(3):
                for d in range(3):
                    depth = d + 1
                    moves = run_puzzle(depth, eval, fen)
                    write_results(moves, fen, "test_results/"+date_time, depth, eval)
    

if __name__ == "__main__":
    
    main()