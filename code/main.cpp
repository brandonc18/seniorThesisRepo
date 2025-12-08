#include "MoveGen.h"
#include <iostream>

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

int main() {
	Board board;
	MoveGen generator;
	MoveList moves;

	while (true) {
		system("clear");
		board.print();

		bool white = (board.getSideToMove() == ecWhite);
		cout << (white ? "White" : "Black") << " to move\n\n";

		generator.generateAllMoves(board, moves);
		cout << moves.size() << " legal moves:\n";

		for (int i = 0; i < moves.size(); i++) {
			const Move &m = moves[i];
			string moveStr = stringSquare[m.from_square] + stringSquare[m.to_square];

			cout << "  " << (i + 1) << ". " << moveStr << "\n";
		}
		cout << "\n> ";

		string input;
		cin >> input;

		if (input.empty())
			continue;

		Move chosenMove;
		bool found = false;

		if (input.length() >= 4) {
			string from = input.substr(0, 2);
			string to = input.substr(2, 2);
			char promo = (input.length() >= 5) ? tolower(input[4]) : 0;

			int fromSq = algebraicToSquare(from);
			int toSq = algebraicToSquare(to);
			if (fromSq == -1 || toSq == -1) {
				cout << "Move not found or illegal.\n";
				continue;
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
			board.makeMove(chosenMove);
		} else {
			continue;
		}
	}

	return 0;
}
