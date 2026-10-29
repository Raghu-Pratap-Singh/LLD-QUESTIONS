#include <bits/stdc++.h>
using namespace std;

// user details will be user for booking process, it can also have other details like acc no etc.
class User_Details {
private:
    string mobile;
    string username;
public:
    User_Details(string mobile, string username) {
        this->mobile = mobile;
        this->username = username;
    }
    ~User_Details() {};
    // getters
    string get_mobile() {
        return this->mobile;
    }

    string get_username() {
        return this->username;
    }
    
};

class Logger {
private:
    int line_number = 1;
    static Logger* instance;
    Logger() {};
public:
    static Logger* get_instance() {
        return Logger::instance;
    }
    ~Logger() {};
    void log_message_in_file(string file_path, string message) {

    }

    void log_message_on_terminal(string message) {
        cout << "Line number : " << this->line_number << endl;
        cout << message << endl;
    }
};

Logger* Logger::instance = new Logger();

class Timestamp {
private:
    int hour;
    int minutes;
public:
    Timestamp(int hr, int mn) {
        this->hour = hr;
        this->minutes = mn;
    }

    ~Timestamp() {};

    // getters
    int get_hour() {
        return this->hour;
    }

    int get_minutes() {
        return this->minutes;
    }
};

// ======== CINEMA MODEL==========
enum class Tier {
    SILVER,
    GOLD,
    PLATINUM,
    RECLINER
};

class Seat {
private:
    string seat_id;
    Tier type_of_seat;
    int row_number;
    int column_number;
public:
    Seat(string id, Tier type_of_seat, int row_number, int col_number) {
        this->seat_id = id;
        this->type_of_seat = type_of_seat;
        this->row_number = row_number;
        this->column_number = col_number;
    }

    ~Seat() {}

    // getters
    string get_id() {
        return this->seat_id;
    }

    Tier get_type_of_seat() {
        return this->type_of_seat;
    }

    int get_row_number() {
        return this->row_number;
    }

    int get_column_number() {
        return this->column_number;
    }
};

// this is audi/screen rrom
class Auditorium {
private:
    vector<vector<Seat*>> seat_matrix;
    string auditorium_id;
public:
    Auditorium (string id, vector<vector<Seat*>> matrix) {
        this->seat_matrix = matrix;
        this->auditorium_id = id;
    };
    ~Auditorium() {
        for (int i = 0; i<this->seat_matrix.size(); i++) {
            for (int j = 0; j<this->seat_matrix[0].size(); j++) {
                delete this->seat_matrix[i][j];
            }
        }
    }

    // getters
    vector<vector<Seat*>>& get_seat_matrix() {
        return this->seat_matrix;
    }

    string get_auditorium_id() {
        return this->auditorium_id;
    }
};

// this is the single point of access of auditoriums to prevent dangling pointers
class Auditorium_Distributor {
private:
    map<string, Auditorium*> mp;
    static Auditorium_Distributor* instance;
    Auditorium_Distributor() {};
public:
    ~Auditorium_Distributor() {
        for (auto&[k,v] : this->mp) {
            delete v;
        }
    };
    static Auditorium_Distributor* get_instance() {
        return Auditorium_Distributor::instance;
    }

    // setter
    void add_auditorium(Auditorium* auditorium) {
        this->mp[auditorium->get_auditorium_id()] = auditorium;
    }

    Auditorium* get_auditorium(string auditorium_id) {
        if (this->mp.find(auditorium_id) == this->mp.end()) {
            Logger::get_instance()->log_message_on_terminal("No such auditorium is registered...");
            return nullptr;
        }
        return this->mp[auditorium_id];
    }
};

Auditorium_Distributor* Auditorium_Distributor::instance = new Auditorium_Distributor();

// this will tell the timing of a movie in a cinema and which auditoriums have that movie assigned 
class Timing_Map {
private:
    map<string, string> mp;// movie_id : audi_id
    map<string, Timestamp*> start_time; //movie_id : start_time
public:
    Timing_Map() {};
    ~Timing_Map() {
        for (auto&[k,v] : this->start_time) {
            delete v;
        }
        
    }

    // set_movie time
    void add_movie(string movie_id, string audi_id, Timestamp* time) {
        this->start_time[movie_id] = time;
        this->mp[movie_id] = audi_id;
    }

    void remove_movie(string movie_id) {
        this->start_time.erase(movie_id);
        this->mp.erase(movie_id);
    }

    Timestamp* get_start_timing(string movie_id) {
        if (this->start_time.find(movie_id)==this->start_time.end()) {
            Logger::get_instance()->log_message_on_terminal("No such movie is here...");
            return nullptr;
        }
        return this->start_time[movie_id];

    }

};

class Cinema {
private:
    set<string> auditoriums;
    string cinema_id;
    string cinema_name;
public:
    Cinema(string cinema_id, string cinema_name) {
        this->cinema_id = cinema_id;
        this->cinema_name = cinema_name;
    };
    ~Cinema() {
    };
    // setter
    void add_auditorium(Auditorium* audi) {
        Auditorium_Distributor::get_instance()->add_auditorium(audi);
        this->auditoriums.insert(audi->get_auditorium_id());
    }
    bool has_auditorium(string auditorium_id) {
        return this->auditoriums.find(auditorium_id)!=this->auditoriums.end();
    }
    Auditorium* get_auditorium(string auditorium_id) {
        return Auditorium_Distributor::get_instance()->get_auditorium(auditorium_id);
    }
    // getter
    string get_cinema_id() {
        return this->cinema_id;
    }
    string get_cinema_name() {
        return this->cinema_name;
    }
};  

// ======== CINEMA MODEL==========

class Movie {
private:
    string movie_id;
    string movie_name;
    Timestamp* duration;
    double base_price;
public:
    Movie(string movie_id, string movie_name, double base_price) {
        this->movie_id = movie_id;
        this->movie_name = movie_name;
        this->base_price = base_price;
    }

    // setters

    void set_duration(Timestamp* t) {
        this->duration = t;
    }

    // getters
    string get_movie_id() {
        return this->movie_id;
    }
    string get_movie_name() {
        return this->movie_name;
    }

    Timestamp* get_duration() {
        return this->duration;
    }

    double get_base_price() {
        return this->base_price;
    }

    // destuctor
    ~Movie() {
        delete this->duration;
    }
};

// single point of cinema getting and setting
class Cinema_Distributor {
private:
    static Cinema_Distributor* instance;
    Cinema_Distributor() {};
    map<string, Cinema*> mp;
public:
    static Cinema_Distributor* get_instance() {
        return Cinema_Distributor::instance;
    }

    void add_cinema(Cinema* cinema) {
        this->mp[cinema->get_cinema_id()] = cinema;
    }

    Cinema* get_cinema(string cinema_id) {
        if (this->mp.find(cinema_id) == this->mp.end()) {
            Logger::get_instance()->log_message_on_terminal("No such cinema is registered...");
            return nullptr;
        }
        return this->mp[cinema_id];
    }
};
Cinema_Distributor* Cinema_Distributor::instance = new Cinema_Distributor();

// single point of access of movies to prevent dangling pointers
class Movie_Distributor {
private:
    map<string, Movie*> mp;
    static Movie_Distributor* instance;
    Movie_Distributor() {};
public:
    ~Movie_Distributor() {
        for (auto&[k,v] : this->mp) {
            delete v;
        }
    }

    static Movie_Distributor* get_instance() {
        return Movie_Distributor::instance;
    }

    // setter
    void add_movie(Movie* movie) {
        this->mp[movie->get_movie_id()] = movie;
    }
    // getter
    Movie* get_movie(string movie_id) {
        if (this->mp.find(movie_id) == this->mp.end()) {
            Logger::get_instance()->log_message_on_terminal("No such movie is registered...");
            return nullptr;
        }
        return this->mp[movie_id];
    }
};
Movie_Distributor* Movie_Distributor::instance = new Movie_Distributor();

// contains meta data about booking request made by a client
enum class STATUS {
    INITIATED,
    SEATES_LOCKED,
    PAYMENT_PENDING,
    CONFIRMED,
    EXPIRED,
    FAILED
};
class Booking_Request;
class State {
public:
    virtual STATUS get_status() = 0;
    virtual void cycle_forward(Booking_Request* req) = 0;
    void fail_the_request(Booking_Request* req);
    void expire_this_request(Booking_Request* req);
    virtual ~State() {};
};

class INITIATED : public State {
public:
    INITIATED() {};
    ~INITIATED() {};
    STATUS get_status() override {
        return STATUS::INITIATED;
    }

    void cycle_forward(Booking_Request* req) override;
};

class SEATS_LOCKED : public State {
    public:
    SEATS_LOCKED() {};
    ~SEATS_LOCKED() {};
    STATUS get_status() override {
        return STATUS::SEATES_LOCKED;
    }
    void cycle_forward(Booking_Request* req) override;
};

class PAYMENT_PENDING : public State {
    public:
    PAYMENT_PENDING() {};
    ~PAYMENT_PENDING() {};
    
    STATUS get_status() override {
        return STATUS::PAYMENT_PENDING;
    }
    void cycle_forward(Booking_Request* req) override;
};

class CONFIRMED : public State {
    public:
    CONFIRMED() {};
    ~CONFIRMED() {};
    
    STATUS get_status() override {
        return STATUS::CONFIRMED;
    }
    void cycle_forward(Booking_Request* req) override;
};

class EXPIRED : public State {
    public:
    EXPIRED () {};
    ~EXPIRED () {};
    
    STATUS get_status() override {
        return STATUS::EXPIRED;
    }
    void cycle_forward(Booking_Request* req) override;
};

class FAILED : public State {
    public:
    FAILED () {};
    ~FAILED () {};
    
    STATUS get_status() override {
        return STATUS::FAILED;
    }
    void cycle_forward(Booking_Request* req) override;
};

// booking request just contains data needed for futhur processing
class Booking_Request {
private:
    // states

    State* current_state;
    INITIATED* initiated_state;
    SEATS_LOCKED* seats_locked_state;
    PAYMENT_PENDING* payment_pending_state;
    CONFIRMED* confirmed_state;
    EXPIRED* expired_state;
    FAILED* failed_state;
    // states

    string req_id = ".";
    string movie_id = ".";
    string cinema_id = ".";
    string auditorium_id = ".";
    Timestamp* start_time;
    User_Details* user_details;
    vector<string> seats_selected;

    long long end_time_in_milliseconds = 0;

    friend class Booking_Request_Builder;

    
    Booking_Request() {
        this->start_time = nullptr;

        this->initiated_state = new INITIATED();
        this->seats_locked_state = new SEATS_LOCKED();
        this->payment_pending_state = new PAYMENT_PENDING();
        this->confirmed_state = new CONFIRMED();
        this->expired_state = new EXPIRED();
        this->failed_state = new FAILED();

        this->current_state = this->initiated_state;
    }
public:
    ~Booking_Request() {
        this->start_time = nullptr;
        delete this->user_details;
        delete this->current_state;
        delete this->initiated_state;
        delete this->seats_locked_state;
        delete this->payment_pending_state;
        delete this->confirmed_state;
        delete this->expired_state;
        delete this->failed_state;
    };
    
    // getters
    string get_chosen_movie_id() {
        return this->movie_id;
    }

    string get_chosen_cinema_id() {
        return this->cinema_id;
    }

    string get_request_id() {
        return this->req_id;
    }

    string get_assigned_auditorium_id() {
        return this->auditorium_id;
    }

    Timestamp* get_start_time_of_movie() {
        return this->start_time;
    }

    long long get_end_time() {
        return this->end_time_in_milliseconds;
    }

    User_Details* get_user_details() {
        return this->user_details;
    }

    vector<string>& get_seats() {
        return this->seats_selected;
    }

    // state dependent delegated functions
    STATUS get_status() {
        return this->current_state->get_status();
    }

    void cycle() {
        this->current_state->cycle_forward(this);
    }

    // state setter
    void set_state(State* s) {
        this->current_state = s;
    }

    // state getters
    State* get_current_state() {
        return this->current_state;
    }



    INITIATED* get_initiated_state() {
        return this->initiated_state;
    }

    SEATS_LOCKED* get_seats_locked_state() {
        return this->seats_locked_state;
    }

    PAYMENT_PENDING* get_payment_pending_state() {
        return this->payment_pending_state;
    }

    CONFIRMED* get_confirmed_state() {
        return this->confirmed_state;
    }

    EXPIRED* get_expired_state() {
        return this->expired_state;
    }

    FAILED* get_failed_state() {
        return this->failed_state;
    }

};

// definations of state functions
void INITIATED::cycle_forward(Booking_Request* req) {
    req->set_state(req->get_seats_locked_state());
}
void SEATS_LOCKED::cycle_forward(Booking_Request* req) {
    req->set_state(req->get_payment_pending_state());
}
void PAYMENT_PENDING::cycle_forward(Booking_Request* req) {
    req->set_state(req->get_confirmed_state());
}
void CONFIRMED::cycle_forward(Booking_Request* req) {
    req->set_state(this);
    // because even if cycle function is called on confirmed state , it should remain confirmed
}
void FAILED::cycle_forward(Booking_Request* req) {
    req->set_state(this);
}
void EXPIRED::cycle_forward(Booking_Request* req) {
    req->set_state(this);
}

void State::fail_the_request(Booking_Request* req) {
    req->set_state(req->get_failed_state());
}
void State::expire_this_request(Booking_Request* req) {
    req->set_state(req->get_expired_state());
}
// definations of state functions

class Booking_Request_Builder {
private:
    Booking_Request* request;    
public:
    Booking_Request_Builder() {
        this->request = new Booking_Request();
    }
    ~Booking_Request_Builder() {
        this->request = nullptr;
    }

    Booking_Request_Builder& set_movie_id(string movie_id) {
        this->request->movie_id = movie_id;
        return *this;
    }

    Booking_Request_Builder& set_cinema_id(string cinema_id) {
        this->request->cinema_id = cinema_id;
        return *this;
    }

    Booking_Request_Builder& set_end_time(long long milliseconds) {
        this->request->end_time_in_milliseconds = milliseconds;
        return *this;
    }

    Booking_Request_Builder& set_auditorium_id(string auditorium_id) {
        this->request->auditorium_id = auditorium_id;
        return *this;
    }

    Booking_Request_Builder& set_timing(Timestamp* start_time) {
        this->request->start_time = start_time;
        return *this;
    }

    Booking_Request_Builder& set_request_id(string request_id) {
        this->request->req_id = request_id;
        return *this;
    }

    Booking_Request_Builder& add_chosen_seats(vector<string> seats) {
        this->request->seats_selected = seats;
        return *this;
    }

    Booking_Request_Builder& add_user_details(User_Details* details) {
        this->request->user_details = details;
        return *this;
    }

    Booking_Request* build() {
        // perform all checks
        if (this->request->movie_id == ".") {
            Logger::get_instance()->log_message_on_terminal("Please set movie_id...");
            return nullptr;
        }
        if (this->request->req_id == ".") {
            Logger::get_instance()->log_message_on_terminal("Please set request id...");
            return nullptr;
        }
        if (this->request->end_time_in_milliseconds == 0) {
            Logger::get_instance()->log_message_on_terminal("Please set timeout...");
            return nullptr;
            
        }
        if (this->request->cinema_id == ".") {
            Logger::get_instance()->log_message_on_terminal("Please set cinema_id...");
            return nullptr;
        }
        if (this->request->auditorium_id == ".") {
            Logger::get_instance()->log_message_on_terminal("Please set auditorium_id...");
            return nullptr;
        }
        if (this->request->start_time == nullptr) {
            Logger::get_instance()->log_message_on_terminal("Please set start time of movie...");
            return nullptr;
        }
        
        if (this->request->user_details == nullptr) {
            Logger::get_instance()->log_message_on_terminal("Please set user details...");
            return nullptr;

        }
        
        if (this->request->seats_selected.size() == 0) {
            Logger::get_instance()->log_message_on_terminal("Please select at least one seat...");
            return nullptr;

        }

        return this->request;



    }
};

class Pricing_Strategy {
public:
    virtual double calculate_price(Booking_Request* request) = 0;
    virtual ~Pricing_Strategy() {};
};

class Standard_Pricing_Strategy : public Pricing_Strategy {
private:
    double weekend_surcharge = 1.2;
    double prime_time_hour_multiplier = 1.2;
public:
    Standard_Pricing_Strategy(double weekend_surcharge, double prime_time_hour_multiplier) {
        this->weekend_surcharge = weekend_surcharge;
        this->prime_time_hour_multiplier = prime_time_hour_multiplier;
    }
    ~Standard_Pricing_Strategy() {};

    double calculate_price(Booking_Request* request) override {
        double total_price = 0.0;
        
        // this considers base price + timing charges + prime time hour multipliers
        double base_cost = Movie_Distributor::get_instance()->get_movie(request->get_chosen_movie_id())->get_base_price();

        // now checks the time if it is a saturday or a sunday than , gives a price hike and also check prime time or not then assignes prime charge multiplication factor
        auto now = std::chrono::system_clock::now();
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);

        std::tm local_time = *std::localtime(&current_time);

        int day = local_time.tm_wday;
        int hour = local_time.tm_hour;

        total_price = base_cost;

        // Saturday = 6, Sunday = 0
        if (day == 0 || day == 6) {
            total_price *= this->weekend_surcharge;
        }

        // Prime time: 6 PM to 10 PM
        if (hour >= 18 && hour < 22) {
            total_price *= this->prime_time_hour_multiplier;
        }

        return total_price;
    }

};

// NOTE: THE selection metadata is just info needed to initialize the request so it is not same as booking request
// booking request is furthur used for processing
class Selection_Metadata {
private:
    string movie_id = ".";
    string cinema_id = ".";
    string auditorium_id = ".";
    Timestamp* start_time;
    User_Details* user_details;
    vector<string> seats_selected;
public:
    Selection_Metadata(string movie_id, string cinema_id, string auditorium_id, Timestamp* start_time, User_Details* user_details, vector<string>& seats_selected) {
        this->movie_id = movie_id;
        this->cinema_id = cinema_id;
        this->auditorium_id = auditorium_id;
        this->start_time = start_time;
        this->user_details = user_details;
        this->seats_selected = seats_selected;
    }

    ~Selection_Metadata() {
        delete this->user_details;
        delete this->start_time;
    }

    // getters
    string get_chosen_movie_id() {
        return this->movie_id;
    }

    string get_chosen_cinema_id() {
        return this->cinema_id;
    }

    string get_assigned_auditorium_id() {
        return this->auditorium_id;
    }

    Timestamp* get_start_time_of_movie() {
        return this->start_time;
    }

    User_Details* get_user_details() {
        return this->user_details;
    }

    vector<string>& get_seats() {
        return this->seats_selected;
    }
};

// this is a low level module which will interact with database 
// so we will follow dependency inversion principle that low level module should be abstraction dependent on high level module(manager here)
class ISeats_Aquisition_Handler {
public:
    virtual bool lock_seats(Booking_Request* req) = 0;
    virtual void unlock_seats(Booking_Request* req) = 0;
    virtual ~ISeats_Aquisition_Handler() = default;
};

class SQL_Database_Handler : public ISeats_Aquisition_Handler {
public:
    SQL_Database_Handler() {};
    ~SQL_Database_Handler() {};

    bool lock_seats(Booking_Request* req) override {
        // this will play with the SQL db and lock the seats for now we are returning true
        return true;
    }

    void unlock_seats(Booking_Request* req) override {
    }
};

class MONGODB_Database_Handler : public ISeats_Aquisition_Handler {
public:
    MONGODB_Database_Handler() {};
    ~MONGODB_Database_Handler() {};

    bool lock_seats(Booking_Request* req) override {
        // this will play with the MONGO db and lock the seats for now we are returning true
        return true;
    }

    void unlock_seats(Booking_Request* req) override {
    }

};

class Request_Handler {
private:
    map<string, Booking_Request*> mp;
public:
    Request_Handler() {};
    ~Request_Handler() {
        for (auto&[k,v] : this->mp) {
            delete v;
        }
    }

    void add_request_to_registery(Booking_Request* req) {
        this->mp[req->get_request_id()] = req;
    }

    void remove_request_from_registery(string request_id) {
        if (this->mp.find(request_id) != this->mp.end()) {
            this->mp[request_id] = nullptr;
            this->mp.erase(request_id);
        }
    }

    Booking_Request* get_request(string request_id) {
        if (this->mp.find(request_id) == this->mp.end()) {
            Logger::get_instance()->log_message_on_terminal("no such request is currently issued...");
            return nullptr;
        }
        return this->mp[request_id];
    }
};

// this will internally follow adapter pattern and its internal implementation depending on gateway
class IPayment_Handler {
public:
    virtual bool make_payment(Booking_Request* req) = 0;
    virtual ~IPayment_Handler() {};
};

class UPI_Payment_Handler : public IPayment_Handler {
public:
    UPI_Payment_Handler() {};
    ~UPI_Payment_Handler() {};

    bool make_payment(Booking_Request* req) override {
        return true;
    }
    
};

class Card_Payment_Handler : public IPayment_Handler {
public:
    Card_Payment_Handler() {};
    ~Card_Payment_Handler() {};
    bool make_payment(Booking_Request* req) override {
        return true;
    }
};

// we will follow template design pattern because from choosing cinema to request completed there should be a fixed flow
class IBooking_Template {
public:
    virtual ~IBooking_Template() = default;
    virtual Booking_Request* initialize_request(Selection_Metadata* data) = 0;
    virtual bool lock_seats(Booking_Request* req) = 0;
    virtual void erase_request(string req_id) = 0;
    virtual void unlock_seats(Booking_Request* req) = 0;
    virtual bool make_payment(Booking_Request* req) = 0;
    virtual bool check_timeout(Booking_Request* req) = 0;
    virtual shared_mutex& get_mutex() = 0;
    // we will use mutex with a unique lock to ensure thread safety and that this booking goes as one atomic operation
    void template_method_book(Selection_Metadata* data) {
        unique_lock<shared_mutex> lock(this->get_mutex());
        // STEP 1 : initialize a request
        Booking_Request* req = this->initialize_request(data);
        

        // if now request fails at any state, cycle it to fail state
        // or if anywhere timeoutoccurs, cycle it to expired
        // STEP 2 : lock the seats according to chosen method of seat locking
        bool isBooked = lock_seats(req);
        bool is_timeout = this->check_timeout(req);
        if (!isBooked && !is_timeout) {
            Logger::get_instance()->log_message_on_terminal("Due to some server error, seats were not booked, please try again later...");
            // failed request
            req->get_current_state()->fail_the_request(req);
            // now unlock the seats
            this->unlock_seats(req);
            return;
            
        } else if (is_timeout) {
            Logger::get_instance()->log_message_on_terminal("Request timed out...");
            req->get_current_state()->expire_this_request(req);
            // now unlock the seats
            this->unlock_seats(req);
            return;
        }

        // else success, cycle to seats locked
        req->cycle();

        // after locking, cycle the request mode to seats locked state if everything goes well


        // STEP 3 : now start payment using the payment gateway integreted already assumed
        // and while the payent is being processed the state should be cycled forward to payment pending inside that function only
        bool paymentDone = this->make_payment(req);
        is_timeout = this->check_timeout(req);
        if (!paymentDone && !is_timeout) {
            // due to some error, payment was not successful
            Logger::get_instance()->log_message_on_terminal("due to some error, payment was not successful");
            // failed request
            req->get_current_state()->fail_the_request(req);
            // now unlock the seats
            this->unlock_seats(req);
            return;
        } else if (is_timeout) {
            Logger::get_instance()->log_message_on_terminal("Request timed out...");
            req->get_current_state()->expire_this_request(req);
            // now unlock the seats
            this->unlock_seats(req);
            return;
        }



        // if we receive true response, cycle the mode to confirmed
        req->cycle();

        

        // at last what ever happens to request it must be removed from existence
        this->erase_request(req->get_request_id());
        lock.unlock();
    }

};

// cinema manager has single job : Managing cinema movies
// + delegtion of other responsibilities
class Cinema_Manager : public IBooking_Template {
private:
    // map for which cinema contains which movies
    map<string, Timing_Map*> cinema_maps;
    shared_mutex mtx;
    Pricing_Strategy* ps;
    ISeats_Aquisition_Handler* seat_handler;
    Request_Handler* rh;
    IPayment_Handler* ph;
    Cinema_Manager() {
        this->rh = new Request_Handler();
    };
    friend class Manager_builder;
    
public:
    ~Cinema_Manager() {
        for (auto&[k,v] : this->cinema_maps) {
            delete v;
        }
        delete this->ps;
        delete this->seat_handler;
        delete this->rh;
        delete this->ph;
    }

    void register_cinema(string cinema_id) {
        unique_lock<shared_mutex> lock(this->mtx);
        this->cinema_maps[cinema_id] = new Timing_Map();
        lock.unlock();
    }
    
    void assign_movie_time(string cinema_id, string movie_id, string auditorium_id, Timestamp* timing) {
        unique_lock<shared_mutex> lock(this->mtx);
        if (this->cinema_maps.find(cinema_id) == this->cinema_maps.end()) {
            Logger::get_instance()->log_message_on_terminal("No such cinema is registered...");
            return;
        }
        this->cinema_maps[cinema_id]->add_movie(movie_id, auditorium_id, timing);
        lock.unlock();
    }

    void remove_movie(string cinema_id, string movie_id) {
        unique_lock<shared_mutex> lock(this->mtx);
        if (this->cinema_maps.find(cinema_id) == this->cinema_maps.end()) {
            Logger::get_instance()->log_message_on_terminal("No such cinema is registered...");
            return;
        }
        this->cinema_maps[cinema_id]->remove_movie(movie_id);
        lock.unlock();
    }

    shared_mutex& get_mutex() override{
        return this->mtx;
    }    
    // booking flow
    Booking_Request* initialize_request(Selection_Metadata* data) override {
        Booking_Request* req = Booking_Request_Builder()
                               .set_movie_id(data->get_chosen_movie_id())
                               .set_cinema_id(data->get_chosen_cinema_id())
                               .set_auditorium_id(data->get_assigned_auditorium_id())
                               .set_timing(data->get_start_time_of_movie())
                               .add_chosen_seats(data->get_seats())
                               .add_user_details(data->get_user_details())
                               .build();

        // register this request to manager
        this->rh->add_request_to_registery(req);

        // return request created
        return req;
    }
    bool lock_seats(Booking_Request* req) override {
        return this->seat_handler->lock_seats(req);
    }
    void unlock_seats(Booking_Request* req) override {
        this->seat_handler->unlock_seats(req);
    }
    void erase_request(string req_id) override {
        this->rh->remove_request_from_registery(req_id);
    }
    bool make_payment(Booking_Request* req) override {
        req->cycle();
        // call the payment handler to handle the payment and return the response
        return this->ph->make_payment(req);
    }
    bool check_timeout(Booking_Request* req) override {
        // Get current time in milliseconds since epoch
        auto now = std::chrono::system_clock::now();
        long long current_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();

        // Return true if current time exceeds the request's deadline
        return current_time_ms > req->get_end_time();
    }
};

class Manager_builder {
private:
    Cinema_Manager* manager;
public:
    Manager_builder() {};
    ~Manager_builder() {
        this->manager = nullptr;
    };

    Manager_builder& set_pricing_strategy(Pricing_Strategy* strategy) {
        this->manager->ps = strategy;
        return *this;
    }

    Manager_builder& set_seat_handler(ISeats_Aquisition_Handler* seat_handler) {
        this->manager->seat_handler = seat_handler;
        return *this;
    }

    Manager_builder& set_payment_hanlder(IPayment_Handler* payment_handler) {
        this->manager->ph = payment_handler;
        return *this;
    }
    Cinema_Manager* build() {
        if (this->manager->ps == nullptr) {
            Logger::get_instance()->log_message_on_terminal("You have not setup the pricing strategy yet...");
            return nullptr;
        }
        if (this->manager->seat_handler == nullptr) {
            Logger::get_instance()->log_message_on_terminal("You have not setup the seat handler yet...");
            return nullptr;
        }
        if (this->manager->ph == nullptr) {
            Logger::get_instance()->log_message_on_terminal("You have not setup the payment handler yet...");
            return nullptr;
        }


        return this->manager;
    }
};

int main() {
    return 0;
}