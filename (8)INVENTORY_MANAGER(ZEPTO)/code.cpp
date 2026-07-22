#include <bits/stdc++.h>
using namespace std;


// =============== INVENTORIES AND REPLENISH STRATEGIES ARE INTERLINKED USING OBSERVER DESIGN PATTERN==================
class Inventory;

class Replenish_Strategy {
protected:
    Inventory* I;
public:
    void set_inventory(Inventory* I) {
        this->I = I;
    }
    virtual void replenish () = 0;
};

class Weekly : public Replenish_Strategy {
public:
    void replenish ();
};

class Monthly : public Replenish_Strategy {
public:
    void replenish ();
};

class Threshold : public Replenish_Strategy {
private:
    int min_limit;
public:
    Threshold (int min_limit) {
        this->min_limit = min_limit;
    }

    void replenish ();
};

class Replenish_Strategy_Factory {
private:
    static Replenish_Strategy_Factory* instance;
    Replenish_Strategy_Factory () {};
public:
    static Replenish_Strategy_Factory* get_instance() {
        return Replenish_Strategy_Factory::instance;
    }

    Replenish_Strategy* create(string name, map<string, int>& attributes) {
        if (name == "Monthly") {
            cout << "Set Replenishing Method to Monthly..." << endl;
            return new Monthly();
        } else if (name == "Weekly") {
            cout << "Set Replenishing Method to Weekly..." << endl;
            return new Weekly();
        } else if (name == "Threshold") {
            cout << "Set Replenishing Method to Threshold..." << endl;
            return new Threshold(attributes["min_limit"]);
        }
        cout << "This replenish method is not supported yet..." << endl;
        return nullptr;

    }
};

Replenish_Strategy_Factory* Replenish_Strategy_Factory::instance = new Replenish_Strategy_Factory();

// invectory is product specefic
class Inventory {
private:
    int max_limit;
    int quantity;
    double product_price;
    int item_id;
    string product_name;
    string last_replenish_date;
    Replenish_Strategy* rs;
public:
    Inventory (int item_id, string product_name, double product_price, int max_limit) {
        this->item_id = item_id;
        this->product_price = product_price;
        this->product_name = product_name;
        this->quantity = 0;
        this->max_limit = max_limit;
        this->rs = nullptr;
        this->last_replenish_date = "";
    }

    int get_id () {
        return this->item_id;
    }

    void set_replenish_strategy (string name, map<string, int> attributes) {
        this->rs = Replenish_Strategy_Factory::get_instance()->create(name, attributes);
        if (this->rs!=nullptr) {
            this->rs->set_inventory(this);
        }
    }

    void add_item(int count) {
        this->quantity+=count;
        this->quantity = min(this->max_limit, this->quantity);
        cout << "Added " << count << " " << this->product_name << " to inventory..." << endl;
        // update replenish date (this will set todays date, but dor demo purposes, we are assigning random date)
        this->last_replenish_date = "22/07/2026";
    }

    void remove_item(int count) {
        this->quantity-=count;
        this->quantity = max(0, this->quantity);
        cout << "Removed " << count << " " << this->product_name << " from inventory..." << endl;
        cout << "Remaining : " << this->quantity << endl; 

        // call replenish method of strategy
        if (this->rs!=nullptr) {
            this->rs->replenish();
        }
        
    }

    int get_limit () {
        return this->max_limit;
    }
    int get_quantity() {
        return this->quantity;
    }

    ~Inventory () = default;
};

void Weekly::replenish() {
    this->I->add_item(this->I->get_limit());
}

void Monthly::replenish() {
    this->I->add_item(this->I->get_limit());
}

void Threshold::replenish() {
    if (this->I->get_quantity() < this->min_limit) {
        this->I->add_item(this->I->get_limit());
    }
}
// =========================================================



// ===================INVENTORY STORE=============

class Inventory_Store {
public:
    virtual Inventory* add_new_item_inventory(int item_id, string item_name, double product_price, int max_limit) = 0;
    virtual void remove_item_inventory(int item_id) = 0;
    virtual void set_replenish_strategy(int item_id, string name, map<string, int> attributes) = 0;
    virtual void add_item_amount(int item_id, int count) = 0;
    virtual int get_available_stock(int item_id) = 0;
    virtual bool check_feasability(int item_id, int demanded_quantity) = 0;
    virtual void get_item(int item_id, int demanded_quantity) = 0;
    // there can be more functions, but 
};

// below are the concrete methods of implementing inventory store
class SQL_Inventory_Store : public Inventory_Store {
public:
    Inventory* add_new_item_inventory(int item_id, string item_name, double product_price, int max_limit) {
        cout << "Added item to SQL store..." << endl;
    }
    void remove_item_inventory(int item_id) {
        cout << "Removed item from SQL store..." << endl;
    }
   
    void set_replenish_strategy(int item_id, string name, map<string, int> attributes) {
        cout << "Replenish_Strategy handled ..." << endl;
    }
    void add_item_amount(int item_id, int count) {
        cout << "item amount added to stocks..." << endl;
    }
    int get_available_stock(int item_id) {
        return 40;
    }
    bool check_feasability(int item_id, int demanded_quantity) {
        return true;
    }
    void get_item(int item_id, int demanded_quantity) {
        // this reduces cout of item from db
        cout << "item amount reduced ..." << endl;
    }
};

class NoSQL_Inventory_Store : public Inventory_Store {
public:
    Inventory* add_new_item_inventory(int item_id, string item_name, double product_price, int max_limit) {
        cout << "Added item to NoSQL store..." << endl;
    }
    void remove_item_inventory(int item_id) {
        cout << "Removed item from NoSQL store..." << endl;
    }
    
    void set_replenish_strategy(int item_id, string name, map<string, int> attributes) {
        cout << "Replenish_Strategy handled ..." << endl;
    }
    void add_item_amount(int item_id, int count) {
        cout << "item amount added to stocks..." << endl;
    }
    int get_available_stock(int item_id) {
        return 40;
    }
    bool check_feasability(int item_id, int demanded_quantity) {
        return true;
    }
    void get_item(int item_id, int demanded_quantity) {
        // this reduces cout of item from db
        cout << "item amount reduced ..." << endl;
    }
};

class IS_Factory {
private:
    static IS_Factory* instance;
    IS_Factory() {};
public:
    static IS_Factory* get_instance() {
        return IS_Factory::instance;
    }

    Inventory_Store* create(string name) {
        if (name == "SQL") {
            return new SQL_Inventory_Store();
        } else if (name == "NoSQL") {
            return new NoSQL_Inventory_Store();
        }
        cout << "This type of Inventory_Store is not supported yet..." << endl;
        return nullptr;
    }
};

IS_Factory* IS_Factory::instance = new IS_Factory();

// =================================================


// ================DARKSTORES=======================

class Location {
private:
    double longitude;
    double latitude;
public:
    Location (double lat, double lon) {
        this->latitude = lat;
        this->longitude = lon;
    }
    // getters
    double get_latitude () {
        return this->latitude;
    }
    double get_longitude () {
        return this->longitude;
    }
};

class Darkstore {
private:
    string storeId;
    Location* geo;
    bool is_open;
    Inventory_Store* is;

    map<int, Inventory*> inventories;
public:
    Darkstore (string storeId, Location* geo) {
        this->geo = geo;
        this->storeId = storeId;
        this->is_open = false;
        this->is = nullptr;
    }

    // function to open and close the store to display availability
    void open_store() {
        this->is_open = true;
    }

    void close_store() {
        this->is_open = false;
    }

    bool is_store_open() {
        return this->is_open;
    }

    Location* get_location () {
        return this->geo;
    }
    // more functionalities

    void set_inventory_store(string name) {
        this->is = IS_Factory::get_instance()->create(name);
    }

    
    // all inventory store functionalities will be deligated using Inventory store
    void add_new_item_inventory(int item_id, string item_name, double product_price, int max_limit) {
        this->inventories[item_id] = this->is->add_new_item_inventory(item_id, item_name, product_price, max_limit);
    }
    void remove_item_inventory(int item_id) {
        this->is->remove_item_inventory(item_id);
    }
    Inventory* get_inventory(int item_id) {
        
        return this->inventories[item_id];
    }
    void set_replenish_strategy(int item_id, string name, map<string, int> attributes) {
        this->is->set_replenish_strategy(item_id, name, attributes);
    }
    void add_item_amount(int item_id, int count) {
        this->is->add_item_amount(item_id, count);
    }
    int get_available_stock(int item_id) {
        return this->is->get_available_stock(item_id);
    }
    bool check_feasability(int item_id, int demanded_quantity) {
        return this->is->check_feasability(item_id, demanded_quantity);
    }
    void get_item(int item_id, int demanded_quantity) {
        this->is->get_item(item_id, demanded_quantity);
    }


    // destructor 
    ~Darkstore() {
        delete this->geo;
        delete this->is;
    }
};

// =================================================

// =============now below are classes related to user side (order, ordermanager, user, finder etc.)
// ==================================================================================================


// =============closest darkstore finder(within k km):for user interface ; this is a heavy computation class, so we will make it singleton ==============================
class Closest_Darkstores_Finder {
private:
    static Closest_Darkstores_Finder* instance;
    Closest_Darkstores_Finder () {};
public:
    static Closest_Darkstores_Finder* get_instance () {
        return Closest_Darkstores_Finder::instance;
    }
    vector<Darkstore*> find_closest_darkstores(Location* user_location, int km) {
        // this does a dijkstra inside using user location , db ,etc. and then gives a vector of closest darkstores
        // for demo purposes we are returning a empty vector
        return {};
    }
};

Closest_Darkstores_Finder* Closest_Darkstores_Finder::instance = new Closest_Darkstores_Finder();

// ===================================================================================================

class User {
private:
    string mobile;
    string name;
    Location* geo;
public:
    User (string name, string mobile, Location* loc) {
        this->name = name;
        this->mobile = mobile;
        this->geo = loc;
    }
    // getters
    string get_name() {
        return this->name;
    }

    string get_mobile () {
        return this->mobile;
    }

    Location* get_location () {
        return this->geo;
    }

    ~User () {
        delete this->geo;
    }
};


// ===========================order related=============================================================
// delivery partners lcocation is used by app gps for live display on refresh to user
class Delivery_Partner {
private:
    string name;
    Location* geo;
    string mobile;
public:
    Delivery_Partner (string name, string mobile, Location* loc){
        this->name = name;
        this->mobile = mobile;
        this->geo = loc;
    }
    // getters
    string get_name() {
        return this->name;
    }

    string get_mobile () {
        return this->mobile;
    }

    Location* get_location () {
        return this->geo;
    }

    ~Delivery_Partner () {
        delete this->geo;
    }
};

class Order {
private:
    User* customer;
    map<int, vector<pair<int, vector<pair<int, int>>>>> delivery_partners;
    // order contains a map{delivery_partner_id : vector of pair{darkstore id, vector<item ids they have to pick, quantity>}}
    // means this delivery guy has to pick the given list of items from respective stores
    int order_id;

public:
    Order (User* u, int order_id) {
        this->customer = u;
        this->order_id = order_id;
    }
    // setters
    void assign(int delivery_partner_id, pair<int, vector<pair<int, int>>> assigned_items) {
        this->delivery_partners[delivery_partner_id].push_back(assigned_items);
    }

    User* get_customer () {
        return this->customer;
    }

    int get_order_id () {
        return this->order_id;
    }
};


// assign order to delivery partner, user, all darkstores required if one cannot fulfil it, this class directly uses db for all this, so it is an abstract class
class Order_Manager {
public:
    virtual Order* create_order (User* u, map<int, vector<pair<int, int>>> cart) = 0;   
};

// creates order object using db and returns to user facade
class SQL_Order_Manager : public Order_Manager {
public:
    Order* create_order(User* u, map<int, vector<pair<int, int>>> cart) {
        // searches and uses the sql db and return an order object
        // for demo, we are returning nullptr
        return nullptr;
    }
};

class MONGODB_Order_Manager : public Order_Manager {
public:
    Order* create_order(User* u, map<int, vector<pair<int, int>>> cart) {
        // searches and uses the mongo db and return an order object
        // for demo, we are returning nullptr
        return nullptr;
    }
};

class Order_Manager_Factory {
private:
    static Order_Manager_Factory* instance;
    Order_Manager_Factory () {};
public:
    static Order_Manager_Factory* get_instance() {
        return Order_Manager_Factory::instance;
    }

    Order_Manager* create_manager (string name) {
        if (name == "SQL") {
            return new  SQL_Order_Manager();
        } else if (name == "MONGODB") {
            return new MONGODB_Order_Manager();
        }
        cout << "This manager is not supported yet..." << endl;
        return nullptr;
    }
};

Order_Manager_Factory* Order_Manager_Factory::instance = new Order_Manager_Factory();

// =========================================================================================


class User_Facade {
private:
    User* u;
    Order* current_order;
    Order_Manager* om;
    vector<Darkstore*> closest_darkstores;
    // these are the darkstores shown to user within his/her mentioned preferred distance radius
    int preferred_radius;
    map<int, vector<pair<int, int>>> cart;
    // cart {storeId, list of itemids chosen from this store}, item chosen by user, and store chosen by system
public:
    User_Facade (User* u, string om_type) {
        this->u = u;
        this->current_order = nullptr;
        this->om = Order_Manager_Factory::get_instance()->create_manager(om_type);
    }

    void set_radius (int km) {
        this->preferred_radius = km;
        // on setting km preference, darkstores will be loaded instantly and shown to user
        this->closest_darkstores = Closest_Darkstores_Finder::get_instance()->find_closest_darkstores(this->u->get_location(), this->preferred_radius);
    }

    void add_item_to_cart(int storeId, int item_id, int quantity) {
        this->cart[storeId].push_back({item_id, quantity});
    }
    void remove_item_from_cart(int storeId, int item_id, int quantity) {
        int target_index = -1;
        int n = this->cart[storeId].size();
        for (int i = 0; i<n; i++) {
            pair<int, int>& it = this->cart[storeId][i];
            if (it.first == item_id) {
                target_index = i;
                break;
            }
        }
        if (target_index!=-1) {
            this->cart[storeId][target_index].second-=quantity;
        }
    }

    void make_order() {
        // this uses order manager to finalise this order using the items in cart
        this->current_order = this->om->create_order(this->u, this->cart);
    }
};

// this app is both store side and user side also
int main() {
    

    return 0;
}