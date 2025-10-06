# Senior Thesis Repo: [Chess Engine]
## Introduction
The Markov Chain is a mathematical model that determines the likelyhood of transitioning from one state to the next. It only depends on the current state, and has no memory of how it got there. In this Chess Engine, it will find every legal move, and then using the markov chain, it will determine the best moves to make, and the win probabilities of those moves. The current state in a chess game can be determined using FEN which is standard notation for the current state of the board. 

### Purpose
The purpose of this engine will be threefold:
- To be enable players to test themselves against an engine that can use 100,000 to 1,000,000 historical games to make statistical decisions
- To enhance player's decision making skills and insight through the probabilities offered by the markov chain
- To evaluate the effectiveness of the markov chain in chess against other people and chess AI's

### Scope
The Engine will:
- Adhere to standard chess rules
- Handle move selection and probabilites through Markov Chain
- Use database built off of PGN files; including moves, wins and losses, ELO's, and FENs
- Print the board in text in a terminal environment

### Definitions, Acronyms, and Abbreviations
- **FEN**: Forsyth-Edwards Notation is the standard notation to describe positions of a chess game, specifically the positions of all pieces on the board at a given moment. 
- **Markov Chain**: a Markov chain is a stochastic process describing a sequence of possible events or states in which the probability of each event depends only on the state attained in the previous event.
- **PGN**: Portable Game Notation (PGN) is a standard plain text format for recording chess games (including moves and other relevant data).

## Overview
The Markov Chain Chess Engine is a terminal application that is designed to determine probabilities of chess moves. It serves to help improve players and/or challenge them at chess.

## Use Cases

### Use Case 1.1: Play Against Engine
- **Overview**: Player challenges the Engine

### Use Case 1.2: Player uses Engine
- **Overview**: Player learns from the Engine's probalities on what are good moves vs bad moves

### Use Case 1.3: Engine against other AI's or players
- **Overview**: User uses engine against others to determine how effective it is
