#include <climits>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

constexpr int SIZE = 3;

constexpr int COMP_WIN = 1;
constexpr int COMP_LOSE = -1;
constexpr int DRAW = 0;

constexpr int PLAYER_COMP = 1;
constexpr int PLAYER_HUMAN = -1;

constexpr int MAX_DEPTH = 7;

int board[SIZE][SIZE] = {0};

char to_x_o(int player) {
    if (player == PLAYER_COMP) {
        return 'O';
    } else if (player == PLAYER_HUMAN) {
        return 'X';
    }
    return ' ';
}

void print_board() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cout << "|"<< to_x_o(board[i][j]);
        }
        cout << "|\n";
    }
}

int check_board() {
    int diag_sum = 0;
    int anti_diag_sum = 0;
    for (int i = 0; i < SIZE; i++) {
        int row_sum = 0;
        int col_sum = 0;
        for (int j = 0; j < SIZE; j++) {
            row_sum += board[i][j];
            col_sum += board[j][i];
            if (i + j == SIZE - 1) {
                anti_diag_sum += board[i][j];
            }
        }
        if (row_sum == -SIZE) {
            return COMP_LOSE;
        } else if (row_sum == SIZE) {
            return COMP_WIN;
        }
        if (col_sum == -SIZE) {
            return COMP_LOSE;
        } else if (col_sum == SIZE) {
            return COMP_WIN;
        }
        diag_sum += board[i][i];
    }
    if (diag_sum == -SIZE) {
        return COMP_LOSE;
    }
    if (diag_sum == SIZE) {
        return COMP_WIN;
    }
    if (anti_diag_sum == -SIZE) {
        return COMP_LOSE;
    }
    if (anti_diag_sum == SIZE) {
        return COMP_WIN;
    }
    return DRAW;
}

bool is_full() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

int minimax(int depth, int player) {
    if (depth > MAX_DEPTH) {
        return 0;
    }
    int result = check_board();
    if (result != 0) {
        return result;
    }
    if (is_full()) {
        return DRAW;
    }
    int best_score = player == PLAYER_COMP ? INT_MIN : INT_MAX;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = player;
                int score = minimax(depth + 1, -player);
                board[i][j] = 0;
                if (player == PLAYER_COMP) {
                    best_score = max(score, best_score);
                } else {
                    best_score = min(score, best_score);
                }
            }
        }
    }
    return best_score;
}

int minimax_abPruning(int depth, int player, int alpha, int beta) {
    if (depth > MAX_DEPTH) {
        return 0;
    }
    int result = check_board();
    if (result != 0) {
        return result;
    }
    if (is_full()) {
        return DRAW;
    }
    int best_score = player == PLAYER_COMP ? INT_MIN : INT_MAX;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = player;
                int score = minimax_abPruning(depth + 1, -player, alpha, beta);
                board[i][j] = 0;
                if (player == PLAYER_COMP) {
                    best_score = max(score, best_score);
                    alpha = max( alpha, best_score);
                } else {
                    best_score = min(score, best_score);
                    beta = min( beta, best_score);
                }
                if (beta <= alpha) {
                    break;
                }
            }
        }
    }
    return best_score;
}

struct Move {
    int row;
    int col;
};

void move(Move &m, int player) {
    board[m.row][m.col] = player;
}

void do_best_move(mt19937 &rng) {
    int best_score = INT_MIN;
    vector<Move> best_moves;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = PLAYER_COMP;
                int score = minimax_abPruning(0, PLAYER_HUMAN, INT_MIN, INT_MAX);
                board[i][j] = 0;
                if (score > best_score) {
                    best_score = score;
                    best_moves.clear();
                    best_moves.push_back({i, j});
                }
                else if (score == best_score) {
                    best_moves.push_back({i, j});
                }
            }
        }
    }

    uniform_int_distribution<int> dist(0, best_moves.size() - 1);
    Move best_move = best_moves[dist(rng)];
    move(best_move, PLAYER_COMP);
}

void init_board() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = 0;
        }
    }
}

int main() {
    random_device rd;
    mt19937 rng(rd());
    init_board();

    while (!is_full()) {
        print_board();
        int row, col;
        cout << "Your turn (row col): ";
        cin >> row >> col;
        row--;
        col--;
        if (board[row][col] != 0) {
            cout << "Invalid move\n";
            continue;
        }
        board[row][col] = PLAYER_HUMAN;
        if (check_board() < 0) {
            print_board();
            cout << "You win\n";
            break;
        }
        if (is_full()) {
            print_board();
            cout << "Draw\n";
            break;
        }
        cout << "Computer's turn:\n";
        do_best_move(rng);
        if (check_board() > 0) {
            print_board();
            cout << "You lose\n";
            break;
        }
        else if (is_full()) {
            print_board();
            cout << "Draw\n";
            break;
        }
    }
}