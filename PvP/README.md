Keep a list of legal moves for each piece.
Keep track of pinned pieces and squares the other player is attacking.
Check for checks
Keep track of which colour piece is being moved.
Can't allow empty positions to be moved.


Creates an array at the program start to calculate the number of square in each 8 compass directions for each square on the board.
After each move, iterate over the whole board and for each piece it finds, calculate every legal move it can make, adding to a list. Doing this and searching for the move should be faster than usign a lot of if statements to see if the move is valid each time, and the bot will need a list of every move to work out the best one anyway



Might need to use FEN notation, at least for the bot, not sure about 2P.

Going to try to use Qt for GUI purposes

V2 list:
Pieces need to be represented by numbers. I can worry about size and optimisation later, just start with the board being a vector of ints.


List to do:
Check states
Do this by keeping track of squares that the enemy is attacking on a turn
Also keep track of pinned pieces for discovered checks
Keep track of all squares with a piece in an array/map
Finish FEN string stuff, only geot the pieces in the right order, need to take in the castling info etc
Need game history for three fold repetition rule

Testing the move list