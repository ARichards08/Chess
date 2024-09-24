Chess Project based off of the two Sebastein Lague youtube videos on creating a chess bot. Intended as a long term project, I'm just looking for it to work initially, and intend to
be optimising it for a long time. Created a 2 player program first, before implementing search and evaulation functions to turn it into a bot.


For the random numbers needed for the zobrist hashing, instead of using the mersenne twister, I am trying a different PRNG called PCG. I am mainly using this as practice as the
mersenne twister is supposed to be fairly slow, and so I was interested in whether another PRNG would be as easy to use as MT.



Notes:
Going to try to use Qt for GUI purposes, until then I am using unicode chess figures in the terminal

List to do:
-FENstring halfmove clock and fullmove number, implement these and the rules
-Use Game History Zobrist Keys to store gamestate evaulations so that we can easily look up the evaulation of a previous gamestate

-Need to improve the MakeMove funciton anyway, Jemma said it was too repetitive so think about a new way to let it out


Also want to try refactoring the code to practice using virtual functions, abstract classes and polymorphism as a whole. I think this should be pretty easy for implement for the Move Generation at the least, and should neaten up how it is done, rather than a series of individual functions. It might be slower this way but seems like good practice