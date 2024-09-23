Chess Project based off of the two Sebastein Lague youtube videos on creating a chess bot. Intended as a long term project, I'm just looking for it to work initially, and intend to
be optimising it for a long time. Created a 2 player program first, before implementing search and evaulation functions to turn it into a bot.


For the random numbers needed for the zobrist hashing, instead of using the mersenne twister, I am trying a different PRNG called PCG. I am mainly using this as practice as the
mersenne twister is supposed to be fairly slow, and so I was interested in whether another PRNG would be as easy to use as MT.



Notes:
Going to try to use Qt for GUI purposes, until then I am using unicode chess figures in the terminal

List to do:
-Need game history for three fold repetition rule
-FENstring halfmove clock and fullmove number, implement these and the rules
-Game History is big on data so need to implement zobrist hashing to keep track of gamestates - makes it easier to look up board evaulations if the same posiiton is reached again in a game.
-Update EnPassantFile in the actual code. See if you actually need lastMoves with this variable working
- Zobrist functions now work, now need to update the key every move, and then store the keys and check for 3 fold repetition

-What if the move is a capture and a pawn promotion????? Check this edge case
-Need to improve the MakeMove funciton anyway, Jemma said it was too repetitive so think about a new way to ley it out

Remaining Zobrist stuff:
- Captures, taking away the hash of the captured piece, including pawn promotion captures. EP captures should already be done
- Updating EP file - This will probably involve refactoring how EnPassant is checked, i.e. lastmoves array is probably redundant now with EnPassantFile
- Actually make sure to include the new key in GameHistory after every move


Also want to try refactoring the code to practice using virtual functions, abstract classes and polymorphism as a whole. I think this should be pretty easy for implement for the Move Generation at the least, and should neaten up how it is done, rather than a series of individual functions. It might be slower this way but seems like good practice