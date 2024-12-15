import http.client
import json
import sys

from puzzle_builder import fen_to_board


def stock_fish_eval(fen):
    fen_without_turn = fen[0:len(fen)-2]
    fen = fen_without_turn + " w"
    conn = http.client.HTTPSConnection("chess-stockfish-16-api.p.rapidapi.com")
    payload = "fen=" + fen
    # print("FEN IN PAYLOAD:|"+payload+"|")
    headers = {
        'content-type': "application/x-www-form-urlencoded",
        'X-RapidAPI-Key': "70e99d544fmsh6bd2eb24f397882p153cc1jsn1a95c2671aa1",
        'X-RapidAPI-Host': "chess-stockfish-16-api.p.rapidapi.com"
    }
    conn.request("POST", "/chess/api", payload, headers)
    res = conn.getresponse()
    data = res.read()
    return data.decode("utf-8")


def board_to_fen(board):
    piece_chars = {6: 'K', 5: 'Q', 4: 'R', 3: 'B', 2: 'N', 1: 'P',
                   -6: 'k', -5: 'q', -4: 'r', -3: 'b', -2: 'n', -1: 'p', 0: '.'}
    
    fen_board = ''
    empty_count = 0
    
    for row in board:
        for square in row:
            if square == 0:
                empty_count += 1
            else:
                if empty_count > 0:
                    fen_board += str(empty_count)
                    empty_count = 0
                fen_board += piece_chars[square]
        
        if empty_count > 0:
            fen_board += str(empty_count)
            empty_count = 0
        
        fen_board += '/'
    
    fen_board = fen_board[:-1]  # Remove the trailing slash
    
    return fen_board


def read_board(index, puzzle_type):
    path = "puzzles/" + puzzle_type + "/" + str(index) + ".txt"
    board_string = ""
    with open(path) as file1:
    # Reading from a file
        board_string = board_string + file1.read()
    return board_string

def string_to_board(board_string):
    array_2d = []
    row = []
    new_row = False

    # Tokenize the input string
    tokens = board_string.split(',')

    for token in tokens:
        # Remove leading and trailing whitespace
        token = token.strip()

        # Handle opening curly brace
        if '{' in token:
            # If it's not the start of the input string, push the current row into the 2D array
            token = token[1:]  # Remove the opening curly brace

        # Handle closing curly brace
        if '}' in token:
            token = token[:-1]  # Remove the closing curly brace
            new_row = True  # Start a new row

        # Token is a number
        num = int(token)

        # If starting a new row, add the first number to the row and reset the flag
        if new_row:
            row.append(num)
            array_2d.append(row)
            row = []
            new_row = False
        else:
            row.append(num)

    # If there's any remaining row, push it into the 2D array
    if row:
        array_2d.append(row)

    return array_2d    

def test(fen):
        # Test the function
    # print("TESTING WITH FISH: ", fen)
    stock_fish_response = json.loads(stock_fish_eval(fen))
    # print("RESPONSE: ", stock_fish_response)
    return stock_fish_response["bestmove"]
