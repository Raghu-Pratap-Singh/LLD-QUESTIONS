#include <bits/stdc++.h>
using namespace std;

// =============TINDER=============




// enum classes fro factories ==================
enum class Gender {
    MALE,
    FEMALE,
    OTHER
};

enum class Viewing_Algorithm_Type {
    STANDARD_K,
    PREFERENCE_K
};

enum class Matching_Algorithm_Type {
    LOCATION,
    PREFERENCE
};
// ==============================










// ========= PROFILES =============
class Location {
private:
    double lon, lat;
public:
    Location (double lon, double lat) {
        this->lon = lon;
        this->lat = lat;
    }

    // getters
    double get_longitude() {
        return this->lon;
    }
    double get_latitude() {
        return this->lat;
    }
    ~Location() = default;
};

class Preferences {
private:
    vector<string> hobbies;
    vector<Gender> genders;
public:
    Preferences () {};
    // setters getters
    void add_hobby(string hobby_name) {
        this->hobbies.push_back(hobby_name);
    }

    void add_gender(Gender g) {
        this->genders.push_back(g);
    }

    vector<string>& get_hobby_preferences() {
        return this->hobbies;
    }

    vector<Gender>& get_gender_preferences () {
        return this->genders;
    }
    ~Preferences() = default;
};

class Profile {
private:
    string name;
    Gender g;
    // NAME IS UNIQUE TO EVERY USER
    Location* geo;
    string mobile;
    string image_path;
    
    // preference has a relation
    Preferences* p;
public:
    Profile (string name, Location* loc, string mobile, Gender g) {
        this->name = name;
        this->geo = loc;
        this->mobile = mobile;
        this->g = g;
    }

    ~Profile() {
        delete this->geo;

    }
    // setters

    void set_preference(Preferences* p) {
        this->p = p;
    }

    void set_image_path (string path) {
        this->image_path = path;
    }

    // getters
    const Gender get_gender() {
        return this->g;
    }

    const Preferences* get_preferences () {
        return this->p;
    }

    const string& get_image_path () {
        return this->image_path;
    }

    const string& get_name () {
        return this->name;
    }

    const string& get_mobile () {
        return this->mobile;
    } 

    const Location* get_location () {
        return this->geo;
    }
};
// ================================














// ======== CLOSE LOCATION PROFILE VIEWING ALGORITHMS ========

//  THESE ALGORITHMS USE USERS PROFILE AND OTHER SETTINGS TO FIND NEARBY SUITABLE PROFILES USING DIFFERENT ALGORITHMS
class Viewing_Algorithm {
public:
    virtual vector<Profile*> find(Profile* user) = 0;
    virtual ~Viewing_Algorithm() = default;
};

class Standard_K_Km_Radius : public Viewing_Algorithm {
private:
    int km;
public:
    Standard_K_Km_Radius (int km) {
        this->km = km;
    }
    vector<Profile*> find(Profile* user) {
        // this performs internal algorithm and returns profile within km kilometers radius

        // for demo purposes, return empty vector
        return {};
    }
};

class Preference_Based_Search_Within_K_Km : public Viewing_Algorithm {
private:
    int km;
public:
    Preference_Based_Search_Within_K_Km (int km) {
        this->km = km;
    }

    vector<Profile*> find(Profile* user) {
        return {};
    }
};



class Viewing_Algorithm_Factory {
private:
    static Viewing_Algorithm_Factory* instance;
    Viewing_Algorithm_Factory () {};
public:
    static Viewing_Algorithm_Factory* get_instance() {
        return Viewing_Algorithm_Factory::instance;
    }

    Viewing_Algorithm* create(Viewing_Algorithm_Type algo_name, map<string, int> attributes) {
        if (algo_name == Viewing_Algorithm_Type::STANDARD_K) {
            return new Standard_K_Km_Radius(attributes["km"]);
        } else if (algo_name == Viewing_Algorithm_Type::PREFERENCE_K) {
            return new Preference_Based_Search_Within_K_Km(attributes["km"]);
        }
        cout << "This Viewing_Algorithm is not supported yet .... "<< endl;
        return nullptr;
    }
};

Viewing_Algorithm_Factory* Viewing_Algorithm_Factory:: instance = new Viewing_Algorithm_Factory();
// ===========================================================


















// ============ OBSERVER PATTERN FOR NOTIFICATION ============
class Notification_Payload {
protected:
    Profile* recipient;
public:
    Profile* get_recipient() {
        return this->recipient;
    }
    virtual string get_formatted_message() = 0;
    virtual ~Notification_Payload() = default;
};

class Match_Notification_Payload : public Notification_Payload {
private:
    Profile* target;
    double percent;
public:
    Match_Notification_Payload (Profile* target, Profile* recipient, double percent) {
        this->target = target;
        this->percent = percent;
        this->recipient = recipient;
    }

    string get_formatted_message() {
        return "You are a"+ to_string(this->percent) + "match with "+ this->target->get_name();
    }
};

class Unread_Notification_Payload : public Notification_Payload {
private:
    Profile* target;
public:
    Unread_Notification_Payload (Profile* target, Profile* recipient) {
        this->target = target;
        this->recipient = recipient;
    }

    string get_formatted_message() {
        return "You have an unread message from " + this->target->get_name();
    }

};

class Observable;
class Observer;
class Observer {
protected:
    Observable* ob;
public:
    string type;
    
    virtual void update(Profile* target_profile, string message) = 0;
    void set_observable (Observable* o) {
        this->ob = o;
    }
    virtual ~Observer() = default;
};
class Observable {
protected:
    vector<Observer*> observers;
public:
    void add_observer (Observer* o) {
        this->observers.push_back(o);
        // also add this reference to o
        o->set_observable(this);
    }

    void remove_observer (Observer* o) {
        // removes observer from observers by matching type of observer
        int target_index = -1;
        int index = 0;
        for (Observer* t : this->observers) {
            if (t == o) {
                target_index = index;
                break;
            }
            index++;
        }
        if (target_index != -1) {
            this->observers.erase(this->observers.begin() + target_index);
            cout << "Removed observer" << endl;
        } else {
            cout << "No such observer exists" << endl;
        }
    }
    virtual void notify (Notification_Payload* payload) = 0;
    virtual ~Observable() = default;
};
// DIFFERENT OBSERVABLES CAN BE APP_UPDATE_SYSTEM (TO SEND USERS NOTIFICATION OF NEW UPDATE)

// ANOTHER CAN BE : MATCHER SYSTEM, WHICH WILL SEND ALL NOTIFICATION RELATED TO MATCH OF 2 USERS

// ANOTHER CAN BE : UNREAD MESSAGE SYSTEM, SAY A SEND A MESSAGE TO B USER, BUT B IS OFFLINE, SO A NOTIFICATION WILL BE SENT TO B
// THERE CAN BE MANY MORE
class Match_System : public Observable {
public:
    Match_System () {}
    
    void notify (Notification_Payload* payload) {
       

        Profile* a = payload->get_recipient();

        for (Observer* o : this->observers) {
            o->update(a, payload->get_formatted_message());

        }
    }
};

class Unread_System : public Observable {
public:
    Unread_System () {}
    
    void notify (Notification_Payload* payload) {
        // this will contain two user in target vector
        Profile* a = payload->get_recipient();
        for (Observer* o : this->observers) {
            
            o->update(a, payload->get_formatted_message());

        }
    }
};
// DIFFERENT OBSERVERS (PUSH NOTIFICATION OBSERVER, EMAIL, WHATSAPP, ETC.)

class Push_Notification_Observer : public Observer {
public:
    void update(Profile* target, string noti) {
        // this send this notification to target thorugh push mechanism
        cout << "Sent Push Notification..." << endl;
    }
};

class Slack_Notification_Observer : public Observer {
public:
    void update(Profile* target, string noti) {
        // this send this notification to target thorugh push mechanism
        cout << "Sent Slack Notification..." << endl;
    }
};

class Email_Notification_Observer : public Observer {
public:
    void update(Profile* target, string noti) {
        // this send this notification to target thorugh push mechanism
        cout << "Sent Slack Notification..." << endl;
    }
};

// ===========================================================












// =======================CHATROOM ===============================
// we know that each chatroom has a room id unique to it==========
// a tinder chatroom has a chatroom with only two users whos match just occured===========================================================

class Random_Id_Generator {
private:
    long long p = 97;
    long long mod = 1e9 + 7;
    vector<long long> power;
    static Random_Id_Generator* instance;
    Random_Id_Generator () {
        power.resize(100001);
        power[0] = 1;
        for (int i = 1; i<=100000; i++) {
            power[i] = (power[i-1] * p) % mod;
        }
    };
public:
    static Random_Id_Generator* get_instance () {
        return Random_Id_Generator::instance;
    }

    long long create_id(Profile* user_1, Profile* user_2) {
        string temp = user_1->get_name() + user_2->get_name();
        // this function generates a random hash using a hash algorithm, we will use polynomial rolling hash

        long long hash = 0;
        hash = temp[0] - 'a' + 1;
        for (char ch : temp) {
            hash = ((hash * (p))%mod + (ch - 'a' + 1))%mod;
        }
        return hash;
    }
};

Random_Id_Generator* Random_Id_Generator::instance = new Random_Id_Generator();

class Chatroom {
private:
    long long room_id;
    Profile* user_1;
    Profile* user_2;
public:
    Chatroom (Profile* user_1, Profile* user_2) {
        
        this->user_1 = min(user_1, user_2);
        this->user_2 = max(user_1, user_2);
        this->room_id = Random_Id_Generator::get_instance()->create_id(this->user_1, this->user_2);
    };

    long long get_room_id () {
        return this->room_id;
    }
    ~Chatroom () {
    }
};

// ===============================================================













// ================== SWIPE HANDLER ==============================
// this will store the swipe in db (sql,mongo,cassandra,) (scalable) and return true if opposite user has also stored them in swipe right
class Swipe_Handler {
public:
    virtual bool save_choice(Profile* a, Profile* b) = 0;
    virtual ~Swipe_Handler() = default;
};
class SQL_Swipe_Handler : public Swipe_Handler {
public:
    bool save_choice(Profile* a, Profile* b) {
        cout << "Saved to sql db" << endl;
        return true;
        // for demo purposes
    }
};

class MONGO_Swipe_Handler : public Swipe_Handler {
public:
    bool save_choice(Profile* a, Profile* b) {
        cout << "Saved to mogno db" << endl;
        return true;
    }
};
// ===============================================================














// ============= MATCHING ALGOS ==============
class Matching_Algorithm {
public:
    virtual double match(Profile* a, Profile* b) = 0;
    virtual ~Matching_Algorithm () = default;
};

class Location_Match : public Matching_Algorithm {
public:
    double match (Profile* a, Profile* b) {
        // this algo matches if ther location distance is within considerable distance
        // for now we are just returning true
        return true;
    }
};

class Preference_Match : public Matching_Algorithm {
public:
    double match(Profile* a, Profile* b) {
        return true;
    }
};



class Matching_Algortihm_Factory {
private:
    static Matching_Algortihm_Factory* instance;
    Matching_Algortihm_Factory () {};
public:
    static Matching_Algortihm_Factory* get_instance() {
        return Matching_Algortihm_Factory::instance;
    }

    Matching_Algorithm* create(Matching_Algorithm_Type algo_name) {
        if (algo_name == Matching_Algorithm_Type::LOCATION) {
            return new Location_Match();
        } else if (algo_name == Matching_Algorithm_Type::PREFERENCE) {
            return new Preference_Match();
        }
        cout << "This Matching Algorithm is not supported yet..." << endl;
        return nullptr;
    }
};

Matching_Algortihm_Factory* Matching_Algortihm_Factory::instance = new Matching_Algortihm_Factory();
// ===========================================
















// =========== FACADE ==========
class Facade {
private:
    Profile* current_user;
    map<Profile*, Chatroom*>matches;
    vector<Profile*> right;
    Viewing_Algorithm* algo;
    vector<Profile*> nearby_profiles;
    Swipe_Handler* sh;
    Matching_Algorithm* m_algo;

    Observable* match_notification;
    Observable* unread_notification;

public:
    Facade () {
        this->current_user = nullptr;
        this->algo = nullptr;
        this->sh = nullptr;
        this->m_algo = nullptr;
        this->match_notification = nullptr;
        this->unread_notification = nullptr;
    };
    void set_db(Swipe_Handler* d) {
        this->sh = d;
    }
    void Login (Profile* p) {
        this->current_user = p;
    }

    void set_algorithm (Viewing_Algorithm_Type name, map<string, int> attributes) {
        // uses algo factory to set up the algo (this encapsulates the internal implementation of object creation from user)
        this->algo = Viewing_Algorithm_Factory::get_instance()->create(name, attributes);
        this->get_nearby_profiles();
    }

    void set_matching_algorithm (Matching_Algorithm_Type name) {
        this->m_algo = Matching_Algortihm_Factory::get_instance()->create(name);
    }

    void get_nearby_profiles () {
        this->nearby_profiles = this->algo->find(this->current_user);
    }

    // this is the main function, on swipe right first the choice is saved to db like in real tinder, then also a match handler function is called, if it finds that the b user has also swiped right on user a, then it will call a matching algorithm(scalable) to see if they are a match
    void swipe_right (Profile* b) {
        bool response = this->sh->save_choice(this->current_user, b);
        if (response) {
            // match chances are there, call the matching algorithm match function
            this->match(b);
        }

    }

    void match (Profile* b) {
        double match_percent = this->m_algo->match(this->current_user, b);
        // provide user a chat room as this is a match,
        this->matches[b] = new Chatroom(this->current_user, b);
        // send a match notification
        this->send_match_notification(new Match_Notification_Payload(b, this->current_user, match_percent));
        this->send_match_notification(new Match_Notification_Payload(this->current_user, b, match_percent));
    }

    void set_match_notification (Observable* o, vector<Observer*>ob) {
        this->match_notification = o;
        for (Observer* oo : ob) {
            this->match_notification->add_observer(oo);
        }
    }

    void set_unread_notification (Observable* o, vector<Observer*>ob) {
        this->unread_notification = o;
        for (Observer* oo : ob) {
            this->unread_notification->add_observer(oo);
        }
    }

    void send_match_notification (Notification_Payload* payload) {
        this->match_notification->notify(payload);
    }

    ~Facade () {
        // Delete current logged-in user if owned by Facade
    delete current_user;
    
    // Delete algorithm instances created via Factories
    delete algo;
    delete m_algo;
    
    // Delete swipe handler DB driver
    delete sh;
    
    // Delete observer subsystems
    delete match_notification;
    delete unread_notification;

    // Clean up allocated chatrooms
    for (auto& entry : matches) {
        delete entry.second;
    }
    }
};
// ===============================================

// ================================this function uses the swipe

int main() {
    // (1) APP INSTANCE (along with what type of db is this app using)
    Facade dpp;


    // (2) CREATE PROFILE


    // (2.5) now user can choose what type of notification he wants, email, push,,etc. for when a match occurs, and when he/she has unread message(scalable)

    // (3) USER LOGS IN

    // (4) USER SETS PREFREED ALGORITHM FOR NEARBY VIEWING ALGORITHMS

    // (4.5) USER ALSO CHOOSES HOW HE WANTS TO BE MATCHED WITH OTHER USER (RIGHT SWIPED USER) (IT CAN BE LOCATION MATCH, PREFERENCE % MATCH,ETC,THESE ALGOS ARE SOLELY BASED ON OTHER USERS AND CURRENT USERS PROFILE)

    // (5) USER SEES SOME PROFILE, BASED ON ALGORITHM CHOSEN EARLIER

    // (6) SWIPE RIGHT/ LEFT

    // (7) USER GETS NOTIFIED ON MATCH, A CHAT ROOM IS ASSIGNED TO USER


    return 0;
}