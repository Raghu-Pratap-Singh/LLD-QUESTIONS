#include <bits/stdc++.h>
using namespace std;


















// =================
class User {
private:
    string name;
    string user_id;
    int elo;
public:
    User(string name, int elo, string user_id) {
        this->name = name;
        this->elo = elo;
        this->user_id = user_id;
    }
    ~User() {};
    // setters and getters
    void set_elo (int elo) {
        this->elo = elo;
    }

    int get_elo () {
        return this->elo;
    }

    string get_name() {
        return this->name;
    }

    string get_user_id() {
        return this->user_id;
    }
    void receive(string from, string message) {
        // this is called by chat mediator for making this user receive message
        cout << "Message received by " << this->name << " ..." << endl;
    }
};

enum class Piece {
    King,
    Knight,
    Queen,
    Bishop,
    Pawn,
    Rook,
    Empty
};

enum class Color {
    White,
    Black,
    None
};

class Element {
private:
    Piece p;
    Color c;
public:
    Element (Piece p, Color c) {
        this->p = p;
        this->c = c;
    }
    // getterrs
    Piece get_piece_name() {
        return this->p;
    }
    Color get_piece_color () {
        return this->c;
    }
};

class Move {
private:
    int prev_x, prev_y;
    int next_x, next_y;
    Element* e;
public:
    Move(int prev_x, int prev_y, int next_x, int next_y, Element* e) {
        this->e = e;
        this->prev_x = prev_x;
        this->prev_y = prev_y;
        this->next_x = next_x;
        this->next_y = next_y;
    }

    pair<int, int> get_destination_coordinates() {
        return {this->next_x, this->next_y};
    }
    pair<int, int> get_source_coordinates() {
        return {this->prev_x, this->prev_y};
    }
    Element* get_element() {
        return this->e;
    }
    ~Move() {
        delete this->e;
    };
};

class Board {
private:
    vector<vector<Element*>> board;
public:
    Board () {
        this->board.assign(8, vector<Element*>(8,nullptr));
        // upper side black
        this->board[0][0] = new Element(Piece::Rook, Color::Black);
        this->board[0][1] = new Element(Piece::Knight, Color::Black);
        this->board[0][2] = new Element(Piece::Bishop, Color::Black);
        this->board[0][3] = new Element(Piece::Queen, Color::Black);
        this->board[0][4] = new Element(Piece::King, Color::Black);
        this->board[0][5] = new Element(Piece::Bishop, Color::Black);
        this->board[0][6] = new Element(Piece::Knight, Color::Black);
        this->board[0][7] = new Element(Piece::Rook, Color::Black);

        for (int i = 0; i<=7; i++) {
            this->board[0][i] = new Element(Piece::Pawn, Color::Black);
        }

        for (int row = 2; row<=5; row++) {
            for (int col = 0; col<=7; col++) {
                this->board[row][col] = new Element(Piece::Empty, Color::None);
            }
        }

        // lower side white
        for (int i = 0; i<=7; i++) {
            this->board[6][i] = new Element(Piece::Pawn, Color::Black);
        }

        this->board.assign(8, vector<Element*>(8,nullptr));
        // upper side black
        this->board[7][0] = new Element(Piece::Rook, Color::White);
        this->board[7][1] = new Element(Piece::Knight, Color::White);
        this->board[7][2] = new Element(Piece::Bishop, Color::White);
        this->board[7][3] = new Element(Piece::Queen, Color::White);
        this->board[7][4] = new Element(Piece::King, Color::White);
        this->board[7][5] = new Element(Piece::Bishop, Color::White);
        this->board[7][6] = new Element(Piece::Knight, Color::White);
        this->board[7][7] = new Element(Piece::Rook, Color::White);

    }
    ~Board() {}
    // setters and getters
    vector<vector<Element*>>& get_board () {
        return this->board;
    }

    Element* get_element(int i, int j) {
        return this->board[i][j];
        return this->board[i][j];
    }
};
// scalable moving rules
class Move_Validator {  
public:
    virtual ~Move_Validator() {};
    virtual bool validate_move(Move* move) = 0;
};

// for now i have only integrated movement validator, not obstacle validator, i am assuming that player knows this much
class Standard_Rules : public Move_Validator {
public:
    bool validate_move(Move* move) {
        // for demo purposes, returning true
        pair<int, int> d = move->get_destination_coordinates();
        pair<int, int> s = move->get_source_coordinates();
        Element* p = move->get_element();
        int col_gap = abs(d.second - s.second);
        int row_gap = abs(s.first - s.first);
        if (p->get_piece_name() == Piece::Rook) {
            if (col_gap == 0 && row_gap>0) {
                return true;
            }
        } else if (p->get_piece_name() == Piece::Knight) {
            if (col_gap == 2 && row_gap == 1) {
                return true;
            }
        } else if (p->get_piece_name() == Piece::Bishop) {
            if (col_gap == row_gap) {
                return true;
            }
        } else if (p->get_piece_name() == Piece::Queen) {
            if ((col_gap == row_gap) || (row_gap == 0) || (col_gap == 0)) {
                return true;
            } 
        } else if (p->get_piece_name() == Piece::King) {
            if ((col_gap == 0 && row_gap == 1) || (col_gap == 1 && row_gap == 0) || (col_gap == 1 && row_gap == 1)) {
                return true;
            }
        } else if (p->get_piece_name() == Piece::Pawn) {
            if (p->get_piece_color() == Color::White && s.first==6) {
                if ((row_gap>0 && row_gap<=2 && col_gap == 0) || (row_gap == 1 && col_gap == 1)) {
                    return true;
                } 
            } else if (p->get_piece_color() == Color::White && s.first!=6) {
                // the player can also cut fiagonally, but we are just checking that pawn canmove diagonally and one step ahead, not cutting here for simplification
                if ((row_gap>0 && row_gap<=1 && col_gap == 0) || (row_gap == 1 && col_gap == 1)) {
                    return true;
                } 
            }
        }
        return false;
    }
};


class Board_Move_Maker {
private:
    Move_Validator* mv;  
public:
    Board_Move_Maker() {
        this->mv = nullptr;
    };

    ~Board_Move_Maker() {
        if (this->mv!=nullptr) {
            delete this->mv;
        }
        else {
            this->mv = nullptr;
        }
    }

    void set_moving_rules (Move_Validator* mv) {
        this->mv = mv;
    }    

    void make_move(Board* b, Move* move) {
        bool is_valid = this->mv->validate_move(move);
        if (!is_valid) {
            return;
        }
        // valid move, modify board, but first check if the destination is empty or their is an oponent piece there, if yes, then remove it
        pair<int, int> d = move->get_destination_coordinates();
        pair<int, int> s = move->get_source_coordinates();
        Element* destination_piece = b->get_element(d.first, d.second);
        if (destination_piece->get_piece_name()!=Piece::Empty && destination_piece->get_piece_color() == move->get_element()->get_piece_color()) {
            return;
        }
        b->get_board()[s.first][s.second] = new Element(Piece::Empty, Color::None);
        b->get_board()[d.first][d.second] = move->get_element();
    }
};

// =================
















// =================
class Check_Validator {
public:
    virtual int check(Board* b) = 0;
    virtual ~Check_Validator() {};
};

// this is also scalable if in future, new modes of chase came
class Standard_Check_Validator : public Check_Validator {
public:
    // 0->not check
    // 1->check
    // 2->check mate
    int check(Board* b) {
        // this checks and returns the check condition accordingly, for now we are just returning 0, for demo purposes
        return 0;
    }
};
// =================























// a chat room for messaging facility in chess game as it is a requirement of question
// ====================================

class IMediator {
public:
    virtual void send_message(string from, string to, string message) = 0;
    virtual void register_user(User* u) = 0;
    virtual ~IMediator() = 0;
};

class Chat_Mediator : public IMediator{
private:
    vector<User*> users;
public:

    Chat_Mediator() {
    }
    ~Chat_Mediator () override {
        for (int i = 0; i<this->users.size(); i++) {
            if (this->users[i]!=nullptr) {
                delete this->users[i];
            }
        }
    }
    // setters and getters;
    void register_user(User* u) override {
        this->users.push_back(u);
        // this performs other socket mapping also, but that is not important here, important is that we are using mediator pattern for chat room
    }
    
    void send_message(string from, string to, string message) override {
        // this uses socket mechanism but we are not implementing that here as it is not necesary for this question
        for (User* u : this->users) {
            if (u->get_user_id() == to) {
                u->receive(from, message);
                break;
            }
        }
        cout << "Message sent..." << endl;
    }
};

// ====================================


















// ====================

class Game {
private:
    int current_turn_parity; // 1(white player) or 2(black player)
    User* arr[2];
    Board_Move_Maker* mover; //validator included
    Check_Validator* cv;
    int game_id;
    Chat_Mediator* cm;
    Board* b;
    bool game_over;
public:
    Game () {
        this->current_turn_parity = 1; // means 1st player
        this->arr[0] = nullptr;
        this->arr[1] = nullptr;
        this->cv = nullptr;
        this->mover = nullptr;
        this->cm = nullptr;
        this->game_id = -1;
        this->game_over = false;
        this->b = nullptr;
    }

    ~Game() {
        if (this->arr[0]!=nullptr) {
            delete this->arr[0];
        }
        if (this->arr[1]!=nullptr) {
            delete this->arr[1];
        }
        if (this->cv!=nullptr) {
            delete this->cv;
        } 
        if (this->mover!=nullptr){
            delete this->mover;
        }

        if (this->cm!=nullptr) {
            delete this->cm;
        }
        if (this->b!=nullptr) {
            delete this->b;
        }
    }
    friend class Game_Builder;
    int get_game_id() {
        return this->game_id;
    }

    // (1) make move
    void make_move(string user_id, Move* move) {
        int player_number;
        if (this->arr[0]->get_user_id() == user_id) {
            player_number = 1;
        } else if (this->arr[1]->get_user_id() == user_id) {
            player_number = 2;
        } else {
            cout << "Invalid player.." << endl;
            return;
        }
        if (player_number!=this->current_turn_parity) {
            cout << "It is not your turn..." << endl;
            return;
        }
        this->mover->make_move(this->b, move);
    }

    // (2) end game
    void end_game() {
        this->game_over = true;
        cout << "game " << this->game_id << " ended.." << endl;
    }

    // (3) send text message while in match to opponent
    void send_message(string from, string to, string message) {
        this->cm->send_message(from, to, message);
    }

    // (4) quit game
    void quit(string user_id) {
        int player_number;
        if (this->arr[0]->get_user_id() == user_id) {
            player_number = 1;
        } else if (this->arr[1]->get_user_id() == user_id) {
            player_number = 2;
        } else {
            cout << "Invalid player.." << endl;
            return;
        }
        cout << "Player " << this->arr[player_number - 1]->get_name() << " quitted.." << endl;
        this->end_game();
    }

    bool is_completed() {
        return this->game_over;
    }

};

class Game_Builder {
private:
    Game* g;
public:
    Game_Builder() {
        this->g = new Game();
    };
    ~Game_Builder() {
    };

    Game_Builder& set_check_validator(Check_Validator* cv){
        this->g->cv = cv;
        return *this;
    }

    Game_Builder& set_board_move_maker(Board_Move_Maker* bmm) {
        this->g->mover = bmm;
        return *this;
    }

    Game_Builder& set_white_player(User* a) {
        this->g->arr[0] = a;
        return *this;
    }

    Game_Builder& set_black_player(User* b) {
        this->g->arr[1] = b;
        return *this;
    }

    Game_Builder& set_game_id(int id) {
        this->g->game_id = id;
        return *this;
    }

    Game_Builder& set_board(Board* b) {
        this->g->b = b;
        return *this;
    }

    Game_Builder& set_chat_mediator(Chat_Mediator* cm) {
        this->g->cm = cm;
        return *this;
    }

    Game* build() {
        if (this->g->cv==nullptr) {
            cout << "Please add a check validator strategy..." << endl;
            return nullptr;
        } else if(this->g->mover == nullptr) {
            cout << "Please add a move checker strategy..." << endl;
            return nullptr;
        } else if (this->g->arr[0] == nullptr) {
            cout << "Please assign white player..." << endl;
            return nullptr;
        } else if (this->g->arr[1] == nullptr) {
            cout << "Please assign black player..." << endl;
            return nullptr;
        } else if (this->g->game_id == -1) {
            cout << "Please assign a game_id..." << endl;
            return nullptr;
        } else if (this->g->cm == nullptr) {
            cout << "Please provide a chat room..." << endl;
            return nullptr;
        } else if (this->g->b == nullptr) {
            cout << "Please allocate a board..." << endl;
            return nullptr;
        }
        cout << "Game created..." << endl;
        return this->g;
    }
};

// ==================















// =========================

// we can also add searcing based on segment trees to optimize search time instead of linear search
// like leaf nodes will be ratings and 

class Match_Making_Strategy {
public:
    virtual Game* find_match(User* a, vector<User*> online_users) = 0;
};

class Standard : public Match_Making_Strategy {
public:
    Game* find_match(User* a, vector<User*> online_users) {
        return nullptr;
    }
};


class Greater_Than_K_Elo : public Match_Making_Strategy {
private:
    int delta;
public:
    Greater_Than_K_Elo(int k) {
        this->delta = k;
    }
    Game* find_match(User* a, vector<User*> online_users) {
        return nullptr;
    }
};

class Smaller_Than_K_Elo : public Match_Making_Strategy {
private:
    int delta;
public:
    Smaller_Than_K_Elo(int k) {
        this->delta = k;
    }
    Game* find_match(User* a, vector<User*> online_users) {
        return nullptr;
    }
};

class Within_A_Range_L_To_R : public Match_Making_Strategy {
private:
    int l, r;
public:
    Within_A_Range_L_To_R(int l, int r) {
        this->l = l;
        this->r = r;
    }
    Game* find_match(User* a, vector<User*> online_users) {
        return nullptr;
    }

};

// =========================













class Match_Making_Request {
private:
    User* a;
    Match_Making_Strategy* ms;
public:
    Match_Making_Request(User* a, Match_Making_Strategy* ms) {
        this->a = a;
        this->ms = ms;
    }
    // getters
    User* get_user() {
        return this->a;
    }
    Match_Making_Strategy* get_strategy() {
        return this->ms;
    }
};











// ==========================

// manages all currently games, users online list, and handles match making, kinda of a Root handler engine
class Game_Handler {
private:
    map<int, Game*> game_id_to_object;
    vector<User*> online_users;
    static Game_Handler* instance;
    Game_Handler() {};
    ~Game_Handler() {}
public:

    static Game_Handler* get_instance() {
        return Game_Handler::instance;
    }

    void add_user(User* u) {
        this->online_users.push_back(u);
    }

    // find match
    void find_match(Match_Making_Request* request) {
        Game* g= request->get_strategy()->find_match(request->get_user(), this->online_users);
        // now a game will be created and inserted to map
        this->game_id_to_object[g->get_game_id()] = g;
    }

    // make move
    void make_move(int game_id, string player_id, Move* move) {
        if (this->game_id_to_object.find(game_id) == this->game_id_to_object.end()) {
            cout << "Invalid game id.." << endl;
            return;
        }
        // valid game
        this->game_id_to_object[game_id]->make_move(player_id, move);

        // check if this game is complete or not
        if (this->game_id_to_object[game_id]->is_completed()) {
            // delete this game
            delete this->game_id_to_object[game_id];
            this->game_id_to_object.erase(game_id);
        }
    }

    // send message
    void send_message(string from, string to, int game_id, string message) {
        if (this->game_id_to_object.find(game_id) == this->game_id_to_object.end()) {
            cout << "Invalid game id.." << endl;
            return;
        }
        // valid game id

        this->game_id_to_object[game_id]->send_message(from, to, message);
    }

    // quit
    void quit(string user_id, int game_id) {
        if (this->game_id_to_object.find(game_id) == this->game_id_to_object.end()) {
            cout << "Invalid game id.." << endl;
            return;
        }
        // valid game_id
        this->game_id_to_object[game_id]->quit(user_id);
        // delete this game
        delete this->game_id_to_object[game_id];
        this->game_id_to_object.erase(game_id);

    }
};

Game_Handler* Game_Handler::instance = new Game_Handler();
// =========================











int main() {

    return 0;
}



