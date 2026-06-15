#include <bits/stdc++.h>
using namespace std;


// IT IS GIVEN IN QUESTION THAT PAYMENT SERVICE IS ALREADY INTEGREATED AND WE HAVE TO DIRECTLY USE IT AS CREATING A PAYMENT DESIGN IS IN ITSELF IS A LLD QUESTION

// ALSO, THIS WE HAVE TO CREATE IS IN RESPECT OF USER ONLY, SO WE HAVE TO PROVIDE FUNCTIONALITIES OF USER PERSPECTIVE ONLY

// USER LOGS IN/CREATES ACCOUNT-> BROWSES RESTARAUNTS-> CAN LOAD MORE RESTARAUNTS-> SELECT A RESTARAUNT-> BROWSE ITS MENU(DISPLAY MENU)-> ADD ITEMS TO CART-> PLACE ORDER(MAKE PAYMENT(ALREADY PROVIDED))-> RESTARAUNT SEND NOTIFICATION TO USER OF SUCCESSFUL ORDER


// MODEL CLASSES
class item {
private:
    string name;
    string price;
    item (string name, string price) {
        this->name = name;
        this->price = price;
    }
    // constructor only callable by  loader class during creation of items to be inserted in menu of corresponding restro

public:
    // this getter only allows loader to call it (encapsulation)
    static item get_item(string name, string price) {
        item it(name, price);
        return it;
    }

    string get_price () {
        return this->price;
    }
    string get_name () {
        return this->name;
    }
};

class Cart {
private:
    vector<pair<item, string>> cart_items;
public:
    void add_to_cart(item& i, string restaraunt_name) {
        cart_items.push_back({i, restaraunt_name});
        cout << "item addedd to cart..." << endl;
    }
    void remove_item(item& i, string restaraunt_name) {
        int idx = 0;

        for (auto& it : this->cart_items) {
            if (it.first.get_name() == i.get_name() && it.second==restaraunt_name) {
                // item found;
                break;
            }
            idx++;
        }

        if (idx < this->cart_items.size()) {
            this->cart_items.erase(this->cart_items.begin() + idx);
            cout << i.get_name() << " removed from cart.." << endl;
        }
    }
    vector<item> get_items () {
        vector<item> ans;
        for (auto& it : this->cart_items) {
            ans.push_back(it.first);
        }
        return ans;
    }
};

class User {
private:
    string name, password, mobile, address;
    Cart c;
public:
    User (string name, string password, string mobile, string address) {
        this->name = name;
        this->password = password;
        this->mobile = mobile;
        this->address = address;
    }
    string get_address() {
        return this->address;
    }
    string get_mobile() {
        return this->mobile;
    }
    Cart& get_cart() {
        return this->c;
    }
    vector<item> get_cart_items () {
        return c.get_items();
    }
};



// AUTH RELATED CLASSES
class Auth_handler {

public:
    virtual void register_user(User& u) = 0;
    virtual void login(string username, string password) = 0;
    ~Auth_handler() = default;
};

class GoogleAuth : public Auth_handler{
public:
    void register_user (User& u) override {
        cout << "registered to Google accounts ..." << endl;
    }
    void login (string username, string password) override {
        cout << "logged in to Google accounts ..." << endl;
    }
};

class GithubAuth : public Auth_handler{
public:
    void register_user (User& u) override {
        cout << "registered to Github ..." << endl;
    }
    void login (string username, string password) override {
        cout << "logged in to GIthub ..." << endl;
    }
};

class Auth_Factory {
public:
    static Auth_handler* initialise_auth(string type) {
        if (type == "GoogleAuth") {
            return new GoogleAuth();
        } else if (type == "GithubAuth") {
            return new GithubAuth();
        }
        else {
            cout << "this method is not available yet..." << endl;
            return nullptr;
        }
    }
    ~Auth_Factory() = default;
};




// UTILITY CLASS FOR RENDERING A LIST OF ITEMS
class items_renderer {
public:
    void display(const vector<item>& item_set) {
        cout << "------------items--------------" << endl;
        for (item it : item_set) {
            cout <<"Name: " <<it.get_name() << endl;
            cout <<"Price: "<<it.get_price()<<endl;
            cout << endl;
        }
        cout <<"-------------------------------" << endl;
    }
};



// MODEL CALSS FOR RESTARAUNT MENU
class Menu {

private:
    // set to prevent duplicate etries, removal and update and insertion is also very fast, 
    vector<item> item_set;

    Menu (vector<item>& items) {
        // this is only callable by loader class while creation of restro menus after fetching from dbs
        this->item_set = items;
    }

public:
    Menu() {};
    static Menu get_menu(vector<item>& items) {
        Menu m(items);
        return m;
    }

    vector<item> get_items() {
        return item_set;
    }
};




// UTILITY CLASS FOR GENERATING AVAILABLE RANDOM ID FOR NEW ORDER
class order_id_generator {
public:
    static int get_id() {
        // this searches the db and generates an available id
        // for demonstration purposes, we are just returning 1
        return 1;
    }
};

// abstract class also to support different types of orders
class Order {
protected:

    int order_id;
    string address;
    string mobile;
    vector<item> ordered_items;
public:
    // price calculation vary on types of order (ocp)
    virtual get_total_price() = 0;
};

// now order can be of two types,home delivery,
class Home_Order : public Order{
public:
    Home_Order(string address, string mobile, vector<item> chosen_items_from_cart) {
        this->ordered_items = chosen_items_from_cart;
        this->mobile = mobile;
        this->address =address;
        this->order_id = order_id_generator::get_id();
        cout << "your order is ready to be home delivered..." << endl;
    }
    int get_total_price() {
        int total = 0;
        for (item& it : this->ordered_items) {
            total+=stoi(it.get_price());
        }
        return total + 100;
        // home delivery takes 100 rupees delivery charge
    }
};

class Pickup_Order : public Order{
public:
    Pickup_Order(string address, string mobile, vector<item> chosen_items_from_cart) {
        this->ordered_items = chosen_items_from_cart;
        this->mobile = mobile;
        this->address =address;
        this->order_id = order_id_generator::get_id();
        cout << "your order is done, be ready to pick it up from restaraunt..." << endl;
    }
    int get_total_price() {
        int total = 0;
        for (item& it : this->ordered_items) {
            total+=stoi(it.get_price());
        }
        return total;
        // no charge for pickup
    }
};




// ORDER HANDLER CLASSES
class Order_handler {
public:
    virtual bool handle_order(Order* o) = 0;
    virtual ~Order_handler() = default;
};

class SQL_Order_handler : public Order_handler {
public:
    SQL_Order_handler () {};
    bool handle_order(Order* o) {
        cout << "checking sql db for handling this order requirements..." << endl;
        // returning true here for demo purposes
        return true;
    }
};

class MONGODB_Order_handler : public Order_handler {
public:
    MONGODB_Order_handler () {};
    bool handle_order(Order* o) {
        cout << "checking Mongo db for handling this order requirements..." << endl;
        // returning true here for demo purposes
        return true;
    }
};
 
// factory method so that other classes just get object without carrying about implementation
class Order_handler_Factory {
public:
    static Order_handler* create_handler(string db_type) {
        if (db_type == "SQL") {
            return new SQL_Order_handler();
        } else if (db_type == "MongoDB") {
            return new MONGODB_Order_handler();
        } else {
            cout << "this db is not suppoted yet..." << endl;
            return nullptr;
        }
    }
};



class Notification_Service {
public:
    virtual void send(string message) = 0;
    ~Notification_Service() = default;
};

class Whatsapp_Service {
public:
    void send(string message) {
        cout << "whatsapp notification sent..." << endl;
    }
};
class SMS_Service {
public:
    void send(string message) {
        cout << "SMS notification sent..." << endl;
    }
};


class Notification_Service_Factory {
public:
    static Notification_Service* create_sender (string type) {
        if (type == "Whatsapp") {
            return new Whatsapp_Service();
        }
        else if (type == "SMS") {
            return new SMS_Service();
        } 
        else {
            cout << "this channel is not supported yet..." << endl;
            return nullptr;
        }
    }

};



// restaraunt interface which user chooses
class Restaraunt {
private: 
    string name;
    Menu m;
    Order_handler* oh;
    Notification_Service* noti; 
    // this constructor is called by REstro_loader only when it fetches restaraunts frombackend and creates objects to insert in current_restaraunts list only, this is not called by user
    Restaraunt (string name, Menu m, string db_inuse, string notification_serive_type) {
        this->name = name;
        this->m = m;
        // make order handler according to the db associated with this restro(use factory method to handle it)
        this->oh = Order_handler_Factory::create_handler(db_inuse);
        this->noti = Notification_Service_Factory::create_sender(notification_serive_type);
    }
public:
    Restaraunt () {};
    static Restaraunt get_restro(string name, Menu m, string db_inuse, string notification_serive_type) {
        // we made a getter for restaraunt creation as only the loader can create restro objects after fetching from db
        Restaraunt r(name, m, db_inuse);
        return r; 
    }
    
    string get_name() {
        return this->name;
    }
    // all responsibilites are delegated to other specific classes

    vector<item> get_menu() {
        return m.get_items();
    }

    // order related operations of restaraunt

    void handle_order(Order* o) {
        if (this->oh==nullptr) {
            cout << "Order handler is not initialised yet, invalid db type was given..." << endl;
            return;
        }
        bool response = this->oh->handle_order(o);
        if (response) {
            cout << "order made, now please make payment..." << endl;
            send_notication("your order is ready, make sure to accept it");
        } else {
            cout << "order declined by restaraunt..." << endl;
        }
    }

    void send_notification (string message) {
        if (this->noti == nullptr) {
            cout <<"notification service not initialised..." << endl;
            return;
        }
        this->noti->send(message);

    }

    ~Restaraunt() {
        delete this->oh;
        delete this->noti;
    }
};






// RESTRO LOADER LCASSES
class Restro_loader {
public:
    // this function analyses which was the last restaraunt in diplay to user, and gets next 20 fro backend using pagination and addds them to current_restaraunts after making Restaraunt object of them
    void load (vector<Restaraunt>& current_restaraunts) {
        cout << "more restaraunts are available..." << endl;
    }
    ~Restro_loader() = default;
};






class Payment_handler {
public:
    bool make_payment(User& u, int target_amount, string payment_method) {
        // this will access db and use that already built payment service according to provided payment method, 

        // for demonstration purposes, we are returning true
        return true;
    }
};



// app interface for user
class App {
private:
    items_renderer tool;
    vector<Restaraunt>current_restaraunts;
    Auth_handler* au;
    
    Payment_handler ph;
    string app_db_name = "SQL";
    Restro_loader loader;
    // this is the restaraunt that the user is currently browsing
    Restaraunt* current_opened_restaraunt;

public:
    App (string type) {
        this->current_opened_restaraunt = nullptr;
        this->au = Auth_Factory::initialise_auth(type);

        
    }
    void create_account(User& u) {
        this->au.register_user(u);
    }
    void login(string username, string password) {
        this->au.login(username, password);
    }

    void load_more_restaraunts() {
        this->loader.load(current_restaraunts);
    }

    void select_restaraunt(int restro_idx) {
        this->current_opened_restaraunt = &this->current_restaraunts[restro_idx];
    }
    void display_menu() {
        this->tool.display(this->current_opened_restaraunt->get_menu());
    }
    void add_item_to_cart(User& u, item& i) {
        u.get_cart().add_to_cart(i, this->current_opened_restaraunt->get_name());
    }

    void remove_from_cart(User& u, item& i) {
        u.get_cart().remove_item(i, this->current_opened_restaraunt->get_name());
    }

    int make_order(User& u, string type) {
        // make order of items chosen in users cart
        Order* o;
        if (type == "Pickup") {
            o = new Pickup_Order(u.get_address(), u.get_mobile(), u.get_cart_items());
        } else if (type == "Home Delivery") {
            o = new Home_Order(u.get_address(), u.get_mobile(), u.get_cart_items());
        } else {
            cout << "invalid order type" << endl;
            return 0;
        }
        
        this->current_opened_restaraunt->handle_order(o);
        // print the details of order to user
        this->tool.display(u.get_cart_items());
        cout <<"total price : "<<o->get_total_price() << endl;
        return o->get_total_price();

    }

    void make_payment(User& u, string payment_method, int target_amount) {
        // as it is given by the question that payment is already built
        if (target_amount == 0) {
            cout << "please select some items" << endl;
            return;
        }
        bool response = ph.make_payment(u, target_amount, payment_method);
        if (response) {
            cout << "payment successful..." << endl;
        } else {
            cout << "payment failed..." << endl;
        }
    }

    ~App() {
        delete this->au;
        
        this->current_opened_restaraunt = nullptr;
        

    }
};


int main () {
    // create app instance with type of authentication
    App app("GoogleAuth"); 

    // create user
    User u("Raghu", "c6h6123", "9677500231", "Mill view Road, Delhi");

    // lgin to app or create if account not exists
    app.create_account(u);

    // load more restaraunts if user wants
    app.load_more_restaraunts();

    // choose a restaraunt to browse
    // int restro_idx = 4; 
    // app.select_restaraunt(restro_idx);
    // giving a random index of app's current restro list for demonstrating selection of restaraunt (for demonstration purposes)

    // now user can display menu
    // app.display_menu();

    // add item from menu to cart by clicking on item
    // create random items
    // int item_index_from_menu = 4;
    
    // // we can have more
    // int item_index_from_menu_2 = 7;
    // // now user can add to his/her cart easily
    // app.add_item_to_cart(u, item_index_from_menu);
    // app.add_item_to_cart(u, item_index_from_menu_2);

    // // similary we can also remove from cart

    // // now make order of items in cart
    // int amount = app.make_order(u);

    // // now make payment
    // app.make_payment(u, "Cash on Delivery", amount);

}