#include <bits/stdc++.h>
using namespace std;


class Logger {
private:
    static Logger* instance;
    int line_number = 1;
    Logger() {};

public:
    static Logger* get_instance() {
        return Logger::instance;
    }

    void log_message_on_terminal(string message) {
        cout << "Line Number : " << this->line_number << endl;
        cout << message << endl;
        this->line_number++;
    }
};

Logger* Logger::instance = new Logger();

class Code;
class Image;
class Paragraph;
class Mathematical_Equation;
class Table;
class I_Visitor;

class Component {
protected:
    double size = 0.0;
    int component_id;
public:
    virtual Component* clone() = 0;
    void set_component_id(int id) {
        this->component_id = id;
    }
    int get_component_id() {
        return this->component_id;
    }
    virtual void display_name(int level) = 0;
    virtual void accept(I_Visitor* visitor) = 0;
    virtual void copy_dfs(map<int, Component*>& mp) = 0;
    void set_size(double sz) {
        this->size = sz;
    }
    double get_size() {
        return this->size;
    }
};

class Composite;
class I_Visitor {
public:
    virtual void visit(Code* code) = 0;
    virtual void visit(Paragraph* para) = 0;
    virtual void visit(Image* img) = 0; 
    virtual void visit(Mathematical_Equation* equation) = 0; 
    virtual void visit(Table* table) = 0;
    virtual void visit(Composite* el) = 0;
};

class Composite : public Component{
protected:
    string name;
    vector<Component*> childs;
public:
    Composite (string name) {
        this->name = name;
        
    }
    void accept(I_Visitor* visitor) override {
        visitor->visit(this);
    }
    void add_child(Component* c) {
        this->childs.push_back(c);
    }
    void display_name(int level) override {
        for (int i = 0; i<level; i++) {
            cout << " = ";
        }
        cout << this->name << endl;
        for (Component*& c : this->childs) {
            cout << "||" << endl;
            c->display_name(level + 1);
        }
        
    }
    Composite* clone() override {
        Composite* cl = new Composite(this->name); 
        cl->set_component_id(this->component_id);
        cl->size = this->size;
        for (Component* c : this->childs) {
            Component* new_child = c->clone();
            cl->childs.push_back(new_child);
        }
        return cl;
    }
    vector<Component*>& get_components () {
        return this->childs;
    }
    void copy_dfs(map<int, Component*>& mp) {
        mp[this->component_id] = this;
        for (Component*& child : this->childs) {
            child->copy_dfs(mp);
        }
    }
};

class Leaf : public Component {
protected:
    string name;
public:
    Leaf(string name) {
        this->name = name;
    }
    

    void display_name(int level) override {
        for (int i = 0; i<level; i++) {
            cout << " = ";
        }
        cout << this->name << endl;
    }

    void copy_dfs(map<int, Component*>& mp) override {
        mp[this->component_id] = this;
    }

    
    virtual void accept(I_Visitor* visitor) = 0;
};

class Code : public Leaf {
private:
    string code;
public:
    Code(string name, string code) : Leaf(name) {
        this->code = code;
    }

    void accept(I_Visitor* visitor) override {
        visitor->visit(this);
    }

    Code* clone() override {
        Code* dup = new Code(this->name, this->code);
        dup->component_id = this->component_id;
        dup->size = this->size;
        return dup;
    }
    
};

class Text_Element {
public:
    virtual string get_text_content() = 0; 
};

class Paragraph : public Leaf, public Text_Element {
private:
    string text_content;
public:
    Paragraph(string name,string text) : Leaf(name) {
        this->text_content = text;
    }
    
    string get_text_content() override {
        return this->text_content;
    }
    void accept(I_Visitor* visitor) override {
        visitor->visit(this);
    }

    Paragraph* clone() override {
        Paragraph* dup = new Paragraph(this->name, this->text_content);
        dup->component_id = this->component_id;
        dup->size = this->size;
        return dup;
    }
};

class Image : public Leaf {
private:
    string image_path;
public:
    Image(string name, string path) : Leaf(name) {
        this->image_path = path;
    }
    
    void accept(I_Visitor* visitor) override {
        visitor->visit(this);
    }

    Image* clone() override {
        Image* dup = new Image(this->name, this->image_path);
        dup->component_id = this->component_id;
        dup->size = this->size;
        return dup;
    }
};

class Mathematical_Equation : public Leaf {
private:
    string equation;
public:
    Mathematical_Equation(string name, string equation) : Leaf(name) {
        this->equation = equation;
    }
    
    void accept(I_Visitor* visitor) override {
        visitor->visit(this);
    }

    Mathematical_Equation* clone() override {
        Mathematical_Equation* dup = new Mathematical_Equation(this->name, this->equation);
        dup->component_id = this->component_id;
        dup->size = this->size;
        return dup;
    }
};

class Table : public Leaf {
private:
    vector<vector<string>> grid;
public:
    Table(string name, vector<vector<string>> grid) : Leaf(name) {
        this->grid = grid;
    };
    
    void accept(I_Visitor* visitor) override {
        visitor->visit(this);
    }

    Table* clone() override {
        Table* dup = new Table(this->name, this->grid);
        dup->component_id = this->component_id;
        dup->size = this->size;
        return dup;
    }
};

class Render_Visitor : public I_Visitor{
public:
    Render_Visitor() {};
    void visit(Code* code) {
        // does the internal implementation for rendering code
    }
    void visit(Paragraph* para) {
        // does the internal implementation for rendering text para
        
    }
    void visit(Image* img) {
        // does the internal implementation for rendering image located at image path
        
    } 
    void visit(Mathematical_Equation* equation) {
        // does the internal implementation for rendering maths equation

    }
    void visit(Table* table) {
        // renders table
    }

    void visit(Composite* c) {
        for (Component*& comp : c->get_components()) {
            comp->accept(this);
        }
    }
};

class Size_Calculator_Visitor : public I_Visitor {
public:
    Size_Calculator_Visitor() {};
    void visit(Code* code) {
        // does internal calculation and calulates code size
    
        // for now , we are implementing this randomly
        code->set_size(23.45);
    }
    void visit(Paragraph* para) {
        para->set_size(40.00);
        
    }
    void visit(Image* img) {
        img->set_size(19.078);
        
    } 
    void visit(Mathematical_Equation* equation) {
        equation->set_size(50.00);
    }
    void visit(Table* table) {
        table->set_size(80.120);
    }

    void visit(Composite* c) {
        double ans = 0.0;
        for (Component*& comp : c->get_components()) {
            comp->accept(this);
            ans+=comp->get_size();
        }
        c->set_size(ans);
    }
    
};

class Vulnerability_Finder_Visitor : public I_Visitor {
public:
    Vulnerability_Finder_Visitor() {};
    void visit(Code* code) {
        // for now , we are telling no vulnerability as implementation is not the main concern of this class
        Logger::get_instance()->log_message_on_terminal("No vulnerability in this code");
    }
    void visit(Paragraph* para) {
        Logger::get_instance()->log_message_on_terminal("No vulnerability in this para");
        
    }
    void visit(Image* img) {
        Logger::get_instance()->log_message_on_terminal("No vulnerability in this image");
        
    } 
    void visit(Mathematical_Equation* equation) {
        Logger::get_instance()->log_message_on_terminal("No vulnerability in this equation");
        
    }
    void visit(Table* table) {
        Logger::get_instance()->log_message_on_terminal("No vulnerability in this table");
    }

    void visit(Composite* c) {
        for (Component*& comp : c->get_components()) {
            comp->accept(this);
        }
    }
};












// mediator design pattern

class Visitor_Map {
private:
    static Visitor_Map* instance;
    Render_Visitor* rv;
    Size_Calculator_Visitor* scv;
    Vulnerability_Finder_Visitor* vfv;
    Visitor_Map() {
        this->rv = new Render_Visitor();
        this->scv = new Size_Calculator_Visitor();
        this->vfv = new Vulnerability_Finder_Visitor();
    };
public:
    ~Visitor_Map() {
        delete this->rv;
        delete this->scv;
        delete this->vfv;
    }

    static Visitor_Map* get_instance() {
        return Visitor_Map::instance;
    }

    Render_Visitor* get_render_visitor() {
        return this->rv;
    }
    Size_Calculator_Visitor* get_size_calculator_visitor() {
        return this->scv;
    }
    Vulnerability_Finder_Visitor* get_vilnerability_finder_visitor() {
        return this->vfv;
    }
};

Visitor_Map* Visitor_Map::instance = new Visitor_Map();

enum class UI_models {
    CANVAS,
    TOOLBAR,
    PROPERTIES_INSPECTOR,
    NOTIFICATION_PANEL
};


// interfacces for mediator design pattern
class IMessage;
class IMember {
public:
    virtual void recieve(IMessage* message) = 0;
    virtual void send(UI_models model, IMessage* message) = 0;
    virtual ~IMember() = default;
};
class IMediator {
public:
    virtual void send_to(UI_models to, IMessage* message) = 0;
    virtual void add_member(UI_models model_name, IMember* member) = 0;
    virtual ~IMediator() = default;
};
class IMessage {
public:
    virtual void EXECUTE(IMember* m) = 0;
    virtual ~IMessage() = default;
};
// ==========================

// concrete messages
// this message will be sent from canvas, toolbar->properties_inpector
class Proprties_Show_Update_Message : public IMessage {
private:
    int target_id;// this is the id whose properties now the panel will shoe
public:
    Proprties_Show_Update_Message(int id) {
        this->target_id = id;
    }
    ~Proprties_Show_Update_Message() {};
    void EXECUTE(IMember* member) override;
};
// this message will be sent to notification panel when ever some change occurs or any thing important takes place
class Notification_Display_Message : public IMessage {
private:
    string message;
public:
    Notification_Display_Message(string message) {
        this->message = message;
    }

    void EXECUTE(IMember* member) override;
};
// ===========================




// tree :
class ID_To_Node {
private:
    int id_number = 1;
    map<int, Component*> mp;
    static ID_To_Node* instance;
    ID_To_Node() {
        mp[1] = new Composite("Root Page");
        this->id_number++;
    };
    friend class WorkSpace;
    void erase() {
        for (auto&[k,v] : this->mp) {
            delete v;
        }

    }

    void reset(int id, map<int, Component*>& mpp) {
        this->id_number = id;
        this->mp = mpp;
    }
public:
    static ID_To_Node* get_instance() {
        return ID_To_Node::instance;
    }
    ~ID_To_Node() {
        for (auto &[k, v] : this->mp) {
            delete v;
        }
    }

    // getter
    int get_current_id_number() {
        return this->id_number;
    }

    map<int, Component*>& get_tree_map() {
        return this->mp;

    }

    Component* get_component(int id) {
        if (this->mp.find(id) == this->mp.end()) {
            Logger::get_instance()->log_message_on_terminal("No such component exists");
            return nullptr;
        }
        return this->mp[id];
    }

    void add_component(int parent_id, Component* c) {
        Component* parent = this->get_component(parent_id);

        // this check is not necessary because user selects insert element from screen, hence it cannot be invalid

        if ((parent == nullptr) || !(dynamic_cast<Composite*> (parent))) {
            Logger::get_instance()->log_message_on_terminal("Not valid insertion...");
            return;
        }
        Composite* p = dynamic_cast<Composite*> (parent);
        p->add_child(c);
        this->mp[this->id_number] = c;
        Logger::get_instance()->log_message_on_terminal("Component with id: "+ to_string(this->id_number) + " added to document...");
        this->id_number++;
    }
};
ID_To_Node* ID_To_Node::instance = new ID_To_Node();
// =======



// ====================================
class Canvas_View : public IMember {
private:
    IMediator* mediator;
public:
    Canvas_View(IMediator* m) {
        this->mediator = m;
        Notification_Display_Message* message = new Notification_Display_Message("canvas initialized...");
        this->send(UI_models::NOTIFICATION_PANEL, message);
        delete message; 
    };
    ~Canvas_View() {
        this->mediator = nullptr;
    }

    void send(UI_models model, IMessage* message) {
        this->mediator->send_to(model, message);
    }

    void recieve(IMessage* message) {
        message->EXECUTE(this);
    }

    void select_node(int id) {
        // notify the properties inpector to now show properties of this element
        IMessage* message = new Proprties_Show_Update_Message(id);
        this->send(UI_models::PROPERTIES_INSPECTOR, message);
        delete message;
    }

    void render_document() {
        Component* root = ID_To_Node::get_instance()->get_component(1);
        root->accept(Visitor_Map::get_instance()->get_render_visitor());
    }

    void display_document_tree() {
        Component* root = ID_To_Node::get_instance()->get_component(1);
        root->display_name(0);
    }

    Component* get_component(int id) {
        return ID_To_Node::get_instance()->get_component(id);
    }

};

class Properties {
private:
    map<string, string> properties;
public:
    Properties() {};
    Properties* clone() {
        Properties* dup = new Properties();
        dup->properties = this->properties;
        return dup;
    }
    ~Properties() {};
    void add_property(string key, string value) {
        this->properties[key] = value;
    }

    void remove_property(string key) {
        this->properties.erase(key);
    }
};

class Properties_Inspector : public IMember {
private:
    map<int, Properties*> props;
    int current_id; // properties of the selected node that are currently being shown
    IMediator* mediator;

    friend class WorkSpace;
    void erase() {
        for (auto&[k,v] : this->props) {
            delete v;
        }
    }

    void reset(int id, map<int,Properties*>& mp) {
        this->props = mp;
        this->current_id = id;
    }
public:
    Properties_Inspector(IMediator* m) {
        this->mediator = m;
        this->current_id = 1;
        // root page id properies are displayed by default
    };
    ~Properties_Inspector() {
        for (auto&[k,v] : this->props) {
            delete v;
        }
        this->mediator = nullptr;
    }

    void set_current_id(int id) {
        this->current_id = id;
        Notification_Display_Message* message = new Notification_Display_Message("Now showing properties of id: " + to_string(id) + " ...");
        this->send(UI_models::NOTIFICATION_PANEL, message);
        delete message;
    }

    int get_current_id() {
        return this->current_id;
    }

    map<int, Properties*>& get_map() {
        return this->props;
    }

    void add_property(int id, string key, string value) {
        this->props[id]->add_property(key, value);
    }

    void remove_property(int id, string key) {
        this->props[id]->remove_property(key);
    }

    void send(UI_models model, IMessage* message) override {
        this->mediator->send_to(model, message);
    }

    void recieve(IMessage* message) {
        message->EXECUTE(this);
    }
};

void Proprties_Show_Update_Message::EXECUTE(IMember* member) {
    Properties_Inspector* p = dynamic_cast<Properties_Inspector*> (member);
    p->set_current_id(this->target_id);
}

class ToolBar : public IMember {
private:
    IMediator* mediator;
    map<int, Properties*> high_level_properties; 

    friend class WorkSpace;

    void erase() {
        for (auto&[k,v] : this->high_level_properties) {
            delete v;
        }
    }

    void reset_properties(map<int, Properties*>& mp) {
        this->high_level_properties = mp;
    }

public:
    ToolBar(IMediator* m) {

        this->mediator = m;
    }    
    ~ToolBar() {
        for (auto&[k, v] : this->high_level_properties) {
            delete v;
        }
        this->mediator = nullptr;
    }
    void add_component(int parent_id, Component*& c) {
        ID_To_Node::get_instance()->add_component(parent_id, c);
    }

    void send(UI_models model, IMessage* message) {
        this->mediator->send_to(model, message);
    }

    map<int, Properties*>& get_map() {
        return this->high_level_properties;
    }

    void recieve(IMessage* message) {
        message->EXECUTE(this);
    }

    void set_property(int id, string key, string value) {
        if (this->high_level_properties.find(id) == this->high_level_properties.end()) {
            this->high_level_properties[id] = new Properties();
        }
        this->high_level_properties[id]->add_property(key, value);
        // notify the properties inspector so that it can show the element with updated high level properties
        IMessage* message = new Proprties_Show_Update_Message(id);
        this->send(UI_models::PROPERTIES_INSPECTOR, message);
        delete message;
    }

    

};

class Notification_Panel : public IMember {
private:
    vector<string> messages; //contains messages appearing in order
    IMediator* mediator;
public:
    Notification_Panel(IMediator* m) {
        this->mediator = m;
    };
    ~Notification_Panel() {
        this->mediator = nullptr;
    };

    void send(UI_models model, IMessage* message) override{
        this->mediator->send_to(model, message);
    }

    // displays latest notification on panel
    void add_message_to_panel(string message) {
        this->messages.push_back(message);
    }

    void recieve(IMessage* message) override {
        message->EXECUTE(this);
    }

    vector<string>& get_messages_displayed() {
        return this->messages;
    }

    void set_messages(vector<string>& m) {
        this->messages = m;
    }
};

void Notification_Display_Message::EXECUTE(IMember* member) {
    Notification_Panel* c = dynamic_cast<Notification_Panel*> (member);
    c->add_message_to_panel(this->message);
};

class Standard_Mediator : public IMediator {
private:
    map<UI_models, IMember*> members;
public:
    Standard_Mediator() {};
    void add_member(UI_models model_name, IMember* member) override {
        this->members[model_name] = member;
    }
    void send_to(UI_models to, IMessage* message) override {
        this->members[to]->recieve(message);
    }
};

// =====================================




// =======================================================
//     MEMENTO PATTERN FOR DOCUMENT STATE RESTORATION
// =======================================================
class State {
// this is the snapshot of all properties and tree that was at a time;
// but for true copy, we will use a copy contructor and maybe : PROTOTYPE / CLONE DESIGN PATTERN
private:
    int id_number = 1;
    map<int, Component*> mp;

    int current_selected_id;
    map<int, Properties*> properties_inspector_properties;

    map<int, Properties*> toolbar_properties;

    vector<string> messages; //messages which were displayed on the notification panel
public:
    State() {};
    int get_id_number() {
        return this->id_number;
    }

    map<int, Component*>& get_mp() {
        return this->mp;
    }

    int get_current_select_id() {
        return this->current_selected_id;
    }
    map<int, Properties*>& get_pi_map() {
        return this->properties_inspector_properties;
    }
    map<int, Properties*>& get_toolbar_map() {
        return this->toolbar_properties;
    }

    vector<string>& get_messages() {
        return this->messages;
    }

    // setters
    void set_tree() {
        // copied id number
        this->id_number = ID_To_Node::get_instance()->get_current_id_number();

        // not to copy the mp of tree, we will just copy the root, and it will according to clone implementation will give us full tree
        Component* new_root = ID_To_Node::get_instance()->get_component(1)->clone();
        // now new root is the true copy

        map<int, Component*> dupe_mp;
        
        // now we will make dupe_mp on basis of this whole tree,using a dfs
        new_root->copy_dfs(dupe_mp);
        this->mp = dupe_mp;
        // now tree is successfully copied with current id_number
    
    }
    void set_properties_inspector_attributes(Properties_Inspector* p) {
        this->current_selected_id = p->get_current_id();
        // now make a true copy of properties map
        map<int, Properties*> dup_map;
        for (auto&[k,v] : p->get_map()) {
            dup_map[k] = v->clone();
        }
        this->properties_inspector_properties = dup_map;
    }
    void set_toolbar_properties(ToolBar* toolbar) {
        map<int, Properties*> dup_map;
        for (auto&[k,v] : toolbar->get_map()) {
            dup_map[k] = v->clone();
        }
        this->toolbar_properties = dup_map;
    }
    void set_messages_displayed(Notification_Panel* p) {
        this->messages = p->get_messages_displayed();
    }
    
    ~State() {
        for (auto& [k, v] : mp) {
            delete v;
        }
        // Clean up properties maps stored in this state snapshot
        for (auto& [k, v] : properties_inspector_properties) {
            delete v;
        }
        for (auto& [k, v] : toolbar_properties) {
            delete v;
        }
    };
};
class Memento {
// the actual snap shot containing the state
private:    
    State* state;
public:
    Memento () {}
    ~Memento () {
        // NOTE: in this we wont delete this pointer, becuase it may be allocated to some restored state used by user of document
        delete state;
    }

    void set_state(State* s) {
        this->state = s;
    }

    State* get_state() {
        return this->state;
    }
};
class Originator {
public:
    virtual void restore(Memento* m) = 0;
    virtual Memento* create_memento() = 0;
    virtual ~Originator() = default;
};
class Caretaker {
private:
    vector<Memento*> mementos;
public:
    Caretaker () {};
    ~Caretaker () {
        for (Memento* m : this->mementos) {
            delete m;
        }
    };
    void add_memento(Originator* ob) {
        this->mementos.push_back(ob->create_memento());
    }
    void undo(Originator* ob, int state_number) {
        if (this->mementos.size() < state_number) {
            Logger::get_instance()->log_message_on_terminal("No such state exist...");
            return;
        }
        ob->restore(this->mementos[state_number]);
        // delete all furthur states
        while (this->mementos.size()>=state_number) {
            Memento* m = this->mementos.back();
            this->mementos.back() = nullptr;
            delete m;
            this->mementos.pop_back();
        }
        Logger::get_instance()->log_message_on_terminal("State restored...");
    }


};
// =======================================================


// firstly, workspace will have
/*
->all ui tools
->mediator
->no need for tree ID to NODE as it is singleton
*/



// =========PARSING AND APPLYING MECHANISM======
class Expression {
private:
    string condition;
    string action;
public:
    Expression() {};
    ~Expression() {};

    // setters
    void set_condition(string condition) {
        this->condition = condition;
    }

    void set_action(string action) {
        this->action = action;
    }

    // getters
    string get_condition() {
        return this->condition;
    }
    string get_action() {
        return this->action;
    }
};

class IText_Parser {
public:
    virtual vector<Expression*> parse(Text_Element* element) = 0;
    virtual ~IText_Parser() {};
};

class Standard_Text_Parser : public IText_Parser {
public:
    Standard_Text_Parser() {};
    ~Standard_Text_Parser() {};
    vector<Expression*> parse(Text_Element* element) override {
        // we know that format of logical expression is like IF .... THEN ....
        // so we need to find "IF" first
        string s = "asda";
        string word = "";
        string content = element->get_text_content();
        vector<string> broken;
        bool space_seen = false;
        for (char c : content) {
            if (c == ' ' && !space_seen && word.size()>0) {
                // means this is first space after a non zero length word
                broken.push_back(word);
                space_seen = true;
                word = "";
            }
            else if (c!=' ') {
                space_seen = false;
                word+=c;
            }
        }
        if (word.size() > 0) {
            broken.push_back(word);
        }

        // now we have broken sentence between spaces
        // now check for IF
        vector<Expression*> expressions;
        int IF_pos = -1;
        int THEN_pos = -1;
        int OB_pos = -1;
        int CB_pos = -1;

        for (int i = 0; i<broken.size(); i++) {
            string w = broken[i];
            if (w == "IF") {
                // means this is start of an expression, but we are still not sure, we still need a THEN and a opening and closing bracket
                IF_pos = i;
                THEN_pos = -1;
                OB_pos = -1;
                CB_pos = -1;
            }

            else if (w == "THEN") {
                THEN_pos = i;
                OB_pos = -1;
                CB_pos = -1;
            }

            else if (w[0] == '(') {
                
                OB_pos = i;
                CB_pos = -1;
            }
            else if (w.back() == ')') {
                
                
                CB_pos = i;
            }

            // now check if all four consecutively are not -1, means we just found a statement
            if (IF_pos!=-1 && THEN_pos!=-1 && OB_pos!=-1 && CB_pos!=-1) {
                Expression* exp = new Expression();
                // statement found

                // extract condition
                string condition = "";
                for (int j = IF_pos + 1; j<=THEN_pos - 1; j++) {
                    if (j!=THEN_pos - 1) {
                        condition+=broken[j] + " ";
                    }
                    else {
                        condition+=broken[j];

                    }
                }
                exp->set_condition(condition);

                // extract action part
                string action = "";
                for (int j = THEN_pos+1; j<=CB_pos; j++) {
                    if (j!=CB_pos) {
                        action+=broken[j] + " ";
                    } else {
                        action+=broken[j];

                    }
                }
                exp->set_action(action);
                expressions.push_back(exp);

                // reset positions
                IF_pos = -1;
                THEN_pos = -1;
                OB_pos = -1;
                CB_pos = -1;
            }


        }
        
        return expressions;
    }
};

class Condition_Checker {
private:
    static Condition_Checker* instance;
    Condition_Checker() {};
    
public:
    ~Condition_Checker() {};
    static Condition_Checker* get_instance() {
        return Condition_Checker::instance;
    }
    bool check(Expression* expression) {
        // this checks the condition is true or not, for now we are just returning true, because high level business logic is not the main concern 
        return true;
    }
};

Condition_Checker* Condition_Checker::instance = new Condition_Checker();

// SSo, expression executor interface must need toolbar and properties inspector references to apply changes
class IExpression_Executor {
public:
    virtual void EXECUTE(Expression* expression, Properties_Inspector* properties_inspector, ToolBar* toolbar) = 0;
    virtual ~IExpression_Executor() {};
};

class Standard_Executor : public IExpression_Executor {
public:
    Standard_Executor() {};
    ~Standard_Executor() {};

    void EXECUTE(Expression* expression, Properties_Inspector* properties_Inspector, ToolBar* toolbar) {
        // now, we will first check whether condition is true
        if (Condition_Checker::get_instance()->check(expression)) {
            // now the condition is true, then action will be taken, we are leaving this for now
        }
    }
};

// =========PARSING AND APPLYING MECHANISM======











class WorkSpace : public Originator{
private:
    Canvas_View* canvas_view;
    Properties_Inspector* properties_inspector;
    ToolBar* toolbar;
    Notification_Panel* notification_panel;
    IMediator* mediator;
public:
    WorkSpace () {
        this->mediator = new Standard_Mediator();
        
        this->notification_panel = new Notification_Panel(this->mediator);
        this->mediator->add_member(UI_models::NOTIFICATION_PANEL,this->notification_panel);
        
        this->canvas_view = new Canvas_View(this->mediator);
        this->mediator->add_member(UI_models::CANVAS,this->canvas_view);
        
        this->toolbar = new ToolBar(this->mediator);
        this->mediator->add_member(UI_models::TOOLBAR,this->toolbar);
        
        this->properties_inspector = new Properties_Inspector(this->mediator);
        this->mediator->add_member(UI_models::PROPERTIES_INSPECTOR,this->properties_inspector);
    }
    ~WorkSpace() {
        delete this->mediator;
        delete this->notification_panel;
        delete this->canvas_view;
        delete this->toolbar;
        delete this->properties_inspector;
    }
    void restore(Memento* m) override {
        // here is the main step, the originator will restore its State elements with true copy !!
        State* s = m->get_state();

        // restore notification panel messages
        this->notification_panel->set_messages(s->get_messages());

        // restore toolbar properties
        this->toolbar->erase();
        this->toolbar->reset_properties(s->get_toolbar_map());

        // restore properties inspector
        this->properties_inspector->erase();
        this->properties_inspector->reset(s->get_current_select_id(), s->get_pi_map());

        // reset id to node tree
        ID_To_Node::get_instance()->erase();
        ID_To_Node::get_instance()->reset(s->get_id_number(), s->get_mp());

    }
    Memento* create_memento() override {
        // here , memento will be returned which will have true copy of state of originator
        Memento* memento = new Memento();

        // create a state object and give it to memento(state should have true copies)
        State* s = new State();
        // true copy tree attributes
        s->set_tree();
        // true copy properties inspector attributes
        s->set_properties_inspector_attributes(this->properties_inspector);
        // true copy properties of toolbar
        s->set_toolbar_properties(this->toolbar);
        // true copy notification panel messages
        s->set_messages_displayed(this->notification_panel);
        memento->set_state(s);
        return memento;
    }
};


int main() {
    
    return 0;
}