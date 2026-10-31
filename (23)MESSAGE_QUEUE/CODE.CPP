#include <bits/stdc++.h>
using namespace std;
// partitions have queues which maintain order within ,
// messages will be stored in queue within partitions
class Message {
private:
    string message_id;
    // all the data needed for consumer to process
public:
    Message(string message_id) {
        this->message_id = message_id;
    }
    ~Message() {}

    string& get_message_id() {
        return this->message_id;
    }

};

class Consumer;
class Partition;
// now to decouple the connection between associated partition-consumer pairs
// we will maintain a mediator kinda thing
class Mediator {
private:
    map<int, string> partition_to_consumer_map;
public:
    Mediator () {}
    ~Mediator () {}
    void add_pair(int partition_number, string consumer_id);
    bool notify(int partition_number, Message* m);
};

class Consumer {
private:
    string consumer_id;
public:
    Consumer (string consumer_id) {
        this->consumer_id = consumer_id;
    }
    ~Consumer() {

    }
    string get_consumer_id() {
        return this->consumer_id;
    }

    bool receive_message(Message* m) {
        // furthur processing her

        // returns ack true is message is processed successfully
        return true;
    }


};

// SINGLE POINT OF GETTING ACCESS TO CONSUMERS
class Consumer_Distributor {
private:
    static Consumer_Distributor* instance;
    map<string, Consumer*> mp;
    Consumer_Distributor() {};
public:
    ~Consumer_Distributor() {
        for (auto&[k,v] : this->mp) {
            delete v;
        }
    }
    static Consumer_Distributor* get_instance() {
        return Consumer_Distributor::instance;
    }
    void add_consumer(Consumer* c) {
        this->mp[c->get_consumer_id()] = c;
    }

    Consumer* get_consumer(string consumer_id) {
        if (this->mp.find(consumer_id) == this->mp.end()) {
            return nullptr;
        }
        return this->mp[consumer_id];
    }
};
Consumer_Distributor* Consumer_Distributor::instance = new Consumer_Distributor();

// DELIVERY SEMANTICS CLASS
class IDelivery_Semantics {
public:
    virtual bool handle_message_delivery(int partition_number, Message* m, Mediator* mediator) = 0; 
};

class At_Most_Once_Delivery : public IDelivery_Semantics {
public:
    At_Most_Once_Delivery() {}
    ~At_Most_Once_Delivery() {}

    bool handle_message_delivery(int partition_number, Message* m, Mediator* mediator);
        // handling here, it only delivers once, doesnt care about the ack
        // prevents dupes
        // message loss may occur
        // useful when once in a while message loss is sufferable but dupes can cause serious problems
};

class Dead_Letter_Queue {
private:
    queue<Message*> poison_messages;
    static Dead_Letter_Queue* instance;
    Dead_Letter_Queue() {}
public:
    static Dead_Letter_Queue* get_instance() {
        return Dead_Letter_Queue::instance;
    }
    ~Dead_Letter_Queue() {
        while (!this->poison_messages.empty()) {
            Message* m = this->poison_messages.front();
            delete m;
            this->poison_messages.pop();
        }
  
    }

    void submit_message(Message* m) {
        this->poison_messages.push(m);
    }
};
Dead_Letter_Queue* Dead_Letter_Queue::instance = new Dead_Letter_Queue();

// rules for retry, retry mechanism
// keeping scalable for future ease
class IRetry_Mechanism {
public:
    virtual bool deliver(int partition_number, Message* m, Mediator* mediator) = 0;
};

class Exponential_Backoff : public IRetry_Mechanism{
private:
    long long number;
    long long power_limit;
    vector<long long> power_value;
    // this will be last wait time, after this message will enter dead letter queue
public:
    Exponential_Backoff(long long base, long long exponent_limit) {
        this->number = base;
        this->power_limit = exponent_limit;
        // precompute power value
        this->power_value.resize(exponent_limit + 1);
        this->power_value[0] = 1;
        for (int i = 1; i<=this->power_limit; i++) {
            this->power_value[i] = this->power_value[i - 1] * this->number;
        }
    }
    ~Exponential_Backoff() {}

    bool deliver(int partition_number, Message* m, Mediator* mediator) override;
};

class At_Least_Once_Delivery : public IDelivery_Semantics {
private:
    IRetry_Mechanism* mech;
public:
    At_Least_Once_Delivery(IRetry_Mechanism* mechanism) {
        this->mech = mechanism;
    }
    ~At_Least_Once_Delivery() {
        delete this->mech;
    }
    bool handle_message_delivery(int partition_number, Message* m, Mediator* mediator) override;
    // once ack is required, then reqtry stops
    // dupes possible
    // typical algo used inside : exponential backoff
    // but we will keep algorithms scalable for future ease
};

class Exactly_Once_Delivery : public IDelivery_Semantics {
public:
    Exactly_Once_Delivery() {}
    ~Exactly_Once_Delivery() {}
    bool handle_message_delivery(int partition_number, Message* m, Mediator* mediator) override;
    // now this is real software engineer level shiii
    // as this is not easy to implement
};

// one thread per partition
// single thread prevents any race condition
class Partition {
private:
    int partition_number = -1;
    queue<Message*> broker;
    Mediator* mediator;
    thread t1;
    bool time_to_go = false;
    IDelivery_Semantics* rules;
    condition_variable cv;
    mutex mtx;
    bool check() {
        return (this->time_to_go || !this->broker.empty());
    }
    void CONSUME() {
        while (true) {
            unique_lock<mutex> lock(this->mtx);

            cv.wait(lock, [this] {
                return this->check();
            });

            if (this->time_to_go) {
                return;
            }
            
            // else there is a task to be consumed
            Message* m = this->broker.front();
            this->broker.pop();

            lock.unlock();
            
            // NOW HERE THE MESSAGE SEMANTICS CHOSEN WILL DECIDE THE FURTHUR ACTION, HOW THE RETRY MECHANISM WILL WORK
            bool isDelivered = rules->handle_message_delivery(this->partition_number, m, this->mediator);
            if (!isDelivered) {
                m = nullptr;
            }
            else {
                delete m;
            }
        }
    }
    Partition () {
        this->rules = nullptr;
        this->mediator = nullptr;
        this->t1 = thread(&Partition::CONSUME, this);
    }
    friend class Partition_Builder;
public:

    ~Partition() {
        unique_lock<mutex> lock(this->mtx);
        this->time_to_go = true;
        cv.notify_one();
        lock.unlock();

        if (this->t1.joinable()) {
            this->t1.join();
        }
        while (!this->broker.empty()) {
            Message* message = this->broker.front();
            delete message;
            this->broker.pop();
        }
        this->mediator = nullptr;
        delete this->rules;
    }

    // adder
    void receive_message(Message* message) {
        unique_lock<mutex> lock(this->mtx);
        this->broker.push(message);
        lock.unlock();

        cv.notify_one();
    }
// now partition will need a thread and a common variable to complete its functionality with mediator    
};

bool Exponential_Backoff::deliver(int partition_number, Message* m, Mediator* mediator) {
    int power = 0;
    while (power<=this->power_limit) {
        bool response = mediator->notify(partition_number, m);
        if (response) {
            // successful delivery
            return true;
        }

        // wait for this->base^power time
        // use sleep of window
        // Retrieve precomputed backoff duration and sleep current thread
        long long wait_time_ms = this->power_value[power] * 1000; // Converting base unit to milliseconds
        this_thread::sleep_for(chrono::milliseconds(wait_time_ms));
        power++;
        
    }
    // after this if function did not return then move to dead letter queue for investigation as this might be a poison message
    Dead_Letter_Queue::get_instance()->submit_message(m);
    return false;
};

bool At_Most_Once_Delivery::handle_message_delivery(int partition_number, Message* m, Mediator* mediator) {
    mediator->notify(partition_number, m);
    return true;
};
bool At_Least_Once_Delivery::handle_message_delivery(int partition_number, Message* m, Mediator* mediator) {
    if (this->mech != nullptr) {
        return this->mech->deliver(partition_number, m, mediator);
    }
    return false;
}


class Partition_Builder {
private:
    Partition* partition;
public:
    Partition_Builder() {
        this->partition = new Partition();
    }
    ~Partition_Builder() {
        this->partition = nullptr;
    }

    Partition_Builder& add_partition_number(int partition_number) {
        this->partition->partition_number = partition_number;
        return *this;
    }

    Partition_Builder& add_mediator(Mediator* mediator) {
        this->partition->mediator = mediator;
        return *this;
    }

    Partition_Builder& set_delivery_semantics(IDelivery_Semantics* semantics) {
        this->partition->rules = semantics;
        return *this;
    }

    Partition* build() {
        if (this->partition->partition_number == -1) {
            return nullptr;
        }
        if (this->partition->mediator == nullptr) {
            return nullptr;
        }
        if (this->partition->rules == nullptr) {
            return nullptr;
        }
        return this->partition;
    }


};


void Mediator::add_pair(int partition_number, string consumer_id) {
    this->partition_to_consumer_map[partition_number] = consumer_id;
}
bool Mediator::notify(int partition_number, Message* m) {
    Consumer* consumer = Consumer_Distributor::get_instance()->get_consumer(this->partition_to_consumer_map[partition_number]);
    if (consumer != nullptr) {
        return consumer->receive_message(m);
    }
    return false;
}
// keeeping the hash function extendible so that new hash functions in future can be integrated seemlessly
class IHasher {
public:
    virtual long long hash(string& message_id) = 0;
};
class Rolling_Hash : public IHasher {
private:
    long long mod;
    long long p;
public:
    Rolling_Hash(long long mod = 1e9, long long p = 31) {
        this->mod = mod;
        this->p = p;
    }
    ~Rolling_Hash() {}

    long long hash(string& message_id) override {
        long long ans = 0;
        for (int i = 0; i<message_id.size(); i++) {
            ans+=((message_id[i] - 'a' + 1) + (this->p * ans) % this->mod) % this->mod;
        }
        return ans;
    }
};

class Partition_Handler {
private:
    IHasher* hash_function;
    map<int, Partition*> mp;
public:
    Partition_Handler(IHasher* hash_function) {
        this->hash_function = hash_function;
    }
    ~Partition_Handler() {
        for (auto&[k,v] : this->mp) {
            delete v;
        }
        delete this->hash_function;
    }
    void add_partition(Mediator* m) {
        // this will create a new partition
        int current_number_of_partitions = mp.size();
        Partition* p = Partition_Builder()
                       .add_partition_number(current_number_of_partitions)
                       .add_mediator(m)
                       .build();
        mp[current_number_of_partitions] = p;
    }
    
    void add_message(Message* m) {
        // now here a hash function will mod n, fill decide where the message will go, and hash function will be on message id
        long long hash_value = this->hash_function->hash(m->get_message_id());
        long long current_number_of_partitions = mp.size();
        int partition_number = (int)(hash_value % current_number_of_partitions);
        this->mp[partition_number]->receive_message(m);
    }

};

class Topic {
private:
    // we know that topic_name is already unique
    // BUT for maintaing standards an id is also issued
    string topic_id;
    string topic_name;
    Partition_Handler* partition_handler;
    Mediator* m;
public:
    Topic(string topic_id, string topic_name, IHasher* hash_function) {
        this->topic_id = topic_id;
        this->topic_name = topic_name;
        this->m = new Mediator();
        this->partition_handler = new Partition_Handler(hash_function);
    }

    ~Topic() {
        delete m;
        delete this->partition_handler;
    }

    string get_topic_name() {
        return this->topic_name;
    }


    void add_partition() {
        this->partition_handler->add_partition(this->m);
    }
    string get_topic_id() {
        return this->topic_id;
    }

    void receive_message(Message* m) {
        this->partition_handler->add_message(m);
    }
};

// SINGLE POINT OF TALKING WITH TOPICS
class Topic_Distributor {
private:
    map<string, Topic*> mp;
    static Topic_Distributor* instance;
    Topic_Distributor() {}
public:
    ~Topic_Distributor() {
        for (auto& [k,v] : this->mp) {
            delete v;
        }
    }

    static Topic_Distributor* get_instance() {
        return Topic_Distributor::instance;
    }

    Topic* get_topic(string topic_name) {
        if (this->mp.find(topic_name) == this->mp.end()) {
            return nullptr;
        }
        return this->mp[topic_name];
    }

    void add_topic(Topic* topic) {
        this->mp[topic->get_topic_name()] = topic;
    }
};
Topic_Distributor* Topic_Distributor::instance = new Topic_Distributor();
class Producer {
private:
    string producer_id;
public:
    Producer(string producer_id) {
        this->producer_id = producer_id;
    }
    ~Producer() {

    }

    void sned_message(string topic_name, Message* m) {
        Topic* t = Topic_Distributor::get_instance()->get_topic(topic_name);
        if (t!=nullptr) {

            t->receive_message(m);
        }
    }


};
int main() {
    return 0;
}