#include <bits/stdc++.h>
using namespace std;

class Logger {
private:
    int line_number = 1;
    static Logger* instance;
    Logger() {}    
public:
    static Logger* get_instance() {
        return Logger::instance;
    }
    void log_in_file(string file_path, string message) {}
    void log_on_terminal( string message) {
        cout << "LINE NUMBER : " << this->line_number;
        cout << message << endl;
        cout << " - " << endl;
    }
};
Logger* Logger::instance = new Logger();

class Timestamp {
private:
    long long milliseconds;
public:
    Timestamp (long long milliseconds) {
        this->milliseconds = milliseconds;
    }

    ~Timestamp () {

    }
    // getter
    long long get_time_of_arrival() {
        return this->milliseconds;
    }

    // operator overloading
    bool operator<(const Timestamp& other) {
        return this->milliseconds<other.milliseconds;
    }

};

// ORDER CONTAINS METADATA REQUIRED FOR EXECUTION ENGINE TO EXECUTE THE ORDER
// and one job that is to execute it self
enum class Type{
    BID,
    ASK
};

enum class STATUS {
    RECEIVED,
    OPEN,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED,
    REJECTED
};

class Order;

class State {
public:
    virtual STATUS get_status() = 0;
    virtual void cycle_forward(Order* o) = 0;
    virtual void execute(Order* o) = 0;
    void cancel(Order* o);
    void reject(Order* o);
    virtual ~State() {};
};

class Received : public State {
public:
    Received() {}
    ~Received() {}

    STATUS get_status() override {
        return STATUS::RECEIVED;
    }
    void execute(Order* o) override;
    void cycle_forward(Order* o) override;
};

class Open : public State {
    public:
    Open() {}
    ~Open() {}
    
    STATUS get_status() override {
        return STATUS::OPEN;
    }
    void cycle_forward(Order* o) override;
    void execute(Order* o) override;
};

class Partially_Filled : public State {
    public:
    Partially_Filled() {}
    ~Partially_Filled() {}
    STATUS get_status() override {
        return STATUS::PARTIALLY_FILLED;
    }
    void cycle_forward(Order* o) override;
    void execute(Order* o) override;
};

class Filled : public State {
    public:
    Filled() {}
    ~Filled() {}
    STATUS get_status()override {
        return STATUS::FILLED;
    }
    void cycle_forward(Order* o) override;
    void execute(Order* o) override;
};

class Cancelled : public State {
    public:
    Cancelled() {}
    ~Cancelled() {}
    
    STATUS get_status() override {
        return STATUS::CANCELLED;
    }
    void cycle_forward(Order* o) override;
    void execute(Order* o) override;
};

class Rejected : public State {
    public:
    Rejected() {}
    ~Rejected() {}
    
    STATUS get_status() override {
        return STATUS::REJECTED;
    }
    void cycle_forward(Order* o) override;
    void execute(Order* o) override;
};

class State_Distributor {
private:
    map<STATUS, State*> mp;
    static State_Distributor* instance;
    State_Distributor() {
        this->mp[STATUS::RECEIVED] = new Received();
        this->mp[STATUS::OPEN] = new Open();
        this->mp[STATUS::CANCELLED] = new Cancelled();
        this->mp[STATUS::FILLED] = new Filled();
        this->mp[STATUS::PARTIALLY_FILLED] = new Partially_Filled();
        this->mp[STATUS::REJECTED] = new Rejected();
    };
public:
    ~State_Distributor() {
        for (auto& [k,v] : this->mp) {
            delete v;
        }
    }
    static State_Distributor* get_instance() {
        return State_Distributor::instance;
    }

    State* get_state(STATUS stat) {
        return this->mp[stat];
    }
};
State_Distributor* State_Distributor::instance = new State_Distributor();

class Order {
protected:
    string order_id;
    string product_id;
    Type type_of_order;
    Timestamp* time_of_arrival;

    
    // states
    State* current_state;
    Received* received_state;
    Open* open_state;
    Partially_Filled* partially_filled_state;
    Filled* filled_state;
    Cancelled* cancelled_state;
    Rejected* rejected_state;

public:
    Order() {
        this->received_state = dynamic_cast<Received*> (State_Distributor::get_instance()->get_state(STATUS::RECEIVED));
        this->open_state = dynamic_cast<Open*> (State_Distributor::get_instance()->get_state(STATUS::OPEN));
        this->partially_filled_state = dynamic_cast<Partially_Filled*> (State_Distributor::get_instance()->get_state(STATUS::PARTIALLY_FILLED));
        this->filled_state = dynamic_cast<Filled*> (State_Distributor::get_instance()->get_state(STATUS::FILLED));
        this->cancelled_state = dynamic_cast<Cancelled*> (State_Distributor::get_instance()->get_state(STATUS::CANCELLED));
        this->rejected_state = dynamic_cast<Rejected*> (State_Distributor::get_instance()->get_state(STATUS::REJECTED));
        this->current_state = this->received_state;        
    }

    

    Type get_type() {
        return this->type_of_order;
    }

    string get_order_id() {
        return this->order_id;
    }

    Timestamp* get_time_of_arrival() {
        return this->time_of_arrival;
    }
    
    ~Order () {
        delete this->time_of_arrival;
    }

    // state getters and setter
    void set_state(State* s) {
        this->current_state = s;
    }

    Received* get_received_state() {
        return this->received_state;
    }

    Open* get_open_state() {
        return this->open_state;
    }

    Partially_Filled* get_partially_filled() {
        return this->partially_filled_state;
    }

    Filled* get_filled_state() {
        return this->filled_state;
    }

    Cancelled* get_cancelled_state() {
        return this->cancelled_state;
    }

    Rejected* get_rejected_state() {
        return this->rejected_state;
    }

    // state function delegators

    void cycle() {
        this->current_state->cycle_forward(this);
    }

    virtual void EXECUTE() = 0;

};

class Limit_Order : public Order {
private:
    Timestamp* target_time;
    double target_price;
public:
    Limit_Order(string order_id, string product_id, Type order_type, Timestamp* target_time, double target_price, Timestamp* time_of_arrival) {
        this->type_of_order = order_type;
        this->product_id = product_id;
        this->order_id = order_id;
        this->time_of_arrival = time_of_arrival;
        this->target_price = target_price;
        this->target_time = target_time;
    };
    ~Limit_Order() {
        delete this->target_time;
    };
    
    void EXECUTE() override {
        this->current_state->execute(this);
    }

    double get_target_price() {
        return this->target_price;

    }

    Timestamp* get_time_of_arrival() {
        return this->time_of_arrival;
    }

};
struct BuyComparator {
    bool operator()(Limit_Order* o1, Limit_Order* o2) {
        double p1 = o1->get_target_price();
        double p2 = o2->get_target_price();
        
        if (p1 != p2) {
            // Max-heap: Lower price has lower priority so higher price stays at .top()
            return p1 < p2;
        }
        
        // FIFO tie-breaker: Older timestamp (smaller value) gets higher priority.
        // Returning true if o1 is newer means o1 sinks below o2.
        return o1->get_time_of_arrival()->get_time_of_arrival() > 
               o2->get_time_of_arrival()->get_time_of_arrival();
    }
};

struct AskComparator {
    bool operator()(Limit_Order* o1, Limit_Order* o2) {
        double p1 = o1->get_target_price();
        double p2 = o2->get_target_price();
        
        if (p1 != p2) {
            // Min-heap implemented via std::priority_queue: 
            // Higher price has lower priority so lowest price stays at .top()
            return p1 > p2;
        }
        
        // FIFO tie-breaker: Older timestamp gets higher priority.
        return o1->get_time_of_arrival()->get_time_of_arrival() > 
               o2->get_time_of_arrival()->get_time_of_arrival();
    }
};

class Market_Order : public Order {
public:
    Market_Order(string order_id,string product_id, Type order_type, Timestamp* time_of_arrival) {
        this->type_of_order = order_type;
        this->product_id = product_id;
        this->time_of_arrival = time_of_arrival;
        this->order_id = order_id;
    }
    
    ~Market_Order () {
    };
    void EXECUTE() override {
        this->current_state->execute(this);
    }
};

// state function definitions, execute, cycle
void Received::cycle_forward(Order* order) {
    order->set_state(order->get_open_state());
}
void Received::execute(Order* order) {
    Logger::get_instance()->log_on_terminal("Wait for the task to enter the queue");
}

void Open::cycle_forward(Order* order) {
    order->set_state(order->get_partially_filled());
}
void Open::execute(Order* order) {
    Logger::get_instance()->log_on_terminal("Running Task : " + order->get_order_id() + " now");
    // internal implementation here
}

void Partially_Filled::cycle_forward(Order* order) {
    order->set_state(order->get_filled_state());
}
void Partially_Filled::execute(Order* order) {
    Logger::get_instance()->log_on_terminal("Task : " + order->get_order_id() + " is being executed already");
}

void Filled::cycle_forward(Order* order) {
    order->set_state(order->get_filled_state());
    // because filled means completed order, so it wont change state
}
void Filled::execute(Order* order) {
    Logger::get_instance()->log_on_terminal("Task : " + order->get_order_id() + " already executed");
}

void Rejected::cycle_forward(Order* order) {
    order->set_state(order->get_rejected_state());
}
void Rejected::execute(Order* order) {
    Logger::get_instance()->log_on_terminal("Task : " + order->get_order_id() + " is rejected");
}

void Cancelled::cycle_forward(Order* order) {
    order->set_state(order->get_cancelled_state());
    Logger::get_instance()->log_on_terminal("Task : " + order->get_order_id() + " was cancelled by a cancel request");
}
void Cancelled::execute(Order* order) {
    Logger::get_instance()->log_on_terminal("Task : " + order->get_order_id() + " was cancelled by a cancel request");
}

class Cancel_Request {
private:
    string target_id;
public:
    Cancel_Request(string target_id) {
        this->target_id = target_id;
    }

    ~Cancel_Request() {}

    // getter
    string get_target_id() {
        return this->target_id;
    }
};

class Order_Book {
private:
    priority_queue<Limit_Order*, vector<Limit_Order*>, BuyComparator> buy_side;
    priority_queue<Limit_Order*, vector<Limit_Order*>, AskComparator> sell_side;
    mutex mtx;

    // workers
    thread t1;
    condition_variable cv;
    bool time_to_go = false;
    map<string, bool> validity_map;
    bool condition() {
        return (this->time_to_go || !this->buy_side.empty() || !this->sell_side.empty());
    }
    void CONSUME() {
        while (true) {

            unique_lock<mutex> lock(this->mtx);
            cv.wait(lock, [this]() {
                return condition();
            });

            if (this->time_to_go) {
                // now its over
                return;
            }
            Order* o = nullptr;
            // else means there is a task available in one of the queues
            if (!this->sell_side.empty()) {
                
                o = this->sell_side.top();
                this->sell_side.pop();
                if (this->validity_map[o->get_order_id()] == false) {
                    // this order has been cancelled
                    o->set_state(o->get_cancelled_state());
                    delete o;
                    continue;
                }
            } else if (!this->buy_side.empty()) {
                o = this->buy_side.top();
                this->buy_side.pop();
                if (this->validity_map[o->get_order_id()] == false) {
                    // this order has been cancelled
                    o->set_state(o->get_cancelled_state());
                    delete o;
                    continue;
                }
            }
            

            lock.unlock();
    
            // now execute the order which we fetched
            o->EXECUTE();
            delete o;
            // rest deciding state is handled inside the execute implementation
        }


    }
public:
    Order_Book() {
        this->t1 = thread(&Order_Book::CONSUME, this);
    }
    ~Order_Book() {
        unique_lock<mutex> lock(this->mtx);
        this->time_to_go = true;
        lock.unlock();

        // now notify the threads to get over
        cv.notify_one();

        if (t1.joinable()) {
            t1.join();
        }

        // now empty all remaining orders
        
        while (!this->buy_side.empty()) {
            Order* a = this->buy_side.top();
            delete a;
            this->buy_side.pop();
        }
        while (!this->sell_side.empty()) {
            Order* a = this->sell_side.top();
            delete a;
            this->sell_side.pop();
        }
    }

    void add_limit_order(Limit_Order* o) {
        o->set_state(o->get_received_state());
        unique_lock<mutex> lock(this->mtx);
        // mark order as valid
        this->validity_map[o->get_order_id()] = true;
        if (o->get_type() == Type::ASK) {
            this->sell_side.push(o);
        } else {
            this->buy_side.push(o);
        }
        o->set_state(o->get_open_state());

        lock.unlock();
        cv.notify_one();
    }

    void execute_market_order(Market_Order* o) {
        unique_lock<mutex> lock(this->mtx);
        o->EXECUTE();
    }

    void register_cancel_request(Cancel_Request* request) {
        unique_lock<mutex> lock(this->mtx);
        // mark order as invalid
        this->validity_map[request->get_target_id()] = false;
        lock.unlock();
    }



    
};

int main() {
    return 0;
}