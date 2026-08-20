#include <bits/stdc++.h>
using namespace std;






// ==============================
// because requiremeent says winning rules should be scalable
class Winning_Validator {
public:
    virtual int check(char ch, vector<vector<char>>& board) = 0;
    ~Winning_Validator() = default;
};

class Standard_Rules : public Winning_Validator {
public:
    int check(char ch, vector<vector<char>>& board) {
        // this is set of standard rules of tic tac toe

        // it check diagonals, it checks vertical and horizontals and returns, for demo purposes, we are returning true
        return true;
    }
};


class Only_Straight_Winners : public Winning_Validator {
public:
    int check(char ch, vector<vector<char>>& board) {
        //  only straight chains winners are allowed
        return true;
    }
};


class Only_Diagonal_Winners : public Winning_Validator {
public:
    int check(char ch, vector<vector<char>>& board) {
        //  only diagonal chains winners are allowed
        return true;
    } 
};

// =============================













// ============================


class Board_Handler {
private:
    char allowed_moves[2];public:
    Board_Handler(char first, char second) {
        this->allowed_moves[0] = first;
        this->allowed_moves[1] = second;
        cout << "allowed_moves are : player 1st(" << first << ") , player 2nd(" << second <<") ..." << endl; 
    };

    ~Board_Handler() {};

    int get_parity(char ch) {
        if (this->allowed_moves[0] == ch) {
            return 0;
        }
        return 1;
    }

    void set_place(int i, int j, char ch, int turn_parity, vector<vector<char>>& board) {

        // VALIDATIONS
        if (board[i][j] != ' ') {
            cout << "Invalid move, please choose an empty cell..." << endl;
            return;
        }

        if (ch != this->allowed_moves[0] && ch != this->allowed_moves[1]) {
            cout << "Invalid move, please enter a valid character : " << this->allowed_moves[0] << " or " << this->allowed_moves[1] << " ..." << endl;
            return;
        }

        if (turn_parity != get_parity(ch)) {
            cout << "Invalid move, you are not playing according to your chance character, you to use : "<< this->allowed_moves[this->get_parity(ch)] <<" ..." << endl;
            return;
        }

        board[i][j] = ch;
        cout << "Turn executed..." << endl;
    }
};

class Board {
private:
    vector<vector<char>> board;
    int n, m;
public:
    Board(int n, int m) {
        this->n = n;
        this->m = m;
        this->board.assign(n, vector<char>(m, ' '));
    }

    ~Board (){
    }

    void render_board() {
        for (int i = 0; i<this->n; i++) {
            cout << "| ";
            for (int j = 0; j<this->m; j++) {
                cout << this->board[i][j] << " | ";
            }
            cout << endl;
        }
    }
    // getter
    vector<vector<char>>& get_board() {
        return this->board;
    }
};
// ===============================












// ===============================

// maps player parity to socket id on which they are connected
class Socket_Map {
private:
    string arr[2];
public:
    Socket_Map() {
        this->arr[0] = "";
        this->arr[1] = "";
    }

    void join(int parity, string socket_id) {
        this->arr[parity] = socket_id;
    }

    // setters
    string get_socket_id(int player) {
        return this->arr[player - 1];
    }
};

// ===============================












// ======================
// IN GAME POP NOTIFICATION DURING MATCH
class In_Game_Notifier {
private:
    
public:
    In_Game_Notifier() {};
    ~In_Game_Notifier() {};

    void notify(string socket_id, string message) {
        // it emits the message to given socket id
        cout << "Message sent..." << endl;
    }
};
// =====================













// =================================
class Game {
private:
    int current_turn;
    Board_Handler* bh;
    Board* board;
    Winning_Validator* wv;
    bool game_over;
    In_Game_Notifier* ig;
    Socket_Map* sm;
    Game () {
        this->current_turn = 0;
        this->game_over = false;
        this->sm = new Socket_Map();
        this->ig = new In_Game_Notifier();
    }
public:
    


    friend class Game_Builder;
    // getters
    vector<vector<char>>& get_board(){
        return this->board->get_board();
    }

    void render_board() {
        this->board->render_board();
    }

    void join_game(int player_number, string socket_id) {
        this->sm->join(player_number - 1, socket_id);
    }

    void make_move(int i, int j, char ch) {
        if (this->game_over) {
            cout << "Game is over now, you can not make a turn..." <<endl;
            return;
        }
        // this will be done in three steps

        // make move
        this->bh->set_place(i, j, ch, this->current_turn, this->board->get_board());

        // winning validator : check if this player is winning
        int response = this->wv->check(ch, this->board->get_board());

        // change turn

        if (response == 1) {
            // notify both players of current players victory
            string message = "Player " + to_string(this->current_turn + 1) + " won!! ";
            if (this->current_turn + 1 == 2) {
                // player 2 won
                this->ig->notify(this->sm->get_socket_id(0), message);
                this->ig->notify(this->sm->get_socket_id(1), "You won...");
            
            } else {
                this->ig->notify(this->sm->get_socket_id(0), "You won...");
                this->ig->notify(this->sm->get_socket_id(1), message);
               
            }
            this->game_over = true;
            return;
        }

        else if (response == 2) {
            this->current_turn = 1 - this->current_turn;
            string message = "Player " + to_string(this->current_turn + 1) + " turn now...";
            this->ig->notify(this->sm->get_socket_id(0), message);
            this->ig->notify(this->sm->get_socket_id(1), message);
        } else if (response == 0) {
            // draw
            this->ig->notify(this->sm->get_socket_id(0), "Draw");
            this->ig->notify(this->sm->get_socket_id(1), "Draw");
        }

        
    }

    ~Game () {
        delete this->bh;
        delete this->wv;
        delete this->board;
        delete this->ig;
        delete this->sm;
    }
};

// ======================================












// =====================================
// Simple builder design pattern to make sure that game is created completely without missing anything...
class Game_Builder {
private:
    Game* g;
public:
    Game_Builder() {
        this->g = new Game();
    };

    Game_Builder& set_board(Board* board) {
        g->board = board;
        return *this;
    }

    Game_Builder& set_board_handler(Board_Handler* bh) {
        g->bh = bh;
        return *this;
    }

    Game_Builder& set_winning_valdiator(Winning_Validator* wv) {
        g->wv = wv;
        return *this;
    }

    Game* build() {
        // validations
        if (g->board == nullptr || g->bh == nullptr || g->wv == nullptr) {
            cout << "Object incomplete..." << endl;
            return nullptr;
        }

        // validated
        cout << "Game successfully created..." << endl;
        return g;
    }
};
// ?====================================











int main() {

    


    return 0;
}