#include <bits/stdc++.h>
using namespace std;


// =======  MODEL CLASSES ==============
// contains location and room allocated
class Room {
private:
    int white_board_cnt;
    int projector_cnt;
    int maximum_capacity;
    int room_id;
public:
    Room(int wbc, int pc, int mc, int room_id) {
        this->white_board_cnt = wbc;
        this->projector_cnt = pc;
        this->maximum_capacity = mc;
        this->room_id = room_id;
    }
    // getters
    int get_white_board_count() const {
        return this->white_board_cnt;
    }
    int get_projector_count() const {
        return this->projector_cnt;
    }
    int get_capacity() const {
        return this->maximum_capacity;
    }
    bool operator<(const Room& other) const {
        return this->room_id < other.room_id;
    }
    int get_room_id() const {
        return this->room_id;
    }

};

class Venue {
private:
    string location;
    Room* room_allocated;
public:
    Venue(string location, Room* rm) {
        this->location = location;
        this->room_allocated = rm;
    }
    // getters
    string get_location () const {
        return this->location;
    }

    Room* get_room() const {
        return this->room_allocated;
    }
};

class Member {
private:
    string name;
    string mobile;
    string email;
public:
    Member(string name, string mobile, string email) {
        this->name = name;
        this->mobile = mobile;
        this->email = email;
    }
    // getters
    string get_name() const {
        return this->name;
    }
    string get_mobile() const {
        return this->mobile;
    }
    string get_email() const {
        return this->email;
    }
};

enum class Event_Type {
    DAILY,
    RECURRING,
    SPECIFIC_DAYS,
    NOT_RECURRENT
};

class User_Schedule_Map;

class Event_Rules {
protected:
    string start_time_on_that_day;
    string end_time_on_that_day;
public:
    virtual ~Event_Rules() {};
    virtual bool add_event(User_Schedule_Map* m) = 0;
    string get_start_time() const {
        return this->start_time_on_that_day;
    }
    string get_end_time() const {
        return this->end_time_on_that_day;
    }
};

class Recurrent_Event : public Event_Rules {
private:
    string first_start_date;
    int day_interval_until_event_occurs_again;
    string last_date;
public:
    Recurrent_Event (string fsd, int d, string ld) {
        this->last_date = ld;
        this->first_start_date = fsd;
        this->day_interval_until_event_occurs_again = d;
    }

    bool add_event(User_Schedule_Map* m);

};

class Non_Recurring_Event : public Event_Rules {
private:
    string date;
public:
    Non_Recurring_Event(string date) {
        this->date = date;
    }

    bool add_event(User_Schedule_Map* m);

};




class Event {
private:
    string title;
    string event_id;
    string description;
    Event_Rules* r;
    Venue* venue;
    vector<Member> members;
    Event_Type type_of_event;

public:
    Event(string event_id, string title, string description, Venue* venue, Event_Type type, vector<Member> member, Event_Rules* r) {
        this->title = title;
        this->event_id = event_id;
        this->description = description;
        this->members = member;
        this->venue = venue;
        this->type_of_event = type;
        this->r = r;
    }
    // getters
    vector<Member>& get_members() {
        return this->members;
    }
    string get_title() const {
        return this->title;
    }
    string get_event_id() const {
        return this->event_id;
    }
    string get_description() const {
        return this->description;
    }
    Venue* get_venue() const {
        return this->venue;
    }
    Event_Type get_event_type() const {
        return this->type_of_event;
    }
    string get_st() const {
        return this->r->get_start_time();
    }
    string get_et() const {
        return this->r->get_end_time();
    }
    Event_Rules* get_event_rules() const {
        return this->r;
    }
};

class Room_Manager_And_Allocator {
private:
    map<int, Room*> rooms;
    map<int, bool> is_taken;
public:
    Room_Manager_And_Allocator() {}
    ~Room_Manager_And_Allocator() {}
    void add_room(Room* r) {
        this->rooms[r->get_room_id()] = r;
        this->is_taken[r->get_room_id()] = false;
    }
    bool is_suitable(Room* room, Event* e) const {
        // for now we are just checking capacity, but we can easily add more checks on projector requirement, etc
        int total_capacity = room->get_capacity();
        int members = e->get_members().size();
        if (total_capacity < members) {
            return false; 
        }
        return true;
    }
    Room* allocate(Event* e) {
        // find event
        int target = -1;
        Room* ans = nullptr;
        for (auto& [k, v] : this->rooms) {
            if (!this->is_taken[k] && this->is_suitable(v, e)) {
                // room found
                ans = v;
                target = k;
                break;
            }   
        }

        // mark as taken if a room is found
        if (ans == nullptr) {
            cout << "Room not available..." << endl;
        } else {
            // mark as taken
            this->is_taken[target] = true;
        }

        // return pointer
        return ans;

    }
};
// ======================================













// ===================================
enum class Response {
    ACCEPTED,
    DECLINED,
    TENTATIVE,
    PENDING
};

class Event_Response_Handler {
private:
    map<string, map<string, Response>> mp;
public:
    Event_Response_Handler() {}
    ~Event_Response_Handler() {}

    void add_event(Event* e) {
        
        for (Member& a : e->get_members()) {
            this->mp[e->get_event_id()][a.get_email()] = Response::PENDING;    
        }
    }

    void mark_response(string event_id, string email, Response r) {
        if (this->mp.find(event_id) == this->mp.end()) {
            cout << "Event does not exist..." << endl;
            return;
        }

        if (this->mp[event_id].find(email) == this->mp[event_id].end()) {
            cout << "No such member exists for this event..." << endl;
            return;
        }        

        this->mp[event_id][email] = r;
        
    }
};
// ===================================












// ============ User Schedule Map =========================
class Interval {
public:
    string start_time;
    string end_time;
    Interval (string start_time, string end_time) {
        this->start_time = start_time;
        this->end_time = end_time;
    }

    bool operator<(const Interval& other) const {
        if (this->start_time == other.start_time) {
            return this->end_time < other.end_time;
        }
        return this->start_time < other.start_time;
    }

};
 
class User_Schedule_Map {
private:
    map<string, map<string, set<Interval>>> mp;
    map<string, bool> dirty;
    // mp[email][day] = [list of intervals where user is busy] 
public:

    User_Schedule_Map() {};
    ~User_Schedule_Map() {};

    void add_user_entry(string email, string date, Interval interval) {
        this->mp[email][date].insert(interval);
        this->dirty[email] = true;
    }

    // we only call merge intervals if there is a query on this data
    // lazy update

    set<Interval> update_set(set<Interval>& s) {
        if (s.empty()) return {};

        set<Interval> ans;
        auto it = s.begin();
        string start = it->start_time;
        string end = it->end_time;
        ++it;

        for (; it != s.end(); ++it) {
            if (end < it->start_time) {
                // completely disjoint
                ans.insert(Interval(start, end));

                // update start and end
                start = it->start_time;
                end = it->end_time;
            } else {
                end = max(end, it->end_time);
            }
        }
        // push last merged interval
        ans.insert(Interval(start, end));

        return ans;
    }
    
    void merge_intervals(string email) {
        // update all dates for this email
        for (auto& it : this->mp[email]) {
            it.second = update_set(it.second);
        }
    }

    vector<Interval> get_busy_slots(string email, string start_date, string end_date) {
        if (this->dirty[email]) {
            this->merge_intervals(email);
            // remove dirty
            this->dirty[email] = false;
        }

        auto it_start = this->mp[email].lower_bound(start_date);
        auto it_end = this->mp[email].upper_bound(end_date);

        vector<Interval> ans;
        for (auto it = it_start; it != it_end; ++it) {
            for (const Interval& x : it->second) {
                ans.push_back(x);
            }
        }

        return ans;

    }

    bool is_free(string email, string date, Interval target) {
        if (this->dirty[email]) {
            merge_intervals(email);
            this->dirty[email] = false;
        }

        const set<Interval>& s = this->mp[email][date];
        for (const Interval& i : s) {
            if (max(target.start_time, i.start_time) < min(target.end_time, i.end_time)) {
                return false;
            }
        }
        return true;
    }

    string increment_time(string time, int m) {
        int hours = stoi(time.substr(0, 2));
        int minutes = stoi(time.substr(3, 2));

        minutes += m;
        hours += minutes / 60;
        minutes %= 60;

        char buffer[9];
        snprintf(buffer, sizeof(buffer), "%02d:%02d:00", hours, minutes);
        return string(buffer);
    }

    Interval* get_earliest_N_minute_window(Event* e, int n, string date) {
        // this function checks for all members and finds the earliest n minute window common to all users
        string start = "00:00:00";
        int hours = n/60;
        int remaining_minutes = n - hours*60;
        string end = to_string(hours) + ":" + to_string(remaining_minutes) + ":00";
        Interval* ans = nullptr;
        while (end <= "24:00:00") {
            Interval i(start, end);
            bool all_free = true;
            for (Member& m : e->get_members()) {
                bool is_free = this->is_free(m.get_email(), date, i);
                if (!is_free) {
                    all_free = false;
                    break;
                }
            }
            if (all_free) {
                // window found
                ans = new Interval(i.start_time, i.end_time);
                return ans;
            }

            // increment start and end by one minute
            start = this->increment_time(start, 1);
            end = this->increment_time(end, 1);
        }

        cout << "no free window on this day" << endl; 
        return ans;
    }
};
// ========================================================

















// ======== NOTIFICATION SYSTEM + MIN EVENT HADNLER =========
// Observable -> event manager/triggerer
// observer -> email_notification_observer, sms_notification_observer, like this is scalable


// notification payload is also scalable
class Notification_Payload {
public:
    virtual const string get_formatted_message() = 0;
    virtual Member* get_target() = 0;
    virtual ~Notification_Payload() {};
};

class Event_Trigger_Payload : public Notification_Payload {
private:
    Member* target;
    string title;
    string description;
    string start_time; // this will be like , 29 /8 /2026, 12:30 pm
    string end_time;
public:
    Event_Trigger_Payload(Member* member, Event* a) {
        this->target = member;
        this->start_time = a->get_st();
        this->end_time = a->get_et();
        this->title = a->get_title();
        this->description = a->get_description();
    }

    ~Event_Trigger_Payload() {

    }

    const string get_formatted_message() override {
        string response = "";
        response+= string("======== ") + this->title + string(" =======");
        response+= "\n";
        response+=this->description;
        response+= "\n";
        response+= string("Start time: ") + this->start_time + "\n";
        response+= string("End time: ") + this->end_time + "\n";
        response+= string(" ==========================================\n");
        return response;
    }
    Member* get_target() override {
        return this->target;
    }
};

class Event_Update_Payload : public Notification_Payload {
private:
    Member* a;
    string title;
    string update;
public:
    Event_Update_Payload(Member* a, Event* e, string update) {
        this->a = a;
        this->title = e->get_title();
        this->update = update;
    }

    const string get_formatted_message() override {
        string response = "";
        response+= string("======= ") + this->title + string(" =======");
        response+="\n";
        response+= this->update;
        response+="\n";
        response+="=================================\n";
        return response;
    }

    Member* get_target() {
        return this->a;
    }
};

class Observer {
public:
    virtual ~Observer() {};
    virtual void update(Member* a, string message) = 0;
};

class Observable {
public:
    virtual ~Observable() {};
    virtual void add_observer(Observer* o) = 0;
    virtual void notify(Notification_Payload* payload)= 0;
};

class Event_Manager : public Observable{
private:
    vector<Observer*> observers;
    Event_Response_Handler* erh;
    User_Schedule_Map* usp;
public:
    Event_Manager() {
        this->erh = new Event_Response_Handler();
        this->usp = new User_Schedule_Map();
    };
    ~Event_Manager() {
        for (Observer* o : this->observers) {
            delete o;
        }
        delete this->erh;
        delete this->usp;
    }
    void add_observer(Observer* o) override {
        this->observers.push_back(o);
    }
    void notify(Notification_Payload* payload) override {
        // notify all observers
        for (Observer* o : this->observers) {
            o->update(payload->get_target(), payload->get_formatted_message());
        }
    }
    void add_event(Event* e) {
        // update user_sch_map
        bool allocated = e->get_event_rules()->add_event(this->usp);
        if (!allocated) {
            cout << "Event not feasible..." << endl;
            return;
        }
        this->erh->add_event(e);
        // event comes her only if it is verified before
        for (Member& m : e->get_members()) {
            // create notification payload
            Notification_Payload* payload = new Event_Trigger_Payload(&m, e);

            // call notify
            this->notify(payload);

            // delete payload
            delete payload;
        }
        
        
    }
    void update_event(Event* e, string update_message) {
        for (Member& m : e->get_members()) {
            // create notification payload
            Notification_Payload* payload = new Event_Update_Payload(&m, e, update_message);

            // call notify
            this->notify(payload);

            // delete payload
            delete payload;
        }
    }
    void record_member_response (string email, string event_id, Response r) {
        this->erh->mark_response(event_id, email, r);
    }
};

class Email_Notification_Observer : public Observer {
public:
    Email_Notification_Observer() {};

    void update(Member* a, string message) {
        cout << "Email sent to: " << a->get_name() << "..." << endl;
    }
};

class SMS_Notification_Observer : public Observer {
public:
    SMS_Notification_Observer() {};

    void update(Member* a, string message) {
        cout << "SMS sent to: " << a->get_name() << "..." << endl;
    }
};

bool Recurrent_Event::add_event(User_Schedule_Map* m) {
    // this performs every check needed using user_map and allocates if possible using usp functionalities

    // not implementing here , because thats not hte main goal
    return true;
}

bool Non_Recurring_Event::add_event(User_Schedule_Map* m) {
    return true;
}
// ====================================================================












int main() {
    return 0;
}