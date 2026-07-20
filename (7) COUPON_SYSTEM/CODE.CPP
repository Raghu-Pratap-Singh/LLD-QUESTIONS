#include <bits/stdc++.h>
using namespace std;

// NOTE: procts and cart entirely depend on application using our coupon helper, but we at least need these cart and product methods for our coupon applier


// coupons on cart and product level(seasonal , loyalty, banking)

// discounts(flat discount, capped discount, percentage discount) on total price of cart

// coupon nesting, maybe indicating decorator pattern, cart coupon nesting and product coupon nesting, 

// now multiple coupons are applicable on single product and hence nesting of coupones is seperate for each product

// same for different carts

// after applying(nesting coupons) on a product/cart,it will have a function to return final price of prodcut/cart

// these decorators are basically coupons
class IProduct_Component {
protected:
    bool is_stackable;
public:
    virtual long long get_price() = 0;
    virtual string& get_id() = 0;

    bool get_status () {
        return this->is_stackable;
    }   
};

class Product : public IProduct_Component {
private:
    long long price;
    string id;
public:
    Product(string id, long long price, bool is_stackable) {
        this->id = id;
        this->price = price;
        this->is_stackable = is_stackable;
    }
    // getters
    long long get_price() {
        return this->price;
    }

    string& get_id() {
        return this->id;
    }
};

class Product_Coupon : public IProduct_Component {
protected:
    IProduct_Component* child;
public:
    string& get_id() {
        return this->child->get_id();
    }
    virtual long long get_price() = 0;
};

// different strategy for coupons (scalable for future)
// it will have a flat price off
class Flat_Coupon : public Product_Coupon {
private:
    long long flat;
public:
    Flat_Coupon(bool is_stackable, long long flat, IProduct_Component* child) {
        if (!child->get_status()) {
            cout << "The coupon you gave is unstackable..." << endl;
            
        }
        this->child = child;
        
        
        this->flat = flat;
        this->is_stackable = is_stackable;
    }

    long long get_price() {
        
        long long child_price = this->child->get_price();
        if (this->child->get_status()) {
            child_price = max((long long)0, child_price - this->flat);
            // apply methods only if this is stackable
        }
    
        return child_price;
    }
};

class Cap_Coupon : public Product_Coupon {
private:
    long long min_limit;
    long long flat;
public:
    Cap_Coupon(bool is_stackable, long long flat, long long limit, IProduct_Component* child) {
        if (!child->get_status()) {
            cout << "The coupon you gave is unstackable..." << endl;
            
        }
        this->child = child;
        this->is_stackable = is_stackable;
        this->flat = flat;
        this->min_limit = limit;
    }

    long long get_price() {
        
        long long child_price = this->child->get_price();
        if (this->child->get_status() && (child_price >= this->min_limit)) {
            child_price-=this->flat;
        }
        return child_price;
    }
};




// now we will do cart coupons
class ICart_Component {
protected:
    bool is_stackable;
public:
    virtual void add_product(IProduct_Component* p) = 0;
    virtual void remove(string p_id) = 0;
    virtual vector<IProduct_Component*>& get_cart() = 0;
    virtual long long get_price() = 0;
    bool get_status() {
        return this->is_stackable;
    }
};

class Cart : public ICart_Component {
private:
    vector<IProduct_Component*> v;
public:
    Cart (bool is_stackable) {
        this->is_stackable = is_stackable;
    };

    void add_product(IProduct_Component* p) {
        this->v.push_back(p);
    }

    void remove(string p_id) {
        int target = -1;
        for (int i = 0; i<this->v.size(); i++) {
            if (this->v[i]->get_id() == p_id) {
                target = i;
                break;
            }
        }

        if (target == -1) {
            cout << "Product not found in cart.." << endl;
        } else {
            v.erase(v.begin() + target);
            cout << "Product removed from cart..." << endl;
        }
    }

    vector<IProduct_Component*>& get_cart() {
        return this->v;
    }

    long long get_price() {
        long long total = 0;
        for (IProduct_Component* p : this->v) {
            total+=p->get_price();
        }
        return total;
    }
};

class Cart_Coupon : public ICart_Component {
protected:
    ICart_Component* child;
public:
    void add_product(IProduct_Component* p) {
        this->child->add_product(p);
    }

    void remove(string p_id) {
        this->child->remove(p_id);
    }
    vector<IProduct_Component*>& get_cart () {
        return this->child->get_cart();
    }
    virtual long long get_price() = 0;
};

// we can have any type of coupon, (scalable, strategy + decorator)=>parameterized decorator
class Percentage_Coupon : public Cart_Coupon {
private:
    long long percent;
public:
    Percentage_Coupon(bool is_stackable, long long percent, ICart_Component* child) {
        this->child = child;
        this->percent = percent;
        this->is_stackable = is_stackable;
    }

    long long get_price() {
        long long child_price = this->child->get_price();
        if (this->child->get_status()) {
            child_price = (long long)((1 - (0.01*(double)this->percent)) * (double)(child_price));    
        }
        return child_price;
        

    }
};

// this one apply benefit based on cart size
class Capped_Coupon : public Cart_Coupon { 
private:
    long long percent;
    long long limit; 
public:
    Capped_Coupon (bool is_stackable, long long percent, long long limit, ICart_Component* child) {
        this->child = child;
        this->limit = limit;
        this->percent = percent;

        this->is_stackable = is_stackable;
    }

    long long get_price() {
        long long child_price = this->child->get_price();
        if (this->child->get_status() && (this->child->get_cart().size() >= this->limit)) {

            child_price = (long long)((1 - (0.01*(double)this->percent)) * (double)(child_price));
        }
        return child_price;
    }
};


// ==============coupon recursive factory============
class Product_Coupon_Factory {
private:
    static Product_Coupon_Factory* instance;
    Product_Coupon_Factory() {};
public:
    static Product_Coupon_Factory* get_instance() {
        return Product_Coupon_Factory::instance;
    }

    IProduct_Component* create(int i, vector<pair<string,map<string, int>>>& coupons, IProduct_Component* p) {
        if (i == coupons.size()) {
            // return base product
            return p;
        }

        IProduct_Component* coup;
        // now here we will make based on ith index product name
        if (coupons[i].first == "Flat_Coupon") {
            coup = new Flat_Coupon(coupons[i].second["is_stackable"], coupons[i].second["flat"], this->create(i+1, coupons, p));
        } else if (coupons[i].first == "Cap_Coupon") {
            coup = new Cap_Coupon(coupons[i].second["is_stackable"], coupons[i].second["flat"], coupons[i].second["limit"], this->create(i+1, coupons, p));
        }
        return coup;

    }

    ~Product_Coupon_Factory() {};
};

class Cart_Coupon_Factory {
private:
    static Cart_Coupon_Factory* instance;
    Cart_Coupon_Factory() {};
public:
    static Cart_Coupon_Factory* get_instance() {
        return Cart_Coupon_Factory::instance;
    }

    ICart_Component* create(int i, vector<pair<string,map<string, int>>>& coupons, ICart_Component* c) {
        if (i == coupons.size()) {
            // return base Cart
            return c;
        }

        ICart_Component* coup;
        // now here we will make based on ith index product name
        if (coupons[i].first == "Percentage_Coupon") {
            coup = new Percentage_Coupon(coupons[i].second["is_stackable"], coupons[i].second["percent"], this->create(i+1, coupons, c));
        } else if (coupons[i].first == "Capped_Coupon") {
            coup = new Capped_Coupon(coupons[i].second["is_stackable"], coupons[i].second["percent"],coupons[i].second["limit"], this->create(i+1, coupons, c));
        }
        return coup;

    }

    ~Cart_Coupon_Factory() = default;
};


Product_Coupon_Factory* Product_Coupon_Factory::instance = new Product_Coupon_Factory();
Cart_Coupon_Factory* Cart_Coupon_Factory::instance = new Cart_Coupon_Factory();


// ==================================================

// user will get only this exposed, so we need a single instance of this only(use eager initialization)
class Facade {
private:
    static Facade* instance;
    Product_Coupon_Factory* pcf;
    Cart_Coupon_Factory *ccf;
    Facade() {
        this->pcf = Product_Coupon_Factory::get_instance();
        this->ccf = Cart_Coupon_Factory::get_instance();
    };
public:

    static Facade* get_instance() {
        return Facade::instance;
    }

    // methods
    // this receives a map/vector of all coupons with their attributes and a product reference to be applied on, and returns the final product with coupons applied
    IProduct_Component* apply_coupon_on_Product(vector<pair<string,map<string, int>>> coupons, IProduct_Component* p) {
        // coupons[i] = {type of coupon, {attributes name : value}}

        // this will use a coupon factory, which will have a recursive function and will create the nesting of all coupons in order

        return this->pcf->create(0, coupons, p);
    }

    // similar recursive function for carts
    ICart_Component* apply_coupon_on_Cart(vector<pair<string,map<string, int>>> coupons, ICart_Component* c) {
        // coupons[i] = {type of coupon, {attributes name : value}}

        // this will use a coupon factory, which will have a recursive function and will create the nesting of all coupons in order

        return this->ccf->create(0, coupons, c);
    }



    ~Facade() {
        this->pcf == nullptr;
        this->ccf = nullptr;
    }
};

Facade* Facade::instance = new Facade();

int main () {
    Facade* coupon_service = Facade::get_instance();

    return 0;
}