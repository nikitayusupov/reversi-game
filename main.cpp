#include <iostream>
#include <cassert>
#include <cmath>
#include <time.h>
#include <algorithm>
#include <string>
#include <cstring>

using std::cout;
using std::max;
using std::min;
using std::cin;
using std::string;

#define endl '\n'
int inf = (int)1e9;

enum Color {
	FREE,
	BLACK,
	WHITE
};

Color my_color;

#define another_color(color) ((color == BLACK) ? WHITE : BLACK)

int di[8] = {0,  0, 1,  1, 1, -1, -1, -1};
int dj[8] = {1, -1, 0, -1, 1,  0, -1,  1};

bool in_bounds(int i, int j) {
	return i >= 0 && j >= 0 && i < 8 && j < 8;
}

struct Desk {
	Color desk[8][8] = {};

	void init() {
		desk[3][3] = WHITE;
		desk[3][4] = BLACK;
		desk[4][3] = BLACK;
		desk[4][4] = WHITE;
	}

	int cnt_free() {
		int ans = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (desk[i][j] == FREE) {
					ans++;
				}
			}
		}
		return ans;
	}

	void print() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (desk[i][j] == FREE) {
					cout << '-';
				} else if (desk[i][j] == WHITE) {
					cout << 'W';
				} else {
					cout << 'B';
				}
			}
			cout << endl;
		}
		cout << endl;
	}

	void read() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				char c;
				cin >> c;
				if (c == '-') {
					desk[i][j] = FREE;
				} else if (c == 'W') {
					desk[i][j] = WHITE;
				} else {
					desk[i][j] = BLACK;
				}
			}
		}
	}

	bool can_go_this_dir(int i, int j, Color color, int dir) const {
		// типа desk[i][j] == color

		int cnt_another_color = 0;
		bool achieve_my_color = 0;
		bool achieve_free = 0;
		Color opponent_color = another_color(color);
		while (true) {
			i += di[dir], j += dj[dir];
			if (!in_bounds(i, j)) {
				break;
			}
			if (desk[i][j] == FREE) {
				achieve_free = true;
				break;
			}
			if (desk[i][j] == opponent_color) {
				++cnt_another_color;
			}
			if (desk[i][j] == color) {
				achieve_my_color = true;
				break;
			}
		}
		if (achieve_free) {
			return false;
		}
		return (cnt_another_color > 0) && achieve_my_color;
	}

	bool can_set_color(int i, int j, Color color) const {
		if (desk[i][j] != FREE) {
			return false;
		}
		bool can = 0;
		for (int dir = 0; dir < 8; ++dir) {
			can |= can_go_this_dir(i, j, color, dir);
		}
		return can;
	}

	void change_color_in_dir(int i, int j, Color color, int dir) {
		assert(desk[i][j] == color);

		bool achieve_my_color = 0;
		Color opponent_color = another_color(color);
		while (true) {
			i += di[dir], j += dj[dir];
			if (!in_bounds(i, j)) {
				break;
			}
			assert(desk[i][j] != FREE);
			if (desk[i][j] == color) {
				achieve_my_color = true;
				break;
			}
			desk[i][j] = color;
		}
		assert(achieve_my_color);
	}

	void set_color(int i, int j, Color color) {
		assert(can_set_color(i, j, color));
		desk[i][j] = color;
		for (int dir = 0; dir < 8; ++dir) {
			if (can_go_this_dir(i, j, color, dir)) {
				change_color_in_dir(i, j, color, dir);
			}
		}
	}

	double evristika() const {

		int coin[8][8] = {
			{30,   -10,  11,  10,   10,  11, -10,   30},
			{-10,  -10,  -4,  1,   1,  -4, -10,  -10},
			{11,   -4,   2,   2,   2,  2,  -4,   11},
			{10,    1,   2,   0,  0,  2,   1,   10},
			{10,    1,   2,   0,  0,  2,   1,   10},
			{11,   -4,   2,   2,   2,  2,   -4,  11},
			{-10,  -10,  -4,  1,   1, -4,   -10, -10},
			{30,   -10,  11,  10,  10,  11,  -10, 30}
		};

		// перебираем угловые клетки
		for (int i = 0; i < 8; i += 7) {
			for (int j = 0; j < 8; j += 7) {
				if (desk[i][j] == my_color) {
					for (int dir = 0; dir < 8; ++dir) {
						int new_i = i + di[dir];
						int new_j = j + dj[dir];
						if (in_bounds(new_i, new_j)) {
							coin[new_i][new_j] *= -1;
						}
					}
				}
			}
		}

		int cnt_free = 0;
		Color opponent_color = another_color(my_color);
		int cnt_my_color = 0;
		int cnt_opponent_color = 0;
		//
		int coins = 0;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (desk[i][j] == my_color) {
					++cnt_my_color;
					coins += coin[i][j];
				} else if (desk[i][j] == opponent_color) {
					++cnt_opponent_color;
					coins -= coin[i][j];
				} else {
					cnt_free++;
				}
			}
		}
		if (cnt_free == 0) {
			if (cnt_my_color > cnt_opponent_color) {
				return inf - 1;
			} else {
				return -inf + 1;
			}
		}
		//
		double first = 100.0 * (cnt_my_color - cnt_opponent_color) / (cnt_my_color + cnt_opponent_color);

		int corners = 0;
		for (int i = 0; i < 8; i += 7) {
			for (int j = 0; j < 8; j += 7) {
				if (desk[i][j] == my_color) {
					++corners;
				} else if (desk[i][j] == opponent_color) {
					--corners;
				}
			}
		}
		//
		corners *= 25;

		int near_corners = 0;
		for (int i = 0; i < 8; i += 7) {
			for (int j = 0; j < 8; j += 7) {
				if (desk[i][j] == FREE) {
					for (int dir = 0; dir < 8; ++dir) {
						int new_i = i + di[dir];
						int new_j = j + dj[dir];
						if (in_bounds(new_i, new_j)) {
							if (desk[new_i][new_j] == my_color) {
								++near_corners;
							} else if (desk[new_i][new_j] == opponent_color) {
								--near_corners;
							}
						}
					}
				}
			}
		}
		//
		near_corners *= -12.5;

		int cnt_me_can_move = 0;
		int cnt_he_can_move = 0;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (can_set_color(i, j, my_color)) {
					++cnt_me_can_move;
				} else if (can_set_color(i, j, opponent_color)) {
					++cnt_he_can_move;
				}
			}
		}

		if (cnt_he_can_move + cnt_me_can_move == 0) {
			if (cnt_my_color > cnt_opponent_color) {
				return inf;
			} else {
				return -inf;
			}
		}

		//
		double mobility = 100.0 * (cnt_me_can_move - cnt_he_can_move) / (cnt_me_can_move + cnt_he_can_move);

		bool my_stab[8][8] = {}, his_stab[8][8] = {};

		for (int i = 0; i < 8; i += 7) {
			for (int j = 0; j < 8; j += 7) {
				if (desk[i][j] == my_color) {
					my_stab[i][j] = true;
				} else if (desk[i][j] == opponent_color) {
					his_stab[i][j] = true;
				}
			}
		}

		// ->
		for (int i = 0; i < 8; i += 7) {
			for (int j = 1; j <= 6; j++) {
				if (my_stab[i][j - 1] && desk[i][j] == my_color) {
					my_stab[i][j] = true;
				}
				if (his_stab[i][j - 1] && desk[i][j] == opponent_color) {
					his_stab[i][j] = true;
				}
			}
		}
		// <-
		for (int i = 0; i < 8; i += 7) {
			for (int j = 6; j >= 1; j--) {
				if (my_stab[i][j + 1] && desk[i][j] == my_color) {
					my_stab[i][j] = true;
				}
				if (his_stab[i][j + 1] && desk[i][j] == opponent_color) {
					his_stab[i][j] = true;
				}
			}
		}
		// |
		// v
		for (int j = 0; j < 8; j += 7) {
			for (int i = 1; i < 7; ++i) {
				if (my_stab[i - 1][j] && desk[i][j] == my_color) {
					my_stab[i][j] = true;
				}
				if (his_stab[i - 1][j] && desk[i][j] == opponent_color) {
					his_stab[i][j] = true;
				}
			}
		}
		// ^
		for (int j = 0; j < 8; j += 7) {
			for (int i = 6; i >= 1; --i) {
				if (my_stab[i + 1][j] && desk[i][j] == my_color) {
					my_stab[i][j] = true;
				}
				if (his_stab[i + 1][j] && desk[i][j] == opponent_color) {
					his_stab[i][j] = true;
				}
			}
		}
		// down right
		for (int i = 1; i < 8; i++) {
			for (int j = 1; j < 7; j++) {
				if (desk[i][j] == my_color && my_stab[i][j - 1] && my_stab[i - 1][j - 1] && my_stab[i - 1][j] && my_stab[i - 1][j + 1]) {
					my_stab[i][j] = true;
				}
				if (desk[i][j] == opponent_color && his_stab[i][j - 1] && his_stab[i - 1][j - 1] && his_stab[i - 1][j] && his_stab[i - 1][j + 1]) {
					his_stab[i][j] = true;
				}
			}
		}
		// to down to left
		for (int i = 1; i < 8; i++) {
			for (int j = 6; j >= 0; j--) {
				if (desk[i][j] == my_color && my_stab[i][j + 1] && my_stab[i - 1][j - 1] && my_stab[i - 1][j] && my_stab[i - 1][j + 1]) {
					my_stab[i][j] = true;
				}
				if (desk[i][j] == opponent_color && his_stab[i][j + 1] && his_stab[i - 1][j - 1] && his_stab[i - 1][j] && his_stab[i - 1][j + 1]) {
					his_stab[i][j] = true;
				}
			}
		}
		// up to right
		for (int i = 6; i >= 0; i--) {
			for (int j = 1; j < 7; j++) {
				if (desk[i][j] == my_color && my_stab[i][j - 1] && my_stab[i + 1][j - 1] && my_stab[i + 1][j] && my_stab[i + 1][j + 1]) {
					my_stab[i][j] = true;
				}
				if (desk[i][j] == opponent_color && his_stab[i][j - 1] && his_stab[i + 1][j - 1] && his_stab[i + 1][j] && his_stab[i + 1][j + 1]) {
					his_stab[i][j] = true;
				}
			}
		}

		// up to left 
		for (int i = 6; i >= 0; i--) {
			for (int j = 6; j >= 0; j--) {
				if (desk[i][j] == my_color && my_stab[i][j + 1] && my_stab[i + 1][j - 1] && my_stab[i + 1][j] && my_stab[i + 1][j + 1]) {
					my_stab[i][j] = true;
				}
				if (desk[i][j] == opponent_color && his_stab[i][j + 1] && his_stab[i + 1][j - 1] && his_stab[i + 1][j] && his_stab[i + 1][j + 1]) {
					his_stab[i][j] = true;
				}
			}
		}
		int my_stability = 0, his_stability = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (my_stab[i][j]) {
					my_stability++;
				} else if (his_stab[i][j]) {
					his_stability++;
				}
			}
		}
		double stability = 0;
		if (his_stability + my_stability != 0) {
			stability = 100. * (my_stability - his_stability) / (my_stability + his_stability);
		}

		if (cnt_free > 32) {
			return (10.0 * first) + (10. * coins) + (80. * mobility) + (800. * corners) + (300. * near_corners) + (400. * stability);
		} else {
			return (10.0 * first) + (10. * coins) + (800. * mobility) + (800. * corners) + (300. * near_corners) + (200. * stability);

		// 1) было    --                    ---     --------------------------------------- 100                     nope
		}

		
		//return 10.0 * (first + coins) +

	}

	Desk(){}
	~Desk(){}

};

#define get_time(t) int(t * 1000.0 / CLOCKS_PER_SEC)

/*
inline int get_time(clock_t& t) {
	return int(t * 1000.0 / CLOCKS_PER_SEC);
}
*/
int max_depth = 4;

std::pair<int, int> answer_ij;

bool minimaxed;
clock_t t;

double minimax(Desk desk, int depth, bool to_max, double alpha, double beta, Color color) {
	if (depth == max_depth) {
		return desk.evristika();
	}
	Color opponent_color = another_color(color);
	if (to_max) {
		std::pair<int, int> best_ij;
		double best_score = -inf;
		int cnt_free = 0;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (desk.can_set_color(i, j, color)) {
					++cnt_free;
					Desk child = desk;
					child.set_color(i, j, color);
					double score = minimax(child, depth + 1, false, alpha, beta, opponent_color);
					if (score > best_score) {
						best_score = score;
						best_ij = {i, j};
					}
					alpha = max(alpha, best_score);
					if (beta <= alpha) {
						break;
					}
				}
			}
		}
		if (depth == 0) {
			//if (cnt_free > 0) answer_ij = best_ij;
			assert(cnt_free > 0);
			answer_ij = best_ij;
			return 0;
		} else {
			if (cnt_free) {
				return best_score;
			} else {
				return desk.evristika();
			}
		}
	} else {
		double best_score = inf;
		int cnt_free = 0;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (desk.can_set_color(i, j, color)) {
					++cnt_free;
					Desk child = desk;
					child.set_color(i, j, color);
					double score = minimax(child, depth + 1, true, alpha, beta, opponent_color);
					if (score < best_score) {
						best_score = score;
					}
					beta = min(beta, best_score);
					if (beta <= alpha) {
						break;
					}
				}
			}
		}
		if (cnt_free) {
			return best_score;
		} else {
			return desk.evristika();
		}
	}
}

double minimax_time(Desk desk, int depth, bool to_max, double alpha, double beta, Color color) {
	if (!minimaxed) {
		return -1;
	}
	clock_t new_t = clock() - t;
	int tm = get_time(new_t);
	if (tm >= 2500) {
		minimaxed = false;
		return -1;
	}
	if (depth == max_depth) {
		return desk.evristika();
	}
	Color opponent_color = another_color(color);
	if (to_max) {
		std::pair<int, int> best_ij;
		double best_score = -inf;
		int cnt_free = 0;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (desk.can_set_color(i, j, color)) {
					++cnt_free;
					Desk child = desk;
					child.set_color(i, j, color);
					double score = minimax_time(child, depth + 1, false, alpha, beta, opponent_color);
					if (!minimaxed) {
						return -1;
					}
					if (score > best_score) {
						best_score = score;
						best_ij = {i, j};
					}
					alpha = max(alpha, best_score);
					if (beta <= alpha) {
						break;
					}
				}
			}
		}
		if (depth == 0) {
			//if (cnt_free > 0) answer_ij = best_ij;
			assert(cnt_free > 0);
			answer_ij = best_ij;
			return 0;
		} else {
			if (cnt_free) {
				return best_score;
			} else {
				return desk.evristika();
			}
		}
	} else {
		double best_score = inf;
		int cnt_free = 0;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (desk.can_set_color(i, j, color)) {
					++cnt_free;
					Desk child = desk;
					child.set_color(i, j, color);
					double score = minimax_time(child, depth + 1, true, alpha, beta, opponent_color);
					if (!minimaxed) {
						return -1;
					}
					if (score < best_score) {
						best_score = score;
					}
					beta = min(beta, best_score);
					if (beta <= alpha) {
						break;
					}
				}
			}
		}
		if (cnt_free) {
			return best_score;
		} else {
			return desk.evristika();
		}
	}
}

void Minimax(Desk desk, int depth, bool to_max, double alpha, double beta, Color color) {
	t = clock();
	minimax(desk, 0, true, -inf, inf, color);
	clock_t new_t = clock() - t;
	//cout << get_time(new_t) << endl;
	int first_time = get_time(new_t);
	int delta = 0;
	int cnt_free = desk.cnt_free();
	while (true) {
		//std::pair<int,int> buffer = answer_ij;
		max_depth++;
		if (max_depth > cnt_free) {
			break;
		}
		delta++;
		minimaxed = true;
		minimax_time(desk, 0, true, -inf, inf, color);
		// new_t = clock() - t;
		//int last_time = get_time(new_t);
		//cout << last_time << endl;
		if (!minimaxed) {
			//answer_ij = buffer;
			break;
		} else {
			//cout << "norm\n";
		}
	}
	max_depth -= delta;

	new_t = clock() - t;
	int last_time = get_time(new_t);
	//cout << first_time << " " << last_time << " " << delta << endl;
}

void solve_black_DEBUG(Desk desk) {
	my_color = BLACK;

	std::ios_base::sync_with_stdio(0);

	while (true) {
		desk.print();

		string s;
		cin >> s;
		if (s == "lose" || s == "win" || s == "draw") {
			break;
		}
		assert(s == "turn" || s == "move");
		if (s == "turn") {
			answer_ij = {-1, -1};
			Minimax(desk, 0, true, -inf, inf, BLACK);
			assert(answer_ij.first != -1 && answer_ij.second != -1);
			cout << "move " << char(answer_ij.second + 'a') << " " << answer_ij.first + 1 << endl;
			cout.flush();
			//assert
			assert(desk.can_set_color(answer_ij.first, answer_ij.second, BLACK));
			desk.set_color(answer_ij.first, answer_ij.second, BLACK);

		} else {
			int i, j;
			char c;
			cin >> c;
			j = c - 'a';
			cin >> c;
			i = c - '1';

			// assert
			assert(desk.can_set_color(i, j, WHITE));
			desk.set_color(i, j, WHITE);
		}

	}

}

void solve_black() {
	my_color = BLACK;

	std::ios_base::sync_with_stdio(0);
	Desk desk;
	desk.init();

	while (true) {
		//desk.print();

		string s;
		cin >> s;
		if (s == "lose" || s == "win" || s == "draw") {
			break;
		}
		assert(s == "turn" || s == "move");
		if (s == "turn") {
			answer_ij = {-1, -1};
			Minimax(desk, 0, true, -inf, inf, BLACK);
			assert(answer_ij.first != -1 && answer_ij.second != -1);
			cout << "move " << char(answer_ij.second + 'a') << " " << answer_ij.first + 1 << endl;
			cout.flush();
			//assert
			assert(desk.can_set_color(answer_ij.first, answer_ij.second, BLACK));
			desk.set_color(answer_ij.first, answer_ij.second, BLACK);

		} else {
			int i, j;
			char c;
			cin >> c;
			j = c - 'a';
			cin >> c;
			i = c - '1';

			// assert
			assert(desk.can_set_color(i, j, WHITE));
			desk.set_color(i, j, WHITE);
		}

	}

}

void solve_white() {
	my_color = WHITE;

	std::ios_base::sync_with_stdio(0);
	Desk desk;
	desk.init();

	while (true) {
		//desk.print();

		string s;
		cin >> s;
		if (s == "lose" || s == "win" || s == "draw") {
			break;
		}
		assert(s == "turn" || s == "move");
		if (s == "turn") {
			answer_ij = {-1, -1};
			Minimax(desk, 0, true, -inf, inf, my_color);
			assert(answer_ij.first != -1 && answer_ij.second != -1);
			cout << "move " << char(answer_ij.second + 'a') << " " << answer_ij.first + 1 << endl;
			cout.flush();
			//assert
			assert(desk.can_set_color(answer_ij.first, answer_ij.second, my_color));
			desk.set_color(answer_ij.first, answer_ij.second, my_color);

		} else {
			int i, j;
			char c;
			cin >> c;
			j = c - 'a';
			cin >> c;
			i = c - '1';

			// assert
			assert(desk.can_set_color(i, j, another_color(my_color)));
			desk.set_color(i, j, another_color(my_color));
		}

	}
}

void solve() {
	std::ios_base::sync_with_stdio(0);
	string s;
	cin >> s;
	assert(s == "init");
	cin >> s;
	assert(s == "black" || s == "white");
	if (s == "black") {
		solve_black();
	} else {
		//assert(false);
		solve_white();
	}
}

int main() {
	/*
	clock_t t;
	t = clock();
	cout << frequency_of_primes(2200400) << endl;
	t = clock() - t;
	cout << get_time(t);
	Color color = another_color(WHITE);
	*/

	std::ios_base::sync_with_stdio(0);

	//Desk desk;
	//desk.init();
	//desk.print();
	/*
	Color color = BLACK;
	while (true) {
	int i, j;
	char c;
	cin >> c;
	if (c == '-') {
	break;
	}
	j = c - 'a';
	cin >> c;
	i = c - '1';
	if (!desk.can_set_color(i, j, color)) {
	cout << "opa\n";
	}
	desk.set_color(i, j, color);
	desk.print();
	color = another_color(color);
	}
	*/
	solve();
	//Desk desk;
	//desk.init();
	//desk.read();
	//desk.print();
	//solve_black_DEBUG(desk);
}
