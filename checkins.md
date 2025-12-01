## Week 1 Summary (09/29/2025)

### This week I worked on:

- I searched for websites that I can download free pgn files from.
- Determined how I want my database to be set up, it will include Games, Plays, FEN'S, ELO's, and other less important stats that are contained in PGN files.
- Researched how to translate these PGN files into my database.

### This week I learned:

- https://www.pgnmentor.com/files.html and https://database.lichess.org/ host free downloads for PGN files.
The first site being PGNs for world top players, the second being larger monthly records of all games on that website.
- Python has a chess library, zstandard, io, and sqlite3 that can easily handle translating PGN files.

### My successes this week were:

- Getting a basic understanding of how to proceed, and finding sources to do so.
- Finding the appropriate libraries to create the python script that will start this project.

### The challenges I faced this week were:

- Starting the script for my database creator.
- Trying to determine if I want to use c++ or python for the main part of the project, both have chess libraries that I can explore.
- Learning to understand chess notation for movements.

---

## Week 2 Summary (10/06/2025)
### This week I worked on:

- My plan for the structure of my chess engine.
- Studied how most chess engines function.

### This week I learned:

- How bitboards can be used to efficiently calculate possible moves.
- Trying to use a markov chain would be to uncertain and would be a brute force method.
- That pieces and board positions have values that help the engine make decisions.
- It is more efficient to calculate psuedo legal moves (moves that a piece could make if there were no obstacles) than straight up calculating legal moves.

### My successes this week were:

- Changing course from Markov to a more traditional chess engine.
- Researching the inner structures of chess engines.
- Choosing c++ as my language to use.

### The challenges I faced this week were:

- Understanding how bitboards work and how to use them.
- Switching course from Markov to Traditional chess engine plan.
- Trying to figure out if I want to use bitboards, or start with something a little simpler yet less efficient.

---

## Week 3 Summary (10/13/2025)
### This week I worked on:

- Started my code and got a starting board layout.
- Planning a good foundation for this project that will make it flexible later on.

### This week I learned:

- How to use U64 or uint64_t for my bitboards.
- How to use bitwise operations and hexadecimal representation for my bitboards.

### My successes this week were:

- Creating a base board layout using bitboards.
- Creating a function to determine piece present at certain locations.
- Creating a function to print the current board state.

### The challenges I faced this week were:

- Understanding hexadecimal in the creation of my bitboards.
- Accidentally deleting my week 3 summary commit.

---

## Week 4 Summary (10/20/2025)
### This week I worked on:

- Starting the Psuedo Legal Move functions, mainly the pawn movement.

### This week I learned:

- How to do pawn movements, 1 or 2 spaces at start, capture, and promotion.
- How to use Rank and File calculations.

### My successes this week were:

- Created a mostly functional getPawnMoves function.
- Used bitboards to for efficient move checking.
- Got notation such as "e3 e4" working instead of numbers.

### The challenges I faced this week were:

- Understanding U64 operations.
- Getting pawn promotion to work without issues.
- Making sure pawn captures only happen when there is something to capture.

---

## Week 5 Summary (10/27/2025)
### This week I worked on:

- My plan for this project for what classes I'll need and what they'll contain.
- Refactoring my Board class and creating a Bitboard class.

### This week I learned:

- Two of the bigger chess search algorithms are Monte-Carlo Tree Search and MiniMax.
- [Monte-Carlo selects a leaf from the root node that hasn't been added to the tree yet, adds it,
   simulates it to the end of the game, and the propagates the result of the game through the tree. ]
- [Minimax searches through the moves it can make to a certain depth and determines a score based on an evaluation function.
   It selects the move that gives you the best score accounting for the opponent making the best counterplay.]

### My successes this week were:

- Creating a Bitboard class for a better foundation in my Board class.
- Refactoring my Board class to use the new Bitboard Class.
- Removed move generation for now, to be moved to my MoveGen class.
- Creating a better plan for the execution of this engine.

### The challenges I faced this week were:

- Creating the Bitboard class and researching how to do certain bitboard operations such as finding trailing zeros.
- Researching what type of Bitboard operations I'll need for a chess engine, such as flipping the bits in a bitboard.
- Trying to determine how many classes I'll need and what they'll do.

---

## Week 6 Summary (11/03/2025)
### This week I worked on:

- Creating my Move struct and MoveGen Class. 
- Updating my Board class to work with the MoveGen class better.

### This week I learned:

- The most efficient way to generate moves for pawns, knights, and kings is to precompute the moves into Bitboards.
- For when I get to sliding pieces (rooks, bishops, queens), I'll use magic bitboards.
- Magic Bitboards are precomputed like the others, but they have to deal with blocking pieces within the piece rays.
- They are then hashed "to map the vector of all relevant occupancies to a range of attack-sets per square"

### My successes this week were:

- Created the Move struct to track moves from one location to another.
- Started the MoveGen class, using a vector to store all the moves.
- Tested my Board function with Bitboard operations to see how I'll make moves easily.

### The challenges I faced this week were:

- Figuring out how to generate moves efficiently. 
- Determining how to best make my MoveGen function since it'll only need to be used once and isn't initiliazed.

---

## Week 7 Summary (11/10/2025)
### This week I worked on:

- Pawn moves generation.
- Research on search functions for when move generation is complete.
- Research for sliding pieces and hashing them.

### This week I learned:

- Pawn moves is more complicated than knights and king movements.
- There's more movements and conditions for pawns than kings and knights.

### My successes this week were:

- Finishing Pawn movement generation.
- Ready to start and finish Knight and King movements faster.

### The challenges I faced this week were:

- Getting Pawn generation to work took longer than expected.
- Trying to use const on Bitboard returns from Board class.

---

## Week 8 Summary (11/17/2025)
### This week I worked on:

- Cleaned up my MoveGen and Board classes.

### This week I learned:

- Didn't learn much, didn't have a lot of time.

### My successes this week were:

- Fixing my MoveGen class, it was having compile issues but is working now.
- Moved const variables to a new header file since it didn't make sense for them to be in the Board class.

### The challenges I faced this week were:

- MoveGen wasn't working how I wanted it to, so I had to redo some of it.

---

## Week 9 Summary (12/01/2025)
### This week I worked on:

- Finishing my MoveGen class.
- Starting to work on the loop for the game.

### This week I learned:

- ChessProgramming.org has lots of information on Magic Bitboards as well as common code that most engines use for
generating the movement boards.

### My successes this week were:

- I finished my move generation for all moves (not including castling and en passant)

### The challenges I faced this week were:

- Getting the magic bitboards to properly find the right U64 movements board for the sliding pieces.

---

## Week 10 Summary (MM/DD/YYYY)
### This week I worked on:

[Your answer here]

### This week I learned:

[Your answer here]

### My successes this week were:

[Your answer here]

### The challenges I faced this week were:

[Your answer here]

---

## Week 11 Summary (MM/DD/YYYY)
### This week I worked on:

[Your answer here]

### This week I learned:

[Your answer here]

### My successes this week were:

[Your answer here]

### The challenges I faced this week were:

[Your answer here]

---

## Week 12 Summary (MM/DD/YYYY)
### This week I worked on:

[Your answer here]

### This week I learned:

[Your answer here]

### My successes this week were:

[Your answer here]

### The challenges I faced this week were:

[Your answer here]

---

## Week 13 Summary (MM/DD/YYYY)
### This week I worked on:

[Your answer here]

### This week I learned:

[Your answer here]

### My successes this week were:

[Your answer here]

### The challenges I faced this week were:

[Your answer here]

---
