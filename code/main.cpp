#include "MoveGen.h"
#include <iostream>
#include <stack>

using namespace std;

// Convert position (ex: e2) to its square index (0–63)
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
	MoveList moves;
	stack<Board> boardStates;
	bool legal = true;

	while (true) {
		system("clear");
		board.print();
		if (!legal) {
			cout << "Illegal Move Put King in Check\n";
		}

		bool white = (board.getSideToMove() == ecWhite);
		cout << (white ? "White" : "Black") << " to move\n\n";

		generator.generateAllMoves(board, moves);
		generator.removeIllegalMoves(board, moves);
		if (moves.size() == 0) {
			system("clear");
			board.print();
			cout << (white ? "White" : "Black") << " loses!\n";
			break;
		}

		cout << moves.size() << " legal moves:\n";

		// for (int i = 0; i < moves.size(); i++) {
		// 	const Move &m = moves[i];

		// 	string moveStr = stringSquare[m.from_square] + stringSquare[m.to_square];

		// 	cout << "  " << (i + 1) << ". " << moveStr << m.is_promotion << "\n";
		// }

		// cout << "> ";

		string input;
		cin >> input;

		if (input.empty())
			continue;

		Move chosenMove;
		bool found = false;

		if (input.length() >= 4) {
			string from = input.substr(0, 2);
			string to = input.substr(2, 2);
			char promoChar = (input.length() >= 5) ? tolower(input[4]) : 0;

			int fromSq = algebraicToSquare(from);
			int toSq = algebraicToSquare(to);
			if (fromSq == -1 || toSq == -1) {
				cout << "Move not found or illegal.\n";
				continue;
			}

			// Map char to our promotion index (1-4)
			int promoIndex = 0;
			if (promoChar) {
				if (promoChar == 'q')
					promoIndex = 4;
				else if (promoChar == 'r')
					promoIndex = 3;
				else if (promoChar == 'b')
					promoIndex = 2;
				else if (promoChar == 'n')
					promoIndex = 1;
			}

			for (const Move &m : moves) {
				if (m.from_square == fromSq && m.to_square == toSq) {
					chosenMove = m;
					found = true;
					break;
				}
			}
		}

		if (found) {
			if (chosenMove.is_promotion > 0)
				pickPromotion(chosenMove);
			boardStates.push(board);
			board.makeMove(chosenMove);
		} else {
			continue;
		}
	}
	while (boardStates.size() > 0) {
		// boardStates.top().print();
		boardStates.pop();
	}

	return 0;
}
