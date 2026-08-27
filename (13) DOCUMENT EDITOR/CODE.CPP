#include <bits/stdc++.h>
using namespace std;


// ==================================
// we will use visitor design pattern to solve this element handling
class IVisitor;

class Element {
protected:
    int element_id;
public:
    Element(int id) {
        this->element_id = id;
    }
    int get_element_id () {
        return this->element_id;
    }
    virtual void accept(IVisitor* v) = 0;
    virtual const string get_content() = 0;
    virtual Element* clone() = 0;
    virtual ~Element() {};

};





class Text_Element : public Element{
private:
    string content;
public:
    Text_Element(int id, string content) : Element(id) {
        this->content = content;
    }

    void accept(IVisitor* v) override;
    const string get_content() override{
        return this->content;
    }
    Text_Element(Text_Element& other) : Element(other.element_id) {
        this->content = other.content;


    }
    Element* clone() override{
        return new Text_Element(*this);
    }
};
class Image_Element : public Element{
private:
    string path;
public:
    Image_Element(int id, string path) : Element(id) {
        this->path = path;
    }
    void accept(IVisitor* v) override;
    const string get_content() override {
        return this->path;
    }
    Image_Element(Image_Element& other) : Element(other.element_id) {
        this->path = other.path;
    }
    Element* clone() override {
        return new Image_Element(*this);
    }
};
class Table_Element : public Element{
private:
    string table_name;
    vector<vector<string>> table;
public:
    Table_Element (int id, string table_name, vector<vector<string>> table) : Element(id) {
        this->table = table;
        this->table_name = table_name;
    }
    Table_Element(Table_Element& other) : Element(other.element_id){
        this->table_name = other.table_name;

        
        this->table = other.table;

    }
    Element* clone() override {
        return new Table_Element(*this);
    }
    void accept(IVisitor* v) override;
    const string get_content() override {
        // just a rough implementation here, for demo only
        string ans = string(":::") + this->table_name + string(":::") +'\n';
        for (int i = 0; i<this->table.size(); i++) {
            string row = "";
            for (int j = 0; j<this->table[0].size(); j++) {
                if (j!=this->table[0].size() - 1) {
                    row+= this->table[i][j] + " -- ";
                }
                else {
                    row+= this->table[i][j];
                }

            }
            row+="\n";
            ans+=row;
        }
        return ans;

    }
};
class Code_Element : public Element{
private:
    string code;
public:
    Code_Element(int id, string code) : Element(id) { 
        this->code = code;
    }
    void accept(IVisitor* v) override;
    const string get_content() override {
        return this->code;
    }
    Code_Element(Code_Element& other) : Element(other.element_id) {
        this->code = other.code;
    }
    Element* clone() {
        return new Code_Element(*this);
    }

};

class IVisitor {
public:
    virtual void visit(Text_Element* e) = 0;
    virtual void visit(Image_Element* e) = 0;
    virtual void visit(Table_Element* e) = 0;
    virtual void visit(Code_Element* e) = 0;
    virtual ~IVisitor() {};
};

void Text_Element::accept (IVisitor* v) {
    v->visit(this);
}

void Image_Element::accept (IVisitor* v) {
    v->visit(this);
}

void Table_Element::accept (IVisitor* v) {
    v->visit(this);
}

void Code_Element::accept (IVisitor* v) {
    v->visit(this);
}


// concrete visiotrs
class Render_Visitor : public IVisitor {
public:
    void visit(Text_Element* e) override{
        cout << e->get_content() <<endl;
    }

    void visit(Image_Element* e) override {
        cout << e->get_content() <<endl;
    }

    void visit(Table_Element* e) override {
        cout << e->get_content() <<endl;
    }

    void visit(Code_Element* e) override {
        cout << e->get_content() <<endl;
    }
};

class Resource_Satistics_Visitor : public IVisitor {
public:
    void visit(Text_Element* e) override {
        cout << "Calculating Text element statistics..." << endl;
    }

    void visit(Image_Element* e) override {
        cout << "Calculating Image element statistics..." << endl;
    }

    void visit(Table_Element* e) override {
        cout << "Calculating Table element statistics..." << endl;
    }

    void visit(Code_Element* e) override {
        cout << "Calculating Code element statistics..." << endl;
    }
};

class Correctness_Visitor : public IVisitor {
public:
    void visit(Text_Element* e) override {
        cout << "Checking Text element Correctness..." << endl;
    }

    void visit(Image_Element* e) override {
        cout << "Checking Image element Correctness..." << endl;
    }

    void visit(Table_Element* e) override {
        cout << "Checking Table element Correctness......" << endl;
    }

    void visit(Code_Element* e) override {
        cout << "Checking Code element Correctness..." << endl;
    }
};


// =============================





















// ====================
// state pattern + memento pattern
class Originator;

class Read_Only; // we can only render document and see correctness
class Write_Only; // we can only add or remove or edit elements
class Review; // in this mode we can get statistics of our document and also render it and see correctness

class Mode {
public:
    virtual void add_element(Originator* o,Element* e) = 0;
    virtual void remove_element(Originator* o, int element_id) = 0;
    virtual void print_statistics(Originator* o) = 0;
    virtual void calculate_correctness(Originator* o) = 0;
    virtual void render(Originator* o) = 0;
    virtual Mode* cycle_mode(Originator* o) = 0;
    virtual Mode* clone() = 0;
    virtual ~Mode() {};
};

class State {
private:
    vector<Element*> ordered_elements;
    Mode* current_mode;

    // all state references
public:
    State() {
        this->current_mode = nullptr;
    };
    State(State& other) {
        this->current_mode = other.current_mode->clone();

        for (Element* e : other.get_elements()) {
            this->ordered_elements.push_back(e->clone());
        }
    }
    ~State() {
        for (int i = 0; i<this->ordered_elements.size(); i++) {
            if (this->ordered_elements[i]!=nullptr) {
                delete this->ordered_elements[i];
            }
        }

        if (this->current_mode!=nullptr) {
            delete this->current_mode;
        }
    }

    Mode* get_current_mode() {
        return this->current_mode;
    }

    void set_current_mode(Mode* m) {
        this->current_mode = m;
    }

    vector<Element*>& get_elements() {
        return this->ordered_elements;
    }
};

class Memento {
private:
    State* s;

public:
    Memento(State* s) {
        this->s = s;
    };
    ~Memento() {
        if (this->s != nullptr) {
            delete this->s;
        }
    }
    State* get_state() {
        return this->s;
    }
};

class Originator {
private:
    State *s;

    // all state references
    Mode* romode;
    Mode* womode;
    Mode* rmode;
public:
    Originator();
    ~Originator() {
        if (this->s!=nullptr) {
            delete this->s;
        }
        if (this->romode!=nullptr) {
            delete this->romode;
        }
        if (this->womode!=nullptr) {
            delete this->womode;
        }
        if (this->rmode!=nullptr) {
            delete this->rmode;
        }
    }
    // restore
    void restore(Memento* m) {
        if (this->s!=nullptr) {
            delete this->s;
        }
        this->s = new State(*m->get_state());
    }

    

    State* get_state() {
        return this->s;
    }

    Memento* create_memento() {
        return new Memento(new State(*this->s));
    }

    vector<Element*>& get_elements() {
        return this->s->get_elements();
    }

    Mode* get_read_only_mode () {
        return this->romode;
    }

    Mode* get_write_only_mode () {
        return this->womode;
    }

    Mode* get_review_mode () {
        return this->rmode;
    }


    // delegated functions
    void render();
    void add_element(Element* e);
    void remove_element(int element_id);
    void print_statistics();
    void calculate_correctness();
    void cycle_mode();
};


// concrete mode implementations
class Read_Only : public Mode {
public:

    void add_element(Originator* o, Element* e) override {
        cout << "You are in read only mode, modification of document is not possible, pleas switch the mode..." << endl;
    }

    void remove_element(Originator* o, int element_id) override {
        cout << "You are in read only mode, modification of document is not possible, pleas switch the mode..." << endl;
    }

    void print_statistics(Originator* o) override {
        cout << "You are in read only mode, switch to review mode for statistics viewing..." << endl;
    }

    void calculate_correctness(Originator* o) override {
        for (Element* e : o->get_state()->get_elements()) {
            e->accept(new Correctness_Visitor());
        }
    }



    void render(Originator* o) override {
        for (Element* e : o->get_state()->get_elements()) {
            e->accept(new Render_Visitor());
        }
    }

    Mode* cycle_mode(Originator* o) override {
        cout << "Mode changed to write only mode..." << endl;
        return o->get_write_only_mode();
    }

    Mode* clone() override {
        return new Read_Only();
    }
};


class Write_Only : public Mode {
public:
    void add_element(Originator* o, Element* e) override {
        o->get_elements().push_back(e);
        cout << "Element inserted in document..." << endl;
    }

    void remove_element(Originator* o, int element_id) override {
        int target_index = -1;
        for (int i = 0; i<o->get_elements().size(); i++) {
            if (o->get_elements()[i]->get_element_id() == element_id) {
                target_index = i;
                break;
            }
        }
        if (target_index == -1) {
            cout << "element is not present in document..." << endl;
            return;
        }

        o->get_elements().erase(o->get_elements().begin() + target_index);
        cout << "Element removed from document..." << endl;

    }

    void print_statistics(Originator* o) override {
        cout << "You are in write only mode, switch to review mode for statistics viewing..." << endl;
    }

    void calculate_correctness(Originator* o) override {
        cout << "You are in write only mode, please switch to read only mode..." << endl;
    }

    void render(Originator* o) override {
        cout << "You are in write only mode, switch to review mode or read only mode for rendering document..." << endl;
    }

    Mode* cycle_mode(Originator* o) override {
        cout << "Mode changed to review mode..." << endl;
        return o->get_review_mode();

    }

    Mode* clone() override {
        return new Write_Only();
    }
};


class Review : public Mode {
public:
    void add_element(Originator* o, Element* e) override {
        cout << "You are in review mode, modification of document is not possible, pleas switch the mode..." << endl;
    }

    void remove_element(Originator* o, int element_id) override {
        cout << "You are in review mode, modification of document is not possible, pleas switch the mode..." << endl;

    }

    void print_statistics(Originator* o) override {
        for (Element* e : o->get_state()->get_elements()) {
            e->accept(new Resource_Satistics_Visitor());
        }
    }

    void calculate_correctness(Originator* o) override {
        cout << "You are in review mode, please switch to read only mode..." << endl;
    }

    void render(Originator* o) override {
        for (Element* e : o->get_state()->get_elements()) {
            e->accept(new Render_Visitor());
        }
    }

    Mode* cycle_mode(Originator* o) override {
        cout << "Mode changed to read only mode..." << endl;
        return o->get_read_only_mode();

    }

    Mode* clone() override {
        return new Review();
    }
};

// originator implementations
Originator::Originator() {
    this->s = new State();

    this->romode = new Read_Only();
    this->womode = new Write_Only();
    this->rmode = new Review();

    this->s->set_current_mode(romode);
}

void Originator::render() {
    this->s->get_current_mode()->render(this);
}

void Originator::add_element(Element* e) {
    this->s->get_current_mode()->add_element(this, e);
}

void Originator::remove_element(int element_id) {
    this->s->get_current_mode()->remove_element(this, element_id);
}

void Originator::print_statistics() {
    this->s->get_current_mode()->print_statistics(this);
}

void Originator::calculate_correctness() {
    this->s->get_current_mode()->calculate_correctness(this);
}

void Originator::cycle_mode() {
    this->s->set_current_mode(this->s->get_current_mode()->cycle_mode(this));
}

class Caretaker {
private:
    vector<Memento*> snapshots;
public:
    Caretaker() {};
    ~Caretaker() {
        for (int i = 0; i<this->snapshots.size(); i++) {
            if (this->snapshots[i]!=nullptr) {
                delete this->snapshots[i];
            }
        }
    }
    void add_snapshot(Originator* o) {
        this->snapshots.push_back(o->create_memento());
    }

    void go_to_nth_snapshot(Originator* o, int n) {
        // set state of originator to nth snapshot
        if (n<this->snapshots.size() && n>=0) {
            o->restore(this->snapshots[n]);
            while (this->snapshots.size()>n + 1) {
                delete this->snapshots.back();
                this->snapshots.pop_back();
            }
        } else {
            cout << "this past is not available..." << endl;
        }
    }
};
// =============================





















int main() {
    
    // (1) initialise a caretaker
    Caretaker* handler = new Caretaker();

    // (2) initialize a document
    Originator* document = new Originator();

    // (3) add elments to document
    document->cycle_mode();
    document->add_element(new Image_Element(25, "c:/users/hp/images"));
    document->add_element(new Text_Element(12, "this is a trial heading"));
    document->render();
    document->cycle_mode();
    document->render();

    // (4) take a snapshot
    handler->add_snapshot(document);

    // (5) again add elements
    document->cycle_mode();
    document->cycle_mode();

    vector<vector<string>> table(3, vector<string>(2));
    table[0][0] = "Name";
    table[0][1] = "Marks";
    table[1][0] = "Raghu";
    table[1][1] = "100";
    table[2][0] = "Ansh";
    table[2][1] = "100";
    document->add_element(new Table_Element(30, "results", table));
    
    document->cycle_mode();
    document->cycle_mode();
    document->render();

    // (6) take another snapshot
    handler->add_snapshot(document);


    // (7) recover older state
    handler->go_to_nth_snapshot(document, 0);

    // (8) render agin
    document->render();

    document->print_statistics();

    
    return 0;
}