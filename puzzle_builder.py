def fen_to_board(fen):
    piece_dict = {'r': -4, 'n': -2, 'b': -3, 'q': -5, 'k': -6, 'p': -1,
                  'R': 4, 'N': 2, 'B': 3, 'Q': 5, 'K': 6, 'P': 1, '.': 0}

    rows = fen.split('/')
    board = []

    for row in rows:
        current_row = []
        for char in row:
            # print("CHAR: ", char)
            if char.isdigit():
                current_row.extend([0] * int(char))
            elif char in piece_dict:
                current_row.append(piece_dict[char])
            else:
                raise ValueError("Invalid FEN string")
        board.append(current_row)

    return board



def create_puzzle(fen, type):
    fen_without_turn = fen[0:len(fen)-2]
    # print("NEW: ", fen_without_turn)
    board = fen_to_board(fen_without_turn)

    board_string = ""
    for row in board:
        row_string = "{"
        count = 0
        for number in row:
            count = count + 1
            if number >= 0:
                row_string = row_string + " "
            row_string = row_string + str(number)
            if count < 8:
                row_string = row_string + ","
        row_string = row_string + "},"
        board_string = board_string + row_string
    board_string = board_string[:-1]

    return board_string