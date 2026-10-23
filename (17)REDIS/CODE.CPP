#include <bits/stdc++.h>
using namespace std;

class Simple_Data_Type {
private:
public:
    virtual ~Simple_Data_Type() {};
};

template <typename T>
class Data : public Simple_Data_Type {
private:
    T value;
public:
    Data(T a) {
        this->value = a;
    }

    T GET() {
        return this->value;
    }

    void SET(T new_val) {
        this->value = new_val;
    }
};

enum class NAME {
    LINKED_LIST,
    SIMPLE,
    NESTED_MAP
};

class Linked_List {
private:
    deque<Simple_Data_Type*> dq;
public:
    Linked_List() {};
    ~Linked_List() {
        while (!this->dq.empty()) {
            Simple_Data_Type* a = this->dq.front();
            this->dq.pop_front();
            delete a;
        }
    }
    void RPUSH(Simple_Data_Type* a) {
        this->dq.push_back(a);
    }
    void LPUSH(Simple_Data_Type* a) {
        this->dq.push_front(a);
    }
    Simple_Data_Type* LPOP() {
        if (!this->dq.empty()) {
            Simple_Data_Type* a = this->dq.front();
            this->dq.pop_front();
            return a;
        }
        return nullptr;
        
    }
    Simple_Data_Type* RPOP() {
        if (!this->dq.empty()) {
            Simple_Data_Type* a = this->dq.back();
            this->dq.pop_back();
            return a;
        }
        return nullptr;
    }
    bool isEmpty() {
        return this->dq.empty();
    }
    int get_size() {
        return this->dq.size();
    }
    Simple_Data_Type*GET(int index) {
        return this->dq[index];
    }
};

class Nested_Map {
private:
    map<string, Simple_Data_Type*> mp;
public:
    Nested_Map() {}
    ~Nested_Map() {
        for (auto &[k, v] : this->mp) {
            delete v;
        }

    }

    void HSET(string key, Simple_Data_Type* a) {
        if (this->mp.find(key)!=this->mp.end()) {
            delete this->mp[key];
        }
        this->mp[key] = a;
    }

    Simple_Data_Type* HGET(string key) {
        if (this->mp.find(key) == this->mp.end()) {
            return nullptr;
        }
        return this->mp[key];
    }

    map<string, Simple_Data_Type*>& get_map() {
        return this->mp;
    }

    void DEL(string key) {
        if (this->mp.find(key)!=this->mp.end()) {
            delete this->mp[key];
            this->mp[key] = nullptr;
            this->mp.erase(key);
        }
    }
};

// this provides a ttl options handling to any key that user wants
class Expiration_Handler {
private:
    map<string, long long> ttl; // {key : time in milliseconds}
    map<string, long long> time_of_last_update; //key : time of last update of this key(ms)
public:
    Expiration_Handler() {}
    ~Expiration_Handler() {}

    void add_ttl(string key, long long milliseconds) {
        this->ttl[key] = milliseconds;
        this->update_last_time(key);
    }
    // because add ttl is always called independently, so it needs a lock within the function execution itself

    void remove_ttl(string key) {
        this->ttl.erase(key);
        this->time_of_last_update.erase(key);
        
    }
    
    void update_last_time(string key) {
        // set this keys last time of update to current
        
        if (this->ttl.find(key)==this->ttl.end()) {
            return;
        }
        auto wall_now = chrono::steady_clock::now();
        long long time_ms = chrono::duration_cast<chrono::milliseconds>(wall_now.time_since_epoch()).count();
        
        this->time_of_last_update[key] = time_ms;
        
    }
    // NOTE: update_last_time and remove ttl, doesnt need a unique lock as it is always called within a critical section already

    // returns true if this key is currently not expired
    bool is_valid(string key) {
        if (this->ttl.find(key) == this->ttl.end()) {
            return true;
        }

        // else key's ttl is defined
        auto wall_now = chrono::steady_clock::now();
        long long time_ms = chrono::duration_cast<chrono::milliseconds>(wall_now.time_since_epoch()).count();
        
        long long diff = time_ms - this->time_of_last_update[key];
        if (diff < this->ttl[key]) {
            return true;
        } else {
            return false;
        }
    }
    

};

class Tool;
// properly increments a numeric valued key with checks
// keeping scalable for future use
class Increment_Handler {
public:
    virtual bool INC(Tool* tool, string key) = 0;
    ~Increment_Handler() = default;
};

class Standard_Increment_Handler : public Increment_Handler {
public:
    bool INC(Tool* tool, string key) override;
};

// this does all the pre checks required before mutating or anything that require checks in tools class
// we will keep this scalable for future rules
class Normalizer {
public:
    virtual bool pre_normal_insertion_check(Tool* tool, string key) = 0;
    virtual bool pre_linked_list_insertion_check(Tool* tool, string key) = 0;
    virtual bool pre_nested_map_insertion_check(Tool* tool, string key) = 0;
    virtual ~Normalizer() = default;
};

class Standard_Normalizer : public Normalizer {
public:
    bool pre_normal_insertion_check(Tool* tool, string key) override;
    bool pre_linked_list_insertion_check(Tool* tool, string key) override;
    bool pre_nested_map_insertion_check(Tool* tool, string key) override;
};

// one job: correctly delete the key and everthing associated with it
class Deletion_Handler {
public:
    virtual void DELETE(Tool* tool, string key) = 0;
    virtual bool NESTED_DELETE(Tool* tool, string key, string key2) = 0;
    virtual ~Deletion_Handler() = default;
};

class Standard_Deletion_Handler : public Deletion_Handler {
public:
    void DELETE(Tool* tool, string key) override;
    bool NESTED_DELETE(Tool* tool, string key, string key2) override;
};

// a series of commands : it will use tool object to execute its commands sequentially when EXECUTE() is called upon it
class Pipeline_Command {
public:
    virtual void EXECUTE(Tool* tool) = 0;
    virtual ~Pipeline_Command() = default;
};

class PUT_Command : public Pipeline_Command {
private:
    string key;
    Simple_Data_Type* d;
public:
    PUT_Command(string key, int value) {
        this->key = key;
        this->d = new Data<int> (value);
    }
    PUT_Command(string key, string value) {
        this->key = key;
        this->d = new Data<string> (value);
    }
    PUT_Command(string key, double value) {
        this->key = key;
        this->d = new Data<double> (value);
    }
    PUT_Command(string key, long long value) {
        this->key = key;
        this->d = new Data<long long> (value);
    }
    ~PUT_Command() {
        delete this->d;
    }
    void EXECUTE(Tool* tool) override;
};

class LPUSH_Command : public Pipeline_Command {
private:
    string key;
    Simple_Data_Type* d;
public:
    LPUSH_Command(string key, int value) {
        this->key = key;
        this->d = new Data<int> (value);
    }
    LPUSH_Command(string key, string value) {
        this->key = key;
        this->d = new Data<string> (value);
    }
    LPUSH_Command(string key, double value) {
        this->key = key;
        this->d = new Data<double> (value);
    }
    LPUSH_Command(string key, long long value) {
        this->key = key;
        this->d = new Data<long long> (value);
    }
    ~LPUSH_Command() {
        delete this->d;
    }
    void EXECUTE(Tool* tool) override;
};

class RPUSH_Command : public Pipeline_Command {
private:
    string key;
    Simple_Data_Type* d;
public:
    RPUSH_Command(string key, int value) {
        this->key = key;
        this->d = new Data<int> (value);
    }
    RPUSH_Command(string key, string value) {
        this->key = key;
        this->d = new Data<string> (value);
    }
    RPUSH_Command(string key, double value) {
        this->key = key;
        this->d = new Data<double> (value);
    }
    RPUSH_Command(string key, long long value) {
        this->key = key;
        this->d = new Data<long long> (value);
    }
    ~RPUSH_Command() {
        delete this->d;
    }
    void EXECUTE(Tool* tool) override;
};

class HSET_Command : public Pipeline_Command {
private:
    string key;
    string field;
    Simple_Data_Type* d;
public:
    HSET_Command(string key, string field, int value) {
        this->key = key;
        this->field = field;
        this->d = new Data<int> (value);
    }
    HSET_Command(string key, string field, string value) {
        this->key = key;
        this->field = field;
        this->d = new Data<string> (value);
    }
    HSET_Command(string key, string field, double value) {
        this->key = key;
        this->field = field;
        this->d = new Data<double> (value);
    }
    HSET_Command(string key, string field, long long value) {
        this->key = key;
        this->field = field;
        this->d = new Data<long long> (value);
    }
    ~HSET_Command() {
        delete this->d;
    }
    void EXECUTE(Tool* tool) override;
};

class Pipeline {
private:
    Tool* tool;
    queue<Pipeline_Command*> command_queue;
public:
    Pipeline(Tool* tool) {
        this->tool = tool;
    };
    ~Pipeline() {
        this->tool = nullptr;
        while (!this->command_queue.empty()) {
            Pipeline_Command* c = this->command_queue.front();
            this->command_queue.pop();
            delete c;
        }
    }

    void PUT(string key, string value) {
        this->command_queue.push(new PUT_Command(key, value));
    }
    void PUT(string key, int value) {
        this->command_queue.push(new PUT_Command(key, value));
    }
    void PUT(string key, double value) {
        this->command_queue.push(new PUT_Command(key, value));
    }
    void PUT(string key, long long value) {
        this->command_queue.push(new PUT_Command(key, value));
    }


    void LPUSH(string key, string value) {
        this->command_queue.push(new LPUSH_Command(key, value));
    }
    void LPUSH(string key, int value) {
        this->command_queue.push(new LPUSH_Command(key, value));
    }
    void LPUSH(string key, double value) {
        this->command_queue.push(new LPUSH_Command(key, value));
    }
    void LPUSH(string key, long long value) {
        this->command_queue.push(new LPUSH_Command(key, value));
    }
    
    void RPUSH(string key, string value) {
        this->command_queue.push(new RPUSH_Command(key, value));
    }
    void RPUSH(string key, int value) {
        this->command_queue.push(new RPUSH_Command(key, value));
    }
    void RPUSH(string key, double value) {
        this->command_queue.push(new RPUSH_Command(key, value));
    }
    void RPUSH(string key, long long value) {
        this->command_queue.push(new RPUSH_Command(key, value));
    }


    void HSET(string key, string field, int value) {
        this->command_queue.push(new HSET_Command(key, field, value));
    }
    void HSET(string key, string field, string value) {
        this->command_queue.push(new HSET_Command(key, field, value));
    }
    void HSET(string key, string field, double value) {
        this->command_queue.push(new HSET_Command(key, field, value));
    }
    void HSET(string key, string field, long long value) {
        this->command_queue.push(new HSET_Command(key, field, value));
    }
    void EXECUTE();

};

class Type_Converter {
public:
    string stringify(Simple_Data_Type* a) {
        string value = "";
        if (auto p = dynamic_cast<Data<int>*>(a)) {
            value = to_string(p->GET());
        }
        else if (auto p = dynamic_cast<Data<double>*>(a)) {
            value = to_string(p->GET());
        }
        else if (auto p = dynamic_cast<Data<long long>*>(a)) {
            value = to_string(p->GET());
        }
        else if (auto p = dynamic_cast<Data<string>*>(a)) {
            value = p->GET();
        }
        return value;
        
    }
};

// tool's job is to perform crud on all types of maps (primary task)
//  + delegation of other tasks (secondary task)
// (prevents it from becoming a god object)
class Transaction_Command {
public:
    virtual void EXECUTE(Tool* tool) = 0;
    virtual ~Transaction_Command() = default;
};

class PUT_Transaction_Command : public Transaction_Command {
private:
    string key;
    Simple_Data_Type* d;
public:
    PUT_Transaction_Command(string key, int value) {
        this->key = key;
        this->d = new Data<int> (value);
    }
    PUT_Transaction_Command(string key, string value) {
        this->key = key;
        this->d = new Data<string> (value);
    }
    PUT_Transaction_Command(string key, double value) {
        this->key = key;
        this->d = new Data<double> (value);
    }
    PUT_Transaction_Command(string key, long long value) {
        this->key = key;
        this->d = new Data<long long> (value);
    }
    ~PUT_Transaction_Command() {
        delete this->d;
    }
    void EXECUTE(Tool* tool) override;
};

class LPUSH_Transaction_Command : public Transaction_Command {
private:
    string key;
    Simple_Data_Type* d;
public:
    LPUSH_Transaction_Command(string key, int value) {
        this->key = key;
        this->d = new Data<int> (value);
    }
    LPUSH_Transaction_Command(string key, string value) {
        this->key = key;
        this->d = new Data<string> (value);
    }
    LPUSH_Transaction_Command(string key, double value) {
        this->key = key;
        this->d = new Data<double> (value);
    }
    LPUSH_Transaction_Command(string key, long long value) {
        this->key = key;
        this->d = new Data<long long> (value);
    }
    ~LPUSH_Transaction_Command() {
        delete this->d;
    }
    void EXECUTE(Tool* tool) override;
};

class RPUSH_Transaction_Command : public Transaction_Command {
private:
    string key;
    Simple_Data_Type* d;
public:
    RPUSH_Transaction_Command(string key, int value) {
        this->key = key;
        this->d = new Data<int> (value);
    }
    RPUSH_Transaction_Command(string key, string value) {
        this->key = key;
        this->d = new Data<string> (value);
    }
    RPUSH_Transaction_Command(string key, double value) {
        this->key = key;
        this->d = new Data<double> (value);
    }
    RPUSH_Transaction_Command(string key, long long value) {
        this->key = key;
        this->d = new Data<long long> (value);
    }
    ~RPUSH_Transaction_Command() {
        delete this->d;
    }
    void EXECUTE(Tool* tool) override;
};

class HSET_Transaction_Command : public Transaction_Command {
private:
    string key;
    string field;
    Simple_Data_Type* d;
public:
    HSET_Transaction_Command (string key, string field, int value) {
        this->key = key;
        this->field = field;
        this->d = new Data<int> (value);
    }
    HSET_Transaction_Command (string key, string field, string value) {
        this->key = key;
        this->field = field;
        this->d = new Data<string> (value);
    }
    HSET_Transaction_Command (string key, string field, double value) {
        this->key = key;
        this->field = field;
        this->d = new Data<double> (value);
    }
    HSET_Transaction_Command (string key, string field, long long value) {
        this->key = key;
        this->field = field;
        this->d = new Data<long long> (value);
    }
    ~HSET_Transaction_Command () {
        delete this->d;
    }
    void EXECUTE(Tool* tool) override;
};

class Transaction {
private:
    Tool* tool;
    queue<Transaction_Command*> command_queue;
public:
    Transaction(Tool* tool) {
        this->tool = tool;
    };
    ~Transaction() {
        while (!this->command_queue.empty()) {
            Transaction_Command* c = this->command_queue.front();
            this->command_queue.pop();
            delete c;
        }
        this->tool = nullptr;
    }
    void EXEC();
    void DISCARD();
    void PUT(string key, string value) {
        this->command_queue.push(new PUT_Transaction_Command(key, value));
    }
    void PUT(string key, int value) {
        this->command_queue.push(new PUT_Transaction_Command(key, value));
    }
    void PUT(string key, double value) {
        this->command_queue.push(new PUT_Transaction_Command(key, value));
    }
    void PUT(string key, long long value) {
        this->command_queue.push(new PUT_Transaction_Command(key, value));
    }


    void LPUSH(string key, string value) {
        this->command_queue.push(new LPUSH_Transaction_Command(key, value));
    }
    void LPUSH(string key, int value) {
        this->command_queue.push(new LPUSH_Transaction_Command(key, value));
    }
    void LPUSH(string key, double value) {
        this->command_queue.push(new LPUSH_Transaction_Command(key, value));
    }
    void LPUSH(string key, long long value) {
        this->command_queue.push(new LPUSH_Transaction_Command(key, value));
    }
    
    void RPUSH(string key, string value) {
        this->command_queue.push(new RPUSH_Transaction_Command(key, value));
    }
    void RPUSH(string key, int value) {
        this->command_queue.push(new RPUSH_Transaction_Command(key, value));
    }
    void RPUSH(string key, double value) {
        this->command_queue.push(new RPUSH_Transaction_Command(key, value));
    }
    void RPUSH(string key, long long value) {
        this->command_queue.push(new RPUSH_Transaction_Command(key, value));
    }


    void HSET(string key, string field, int value) {
        this->command_queue.push(new HSET_Transaction_Command(key, field, value));
    }
    void HSET(string key, string field, string value) {
        this->command_queue.push(new HSET_Transaction_Command(key, field, value));
    }
    void HSET(string key, string field, double value) {
        this->command_queue.push(new HSET_Transaction_Command(key, field, value));
    }
    void HSET(string key, string field, long long value) {
        this->command_queue.push(new HSET_Transaction_Command(key, field, value));
    }
};

class Tool {
private:
    map<string, Simple_Data_Type*> simple_map;
    map<string, Linked_List*> LL_map;
    map<string, Nested_Map*> nm_map;
    map<string, NAME> Type;
    Expiration_Handler* eh;
    Normalizer* nm;
    Deletion_Handler* dh;
    Increment_Handler* ih;
    Type_Converter* tc;
    shared_mutex s_mtx;
    static Tool* instance;
    
    Tool () {
        this->eh = new Expiration_Handler();
        this->nm = new Standard_Normalizer();
        this->dh = new Standard_Deletion_Handler();
        this->ih = new Standard_Increment_Handler();
        this->tc = new Type_Converter();
    };
    friend class Standard_Increment_Handler;
    friend class Standard_Normalizer;
    friend class Standard_Deletion_Handler;
    friend class PUT_Transaction_Command;
    friend class LPUSH_Transaction_Command;
    friend class RPUSH_Transaction_Command;
    friend class HSET_Transaction_Command;
    friend class Transaction;
    // only the froend classes can access these getters
    map<string, Simple_Data_Type*>& get_simple_map () {
        return this->simple_map;
    }
    map<string, Linked_List*>& get_LL_map () {
        return this->LL_map;
    }
    map<string, Nested_Map*>& get_nm_map () {
        return this->nm_map;
    }
    map<string, NAME>& get_Type () {
        return this->Type;
    }
    shared_mutex& get_shared_mutex() {
        return this->s_mtx;
    }
    Expiration_Handler* get_expiration_handler() {
        return this->eh;
    }
    Deletion_Handler* get_deletion_handler() {
        return this->dh;
    }
    Normalizer* get_normalizer() {
        return this->nm;
    }
public:

    
    static Tool* get_instance() {
        return Tool::instance;
    }
    ~Tool () {
        delete this->eh;
        delete this->nm;
        delete this->dh;
        delete this->ih;
        delete this->tc;
        for (auto &[k, v] : this->simple_map) {
            delete v;
        }
        for (auto &[k, v] : this->LL_map) {
            delete v;
        }
        for (auto &[k, v] : this->nm_map) {
            delete v;
        }
    }
    
    // getters
   
    void add_ttl(string key, long long milliseconds) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        this->eh->add_ttl(key, milliseconds);
        lock.unlock();
    }
    void remove_ttl(string key) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        this->eh->remove_ttl(key);
        lock.unlock();
    }
    
    void PUT(string key, string value) {
        unique_lock<shared_mutex> lock(this->s_mtx); 
        bool response = this->nm->pre_normal_insertion_check(this, key);
        if (!response) {
            return;
        }
        
        this->simple_map[key] = new Data<string> (value);
        this->eh->update_last_time(key);
        lock.unlock();
    }
    void PUT(string key, int value) {
        unique_lock<shared_mutex> lock(this->s_mtx); 
        bool response = this->nm->pre_normal_insertion_check(this, key);
        if (!response) {
            return;
        }
        this->simple_map[key] = new Data<int> (value);
        this->eh->update_last_time(key);
        lock.unlock();
    }
    void PUT(string key, long long value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_normal_insertion_check(this, key);
        if (!response) {
            return;
        }

        this->simple_map[key] = new Data<long long> (value);
        this->eh->update_last_time(key);
        lock.unlock();
    }
    void PUT(string key, double value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_normal_insertion_check(this, key);
        if (!response) {
            return;
        }
        this->simple_map[key] = new Data<double> (value);
        this->eh->update_last_time(key);
        lock.unlock();
    }
    void INCR(string key) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->ih->INC(this, key);
        if (!response) {
            return;
        }
        this->eh->update_last_time(key);
        lock.unlock();
    }    
    string GET(string key) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        string ans = "";
        
        bool response = this->eh->is_valid(key);
        if (!response) {
            // remove this key as it has expired
            this->dh->DELETE(this, key);
            return "";
        }
        
        // check in which map does this key exist, simple or complex
        if (this->Type.find(key)!=this->Type.end() && this->Type[key] == NAME::SIMPLE) {
            ans = this->tc->stringify(this->simple_map[key]);
        }
        
        return ans;
        lock.unlock();
    }

    void LPUSH(string key, string value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_linked_list_insertion_check(this, key);
        if (!response) {
            return;
        }

        
        Simple_Data_Type* a = new Data<string>(value);

        this->LL_map[key]->LPUSH(a);
        this->eh->update_last_time(key);
        lock.unlock();

    }
    void LPUSH(string key, int value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_linked_list_insertion_check(this, key);
        if (!response) {
            return;
        }

        Simple_Data_Type* a = new Data<int>(value);

        this->LL_map[key]->LPUSH(a);
        this->eh->update_last_time(key);
        lock.unlock();

    }
    void LPUSH(string key, double value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_linked_list_insertion_check(this, key);
        if (!response) {
            return;
        }

        Simple_Data_Type* a = new Data<double>(value);
        
        this->LL_map[key]->LPUSH(a);
        this->eh->update_last_time(key);
        lock.unlock();

    }
    void LPUSH(string key, long long value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_linked_list_insertion_check(this, key);
        if (!response) {
            return;
        }

        Simple_Data_Type* a = new Data<long long>(value);

        this->LL_map[key]->LPUSH(a);
        this->eh->update_last_time(key);
        lock.unlock();
        
    }
    
    string LPOP(string key) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        if (!this->eh->is_valid(key)) {
            cout << "Key expired..." << endl;
            this->dh->DELETE(this, key);
            return "";
        }
        
        // this is a read operation, it should have a shared_lock

        if (this->Type.find(key)!=this->Type.end() && this->Type[key]!=NAME::LINKED_LIST) {
            cout << "Key already exists in different configuration..." << endl;
            return "";
            // after returning , lock is automatically destroyed
        }
        
        Simple_Data_Type* a = nullptr;
        if (this->LL_map.find(key)!=this->LL_map.end()) {
            a = this->LL_map[key]->LPOP();
            if (this->LL_map[key]->isEmpty()) {
                delete this->LL_map[key];
                this->Type.erase(key);
                this->LL_map.erase(key);
                this->eh->remove_ttl(key);
            }
            

        }
        string ans = this->tc->stringify(a);
        delete a;
        lock.unlock();
        return ans;

    }

    void RPUSH(string key, string value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_linked_list_insertion_check(this, key);
        if (!response) {
            return;
        }
        Simple_Data_Type* a = new Data<string>(value);

        this->LL_map[key]->RPUSH(a);
        this->eh->update_last_time(key);
        lock.unlock();
    }
    void RPUSH(string key, int value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_linked_list_insertion_check(this, key);
        if (!response) {
            return;
        }
        Simple_Data_Type* a = new Data<int>(value);

        this->LL_map[key]->RPUSH(a);
        this->eh->update_last_time(key);
        lock.unlock();
    }
    void RPUSH(string key, double value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_linked_list_insertion_check(this, key);
        if (!response) {
            return;
        }
        Simple_Data_Type* a = new Data<double>(value);

        this->LL_map[key]->RPUSH(a);
        this->eh->update_last_time(key);
        lock.unlock();
    }
    void RPUSH(string key, long long value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_linked_list_insertion_check(this, key);
        if (!response) {
            return;
        }
        Simple_Data_Type* a = new Data<long long>(value);
        
        this->LL_map[key]->RPUSH(a);
        this->eh->update_last_time(key);
        lock.unlock();
    }
    
    string RPOP(string key) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        if (!this->eh->is_valid(key)) {
            cout << "Key expired..." << endl;
            this->dh->DELETE(this, key);
            return "";
        }

        if (this->Type.find(key)!=this->Type.end() && this->Type[key]!=NAME::LINKED_LIST) {
            cout << "Key already exists in different configuration..." << endl;
            return "";
        }

        Simple_Data_Type* a = nullptr;
        if (this->LL_map.find(key)!=this->LL_map.end()) {
            a = this->LL_map[key]->RPOP();
            if (this->LL_map[key]->isEmpty()) {
                delete this->LL_map[key];
                this->Type.erase(key);
                this->LL_map.erase(key);
                this->eh->remove_ttl(key);
            }
            

        }
        string ans = this->tc->stringify(a);
        delete a;
        lock.unlock();
        return ans;

    }

    vector<string> LRANGE(string key, int start, int end) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        // firstly convert negative to positive indexing

        bool response = this->eh->is_valid(key);
        if (!response) {
            cout << "This key has expired..." << endl;
            this->dh->DELETE(this, key);
            return {};
        }
        
        if (this->Type.find(key) == this->Type.end() || this->Type[key]!=NAME::LINKED_LIST) {
            return {};
        }
        
        // after the above checks we have valid entry key
        int n = this->LL_map[key]->get_size();

        // convert negative to positive indexing here
        if (end < 0) {
            // means end is negative
            
            // now -1->n-1 th index
            // now -2->n-2 th index
            // therefore, -end = n - end th index
            end = n + end;
            
        }

        if (start < 0) {
            start = n + start;
        }
        // ==========================================
        
        if (end < start) {
            return {};
        }
        if ((start >=n) || end<0) {
            return {};
        }

        // normalize range
        end = min(n - 1, end);
        start = max(0, start);

        vector<string> ans;
        // collect answer in this range
        for (int i = start; i<=end; i++) {
            ans.push_back(this->tc->stringify(this->LL_map[key]->GET(i)));
        }

        lock.unlock();
        return ans;
    }

    void HSET(string key, string key2, string value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_nested_map_insertion_check(this, key);
        if (!response) {
            return;
        }
        Simple_Data_Type* a = new Data<string>(value);

        this->nm_map[key]->HSET(key2, a);
        this->eh->update_last_time(key);
        lock.unlock();
        
    }
    void HSET(string key, string key2, int value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_nested_map_insertion_check(this, key);
        if (!response) {
            return;
        }
        Simple_Data_Type* a = new Data<int>(value);

        this->nm_map[key]->HSET(key2, a);
        this->eh->update_last_time(key);
        lock.unlock();
        
    }
    void HSET(string key, string key2, double value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_nested_map_insertion_check(this, key);
        if (!response) {
            return;
        }
        Simple_Data_Type* a = new Data<double>(value);
        
        
        this->nm_map[key]->HSET(key2, a);
        this->eh->update_last_time(key);
        lock.unlock();
    }
    void HSET(string key, string key2, long long value) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->nm->pre_nested_map_insertion_check(this, key);
        if (!response) {
            return;
        }
        Simple_Data_Type* a = new Data<long long>(value);

        this->nm_map[key]->HSET(key2, a);
        this->eh->update_last_time(key);
        lock.unlock();
        
    }
    
    string HGET(string key, string key2) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        if (!this->eh->is_valid(key)) {
            cout << "Key expired..." << endl;
            this->dh->DELETE(this, key);
            return "";
        }

        if (this->Type.find(key)!=this->Type.end() && this->Type[key]!=NAME::NESTED_MAP) {
            cout << "key already taken in different configuration..." << endl;
            return "";
        }
        
        else if (this->Type.find(key) == this->Type.end()) {
            cout << "No such key..." << endl;
            return "";
        }


        if (this->Type[key] == NAME::NESTED_MAP) {
            // valid
            return this->tc->stringify(this->nm_map[key]->HGET(key2));
        }
        lock.unlock();
        return "";
    }
    map<string, string> HGETALL(string key) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        // check expiration
        if (!this->eh->is_valid(key)) {
            this->dh->DELETE(this, key);
            return {};
        }

        // check for already differently configured
        if (this->Type.find(key) == this->Type.end() || this->Type[key]!=NAME::NESTED_MAP) {
            return {};
        }

        // valid path
        map<string, string> ans;
        for (auto &[k, v] : this->nm_map[key]->get_map()) {
            ans[k] = this->tc->stringify(v);
        }

        lock.unlock();
        return ans;
    }
    void HDEL(string key, string key2) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        bool response = this->dh->NESTED_DELETE(this, key, key2);
        if (!response) {
            return;
        }
        this->eh->update_last_time(key);
        lock.unlock();
    }

    void DELETE(string key) {
        unique_lock<shared_mutex> lock(this->s_mtx);
        this->dh->DELETE(this, key);
        lock.unlock();
    }

    Pipeline CREATE_PIPELINE() {
        return Pipeline(this);
    }
    Transaction CREATE_TRANSACTION() {
        return Transaction(this);
    }
};

// eager initialization============
Tool* Tool::instance = new Tool();
// ================================

void Standard_Deletion_Handler::DELETE(Tool* tool, string key) {
    tool->get_expiration_handler()->remove_ttl(key);
    if (tool->get_simple_map().find(key)!=tool->get_simple_map().end()) {
        delete tool->get_simple_map()[key];
        tool->get_simple_map()[key] = nullptr;
        tool->get_simple_map().erase(key);
    } else if (tool->get_LL_map().find(key)!=tool->get_LL_map().end()) {
        delete tool->get_LL_map()[key];
        tool->get_LL_map()[key] = nullptr;
        tool->get_LL_map().erase(key);
    } else if (tool->get_nm_map().find(key)!=tool->get_nm_map().end()) {
        delete tool->get_nm_map()[key];
        tool->get_nm_map()[key] = nullptr;
        tool->get_nm_map().erase(key);
    }
    tool->get_Type().erase(key);
    
}
bool Standard_Deletion_Handler::NESTED_DELETE(Tool* tool, string key, string key2) {
    if (tool->get_Type().find(key) == tool->get_Type().end() || tool->get_Type()[key]!=NAME::NESTED_MAP) {
        return false;
    }
    if (!tool->get_expiration_handler()->is_valid(key)) {
        delete tool->get_nm_map()[key];
        tool->get_expiration_handler()->remove_ttl(key);
        tool->get_nm_map()[key] = nullptr;
        tool->get_nm_map().erase(key);
        tool->get_Type().erase(key);
        return false;
    }
    // if key is valid, delete nested field
    tool->get_nm_map()[key]->DEL(key2);
    return true;

}

bool Standard_Normalizer :: pre_normal_insertion_check(Tool* tool, string key){
    if (!tool->get_expiration_handler()->is_valid(key)) {
        tool->get_deletion_handler()->DELETE(tool, key);
        // means this key is expired according to ttl provided, so it can be initialized with new configuration
    }
    if (tool->get_Type().find(key)!=tool->get_Type().end() && tool->get_Type()[key]!=NAME::SIMPLE) {
        cout << "Key already exists with different configuration..." << endl;
        return false;
    }
    
    if (tool->get_Type().find(key) == tool->get_Type().end()){
        tool->get_Type()[key] = NAME::SIMPLE;
    }
    if (tool->get_simple_map().find(key)!=tool->get_simple_map().end()) {
        // delete previous pointer
        delete tool->get_simple_map()[key];
    }
    tool->get_simple_map()[key] = nullptr;
    return true;
}
bool Standard_Normalizer :: pre_linked_list_insertion_check(Tool* tool, string key){
    if (!tool->get_expiration_handler()->is_valid(key)) {
        tool->get_deletion_handler()->DELETE(tool, key);
        // means this key is expired according to ttl provided, so it can be initialized with new configuration
    }
    if (tool->get_Type().find(key)!=tool->get_Type().end() && tool->get_Type()[key]!=NAME::LINKED_LIST) {
        cout << "Key already exists in different configuration..." << endl;
        return false;
    }
    
    
    if (tool->get_Type().find(key)==tool->get_Type().end()) {
        tool->get_LL_map()[key] = new Linked_List();
        tool->get_Type()[key] = NAME::LINKED_LIST;
    }
    return true;
}
bool Standard_Normalizer :: pre_nested_map_insertion_check(Tool* tool, string key){
    if (!tool->get_expiration_handler()->is_valid(key)) {
        tool->get_deletion_handler()->DELETE(tool, key);
        // means this key is expired according to ttl provided, so it can be initialized with new configuration
    }
    if (tool->get_Type().find(key)!=tool->get_Type().end() && tool->get_Type()[key]!=NAME::NESTED_MAP) {
        cout << "key already taken in different configuration..." << endl;
        return false;
    }
    
    if (tool->get_Type().find(key) == tool->get_Type().end()) {
        tool->get_Type()[key] = NAME::NESTED_MAP;
        tool->get_nm_map()[key] = new Nested_Map();
    }
    return true;
}

bool Standard_Increment_Handler::INC(Tool* tool, string key) {
        // firstly check if the key is not expired
        if (!tool->get_expiration_handler()->is_valid(key)) {
            tool->get_deletion_handler()->DELETE(tool, key);
            // means this key is expired according to ttl provided,so return
            return false;
        }

        // if it is not expired, see that it should have a simple data type
        if (tool->get_Type().find(key)==tool->get_Type().end() || tool->get_Type()[key]!=NAME::SIMPLE) {
            return false;
        }


        Simple_Data_Type* a = tool->get_simple_map()[key];
        if (dynamic_cast<Data<string>*>(a)) {
            return false;
        }
        else if (auto p = dynamic_cast<Data<int>*>(a)) {
            p->SET(p->GET() + 1);
        }
        else if (auto p = dynamic_cast<Data<double>*>(a)) {
            p->SET(p->GET() + 1);
        }
        else if (auto p = dynamic_cast<Data<long long>*>(a)) {
            p->SET(p->GET() + 1);
        }
        return true;


    }

void PUT_Command::EXECUTE(Tool* tool) {
    if (auto p = dynamic_cast<Data<int>*> (this->d)) {
        tool->PUT(this->key, p->GET());
    }
    else if (auto p = dynamic_cast<Data<string>*> (this->d)) {
        tool->PUT(this->key, p->GET());
    }
    else if (auto p = dynamic_cast<Data<double>*> (this->d)) {
        tool->PUT(this->key, p->GET());
    }
    else if (auto p = dynamic_cast<Data<long long>*> (this->d)) {
        tool->PUT(this->key, p->GET());
    }
};
void LPUSH_Command::EXECUTE(Tool* tool) {
    if (auto p = dynamic_cast<Data<int>*> (this->d)) {
        tool->LPUSH(this->key, p->GET());
    }
    else if (auto p = dynamic_cast<Data<string>*> (this->d)) {
        tool->LPUSH(this->key, p->GET());
    }
    else if (auto p = dynamic_cast<Data<double>*> (this->d)) {
        tool->LPUSH(this->key, p->GET());
    }
    else if (auto p = dynamic_cast<Data<long long>*> (this->d)) {
        tool->LPUSH(this->key, p->GET());
    }
};
void RPUSH_Command::EXECUTE(Tool* tool) {
    if (auto p = dynamic_cast<Data<int>*> (this->d)) {
        tool->RPUSH(this->key, p->GET());
    }
    else if (auto p = dynamic_cast<Data<string>*> (this->d)) {
        tool->RPUSH(this->key, p->GET());
    }
    else if (auto p = dynamic_cast<Data<double>*> (this->d)) {
        tool->RPUSH(this->key, p->GET());
    }
    else if (auto p = dynamic_cast<Data<long long>*> (this->d)) {
        tool->RPUSH(this->key, p->GET());
    }
};
void HSET_Command::EXECUTE(Tool* tool) {
    if (auto p = dynamic_cast<Data<int>*> (this->d)) {
        tool->HSET(this->key, this->field, p->GET());
    }
    else if (auto p = dynamic_cast<Data<string>*> (this->d)) {
        tool->HSET(this->key, this->field, p->GET());
    }
    else if (auto p = dynamic_cast<Data<double>*> (this->d)) {
        tool->HSET(this->key, this->field, p->GET());
    }
    else if (auto p = dynamic_cast<Data<long long>*> (this->d)) {
        tool->HSET(this->key, this->field, p->GET());
    }
};
void Pipeline::EXECUTE() {
    while (!this->command_queue.empty()) {
        Pipeline_Command* c = this->command_queue.front();
        this->command_queue.pop();
        c->EXECUTE(this->tool);
        delete c;
    }
};

// IDEA: to ensure atomicity for all queues operations together these transaction commands use the tools items instead of tool's functionalities
void PUT_Transaction_Command::EXECUTE(Tool* tool) {
    bool response = tool->get_normalizer()->pre_normal_insertion_check(tool, this->key);
    if (!response) {
        return;
    }
    if (auto p = dynamic_cast<Data<int>*> (this->d)) {
        tool->get_simple_map()[key] = new Data<int> (p->GET());
    }
    else if (auto p = dynamic_cast<Data<string>*> (this->d)) {
        tool->get_simple_map()[key] = new Data<string> (p->GET());
    }
    else if (auto p = dynamic_cast<Data<double>*> (this->d)) {
        tool->get_simple_map()[key] = new Data<double> (p->GET());
    }
    else if (auto p = dynamic_cast<Data<long long>*> (this->d)) {
        tool->get_simple_map()[key] = new Data<long long> (p->GET());
    }
    tool->get_expiration_handler()->update_last_time(key);

};
void LPUSH_Transaction_Command::EXECUTE(Tool* tool) {
    bool response = tool->get_normalizer()->pre_linked_list_insertion_check(tool, key);
    if (!response) {
        return;
    }
    if (auto p = dynamic_cast<Data<int>*> (this->d)) {
        Simple_Data_Type* a = new Data<int>(p->GET());
        tool->get_LL_map()[key]->LPUSH(a);
    }
    else if (auto p = dynamic_cast<Data<string>*> (this->d)) {
        Simple_Data_Type* a = new Data<string>(p->GET());
        tool->get_LL_map()[key]->LPUSH(a);
    }
    else if (auto p = dynamic_cast<Data<double>*> (this->d)) {
        Simple_Data_Type* a = new Data<double>(p->GET());
        tool->get_LL_map()[key]->LPUSH(a);
    }
    else if (auto p = dynamic_cast<Data<long long>*> (this->d)) {
        Simple_Data_Type* a = new Data<long long>(p->GET());
        tool->get_LL_map()[key]->LPUSH(a);
    }
    tool->get_expiration_handler()->update_last_time(key);
};
void RPUSH_Transaction_Command::EXECUTE(Tool* tool) {
    bool response = tool->get_normalizer()->pre_linked_list_insertion_check(tool, key);
    if (!response) {
        return;
    }
    if (auto p = dynamic_cast<Data<int>*> (this->d)) {
        Simple_Data_Type* a = new Data<int>(p->GET());
        tool->get_LL_map()[key]->RPUSH(a);
    }
    else if (auto p = dynamic_cast<Data<string>*> (this->d)) {
        Simple_Data_Type* a = new Data<string>(p->GET());
        tool->get_LL_map()[key]->RPUSH(a);
    }
    else if (auto p = dynamic_cast<Data<double>*> (this->d)) {
        Simple_Data_Type* a = new Data<double>(p->GET());
        tool->get_LL_map()[key]->RPUSH(a);
    }
    else if (auto p = dynamic_cast<Data<long long>*> (this->d)) {
        Simple_Data_Type* a = new Data<long long>(p->GET());
        tool->get_LL_map()[key]->RPUSH(a);
    }
    tool->get_expiration_handler()->update_last_time(key);
};
void HSET_Transaction_Command::EXECUTE(Tool* tool) {
    bool response = tool->get_normalizer()->pre_nested_map_insertion_check(tool, this->key);
    if (!response) {
        return;
    }
    if (auto p = dynamic_cast<Data<int>*> (this->d)) {
        Simple_Data_Type* a = new Data<int>(p->GET());

        tool->get_nm_map()[key]->HSET(this->field, a);
    }
    else if (auto p = dynamic_cast<Data<string>*> (this->d)) {
        Simple_Data_Type* a = new Data<string>(p->GET());

        tool->get_nm_map()[key]->HSET(this->field, a);
    }
    else if (auto p = dynamic_cast<Data<double>*> (this->d)) {
        Simple_Data_Type* a = new Data<double>(p->GET());

        tool->get_nm_map()[key]->HSET(this->field, a);
    }
    else if (auto p = dynamic_cast<Data<long long>*> (this->d)) {
        Simple_Data_Type* a = new Data<long long>(p->GET());

        tool->get_nm_map()[key]->HSET(this->field, a);
    }
    tool->get_expiration_handler()->update_last_time(key);
};
void Transaction::DISCARD () {
    while (!this->command_queue.empty()) {
        Transaction_Command* c = this->command_queue.front();
        this->command_queue.pop();
        delete c;
    }
};
void Transaction::EXEC() {
    // this first aquires tool's shared mutex and puts a unique lock on it
    unique_lock<shared_mutex> lock(tool->get_shared_mutex());
    // within this critical section all queued operations are run and hence they act as one atomic operation
    while (!this->command_queue.empty()) {
        Transaction_Command* c = this->command_queue.front();
        this->command_queue.pop();
        c->EXECUTE(this->tool);
        delete c;
    }
    lock.unlock();
}

int main() {
    return 0;
}