#include <bits/stdc++.h>
using namespace std;




// ===================================================
class User {
private:
    string name;
    // name is unique key
    string mobile;
public:
    User (string name, string mobile) {
        this->name = name;
        this->mobile = mobile;
        cout << "User " << name << " created successfully..." << endl;
    }
    ~User() {};
    // getters
    const string& get_name () {
        return this->name;
    }

    const string& get_mobile () {
        return this->mobile;
    }
};

class UserMap {
private:
    map<string, User*> mp;
public:
    UserMap () {};
    ~UserMap() {};
    // add 
    void add_user(User* u) {
        if (this->mp.find(u->get_name())!=this->mp.end()) {
            // user already exists
            cout << "user already fitted..." << endl;
            return;
        }
        // not exist case
        this->mp[u->get_name()] = u;
        cout << "User added..." << endl;
    }

    // remove
    void remove_user(User* u) {
        if (this->mp.find(u->get_name())==this->mp.end()) {
            // user not exists
            cout << "user does not exist..." << endl;
            return;
        }
        // exist case
        this->mp.erase(u->get_name());
        cout << "User removed..." << endl;
    }


    User* get_user(string name) {
        if (this->mp.find(name)!=this->mp.end()) {
            return this->mp[name];
        }
        cout << "User does not exist..." << endl;
        return nullptr;

    }
};

struct Transaction {
    int id;
    string sender;
    string receiver;
    double amount;
    Transaction () {};
};

class GreedySetteler {
public:
    static vector<Transaction> resolve(map<string, double> net_balance) {
        priority_queue<pair<double, string>> creditors;
        priority_queue<pair<double, string>> debitors;

        for (auto& it : net_balance) {
            if (it.second > 1e-6) {
                creditors.push({it.second, it.first});
            } else if (it.second < -1e-6) {
                debitors.push({-it.second, it.first});
            }
        }
        vector<Transaction> debts;
        while (!creditors.empty() && !debitors.empty()) {
            pair<double, string> max_creditor = creditors.top();
            pair<double, string> max_debitor = debitors.top();
            creditors.pop();
            debitors.pop();

            double settle_amount = min(max_creditor.first, max_debitor.first);
            
            Transaction t;
            t.sender = max_debitor.second;
            t.receiver = max_creditor.second;
            t.amount = settle_amount;
            t.id = debts.size();

            debts.push_back(t);

            double remaining_credit = max_creditor.first - settle_amount;
            if (remaining_credit > 1e-6) {
                creditors.push({remaining_credit, max_creditor.second});
            }
            double remaining_debit = max_debitor.first - settle_amount;
            if (remaining_debit > 1e-6) {
                debitors.push({remaining_debit, max_debitor.second});
            }

        }
        return debts;
    }
};

class SplitStrategy {
protected:
    map<string, double> mp; 
    double total_amount;
    map<User*, double> payers;
    vector<User*> contributors;
public:
    SplitStrategy(double amount, map<User*, double> payers, vector<User*> contributors) {
        this->total_amount = amount;
        this->payers = payers;
        this->contributors = contributors;
    }
    virtual ~SplitStrategy() = default;
    map<string, double>& get_split() {
        return this->mp;
    }
};

class EqualSplit : public SplitStrategy {
public:
    EqualSplit(double amount, map<User*, double> payers, vector<User*> contributors) : SplitStrategy(amount, payers, contributors) {

        // because in equal split, only payers are needed to equally split
        
        double per_head = this->total_amount / (double) (this->contributors.size());

        for (auto& it : this->contributors) {

            this->mp[it->get_name()] = -per_head;
        }

        for (auto& it : this->payers) {
            this->mp[it.first->get_name()]+=it.second;
        }

    }
};

class ExactSplit : public SplitStrategy { 
public:
    ExactSplit (double amount, map<User*, double> payers, vector<User*> contributors, map<User*, double> due) : SplitStrategy(amount, payers, contributors) {
        
        for (auto& it : due) {

            this->mp[it.first->get_name()]-=it.second;
        }

        for (auto& it : this->payers) {
            this->mp[it.first->get_name()]+=it.second;
        }



    }
};

class SharedBasedSplit : public SplitStrategy {
public:
    SharedBasedSplit (double amount, map<User*, double> payers, vector<User*> contributors, map<User*, double> percentage) : SplitStrategy(amount, payers, contributors) {
        for (auto& it : percentage) {
            this->mp[it.first->get_name()] = -it.second * 0.01 * this->total_amount;
        }

        // remove payers contri
        for (auto& it : this->payers) {
            this->mp[it.first->get_name()] += it.second;
        }
    }
};

// ===================================================












// ===================================================

// I AM DEFINING A EVENT (A GOA TRIP, A PARTY, A HOTEL DINNER)
// A GROUP WILL CONTAIN ALL EVENTS RELATED TO THIS GROUP

class Event {
private:
    int event_id;
    string event_name;
    string date;
    SplitStrategy* method;
public:
    Event (string event_name, int id, string date, SplitStrategy* method) {
        this->event_name = event_name;
        this->event_id = id;
        this->date = date;
        this->method = method;
    }

    ~Event() {
        delete this->method;
    }

    string get_name () {
        return this->event_name;
    }

    map<string, double>& get_split() {
        return this->method->get_split();
    }

    int get_eventid () {
        return this->event_id;
    }
};

// ===================================================











// ===================================================

// IMPORTANT : netbalances is the ultimate source of truth , grredy settlers vector of transactions is just a suggested view

class Group {
private:
    int group_id; //unique
    string group_name;
    unordered_set<string> member_names;
    vector<User*> members;
    map<string, double> net_balances;
    vector<Event*> events;
    vector<Transaction> global_debts;
    bool lazy = false;

public:
    Group (int id, string name) {
        this->group_id = id;
        this->group_name = name;
        cout << name << " group created successfully..." << endl;
    }

    ~Group () {
        for (Event* e : events) {
            delete e;
        }
    };

    int get_id () {
        return this->group_id;
    }

    string get_group_name () {
        return this->group_name;
    }

    vector<User*>& get_members () {
        return this->members;
    }

    void add_member(User* u) {
        if (this->member_names.count(u->get_name())) return;
        this->members.push_back(u);
        this->member_names.insert(u->get_name());
        this->net_balances[u->get_name()] = 0.0;
        cout << u->get_name() << " successfully added to " << this->group_name <<"..."<< endl;
    }

    bool is_member(const string& user_name) {
        return this->member_names.count(user_name) > 0;
    }

    void add_event(Event* event) {
        this->events.push_back(event);
        // update global debts and map
        this->lazy = true;

        for (auto& it : event->get_split()) {
            this->net_balances[it.first]+=it.second;
        }
    }

    vector<Transaction>& get_simplified_debts() {
        if (this->lazy) {
            this->global_debts = GreedySetteler::resolve(this->net_balances);
            this->lazy = false;
        }
        return this->global_debts;
    }

    // pay
    void pay(User* sender, User* receiver, double amount) {
        if (amount < 0.0) {
            cout << "Amount must be positive..." << endl;
            return;
        }
        if (this->member_names.count(receiver->get_name())==0 || this->member_names.count(sender->get_name())==0) {
            cout << "Please make a valid transaction..." << endl;
            return;
        }

        this->net_balances[sender->get_name()]+=amount;
        this->net_balances[receiver->get_name()]-=amount;
        this->lazy = true;
        cout << "Payment of $" << amount << " from " << sender->get_name() 
         << " to " << receiver->get_name() << " recorded successfully." << endl;
    }

    bool settled (User* u) {
        if (this->net_balances.find(u->get_name()) == this->net_balances.end()) {
            cout << "User not in transaction..." << endl;
            return true;
        }

        return this->net_balances[u->get_name()] < 1e-6 && this->net_balances[u->get_name()] > -1e-6;
    }

    void leave (User* u) {
        if (this->member_names.count(u->get_name()) == 0) {
            cout << "Member not in group..." << endl;
            return;
        }

        if (!this->settled(u)) {
            // he/she can not leave
            cout << "User cannot leave as debt is due..." << endl;
            return;
        }

        this->member_names.erase(u->get_name());
        this->net_balances.erase(u->get_name());
        int target_index = -1;
        int k = 0;
        for (auto& it : this->members) {
            if (it->get_name() == u->get_name()) {
                target_index = k;
                break;
            }
            k++;
        }
        if (target_index == -1) {
            return;
        }
        this->members.erase(this->members.begin() + target_index);
        cout << "User left successfully..." << endl;
    }
};

class GroupMap {
private:
    map<int, Group*> mp;
public:
    GroupMap () {};
    ~GroupMap() {
        for (auto& pair : mp) {
            delete pair.second;
        }
    };
    // add 
    void add_group(Group* g) {
        if (this->mp.find(g->get_id())!=this->mp.end()) {
            // group already exists
            cout << "group already fitted..." << endl;
            return;
        }
        // not exist case
        this->mp[g->get_id()] = g;
        cout << "Group added..." << endl;
    }

    // remove
    void remove_group(Group* g) {
        if (this->mp.find(g->get_id())==this->mp.end()) {
            // group not exists
            cout << "group does not exist..." << endl;
            return;
        }
        // exist case
        this->mp.erase(g->get_id());
        cout << "Group removed..." << endl;
    }


    Group* get_group(int id) {
        if (this->mp.find(id)!=this->mp.end()) {
            return this->mp[id];
        }
        cout << "Group does not exist..." << endl;
        return nullptr;

    }
};
// ===================================================











// Notification System ===============================
class Observable;
class Observer {
protected:
    int id;
    Observable* obs;
public:    
    Observer (int id) {
        this->id = id;
    }

    ~Observer () = default;
    void set_master (Observable* o) {
        this->obs = o;
    }

    int get_id () {
        return this->id;
    }

    virtual void update(User*u, string message) = 0;
};

class Notification_Payload {
protected:
    User* target;
public:
    Notification_Payload (User* u) {
        this->target = u;
    }
    User* get_target () {
        return this->target;
    }
    virtual string get_formatted_message() = 0;
};

class New_Expense_Payload : public Notification_Payload {
private:
    Group* g;
public:
    New_Expense_Payload (User* u, Group* g) : Notification_Payload(u) {
        this->g = g;
    }

    string get_formatted_message () {
        string result = "A new expense was added to your group " + this->g->get_group_name();
        return result;
    }
};

class Expense_Settled_Payload : public Notification_Payload {
private:
    Group* g;
public:
    Expense_Settled_Payload (User* u, Group* g) : Notification_Payload(u) {
        this->g = g;
    }

    string get_formatted_message () {
        string result = "Congrats " + this->target->get_name() + ", you are now settled for the group " + g->get_group_name() + "...";
        return result;
    }
};

class Observable {
protected:
    vector<Observer*> observers;
public:
    void add_observer(Observer* o) {
        o->set_master(this);
        this->observers.push_back(o);

    }

    void remove_observer(Observer* o) {
        int target_index = -1;
        for (int i = 0; i<this->observers.size(); i++) {
            Observer* ob = this->observers[i];
            if (o->get_id() == ob->get_id()) {
                target_index = i;
                break;
            }
        }
        if (target_index == -1) {
            cout <<"No such observer exists..." << endl;
            return;
        }

        this->observers.erase(this->observers.begin() + target_index);
        cout << "Observer removed successfully..." << endl;
    }

    virtual void notify(Notification_Payload* p) = 0;

    ~Observable() = default;
};

class New_Expense_Notification : public Observable {
public:
    void notify (Notification_Payload* p) {
        for (Observer* o : this->observers) {
            o->update(p->get_target(), p->get_formatted_message());
        }
        cout << p->get_target()->get_name() << " was notified about the new expense successfully..." << endl;
    }
};

class Expense_Settled_Notification : public Observable {
public:
    void notify (Notification_Payload* p) {
        for (Observer* o : this->observers) {
            o->update(p->get_target(), p->get_formatted_message());
        }
        cout << p->get_target()->get_name() << " was notified about the settled expense successfully..." << endl;
    }
};

class Push_Notification : public Observer {
public:
    Push_Notification(int id) : Observer(id) {};

    void update(User* target, string message) {
        cout << "Push notification sent..." << endl;
    }
};

class Slack_Notification : public Observer {
public:
    Slack_Notification(int id) : Observer(id) {};

    void update(User* target, string message) {
        cout << "Slack notification sent..." << endl;
    }
};

class Email_Notification : public Observer {
public:
    Email_Notification(int id) : Observer(id) {};

    void update(User* target, string message) {
        cout << "Email sent..." << endl;
    }
};

enum class type {
    NEW_EXPENSE_NOTIFICATION,
    EXPENSE_SETTLED_NOTIFICATION
};

enum class Observer_type {
    PUSH_NOTIFICATION,
    EMAIL_NOTIFICATION,
    SLACK_NOTIFICATION
};

class Observer_Map {
private:
    map<Observer_type, Observer*> mp;
public:
    Observer_Map() {
        this->mp[Observer_type::PUSH_NOTIFICATION] = new Push_Notification(1);
        this->mp[Observer_type::EMAIL_NOTIFICATION] = new Email_Notification(2);
        this->mp[Observer_type::SLACK_NOTIFICATION] = new Slack_Notification(3);
    };
    ~Observer_Map() {
        for (auto& pair : mp) {
            delete pair.second;
        }
    };

    Observer* get_observer(Observer_type obt) {
        if (this->mp.find(obt)!=this->mp.end()) {
            return this->mp[obt];
        }
        cout << "Such type of observer is not supported yet.." << endl;
        return nullptr;
    }
};

// this is used by facade to send notification
class Notification_Manager {
private:

    New_Expense_Notification* nen;
    Expense_Settled_Notification* esn;
    Observer_Map* om;

    
public:
    Notification_Manager () {
        this->nen = new New_Expense_Notification();
        this->esn = new Expense_Settled_Notification();
        this->om = new Observer_Map();
    }
    ~Notification_Manager() {
        delete this->nen;
        delete this->esn;
        delete this->om; 
    }
    // send new expense notification function

    void send (type notification_type, Notification_Payload* p) {
        if (notification_type == type::NEW_EXPENSE_NOTIFICATION) {
            this->nen->notify(p);
        } else if (notification_type == type::EXPENSE_SETTLED_NOTIFICATION) {
            this->esn->notify(p);
        } else {
            cout << "this type of notification service is not supported yet...." << endl;
        }
    }

    void add_observer(type notification_type, Observer_type ot) {
        Observer* o = this->om->get_observer(ot);
        if (o == nullptr) {
            return;
        }
        if (notification_type == type::NEW_EXPENSE_NOTIFICATION) {
            this->nen->add_observer(o);
        } else if (notification_type == type::EXPENSE_SETTLED_NOTIFICATION) {
            this->esn->add_observer(o);
        } else {
            cout << "this type of notification service is not supported yet...." << endl;
        }
    }
};



// ===================================================














class Facade {
private:
    GroupMap* gm;
    UserMap* um;
    Notification_Manager* nm;
public:
    Facade () {
        this->um = new UserMap();
        this->gm = new GroupMap();
        this->nm = new Notification_Manager();
    };

    ~Facade () {
        delete this->um;
        delete this->gm;
        delete this->nm;
    };

    void add_user(User* u) {
        this->um->add_user(u);
    }
    void remove_user(User* u) {
        this->um->remove_user(u);
    }

    void add_group(Group* g) {
        this->gm->add_group(g);
    }

    void remove_group(Group* g) {
        this->gm->remove_group(g);
    }

    vector<Transaction>& get_simplified_debts(Group* g) {
        return this->gm->get_group(g->get_id())->get_simplified_debts();
    }

    void add_event(Group* g, Event* e) {
        this->gm->get_group(g->get_id())->add_event(e);
        // loop over all group participants to send them new wvent addition messager
        for (User* u : g->get_members()) {
            // create a notification payload
            Notification_Payload* payload = new New_Expense_Payload(u, g);

            this->nm->send(type::NEW_EXPENSE_NOTIFICATION, payload);

            // delete reference
            delete payload;
        }
    }

    void pay(User* sender, User* receiver, double amount, Group* g) {
        // pay
        this->gm->get_group(g->get_id())->pay(sender, receiver, amount);
        

        // check if debt is settled in this group of either user, send notification to that settlted user/users
        if (this->gm->get_group(g->get_id())->settled(sender)) {
            cout << sender->get_name() <<" is now settled for this group and is now free to leave group " << g->get_group_name() << endl;
            Notification_Payload* payload = new Expense_Settled_Payload(sender, this->gm->get_group(g->get_id()));
            this->nm->send(type::EXPENSE_SETTLED_NOTIFICATION, payload);
            delete payload;
        }

        if (this->gm->get_group(g->get_id())->settled(receiver)) {
            cout << receiver->get_name() <<" is now settled for this group and is now free to leave group " << g->get_group_name() << endl;
            Notification_Payload* payload = new Expense_Settled_Payload(receiver, this->gm->get_group(g->get_id()));
            this->nm->send(type::EXPENSE_SETTLED_NOTIFICATION, payload);
            delete payload;
        }
    }

    void add_observer(Observer_type ot, type t) {
        this->nm->add_observer(t, ot);
    }

    void leave(User* u, Group* g) {
        this->gm->get_group(g->get_id())->leave(u);
    }
};


// ===================================================

// ===================================================
int main() {
    return 0;
}