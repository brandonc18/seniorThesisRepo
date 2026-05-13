#include "MoveGen.h"
#include "Search.h"
#include "chess-types.h"
#include <iostream>

using namespace std;

// Convert position to its square index (0–63)
int algebraicToSquare(const string &s) {
	if (s.length() != 2)
		return -1;
	char file = tolower(s[0]);
	char rank = s[1];
	if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
		return -1;
	return (rank - '1') * 8 + (file - 'a');
}

void pickPromotion(Move &m) {
	while (true) {
		cout << "Pick Promotion (q, n, b, r): ";
		char choice;
		cin >> choice;
		if (choice == 'q') {
			m.is_promotion = 1;
			break;
		} else if (choice == 'n') {
			m.is_promotion = 2;
			break;
		} else if (choice == 'b') {
			m.is_promotion = 3;
			break;
		} else if (choice == 'r') {
			m.is_promotion = 4;
			break;
		}
		cout << "\nInvalid choice.\n";
	}
}

int main() {
	Board board;
	MoveGen generator;
	Search searcher(generator);
	int totalMoves = 0;

	cout << "1. Play against Bot (Human vs Bot)\n";
	cout << "2. Watch Bot vs Bot\n";

	int choice;
	cin >> choice;

	bool humanPlays = true;
	int whiteDepth = 3;
	int blackDepth = 3;

	if (choice != 1) {
		humanPlays = false;
		cout << "White bot search depth: ";
		cin >> whiteDepth;
		cout << "Black bot search depth: ";
		cin >> blackDepth;
	} else {
		cout << "Bot search depth: ";
		cin >> blackDepth;
		whiteDepth = blackDepth;
	}

	while (true) {
		system("clear");

		bool whiteToMove = (board.getSideToMove() == ecWhite);

		// Generate legal moves
		MoveList moves;
		generator.generateAllMoves(board, moves);
		generator.removeIllegalMoves(board, moves);

		cout << moves.size() << " legal moves\n";
		board.print();

		if (moves.empty()) {
			int kingSq = (whiteToMove ? board.getWhiteKing() : board.getBlackKing()).pop_lsb();
			bool inCheck = generator.isSquareAttacked(board, kingSq, !whiteToMove);
			cout << (whiteToMove ? "White" : "Black") << (inCheck ? " is checkmated!" : " stalemate!") << "\n";
			break;
		}

		cout << (whiteToMove ? "White" : "Black") << " to move\n";

		Move bestMove;

		if (whiteToMove && humanPlays) {
			string input;
			cin >> input;

			if (input.length() < 4) {
				cout << "Invalid input format.\n";
				continue;
			}

			string fromStr = input.substr(0, 2);
			string toStr = input.substr(2, 2);
			char promoChar = (input.length() >= 5) ? tolower(input[4]) : 0;

			int fromSq = algebraicToSquare(fromStr);
			int toSq = algebraicToSquare(toStr);

			// Find the move in the legal list
			bool found = false;
			for (const Move &m : moves) {
				if (m.from_square == fromSq && m.to_square == toSq) {
					bestMove = m;
					found = true;
					break;
				}
			}

			if (!found) {
				continue;
			}

			if (bestMove.is_promotion > 0) {
				pickPromotion(bestMove);
			}

			board.makeMove(bestMove);

		} else {
			cout << "Bot is thinking...\n";

			uint64_t leaves = 0;
			int searchDepth = whiteToMove ? whiteDepth : blackDepth;

			bestMove = searcher.findBestMove(board, searchDepth);
			board.makeMove(bestMove);
			totalMoves++;
		}
		if (totalMoves > 250) {
			cout << "Draw by repetition\n";
			break;
		}
	}

	// cout << totalMoves << endl;

	return 0;
}
