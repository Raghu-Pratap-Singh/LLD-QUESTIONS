#include <bits/stdc++.h>
#include <windows.h>
using namespace std;

// PAYMENT GATEWAY:
// what is a gateway : the classes or a model through which the call from another application is reaching the payment site/app/model is called gateway

// we can have different gateway, paytm gateway, phonepe gateway, razorpay gateway to talk to different apps, external calls

// happy flow : 
// client gives us a payment request (all details about payment, acc no, bank name, amount requested, etc, target account number, password) to our gateway chosen, then it will talk to the respective paytm, razorpay banking system, then get rsponse of success or decline, then we will see furthur of repeat, or instant decline

// requirements :

// (1) multiple gateways should be supported, (ocp, strategy, factory) 
// (2) we can easily add new gateways in fututre
// (3) there should be a standard payment flow, with required validations, (template method)

// (4) ERROR HANDLING : (LOGGING oF ERRORS), retry mechanisms based on some fixed count of repetetions,

// power function using binary exponentiation 
int pow (int a, int b) {
    int res = 1;
    while (b) {
        if (b & 1) {
            res = res * a;
        }
        b>>=1;
        a = a * a;
    }
    return res;
}

// ==================== MODEL CLASSES ======================
// sender credentials class
class Sender_Credentials {
private:
    map<string, string> credentials;
public:
    Sender_Credentials () {};
    // setters and getters
    void set_credentials(map<string, string> mp) {
        this->credentials = mp;
    }

    map<string, string>& get_credentials() {
        return this->credentials;
    }
};

// this is the payload which is sent to gateway for processing
class Payload {
private:
    // target credentials
    string merchant_ID;
    string secret_token;
    // amount and context
    string amount;
    string currency;
    string merchant_transaction_ID;
    // sender credentials : this depends on payment method used
    Sender_Credentials sc;
    // authentication proof:mostly otp, upi pin,actually this is the last permission of sender to send money
    string token;

public:
    Payload (string merchant_ID, string secret_token, string amount, string currency, string merchant_transaction_ID, Sender_Credentials sc, string token) {
        this->merchant_ID = merchant_ID;
        this->secret_token = secret_token;
        this->amount = amount;
        this->currency = currency;
        this->merchant_transaction_ID = merchant_transaction_ID;
        this->sc = sc;
        this->token = token;
    }

    // getters of data
    string& get_merchant_ID () {
        return this->merchant_ID;
    }

    string& get_secret_token () {
        return this->secret_token;
    }

    string& get_amount () {
        return this->amount;
    }

    string& get_currency () {
        return this->currency;
    }

    string& get_merchant_transaction_ID () {
        return this->merchant_transaction_ID;
    }

    Sender_Credentials& get_sender_credentials () {
        return this->sc;
    }

    string& get_token () {
        return this->token;
    }
};

// =========================================================


// ==========payment gateway classes actual=================
// template design pattern
class Gateway {
protected:
    Payload* payload;
public:

    virtual bool validate() = 0;
    virtual bool initiate() = 0;
    virtual ~Gateway () {};
    pair<bool, string> template_method(Payload& payload) {
        // this method simply calls all steps one by one
        this->payload = &payload;
        bool response = validate();
        if (response) {
            // user validated, only now proceed
            bool is_success = initiate();
            if (is_success) {
                // means payment successful
                return {1, "Payment Scuccessful..."};
            }
        } else {
            this->payload = nullptr;
            return {0, "User not authenticated..."};
        }
    
        return {0, "Payment unsuccessful..."};
    }
};


// these concrete classes will talk to the respective exposed api endpoints of paytm, razorpay
class Paytm_Gateway : public Gateway {
public:
    Paytm_Gateway () {};
    bool validate() {
        // this will suitably use the payload data to validate according to paytm, for demonstration purposes, we are just returning true=>validated user
        return true;
    }

    bool initiate() {
        // this will suitably initiate payment according to paytm endpoint requirements, for now we are just returning true for demonstration purposes
        return true;
    }
    ~Paytm_Gateway () {};
};

class Razorpay_Gateway : public Gateway {
public:
    Razorpay_Gateway () {};
    bool validate() {
        return false;
    }

    bool initiate() {
        return true;
    }
    ~Razorpay_Gateway () {};
};

// ===========================================================




// ============FACTORIES : make them singleton==============

// gateway factory
class Gateway_Factory {
private:
    static Gateway_Factory* instance;
    Gateway_Factory () {};
public:
    static Gateway_Factory* get_instance() {
        return Gateway_Factory::instance;
    }

    Gateway* create(string gateway_name) {
        if (gateway_name == "Paytm") {
            cout << "Gateway set to Paytm" << endl;
            return new Paytm_Gateway();
        } else if (gateway_name == "Razorpay") {
            cout << "Gateway set to Razorpay" << endl;
            return new Razorpay_Gateway();
        }
        cout << "This gateway is not supported yet.." << endl;
        return nullptr;
    }
};


// eager initialisation of singleton classes
Gateway_Factory* Gateway_Factory::instance = new Gateway_Factory();


// =========================================================


// =================RETRY MECHANISM, we will use this to make payment=================
class Payment_Handler {
public:
    virtual pair<bool,string> start_payment_procedure(Gateway* gateway, Payload& payload) = 0;
};

class Linear : public Payment_Handler {
private:
    int timeout_seconds;
    int retry_count;
public:
    Linear(int cnt, int timeout) {
        this->retry_count = cnt;
        this->timeout_seconds = timeout;
    }

    pair<bool,string> start_payment_procedure(Gateway* gateway, Payload& payload) {
        pair<bool, string> last_response;
        for (int attempt = 1; attempt<=this->retry_count; attempt++) {
            last_response = gateway->template_method(payload);
            cout << "Attempt no :" << attempt <<", trying transaction..." << endl;
            if (last_response.first) {
                // successful payment
                break;
            }
            cout << "transaction failed, trying again in "<< this->timeout_seconds << " seconds..." << endl;
            // wait for timeout
            Sleep(this->timeout_seconds * 1000);
            // now try again
        }
        return last_response;
    }
};

class Exponential_Backoff : public Payment_Handler {
private:
    int base;
    int multiplier;
    int retry_count;
public:
    Exponential_Backoff(int cnt, int base, int multiplier) {
        this->retry_count = cnt;
        this->base = base;
        this->multiplier = multiplier;
    }

    pair<bool,string> start_payment_procedure(Gateway* gateway, Payload& payload) {
        pair<bool, string> last_response;
        for (int attempt = 1; attempt<=this->retry_count; attempt++) {
            cout << "Attempt no :" << attempt <<", trying transaction..." << endl;
            last_response = gateway->template_method(payload);
            if (last_response.first) {
                // successful payment
                break;
            }
            // wait for timeout
            int sleep_duration = this->base * pow(this->multiplier, attempt);
            cout << "transaction failed, trying again in "<< sleep_duration << " seconds..." << endl;
            
            Sleep(sleep_duration * 1000); // Semicolon added, converted seconds to ms
            // now try again
        }
        return last_response;
    }

};

// payment handler factory ==========================================================
class Payment_Handler_Factory {
private:
    static Payment_Handler_Factory* instance;
    Payment_Handler_Factory () {};
public:
    static Payment_Handler_Factory* get_instance() {
        return Payment_Handler_Factory::instance;
    }

    Payment_Handler* create(string name, map<string, int>& attributes) {
        if (name == "Linear") {
            cout << "Set retry mechanism to Linear..." << endl;
            return new Linear(attributes["cnt"], attributes["timeout"]);
        } else if (name == "Exponential_Backoff") {
            cout << "Set retry mechanism to Exponential_Backoff..." << endl;
            return new Exponential_Backoff(attributes["cnt"], attributes["base"], attributes["multiplier"]);
        }
        cout << "This retry mechanism is not supported yet..." << endl;
        return nullptr;
    }
};

// eager==============
Payment_Handler_Factory* Payment_Handler_Factory::instance = new Payment_Handler_Factory();

// ===================================================================================


// ====================FACADE for pg======================
class Facade {
private:
    Gateway* gateway;
    Gateway_Factory* gwf;
    Payment_Handler* ph;
    Payment_Handler_Factory* phf;
public:
    Facade () {
        this->gwf = Gateway_Factory::get_instance();
        this->phf = Payment_Handler_Factory::get_instance();
    };

    void set_retry_mechanism(string name, map<string, int>& mp) {
        this->ph = this->phf->create(name, mp);
    }

    void set_gateway(string name) {
        this->gateway = this->gwf->create(name);
    }

    void make_payment(Payload& payload) {
        // this will call its template method of the gateway, everything else will be handled there
        pair<bool, string> response = this->ph->start_payment_procedure(this->gateway, payload);
        cout << response.second << endl;
    }

    ~Facade () {

        this->gwf = nullptr;
        this->phf = nullptr;
        delete this->gateway;
        delete this->ph;
    }
};

// ======================================================
int main () {
    Facade pg;

    // (1) choose a gateway, paytm, razorpay,
    pg.set_gateway("Razorpay");

    // (2) make the payload to be sent, this is made by the app using our gateway

    // create sender credentials using factory method(otp, upi pin)
    map<string, string> mp = {
        {"method_name", "UPI"},
        {"UPI PIN","8876"}
    };
    Sender_Credentials sc;
    sc.set_credentials(mp);
    // other way is otp
    // another way is card

    Payload payload(
            "mid_rzp_live_9F8x2Ka9P",
            "sk_live_abc123xyz789secret",
            "50000",
            "INR",
            "TXN_2026_07_19_98432",
            sc,
            "tok_1NzA8eLkd92xQp01"
        );

    // (3) set count for retry mechanism
    map<string, int> attributes = {
        {"cnt",3},
        {"base", 1},
        {"multiplier", 2}
    };
    pg.set_retry_mechanism("Exponential_Backoff", attributes);

    // (4) make payment by sending payload to gateway
    pg.make_payment(payload);


    return 0;
}