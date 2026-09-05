#include <bits/stdc++.h>
#include <mutex>
#include <chrono>
using namespace std;

// Design a parking lot management system
enum class Vehicle_Type {
    Truck,
    Bus,
    Car,
    Bike
};

class Vehicle {
private:
    Vehicle_Type vt;
    string number_plate; // unique identifier
    double vehicle_length;
public:
    Vehicle(Vehicle_Type vt, string number_plate, double length) {
        this->vt = vt;
        this->number_plate = number_plate;
        this->vehicle_length = length;
    }
    ~Vehicle() {};
    // getters
    Vehicle_Type get_vehicle_type() {
        return this->vt;
    }

    string get_number_plate() {
        return this->number_plate;
    }

    double get_vehicle_length() {
        return this->vehicle_length;
    }
};

class Receipt {
private:
    string number_plate;
    Vehicle_Type vt;
    double price;
public:
    Receipt(string number_plate, Vehicle_Type vt, double price) {
        this->number_plate = number_plate;
        this->vt = vt;
        this->price = price;
    }

    void render_receipt() {
        cout << "Receipt for vehicle " << this->number_plate << endl;
        cout << "Vehicle type : ";
        if (this->vt == Vehicle_Type::Bike) {
            cout << "Bike" << endl;
        } else if (this->vt == Vehicle_Type::Car) {
            cout << "Car" << endl;
        } else if (this->vt == Vehicle_Type::Bus) {
            cout << "Bus" << endl;
        } else if (this->vt == Vehicle_Type::Truck) {
            cout << "Truck" << endl;
        }
        cout << "Price : " << this->price << endl;
    }

    double get_price() {
        return this->price;
    }
    ~Receipt() {};
};

class Level;
// COMPOSITE DESIGN PATTERN FOR SINGLE AND MULTI CONTAINMENT SPOTS
class Spot {
public:
    virtual string print_assigned_spots() = 0;
    virtual void free_spot(Level* l) = 0;
    virtual ~Spot() {};
};

class Single : public Spot {
private:
    int spot_size;
    int spot_length;
    // 0->compact, 1->regular, 2->large
    int spot_id_x;
    int spot_id_y;
public:
    Single(int spot_size, int spot_id_x, int spot_id_y) {
        this->spot_size = spot_size;
        this->spot_id_x = spot_id_x;
        this->spot_id_y = spot_id_y;
        if (spot_size == 0) {
            this->spot_length = 3; //3 meters for compact spot
        } else if (spot_size == 1) {
            this->spot_length = 5; //5 meters for regular spot
        } else {
            this->spot_length = 8; //8 meters for large spot
        }
    }

    string print_assigned_spots() override {
        string loc;
        if (this->spot_size == 0) {
            loc = "C-" + to_string(this->spot_id_x) + "-" + to_string(this->spot_id_y);
        } else if (this->spot_size == 1) {
            loc = "R-" + to_string(this->spot_id_x) + "-" + to_string(this->spot_id_y);
        } else {
            loc = "L-" + to_string(this->spot_id_x) + "-" + to_string(this->spot_id_y);
        }
        string response = "Assigned spot : " + loc + "\n";
        return response;
    }
    void free_spot(Level* l);
};

// this will be a combinarion of multiple similar spots allocated for large vehicles
// combination of multiple single spots
class Combined : public Spot {
private:
    vector<Spot*> spots;
public:
    Combined (vector<Spot*> spots) {
        this->spots = spots;
    }

    string print_assigned_spots() override {
        string response = "";
        for (Spot* s : this->spots) {
            response+=s->print_assigned_spots();
        }
        return response;
    }
    void free_spot(Level* l);
};
// ================================================================













// =============================================================
class Pricing_Strategy {
private:
    static Pricing_Strategy* instance;
    Pricing_Strategy() {};
    ~Pricing_Strategy() {};
public:
    static Pricing_Strategy* get_instance() {
        return Pricing_Strategy::instance;
    }
    double calculate_price(Vehicle_Type vt, long long time_of_entry, long long time_of_exit) {
        double price = 0.0;
        // firstly apply flat rate depending on vehicle
        if (vt == Vehicle_Type::Bike) {
            price += 20.0; // flat rate for bike
        } else if (vt == Vehicle_Type::Car) {
            price += 50.0; // flat rate for car
        } else if (vt == Vehicle_Type::Bus) {
            price += 100.0; // flat rate for bus
        } else if (vt == Vehicle_Type::Truck) {
            price += 150.0; // flat rate for truck
        }

        // then apply hourly charge
        int hours_covered = (time_of_exit - time_of_entry)/(1000 * 60 * 60);
        double hourly_cost = (double) hours_covered * 70.0; //rupees per hour
        price += hourly_cost;

        // then apply nightly charges
        int nights_covered = (time_of_exit - time_of_entry)/(1000 * 60 * 60 * 24);
        double nightly_cost = (double) nights_covered * 200.0; //rupees per night
        price += nightly_cost;

        return price;

    }
};
Pricing_Strategy* Pricing_Strategy::instance = new Pricing_Strategy();
// =============================================================
















// ===============================================================
class Spot_Handler {
private:
    mutex mtx;
    static Spot_Handler* instance;
    Spot_Handler() {};
public:
    static Spot_Handler* get_instance() {
        return Spot_Handler::instance;
    }

    void assign(Level* l, Vehicle* v);
    void unassign(Level* l, Vehicle* v);
};

Spot_Handler* Spot_Handler::instance = new Spot_Handler();

class Level {
private:
    map<string, Spot*> mp; // {vehicle number plate, Spot allocated} : so that when the owner of car asks where is his/her vehicle parked, then we can quickly answer the spot/s allocated to his/her vehicle
    map<string, long long> time_of_entry;
    int total_spots;
    int empty_spots;

    vector<vector<pair<bool, int>>> spots; // {is_empty, spot_size} : 0->compact, 1->regular, 2->large


public:
    Level(int total_spots, vector<vector<pair<bool, int>>> spots) {
        this->total_spots = total_spots;
        this->empty_spots = total_spots;
        // because initially all spots will be empty
        this->spots = spots;
    }
    ~Level() {
        for (auto& [plate, spot] : this->mp) {
            delete spot;
        }
    }

    // setter
    void assign_spot(Vehicle* v) {
        Spot_Handler::get_instance()->assign(this, v);
        auto wall_now = chrono::system_clock::now();
        long long time_ms = chrono::duration_cast<chrono::milliseconds>(wall_now.time_since_epoch()).count();
        this->time_of_entry[v->get_number_plate()] = time_ms;
    }

    void unassign_spot(Vehicle* v) {
        Spot_Handler::get_instance()->unassign(this, v);
        // here the price will be decided
        this->time_of_entry.erase(v->get_number_plate());
    }

    Spot* get_assigned_spot(string plate_number) {
        if (this->mp.find(plate_number) == this->mp.end()) {
            cout << "No such vehicle is present on this level..." << endl;
            return nullptr;
        }

        // vehicle found
        return this->mp[plate_number];
    }

    vector<vector<pair<bool, int>>>& get_spots() {
        return this->spots;
    }
    long long get_time_of_entry(string plate_number) {
        if (this->time_of_entry.find(plate_number) == this->time_of_entry.end()) {
            cout << "No such vehicle is present on this level..." << endl;
            return -1;
        }
        return this->time_of_entry[plate_number];
    }
    map<string, Spot*>& get_map() {
        return this->mp;
    }
};

void Single::free_spot(Level* l) {
    vector<vector<pair<bool, int>>>& spots = l->get_spots();
    spots[this->spot_id_x][this->spot_id_y].first = true;
    cout << "Freed spot : " << this->print_assigned_spots() << endl;
}

void Combined::free_spot(Level* l) {
    for (Spot* s : this->spots) {
        s->free_spot(l);
    }
}

void Spot_Handler::assign(Level* l, Vehicle* v) {
    lock_guard<mutex> lock(Spot_Handler::mtx);
    vector<vector<pair<bool, int>>>& spots = l->get_spots();
    if (v->get_vehicle_type() == Vehicle_Type::Bike) {
        for (int i = 0; i<spots.size(); i++) {
            for (int j = 0; j<spots[i].size(); j++) {
                if (spots[i][j].first == true) {
                    // as bike can take any spot, so we can assign it to any spot
                    spots[i][j].first = false;
                    Spot* s = new Single(spots[i][j].second, i, j);
                    l->get_map()[v->get_number_plate()] = s;
                    cout << "Assigned spot to vehicle " << v->get_number_plate() << " : " << s->print_assigned_spots() << endl;
                    return;
                }
            }
        }
    } else if (v->get_vehicle_type() == Vehicle_Type::Car) {
        for (int i = 0; i<spots.size(); i++) {
            for (int j = 0; j<spots[i].size(); j++) {
                if (spots[i][j].first == true && spots[i][j].second >= 1) {
                    // as car can take regular or large spot, so we can assign it to any regular or large spot
                    spots[i][j].first = false;
                    Spot* s = new Single(spots[i][j].second, i, j);
                    l->get_map()[v->get_number_plate()] = s;
                    cout << "Assigned spot to vehicle " << v->get_number_plate() << " : " << s->print_assigned_spots() << endl;
                    return;
                }
            }
        }
    } else if (v->get_vehicle_type() == Vehicle_Type::Bus || v->get_vehicle_type() == Vehicle_Type::Truck) {
    //    here we need to assign multiple large spots to a bus or truck in same row, so we need to run a fixed sliding window algorithm to find consecutive empty spots, the number of consecutive spots will depend on bus and truck length, 
        double vehicle_length = v->get_vehicle_length();
        int spots_needed = ceil(vehicle_length / 8.0); // as each large spot is 8 meters long

        for (int row = 0; row<spots.size(); row++) {
            int low = 0;
            int r = 0;
            // initial window
            map<int, int> spot_size_count; // {spot_size, count} : to keep track of how many spots of each size are present in the current window
            while (r<spots_needed - 1) {
                spot_size_count[spots[row][r].second]++;
                r++;
            }

            while (r<spots[row].size()) {
                // update
                spot_size_count[spots[row][r].second]++;

                // check
                if (spot_size_count.size() == 1 && spot_size_count.begin()->first == 2) {
                    // all spots in the current window are large spots, so we can assign them to the bus or truck
                    vector<Spot*> assigned_spots;
                    for (int i = low; i<=r; i++) {
                        spots[row][i].first = false;
                        Spot* s = new Single(spots[row][i].second, row, i);
                        assigned_spots.push_back(s);
                    }
                    Spot* combined_spot = new Combined(assigned_spots);
                    l->get_map()[v->get_number_plate()] = combined_spot;
                    cout << "Assigned spots to vehicle " << v->get_number_plate() << " : " << endl << combined_spot->print_assigned_spots() << endl;
                    return;
                }
                // else move
                spot_size_count[spots[row][low].second]--;
                if (spot_size_count[spots[row][low].second] == 0) {
                    spot_size_count.erase(spots[row][low].second);
                }
                low++;
                r++;
            }
        }
        
        
    } 
    else {

        cout << "No available spot for vehicle " << v->get_number_plate() << endl;
    }
}

void Spot_Handler::unassign(Level* l, Vehicle* v) {
    lock_guard<mutex> lock(Spot_Handler::mtx);
    
    Spot* s = l->get_assigned_spot(v->get_number_plate());
    s->free_spot(l);


    delete l->get_map()[v->get_number_plate()];
    l->get_map().erase(v->get_number_plate());
}

// ==============================================================





















// ====PAYMENT SYSTEM {here payment gateway is easily pluggable}=============================
// this uses payment gateway, but that is not our current concern as that is separate system design, we just assume that it uses it
class Payment_Handler_Payload {
protected:
    map<string, string> mp;
public:
    map<string, string> get_attributes() {
        return this->mp;
    }
    virtual ~Payment_Handler_Payload() {};
};

class Cash_Payment_Handler_Payload : public Payment_Handler_Payload {
public:
    Cash_Payment_Handler_Payload(double amount) {
        this->mp["amount"] = to_string(amount);
    }
};

class Card_Payment_Handler_Payload : public Payment_Handler_Payload {
public:
    Card_Payment_Handler_Payload(double amount, string card_number, string card_holder_name, string expiry_date, string cvv) {
        this->mp["amount"] = to_string(amount);
        this->mp["card_number"] = card_number;
        this->mp["card_holder_name"] = card_holder_name;
        this->mp["expiry_date"] = expiry_date;
        this->mp["cvv"] = cvv;
    }
};

class UPI_Payment_Handler_Payload : public Payment_Handler_Payload {
public:
    UPI_Payment_Handler_Payload(double amount, string upi_id) {
        this->mp["amount"] = to_string(amount);
        this->mp["upi_id"] = upi_id;
    }
};

enum class Payment_Method {
    Cash,
    Card,
    UPI
};

class Payment_Handler {
public:
    virtual ~Payment_Handler() {};
    virtual void make_payement(Payment_Handler_Payload* payload) = 0;
};

class Cash_Payment_Handler : public Payment_Handler {
private:
    mutex mtx;
public:
    void make_payement(Payment_Handler_Payload* payload) override {
        lock_guard<mutex> lock(this->mtx);
        cout << "Payment made using cash..." << endl;
    }
};

class Card_Payment_Handler : public Payment_Handler {
private:
    mutex mtx;
public:
    void make_payement(Payment_Handler_Payload* payload) override {
        lock_guard<mutex> lock(this->mtx);      
        cout << "Payment made using card..." << endl;
    }
};

class UPI_Payment_Handler : public Payment_Handler {
private:
    mutex mtx;
public:
    void make_payement(Payment_Handler_Payload* payload) override {
        lock_guard<mutex> lock(this->mtx);
        cout << "Payment made using UPI..." << endl;
    }
};
// ===============================================
















// Interactive class for the system
class Facade {
private:
    // this will have several levels
    vector<Level*> levels;
    map<Payment_Method, Payment_Handler*> payment_handlers;
    // we maintain a map as a parking lot manager should have maximum possible handlers for each payment method, so that if one handler is busy, then another handler can take care of the payment
public:
    Facade(vector<Level*> levels) {
        this->levels = levels;
        this->payment_handlers[Payment_Method::Cash] = new Cash_Payment_Handler();
        this->payment_handlers[Payment_Method::Card] = new Card_Payment_Handler();
        this->payment_handlers[Payment_Method::UPI] = new UPI_Payment_Handler();
        // we can add more payment handlers here if needed
    }

    ~Facade() {
        for (auto& [method, handler] : this->payment_handlers) {
            delete handler;
        }
    }

    // delegated functionalities
    void assign_spot(int level, Vehicle* v) {
        if (level < 0 || level >= this->levels.size()) {
            cout << "Invalid level number..." << endl;
            return;
        }
        this->levels[level]->assign_spot(v);
    }
    Receipt* free_spot(int level, Vehicle* v) {
        if (level < 0 || level >= this->levels.size()) {
            cout << "Invalid level number..." << endl;
            return nullptr;
        }
        auto wall_now = chrono::system_clock::now();
        long long time_ms_of_exit = chrono::duration_cast<chrono::milliseconds>(wall_now.time_since_epoch()).count();

        // Calculate price and generate receipt
        double price = Pricing_Strategy::get_instance()->calculate_price(v->get_vehicle_type(),this->levels[level]->get_time_of_entry(v->get_number_plate()), time_ms_of_exit);
        Receipt* receipt = new Receipt(v->get_number_plate(), v->get_vehicle_type(), price);
        this->levels[level]->unassign_spot(v);
        // NOW MAKE PRICE CALCULATION AND GENERATE RECEIPT
        
        return receipt;
    }
    void get_assigned_level(int level, Vehicle* v) {
        
        Spot* s = this->levels[level]->get_assigned_spot(v->get_number_plate());
        if (s == nullptr) {
            cout << "No such vehicle is present on this level..." << endl;
            return;
        }

        cout << s->print_assigned_spots() << endl;

    }
    void make_payment(Receipt* receipt, Payment_Method method, Payment_Handler_Payload* payload) {
        if (this->payment_handlers.find(method) == this->payment_handlers.end()) {
            cout << "No payment handler available for this payment method..." << endl;
            return;
        }
        this->payment_handlers[method]->make_payement(payload);
    }
};









int main() {
    // demonstration here

    // (1) create leveles
    vector<Level*> levels;

    vector<vector<pair<bool, int>>> spots_level_1 = {
        {{true, 0}, {true, 0}, {true, 0}, {true, 1}, {true, 1}, {true, 1}, {true, 2}, {true, 2}, {true, 2}},
        {{true, 0}, {true, 0}, {true, 0}, {true, 1}, {true, 1}, {true, 1}, {true, 2}, {true, 2}, {true, 2}},
        {{true, 0}, {true, 0}, {true, 0}, {true, 1}, {true, 1}, {true, 1}, {true, 2}, {true, 2}, {true, 2}}
    };

    Level* level_1 = new Level(27, spots_level_1);
    levels.push_back(level_1);

    vector<vector<pair<bool, int>>> spots_level_2 = {
        {{true, 0}, {true, 0}, {true, 0}, {true, 1}, {true, 1}, {true, 1}, {true, 2}, {true, 2}, {true, 2}},
        {{true, 0}, {true, 0}, {true, 0}, {true, 1}, {true, 1}, {true, 1}, {true, 2}, {true, 2}, {true, 2}},
        {{true, 0}, {true, 0}, {true, 0}, {true, 1}, {true, 1}, {true, 1}, {true, 2}, {true, 2}, {true, 2}}
    };
    
    Level* level_2 = new Level(27, spots_level_2);
    levels.push_back(level_2);

    // (2) create facade
    Facade* facade = new Facade(levels);

    // (3) create vehicles
    Vehicle* bike_1 = new Vehicle(Vehicle_Type::Bike, "BIKE-1", 1.5);
    Vehicle* car_1 = new Vehicle(Vehicle_Type::Car, "CAR-1", 4.0);
    Vehicle* bus_1 = new Vehicle(Vehicle_Type::Bus, "BUS-1", 12.0);
    Vehicle* truck_1 = new Vehicle(Vehicle_Type::Truck, "TRUCK-1", 15.0);

    // (4) assign spots
    facade->assign_spot(0, bike_1);
    facade->assign_spot(0, car_1);
    facade->assign_spot(0, bus_1);
    facade->assign_spot(0, truck_1);

    // // (5) get assigned spots
    facade->get_assigned_level(0, bike_1);
    facade->get_assigned_level(0, car_1);
    facade->get_assigned_level(0, bus_1);
    facade->get_assigned_level(0, truck_1);

    // // (6) free spots and generate receipts
    Receipt* receipt_bike = facade->free_spot(0, bike_1);
    receipt_bike->render_receipt();

    Receipt* receipt_car = facade->free_spot(0, car_1);
    receipt_car->render_receipt();

    Receipt* receipt_bus = facade->free_spot(0, bus_1);
    receipt_bus->render_receipt();

    Receipt* receipt_truck = facade->free_spot(0, truck_1);
    // cout << "Receipt for truck : " << endl;
    receipt_truck->render_receipt();

    // (7) make payloads, then payment
    Payment_Handler_Payload* payload_bike = new Cash_Payment_Handler_Payload(receipt_bike->get_price());
    facade->make_payment(receipt_bike, Payment_Method::Cash, payload_bike);

    Payment_Handler_Payload* payload_car = new Card_Payment_Handler_Payload(receipt_car->get_price(), "1234-5678-9012-3456", "John Doe", "12/25", "123");
    facade->make_payment(receipt_car, Payment_Method::Card, payload_car);

    Payment_Handler_Payload* payload_bus = new UPI_Payment_Handler_Payload(receipt_bus->get_price(), "john@upi");
    facade->make_payment(receipt_bus, Payment_Method::UPI, payload_bus);

    Payment_Handler_Payload* payload_truck = new Cash_Payment_Handler_Payload(receipt_truck->get_price());
    facade->make_payment(receipt_truck, Payment_Method::Cash, payload_truck);

    // (8) clean memory
    delete bike_1;
    delete car_1;
    delete bus_1;
    delete truck_1;

    delete receipt_bike;
    delete receipt_car;
    delete receipt_bus;
    delete receipt_truck;
    delete facade;



    return 0;
}