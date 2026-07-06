#include <bits/stdc++.h>
using namespace std;
// ====================MODEL CLASSES===================

class Singer {
private:
    string name;
    int followers;
public:
    Singer (string name, int followers) {
        this->name = name;
        this->followers = followers;
    }

    // getters
    string get_name () {
        return this->name;
    }
    int get_followers () {
        return this->followers;
    }
    ~Singer () {};
};

class Song {
private:
    string duration;
    string name;
    vector<Singer> singers;
public:
    Song(string name, string duration, vector<Singer> artists) {
        this->name = name;
        this->duration = duration;
        this->singers = artists;
    }
    // getters
    string get_name() {
        return this->name;
    }
    string get_duration() {
        return this->duration;
    }
    vector<Singer>& get_singers() {
        return this->singers;
    }

    ~Song() {};
};

class Playlist {
private:
    string name;
    vector<Song> songs;
    vector<int> preferred_order;
public:
    Playlist(string name) {
        this->name = name;
    };
    // getter and setters
    string get_name() {
        return this->name;
    }

    vector<Song>& get_songs() {
        return this->songs;
    }

    void set_custom_order(vector<int> order) {
        
        this->preferred_order = order;
        cout << "setted custom order for this playlist..." << endl;
    }

    vector<int>& get_order() {
        return this->preferred_order;
    }

    ~Playlist() {}
};

class User {
private:
    string name;
    string phone;
    vector<Playlist> playlists;
public:
    User (string name, string phone) {
        this->name = name;
        this->phone = phone;
    }

    // getters
    string get_name() {
        return this->name;
    }

    string get_phone() {
        return this->phone;
    }

    vector<Playlist>& get_playlists() {
        return this->playlists;
    }

    ~User () {}
};

// =======================================================



// ====================ALGORITHM USING STRATEGY PATTERN==========
class Playlist_algorithm {
public:
    virtual void play(Playlist& p, Song*& current_song) = 0;
    virtual ~Playlist_algorithm() {};
};

class Sequential : public Playlist_algorithm {
public:
    void play(Playlist& p, Song*& current_song) {
        // we cannot implement here fully, so just printng a statement for demonstration purposes
        cout << "Playing this playlist in sequential order..." << endl;
        if (p.get_songs().size()>0) {
            current_song = &p.get_songs()[0];
        }
    }
};

class Reverse : public Playlist_algorithm {
public:
    void play(Playlist& p, Song*& current_song) {
        // we cannot implement here fully, so just printng a statement for demonstration purposes
        cout << "Playing this playlist in Reverse order..." << endl;
        if (p.get_songs().size()>0) {
            current_song = &p.get_songs()[p.get_songs().size() - 1];
        }
    }
};

class Random : public Playlist_algorithm {
public:
    void play(Playlist& p, Song*& current_song) {
        // we cannot implement here fully, so just printng a statement for demonstration purposes
        cout << "Playing this playlist in Random order..." << endl;
        if (p.get_songs().size()>0) {
            current_song = &p.get_songs()[p.get_songs().size() - 1];
        }
    }
};

class Custom : public Playlist_algorithm {
public:
    void play(Playlist& p, Song*& current_song) {
        // we cannot implement here fully, so just printng a statement for demonstration purposes
        if (p.get_order().size() == 0) {
            cout << "please set a preference order for this playlist first..." << endl;
            return;
        }
        cout << "Playing this playlist in preferred order of this playlist..." << endl;
        // let us also print name of songs to be played in order of given preference also
        int k = 1;
        for (int idx : p.get_order()) {
            cout << "Song " << k << ":"  << p.get_songs()[idx].get_name() << endl;
            k++;
        }
        if (p.get_songs().size()>0) {
            current_song = &p.get_songs()[p.get_order()[0]];
        }
    }
};
// ===============================================================





//=============== MANAGERS AND FACTORIES: SINGLETON CLASSES AS WE DONT WANT TO CREATE THEIR OBJECTS AGAIN AND AGAIN==============

// PLAYLIST MODIFIER : this class has only one reponsibility, to add or remove songs from a target playlist
class Playlist_Modifier {
private:
    static Playlist_Modifier* instance;
    Playlist_Modifier () {};
public:
    static Playlist_Modifier* get_instance() {
        return Playlist_Modifier::instance; 
    }

    // add song
    void add_song(Playlist& p, Song& song) {

        for (Song& s : p.get_songs()) {
            if (s.get_name() == song.get_name()) {
                // song already exists
                cout << song.get_name() << "is already in "<< p.get_name() << endl;
                return;
            }
        }
        // this is a new song, add it to playlist
        p.get_songs().push_back(song);
        cout << song.get_name() << " added to " << p.get_name() << " successfully" <<  endl;
    }

    // remove song
    void remove_song(Playlist& p, Song& song) {
        int target = -1;
        int idx = 0;
        for (Song& s : p.get_songs()) {
            if (s.get_name() == song.get_name()) {
                // song exists in this playlist
                target = idx;
                break;
            }
            idx++;
        }

        if (target == -1) {
            cout << "no such song exists in " << p.get_name() << endl;
            return;
        }
        // son exists in playlist
        p.get_songs().erase(p.get_songs().begin() + target);
        cout << song.get_name() << " successfully removed from " << p.get_name() << endl;
    }

    ~Playlist_Modifier() {}
};

// only one responsibility, creation and deletion of playlists for a user
class Playlist_Factory {
private:
    static Playlist_Factory* instance;
    Playlist_Factory () {};
public:
    static Playlist_Factory* get_instance() {
        return Playlist_Factory::instance; 
    }

    void create_playlist(string name, User& u) {
        u.get_playlists().push_back(Playlist(name));
        cout << "Playlist created successfully..." << endl;
    }

    void delete_playlist(string name, User& u) {
        int target_index = -1;
        int index = 0;
        for (Playlist& p : u.get_playlists()) {

            if (p.get_name() == name) {
                target_index = index;
                break;
            }
            index++;
        }
        if (target_index!=-1) {
            u.get_playlists().erase(u.get_playlists().begin() + target_index);
            cout << name << " playlist removed successfully from "<<u.get_name()<<"'s playlists..." << endl;
            return;
        }
        cout << "no such playlist exists in " << u.get_name() <<"'s playlists..." << endl;

    }
    ~Playlist_Factory() {};
};

// only one responsibility , creating algos for playlist
class Algo_Factory {
private:
    static Algo_Factory* instance;
    Algo_Factory () {};
public:
    static Algo_Factory* get_instance() {
        return Algo_Factory::instance; 
    }

    Playlist_algorithm* create(string algo_name) {
        if (algo_name == "Sequential") {
            return new Sequential();
        } else if (algo_name == "Reverse") {
            return new Reverse();
        } else if (algo_name == "Random") {
            return new Random();
        } else if (algo_name == "Custom") {
            return new Custom();
        }
        cout << "such algorithm is not supported yet..." << endl;
        return nullptr;

    }    
    ~Algo_Factory() {};
};





// =================EAGER INITIALISATION=====================
Playlist_Modifier* Playlist_Modifier::instance = new Playlist_Modifier();
Playlist_Factory* Playlist_Factory::instance = new Playlist_Factory();
Algo_Factory* Algo_Factory::instance = new Algo_Factory();

// ====================================================================================================




// =====================output device : adapter pattern to be used===============================
// IMPORTANT : REMEMBER THAT ALL OUTPUT DEVICES PROVIDE US THEIR 3RD PARTY APIS AND WE USE THEM, NNOW TO SUPPORT THEM , WE NNED TO USE ADAPTER PATTERN


// =======adaptees (3rd party apis) : they have their specific criteria for connection
class Wired_Speaker {
public:
    // this is the exposed api say
    bool connection_request(string request_data) {
        // we send some signal say request_data to this request, and it will connect
        // for now we are returning one

        return 1;
    }
    ~Wired_Speaker() {};
};

class Bluetooth_Speaker {
public:
    bool connection_request(string request_data, string model_number) {
        // for now we are returning one

        return 1;
    }
    ~Bluetooth_Speaker () {};
};

class Phone_Speaker {
public:
    bool connection_request(string raw_data) {
        return 1;
    }
    ~Phone_Speaker() {};
};


// =======================adapters=============================
class IOutput {

public:
    // retruns 1 if connection successful, else 0
    virtual bool connect(string raw_data) = 0;
    virtual ~IOutput() {};
};

class Wired_Adapter : public IOutput {
private:
    Wired_Speaker* ws;
public:
    Wired_Adapter () {
        this->ws = new Wired_Speaker();
    }

    bool connect(string raw_data) {
        // this functionwill accordingly adapt this data according to specific request of wired spealer api how it demands and send(adapt)

        string data = ":///.." + raw_data + "///...";
        // just for demonstration we do this
        bool response = this->ws->connection_request(data);
        if (response) {
            cout << "Connected to wired Speaker.." << endl;
            return 1;
        }
        cout << "Some error occured.." << endl;
        return 0;
        
    }
    ~Wired_Adapter() {
        delete this->ws;
    }
};

class Bluetooth_Adapter : public IOutput {
private:
    Bluetooth_Speaker* bs;
public:
    Bluetooth_Adapter () {
        this->bs = new Bluetooth_Speaker();
    }

    bool connect(string raw_data) {
        // this functionwill accordingly adapt this data according to specific request of wired spealer api how it demands and send(adapt)

        string data = ":" + raw_data + "??";
        // extract model number
        string model_number = raw_data.substr(0, 5);
        // just for demonstration we do this
        bool response = this->bs->connection_request(data, model_number);
        if (response) {
            cout << "Connected to Bluetooth Speaker.." << endl;
            return 1;
        }
        cout << "Some error occured.." << endl;
        return 0;
        
    }

    ~Bluetooth_Adapter () {
        delete this->bs;
    }
};

class Phone_Adapter : public IOutput {
private:
    Phone_Speaker* ps;
public:
    Phone_Adapter () {
        this->ps = new Phone_Speaker();
    }

    bool connect(string raw_data) {
        
        bool response = this->ps->connection_request(raw_data);
        if (response) {
            cout << "Connected to Phone Speaker.." << endl;
            return 1;
        }
        cout << "Some error occured.." << endl;
        return 0;
        
    }

    ~Phone_Adapter() {
        delete this->ps;
    }
};
// =====================output factory : singleton for reduced memory overhead of recreating this factory instance for same purpose============================

class Output_Device_Factory {
private:
    static Output_Device_Factory* instance;
    Output_Device_Factory () {};
public:
    static Output_Device_Factory* get_instance() {
        return Output_Device_Factory::instance;
    }

    IOutput* get_ouput_device(string name) {
        if (name == "Wired") {
            return new Wired_Adapter();
        } else if (name == "Bluetooth") {
            return new Bluetooth_Adapter();
        } else if (name == "Phone") {
            return new Phone_Adapter();
        }
        cout << "This device is not supported yet..." << endl;
        return nullptr;
    }

    ~Output_Device_Factory () {};
};

// EAGER INITIALISATION===============
Output_Device_Factory* Output_Device_Factory::instance = new Output_Device_Factory();

// =================================================================

// only one responsibility , fetch any required date of users phone for connection from db
// in future data may need to be fetched from different databases and cloud even , so we need to make this fetcher scalable
class Data_Fetcher {
public:
    virtual string fetch(User& u) = 0;
    virtual ~Data_Fetcher() {};
};

class SQL_Data_Fetcher : public Data_Fetcher {
public:
    string fetch(User& u) {
        // fetches accordingly the data required for connection
        // for demonstration purposes we are returning random string
        return "00mnh-uiui";
    }
};

class MONGODB_Data_Fetcher : public Data_Fetcher {
public:
    string fetch(User& u) {
        // fetches accordingly the data required for connection
        // for demonstration purposes we are returning random string
        return "10qwh-umni";
    }
};

class GOOGLE_DRIVE_Data_Fetcher : public Data_Fetcher {
public:
    string fetch(User& u) {
        // fetches accordingly the data required for connection
        // for demonstration purposes we are returning random string
        return "10qwh-um-GOOGLE-ni";
    }
};


class Fetcher_Factory {
private:
    static Fetcher_Factory* instance;
    Fetcher_Factory() {};
public:
    static Fetcher_Factory* get_instance() {
        return Fetcher_Factory::instance;
    }
    Data_Fetcher* get_fetcher(string name) {
        if (name == "SQL_Data_Fetcher") {
            return new SQL_Data_Fetcher();
        } else if (name == "MONGODB_Data_Fetcher") {
            return new MONGODB_Data_Fetcher();
        } else if (name == "GOOGLE_DRIVE_Data_Fetcher") {
            return new GOOGLE_DRIVE_Data_Fetcher();
        }
        cout << "this fetcher is not supported yet..." << endl;
        return nullptr;
    }
};
Fetcher_Factory* Fetcher_Factory::instance = new Fetcher_Factory();

// ====================SINGLE SONG HANDLER (WHEN SONG IS PLAYED RANDOMELY AND NOT FROM ANY PLAYLIST ALGORITHM)==========================
class Playback_Handler {
public:
    Playback_Handler() {
    }

    void play_song(Song& s, Song*& current_song, bool& is_playing) {
        is_playing = true;
        current_song = &s;
        cout << "Now playing : "<< s.get_name() << endl;
    }

    void stop_song(Song*& current_song, bool& is_playing) {
        if (is_playing) {
            cout << "Stopped : "<< current_song->get_name() << endl;
            is_playing = false;
            return;
        }


    }
};
// HIGH LEVEL MODULE (FACADE) AND LOW LEVEL MODULES FETCHERS OF DIFFERENT TYPES BOTH ARE DEPENDENT ON ABSTRACTIONS NA DNOT DIRECTLY (DIP)
// ==================FACADE FOR USER===========================
class Facade {
private:
    string raw_data;
    User* active_user;
    Song* current_song;
    Data_Fetcher* fetcher;
    IOutput* output;
    Playback_Handler* ph;
    Playlist_algorithm* selected_algorithm;
    // helper, managers, factories
    Playlist_Modifier* pm;
    Playlist_Factory* pf;
    Output_Device_Factory* of;
    Fetcher_Factory* ff;
    Algo_Factory* af;
    // as only one song can be played,
    bool is_playing;
    // this will tell us whether the current song(if not nullptr) is in paused or playing state
     
public:
    Facade () {
        this->pm = Playlist_Modifier::get_instance();
        this->ph = new Playback_Handler();
        this->pf = Playlist_Factory::get_instance();
        this->af = Algo_Factory::get_instance();
        this->of = Output_Device_Factory::get_instance();
        this->ff = Fetcher_Factory::get_instance();

        this->active_user = nullptr;
        this->current_song = nullptr;
        this->selected_algorithm = this->af->create("Sequential");
        // because in most apps, if no option is chosen, songs are played sequentially
        this->is_playing = false;
        this->fetcher = this->ff->get_fetcher("SQL_Data_Fetcher");
        // by default ouput device is phone speaker
        this->output = this->of->get_ouput_device("Phone");
        
    };

    void set_fetcher(string name) {
        this->fetcher = this->ff->get_fetcher(name);
    }


    // as login is not the main concern here, we just directly
    void login (string name, string phone) {
        this->active_user = new User(name, phone);
        cout << "logged in successfully..." << endl;
        this->is_playing = false;
        this->current_song = nullptr;
        // connect to default
        this->output->connect(this->fetcher->fetch(*this->active_user));
    }

    void change_output_device(string name) {
        this->output = this->of->get_ouput_device(name);
        this->output->connect(this->fetcher->fetch(*this->active_user));
    }

    void create_playlist(string playlist_name) {
        this->pf->create_playlist(playlist_name, *this->active_user);
    }

    void set_playlist_algorithm(string algo_name) {
        this->selected_algorithm = this->af->create(algo_name);
        if (this->selected_algorithm != nullptr) {
            // means user gaveinvalid algo name
            cout << "setted playing algorithm to " << algo_name << "..." << endl;
            return;
        }
        else {
            this->selected_algorithm = this->af->create("Sequential");
        }   
    }

    void set_custom_order(Playlist& p, vector<int> order) {
        p.set_custom_order(order);
    }

    void add_song(Playlist& p, Song& s) {
        // as user must have selected this playlist from his own list, so it willl always exist
        this->pm->add_song(p, s);
    }

    void delete_song(Playlist& p, Song& s) {
        
        this->pm->remove_song(p, s);
    }

    void play_song(Song& s) {
        // just some random song, no song from playlist, play directly, as we cant play it in real, just log some statement for demonstration purposes
        this->ph->play_song(s, this->current_song, this->is_playing);
    }
    void stop_song() {
        this->ph->stop_song(this->current_song, this->is_playing);
    }

    Playlist& get_playlist(string playlist_name) {
        for (Playlist& p : this->active_user->get_playlists()) {
            if (p.get_name() == playlist_name) {
                return p;
            }
        }

        cout << "no such playlist exists..." << endl;
        throw runtime_error("Playlist not found");
    }

    void play_playlist(Playlist& p) {
        // now the algorithm will handle the current playing song, and hence we need to pass that pointer variable

        // now as we cannot really play and change spongs here, we will assign the first song in playlist to current song
        this->is_playing = true;
        this->selected_algorithm->play(p, this->current_song);
        cout <<endl << this->current_song->get_name() << endl;
    }


    ~Facade () {
        delete this->active_user;
        this->current_song = nullptr;
        delete this->selected_algorithm;
        delete this->output;
        this->pm = nullptr;
        this->pf = nullptr;
        this->of = nullptr;
        this->ff = nullptr;
        delete this->fetcher;
        delete this->ph;
    }
};

int main() {
    // (1) user opens the app and logs in
    Facade app;
    app.login("Raghu Pratap Singh", "9875600023");

    // (2) create a playlist
    app.create_playlist("Liked Songs");

    // (2) add a song to a playlist

    // for demonstration purposes, we have hardcoded this song, otherwise it should have been added by database fetching and then selected by user, but as this is users perspective of music app, we shouldnt worry about this
    Singer s1 = Singer("Prathmesh", 1231234);
    vector<Singer> artists = {s1};
    Song song1("Main Lagta Kaun", "3:34", artists);

    Singer s2 = Singer("NVK", 12334);
    vector<Singer> artists2 = {s2};
    Song song2("Lesgo", "3:30", artists2);

    Singer s3 = Singer("Naam Sujal", 1239934);
    vector<Singer> artists3 = {s3};
    Song song3("PYAAR", "2:50", artists3);



    app.add_song(app.get_playlist("Liked Songs"), song1);
    app.add_song(app.get_playlist("Liked Songs"), song3);
    app.add_song(app.get_playlist("Liked Songs"), song2);

    // (3) remove a song from a playlist
    // app.delete_song(app.get_playlist("Liked Songs"), song1);


    // (4) set favourite ordering of songs in particular playlist
    vector<int> order = {2, 0, 1};
    app.set_custom_order(app.get_playlist("Liked Songs"), order);

    // (5) set playing algorithm of a playlist
    app.set_playlist_algorithm("Custom");

    // (6) play a single song2
    app.play_song(song2);

    // (7) play a playlist
    app.play_playlist(app.get_playlist("Liked Songs"));

    // (8) stop playing song
    app.stop_song();
    

    // (9) connect to listening device (wired speaker, blutooth speaker, headfone etc;)
    app.change_output_device("Bluetooth");
    app.change_output_device("Wired");

    
    return 0;
}