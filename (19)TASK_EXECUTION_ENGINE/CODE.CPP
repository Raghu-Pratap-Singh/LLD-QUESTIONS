#include <bits/stdc++.h>
using namespace std;

// single ton logger class
class Logger {
private:
    int line_no;
    static Logger* instance;
    Logger() {
        this->line_no = 0;
    };
public:
    ~Logger() {};
    static Logger* get_instance() {
        return Logger::instance;
    }

    void log_message_on_terminal(string message) {
        cout << "\n";
        cout << "LINE NUMBER : " << this->line_no << endl;
        cout << message << endl;
        this->line_no++;
    }

    void log_message_in_file(string file_path, string message) {
        // this uses file handling to log the message in the mentioned file path file
        // for now we are leaving it
    }

};

enum class Response {
    SUCCESS,
    FAILED,
    ALREADY_COMPLETED,
    RUNNING,
    SKIPPED,
    RETRIED,
    PENDING,
    QUEUED
};

Logger* Logger::instance = new Logger();
// keeping Task scalable
class Module;

class Task;

class State {
public:
    virtual Response EXECUTE(Task* t, Module* mod) = 0;
    virtual void LOG_STATUS(Task* t) = 0;
    virtual Response GET_STATUS() = 0;
    virtual ~State() = default;
};

class Task {
protected:
    State* current_state;

    State* pending_state;
    State* queued_state;
    State* running_state;
    State* success_state;
    State* failed_state;
    State* skipped_state;
    State* retried_state;

    vector<string> dependencies;
    string task_id;
public:
    Task();
    ~Task();
    const string get_task_id() {
        return this->task_id;
    }
    void add_dependency(string task_id) {
        this->dependencies.push_back(task_id);
    }
    vector<string>& get_dependencies() {
        return this->dependencies;
    }

    State* get_pending_state() {
        return this->pending_state;
    }

    State* get_running_state() {
        return this->running_state;
    }
    State* get_queued_state() {
        return this->queued_state;
    }
    State* get_success_state() {
        return this->success_state;
    }
    State* get_failed_state() {
        return this->failed_state;
    }
    State* get_skipped_state() {
        return this->skipped_state;
    }
    State* get_retried_state() {
        return this->retried_state;
    }
    void set_state(State* s) {
        this->current_state = s;
    }
    virtual Response EXECUTE() = 0;
    Response GET_STATUS();
    void LOG_STATUS();
};

class Pending : public State {
public:
    Pending() {};
    Response EXECUTE(Task* t, Module* mod) override;
    Response GET_STATUS() override;
    void LOG_STATUS(Task* t) override;
};
class Queued : public State {
public:
    Queued() {};
    Response EXECUTE(Task* t, Module* mod) override ;
    Response GET_STATUS() override;
    void LOG_STATUS(Task* t) override;
};
class Running : public State {
public:
    Running() {};
    Response EXECUTE(Task* t, Module* mod) override;
    Response GET_STATUS() override;
    void LOG_STATUS(Task* t) override;
};
class Failed : public State {
public:
    Failed() {};
    Response EXECUTE(Task* t, Module* mod) override;
    Response GET_STATUS() override;
    void LOG_STATUS(Task* t) override;
};
class Skipped : public State {
public:
    Skipped() {};
    Response EXECUTE(Task* t, Module* mod) override;
    Response GET_STATUS() override;
    void LOG_STATUS(Task* t) override;
};
class Retried : public State {
public:
    Retried() {};
    Response EXECUTE(Task* t, Module* mod) override;
    Response GET_STATUS() override;
    void LOG_STATUS(Task* t) override;
};
class Success : public State {
public:
    Success() {};
    Response EXECUTE(Task* t, Module* mod) override;
    Response GET_STATUS() override;
    void LOG_STATUS(Task* t) override;
};










Task::Task () {
    this->pending_state = new Pending();
    this->queued_state = new Queued();
    this->retried_state = new Retried();
    this->running_state = new Running();
    this->skipped_state = new Skipped();
    this->failed_state = new Failed();
    this->success_state = new Success();
    this->current_state = this->pending_state;
}
Task::~Task() {
    this->current_state = nullptr;
    delete this->current_state;
    delete this->pending_state;
    delete this->running_state;
    delete this->success_state;
    delete this->failed_state;
    delete this->skipped_state;
    delete this->retried_state;
}
class Shell_Script_Task : public Task {
private:
    string code;
public:
    Shell_Script_Task(string task_id, string code) {
        this->task_id = task_id;
        this->code = code;
        
    }
    Response EXECUTE() override;
};

class DB_Query_Task : public Task {
    private:
    string query;
    public:
    DB_Query_Task(string task_id, string query) {
        this->task_id = task_id;
        this->query = query;
    }
    
    Response EXECUTE() override;
};

class HTTP_Request_Task : public Task {
    private:
    map<string, string> headers;
    public:
    HTTP_Request_Task(string task_id) {
        this->task_id = task_id;
    }
    
    void add_header(string key, string value) {
        this->headers[key] = value;
    }
    
    Response EXECUTE() override;
    
};

class File_Format_Conversion_Task : public Task {
    private:
    string file_path;
    string target_format;
    
    public:
    File_Format_Conversion_Task(string task_id, string file_path, string target) {
        this->task_id = task_id;
        this->file_path = file_path;
        this->target_format = target;
    }
    Response EXECUTE() override;
    
};

class File_Aggregation_Task : public Task {
    private:
    vector<string> file_paths_to_be_merged;
    public:
    File_Aggregation_Task(string task_id) {
        this->task_id = task_id;
    }
    
    void add_file_path(string path) {
        this->file_paths_to_be_merged.push_back(path);
    }
    
    Response EXECUTE() override;
};

class Data_Cleansing_Task : public Task {
    private:
    string path;
    public:
    Data_Cleansing_Task(string task_id, string path) {
        this->task_id = task_id;
        this->path = path;
    }
    Response EXECUTE() override;

};












// modules
// we are following dependency inversion principle
// that is using abstractions to connect low level modules

class Module {
public:
    virtual bool RUN_TASK(Task* t) = 0;
    virtual ~Module() = default;
};

// NOTE: all these modules will change state of tasks accordingly forward depending on how their execution went using tasks::change_state function
class Shell_Module : public Module {
public:
    bool RUN_TASK(Task* t) {
        // this takes the shell script task and runs
        Logger::get_instance()->log_message_on_terminal("Running Shell task : " + t->get_task_id());
        Shell_Script_Task* original = dynamic_cast<Shell_Script_Task*> (t);

        // for demo purposes we areretruning 1
        return true;
    }
    
};

class DB_Module : public Module {
    public:
    bool RUN_TASK(Task* t) {
        // this takes the shell script task and runs
        Logger::get_instance()->log_message_on_terminal("Running Database task : " + t->get_task_id());
        DB_Query_Task* original = dynamic_cast<DB_Query_Task*> (t);
        return true;
    }
    
};

class HTTP_Request_Module : public Module {
    public:
    bool RUN_TASK(Task* t) {
        // this takes the shell script task and runs
        Logger::get_instance()->log_message_on_terminal("Running HTTP query task : " + t->get_task_id());
        HTTP_Request_Task* original = dynamic_cast<HTTP_Request_Task*> (t);
        return true;
    }
    
};

class File_Format_Converter_Module : public Module {
    public:
    bool RUN_TASK(Task* t) {
        // this takes the shell script task and runs
        Logger::get_instance()->log_message_on_terminal("Running File format conversion task : " + t->get_task_id());
        File_Format_Conversion_Task* original = dynamic_cast<File_Format_Conversion_Task*> (t);
        return true;
    }
    
};

class File_Aggregator_Module : public Module {
    public:
    bool RUN_TASK(Task* t) {
        // this takes the shell script task and runs
        Logger::get_instance()->log_message_on_terminal("Running File aggregation task : " + t->get_task_id());
        File_Aggregation_Task* orignal = dynamic_cast<File_Aggregation_Task*> (t);
        return true;
    }
    
};

class Data_Cleanser_Module : public Module {
    public:
    bool RUN_TASK(Task* t) {
        // this takes the shell script task and runs
        Logger::get_instance()->log_message_on_terminal("Running Data cleaning task : " + t->get_task_id());
        Data_Cleansing_Task* original = dynamic_cast<Data_Cleansing_Task*> (t);
        return true;
    }
    
};

class Module_Distributer {
private:
    Shell_Module* sm;
    DB_Module* dbm;
    HTTP_Request_Module* rm;
    File_Format_Converter_Module* ffcm;
    File_Aggregator_Module* fam;
    Data_Cleanser_Module* dcm;

    static Module_Distributer* instance;
    Module_Distributer() {
        this->sm = new Shell_Module();
        this->dbm = new DB_Module();
        this->rm = new HTTP_Request_Module();
        this->ffcm = new File_Format_Converter_Module();
        this->fam = new File_Aggregator_Module();
        this->dcm = new Data_Cleanser_Module();
    }
public:
    static Module_Distributer* get_instance() {
        return Module_Distributer::instance;
    }

    ~Module_Distributer() {
        delete this->dbm;
        delete this->dcm;
        delete this->fam;
        delete this->ffcm;
        delete this->rm;
        delete this->sm;
    }

    // getters;
    Shell_Module* get_shell_module() {
        return this->sm;
    }
    DB_Module* get_DB_module() {
        return this->dbm;
    }

    HTTP_Request_Module* get_http_request_module() {
        return this->rm;
    }

    File_Format_Converter_Module* get_file_format_converter_module() {
        return this->ffcm;
    } 

    File_Aggregator_Module* get_file_aggregator_module() {
        return this->fam;
    }

    Data_Cleanser_Module* get_data_cleanser_module() {
        return this->dcm;
    }
    
};
Module_Distributer* Module_Distributer::instance = new Module_Distributer();










Response Queued::EXECUTE(Task* t, Module* mod) {
    // if this method is called from queue on pending state of a task, then it means it prerequisite tasks are done and it first enters running state
    t->set_state(t->get_running_state());
    bool response = mod->RUN_TASK(t);
    if (response) {
        return Response::SUCCESS;
    }
    return Response::FAILED;
}

void Queued::LOG_STATUS(Task* t) {
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() + " is in Queued state.");
}


Response Pending::EXECUTE(Task* t, Module* mod) {
    // if this method is called from queue on pending state of a task, then it means it prerequisite tasks are done and it first enters running state
    
    return Response::PENDING;
}

void Pending::LOG_STATUS(Task* t) {
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() + " is in Pending state.");
}

Response Running::EXECUTE(Task* t, Module* mod) {
    // if execute is called on a running task, it is returned
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() +" is already running...");
    return Response::RUNNING;
}

void Running::LOG_STATUS(Task* t) {
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() + " is in Running state.");
}

Response Success::EXECUTE(Task* t, Module* mod) {
    // now if the execute function is called on already successful task, nothing happens
    
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() +" is already completed...");
    return Response::ALREADY_COMPLETED;
}

void Success::LOG_STATUS(Task* t) {
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() + " is in Success state.");
}

Response Failed::EXECUTE(Task* t, Module* mod) {
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() +" has failed to complete...");
    return Response::ALREADY_COMPLETED;
    
}

void Failed::LOG_STATUS(Task* t) {
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() + " is in Failed state.");
}

Response Skipped::EXECUTE(Task* t, Module* mod) {
    
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() +" will not BE EXECUTED as its pre-requisite task were not executed properly, so it is skipped for now...");
    return Response::SKIPPED;
}

void Skipped::LOG_STATUS(Task* t) {
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() + " is in Skipped state.");
}

Response Retried::EXECUTE(Task* t, Module* mod) {
    Logger::get_instance()->log_message_on_terminal("This task: " + t->get_task_id() + " is added to queue again for later retry");
    return Response::RETRIED;
}

void Retried::LOG_STATUS(Task* t) {
    Logger::get_instance()->log_message_on_terminal("Task: " + t->get_task_id() + " will be retried again.");
}


Response Queued::GET_STATUS() {
    return Response::QUEUED;
}
Response Pending::GET_STATUS() {
    return Response::PENDING;
}
Response Running::GET_STATUS() {
    return Response::RUNNING;
}
Response Success::GET_STATUS() {
    return Response::SUCCESS;
}
Response Skipped::GET_STATUS() {
    return Response::SKIPPED;
}
Response Failed::GET_STATUS() {
    return Response::FAILED;
}
Response Retried::GET_STATUS() {
    return Response::RETRIED;
}


void Task::LOG_STATUS() {
    this->current_state->LOG_STATUS(this);
}
Response Task::GET_STATUS() {
    return this->current_state->GET_STATUS();
}
Response Shell_Script_Task::EXECUTE() {
    return this->current_state->EXECUTE(this, Module_Distributer::get_instance()->get_shell_module());
}
Response DB_Query_Task::EXECUTE() {
    return this->current_state->EXECUTE(this, Module_Distributer::get_instance()->get_DB_module());
}
Response HTTP_Request_Task::EXECUTE() {
    return this->current_state->EXECUTE(this, Module_Distributer::get_instance()->get_http_request_module());
}
Response File_Format_Conversion_Task::EXECUTE() {
    return this->current_state->EXECUTE(this, Module_Distributer::get_instance()->get_file_format_converter_module());
}
Response File_Aggregation_Task::EXECUTE() {
    return this->current_state->EXECUTE(this, Module_Distributer::get_instance()->get_file_aggregator_module());
}
Response Data_Cleansing_Task::EXECUTE() {
    return this->current_state->EXECUTE(this, Module_Distributer::get_instance()->get_data_cleanser_module());
}

class Task_Map {
private:
    map<string, Task*> tasks;
public:
    Task_Map() {};
    void add_task(Task* t) {
        this->tasks[t->get_task_id()] = t;
    }
    Task* get_task(string task_id) {
        if (this->tasks.find(task_id)!=this->tasks.end()) {
            return this->tasks[task_id];
        }
        return nullptr;
    }
    bool has_task(string task_id) {
        return this->tasks.find(task_id)!=this->tasks.end();
    }
    ~Task_Map() {
        for (auto &[k, v] : this->tasks) {
            delete v;
        }
    }
};

class DAG {
private:
    Task_Map* tm;
    map<string, vector<string>> adj;
    string graph_id = "";
    friend class DAG_Builder;
public:
    DAG() {
        this->tm = new Task_Map();
    }

    string get_graph_id() {
        return this->graph_id;
    }

    ~DAG() {
        delete this->tm;
    }

    // getters
    Task_Map* get_task_map() {
        return this->tm;
    }

    map<string, vector<string>>& get_adj() {
        return this->adj;
    }
    
    

};

class Connection_Verifier {
private:
static Connection_Verifier* instance;
    Connection_Verifier() {};
public:
    static Connection_Verifier* get_instance() {
        return Connection_Verifier::instance;
    }

    bool is_valid(DAG& graph) {
        for (auto &[task_id, v] : graph.get_adj()) {
            if (!graph.get_task_map()->has_task(task_id)) {
                return false;
            }
            // now check its dependecncies : tasks which depend on this task
            for (const string child_id : v) {
                if (!graph.get_task_map()->has_task(child_id)) {
                    return false;
                }
            }
        }
        return true;
    }
};

class Cycle_Detector {
private:
    static Cycle_Detector* instance;
    Cycle_Detector() {};
public:
    static Cycle_Detector* get_instance() {
        return Cycle_Detector::instance;
    }

    bool dfs(const string& node, const string& parent, map<string, vector<string>>& adj, set<string>& visited, set<string>& path_visited) {
        visited.insert(node);
        path_visited.insert(node);
        
        // recurse
        for (string& child : adj[node]) {
            if (visited.find(child) == visited.end()) {
                // recurse
                bool response = dfs(child, node, adj, visited, path_visited);
                if (response) {
                    return true;
                }


            } else if (path_visited.find(child)!=path_visited.end()) {
                // cycle found in directed graph
                return true;
            }

        }

        // backtrack
        path_visited.erase(node);
        return false;
    }

    bool is_valid(DAG& graph) {
        set<string> visited;
        for (auto &[task_id, v] : graph.get_adj()) {
            if (visited.find(task_id) == visited.end()) {
                set<string> path_visited;
                string parent = "";
                bool response = this->dfs(task_id, parent, graph.get_adj(), visited, path_visited);
                if (response) {
                    return false;
                }
            }
        }
        return true;
    }

};

Connection_Verifier* Connection_Verifier::instance = new Connection_Verifier();
Cycle_Detector* Cycle_Detector::instance = new Cycle_Detector();

class DAG_Builder {
private:
    DAG graph;
public:
    DAG_Builder() {};
    ~DAG_Builder() {};

    DAG_Builder& add_task(Task* t) {
        this->graph.adj[t->get_task_id()] = t->get_dependencies();
        this->graph.tm->add_task(t);
        return *this;
    }

    DAG_Builder& set_graph_id(string id) {
        this->graph.graph_id = id;
        return *this;
    }

    DAG* build() {
        // perform both checks for invalid dag
        if (this->graph.graph_id == ".") {
            Logger::get_instance()->log_message_on_terminal("Assign an ID to graph...");
            return nullptr;
        }
        // check that for all members in the adjacency map, the pointer should be in task map
        bool is_connection_correct = Connection_Verifier::get_instance()->is_valid(this->graph);
        // perform a cycle check, there should be no cycle
        bool cycle_not_present = Cycle_Detector::get_instance()->is_valid(this->graph);

        if (!is_connection_correct) {
            Logger::get_instance()->log_message_on_terminal("Connection is not correct, please generate a valid directed graph...");
            return nullptr;
        }
        
        else if (!cycle_not_present) {
            Logger::get_instance()->log_message_on_terminal("Graph has a cycle...");
            
            return nullptr;
        }
        return &this->graph;
    }

};

class Starting_Task_Finder {
private:
    static Starting_Task_Finder* instance;
    Starting_Task_Finder() {};
public:
    static Starting_Task_Finder* get_instance() {
        return Starting_Task_Finder::instance;
    }

    vector<Task*> identify_starting_tasks(DAG* graph) {
        map<string, int> f;
        for (auto &[k, v] : graph->get_adj()) {
            f[k] = 0;
            for (string& child : v) {
                f[child]++;
            }
        }

        vector<Task*> ans;
        for (auto&[k,v] : f) {
            if (v == 0) {
                ans.push_back(graph->get_task_map()->get_task(k));
            }
        }
        return ans;
    }

};

Starting_Task_Finder* Starting_Task_Finder::instance = new Starting_Task_Finder();

class Tool;
class Execution_Handler {
protected:
    Tool* tool;
public:
    virtual void execute_task(Task* t) = 0;
    virtual ~Execution_Handler() = default;
};

// thisis a standard executioner, it doesnt offer any retries
class Standard_Execution_Handler : public Execution_Handler {
public:
    Standard_Execution_Handler(Tool* tool) {
        this->tool = tool;
    }
    ~Standard_Execution_Handler() {
        this->tool = nullptr;
    }
    void execute_task(Task* t) override;
};

// this offers a retry limit per task
// this is a retry handler which doesnt give any delay time, just normally inserts the tasks again in queue until their retry limit is reached
class Retry_Execution_Handler : public Execution_Handler {
private:
    int retry_limit;
    map<string, int> trial_number;
public:
    Retry_Execution_Handler(int n, Tool* tool) {
        this->retry_limit = n;
        this->tool = tool;
    }

    void execute_task(Task* t) override;
};

// we can easily add a new handler with delayed addition to queue, no big deal but that is not the concern here, so we are just leaving it


class Scheduler {
protected:
    Tool* tool;
public:
    virtual void schedule_dag(DAG* graph, long long gap) = 0;
    virtual ~Scheduler() {};
};

class Standard_Scheduler : public Scheduler {
private:

    mutex mtx;
    map<string, DAG*> id_to_graph_map;
    priority_queue<pair<long long, string>> scheduled_time;
    thread t1;
    condition_variable cv;
    bool stop = false;
    bool just_task_pushed = false;
    long long next_wake_up_wait_time;
    bool check() {
        return (this->just_task_pushed || this->stop);
    }
   
public:
    void CONSUME();
    Standard_Scheduler(Tool* tool) {
        this->next_wake_up_wait_time = 1e5;
        this->tool = tool;

        this->t1 = thread(&Standard_Scheduler::CONSUME, this);
    }

    ~Standard_Scheduler() {
        
        unique_lock<mutex> lock(this->mtx);
        this->stop = true;
        
        lock.unlock();
        
        
        this->cv.notify_one();
        if (this->t1.joinable()) {
            this->t1.join();
        }
        this->tool = nullptr;

    }

    void schedule_dag(DAG* dag, long long gap_in_millisconds_from_now) override{
        
        auto wall_now = chrono::steady_clock::now();
        long long time_ms = chrono::duration_cast<chrono::milliseconds>(wall_now.time_since_epoch()).count();
        
        unique_lock<mutex> lock(this->mtx);
        if (this->stop) {
            return;
        }
        this->id_to_graph_map[dag->get_graph_id()] = dag;
        this->scheduled_time.push({-(time_ms + gap_in_millisconds_from_now), dag->get_graph_id()});
        this->just_task_pushed = true;
        lock.unlock();

        this->cv.notify_one();
    }
};

class Tool {
private:
    map<string, int> prerequisites_remaining;
    queue<Task*> tasks;
    thread t1;
    thread t2;
    thread t3;
    thread t4;
    condition_variable cv;
    mutex mtx;
    mutex prerequisite_mutex;
    bool stop = false;
    map<string, DAG*> node_to_graph;
    Execution_Handler* eh;
    Scheduler* sh;

    friend class Standard_Execution_Handler;
    friend class Retry_Execution_Handler;

    mutex& get_pm() {
        return this->prerequisite_mutex;
    }

    map<string, int>& get_prerequisite_map() {
        return this->prerequisites_remaining;
    }
    map<string, DAG*>& get_node_to_graph_map() {
        return this->node_to_graph;
    }
    void CONSUME() {
        while (1) {
            unique_lock<mutex> lock(this->mtx);

            // sleep until a task is in queue or shutdown is called
            this->cv.wait(lock, [this] {
                return !this->tasks.empty() || this->stop;
            });

            // shutdown when no tasks left
            if (this->stop && this->tasks.empty()) {
                return;
            }

            // critical section
            Task* task = this->tasks.front();
            this->tasks.pop();
            lock.unlock();

            // execute the task after releasing the lock
            this->eh->execute_task(task);

        }
    }

public:
    Tool(Execution_Handler* eh, Scheduler* sc) {
        this->t1 = thread(&Tool::CONSUME, this);
        this->t2 = thread(&Tool::CONSUME, this);
        this->t3 = thread(&Tool::CONSUME, this);
        this->t4 = thread(&Tool::CONSUME, this);
        this->eh = eh;
        this->sh = sc;
        
    }
    ~Tool() {
        unique_lock<mutex> lock(this->mtx);
        this->stop = true;
        lock.unlock();

        this->cv.notify_all();
        if (t1.joinable()) {
            t1.join();
        }
        if (t2.joinable()) {
            t2.join();
        }
        if (t3.joinable()) {
            t3.join();
        }
        if (t4.joinable()) {
            t4.join();
        }
        delete this->eh;
        delete this->sh;
    }

    void schedule_dag(DAG* graph, long long gap) {
        this->sh->schedule_dag(graph, gap);
    }

    void add_DAG(DAG* graph) {
        vector<Task*> starting_tasks = Starting_Task_Finder::get_instance()->identify_starting_tasks(graph);

        for (auto&[k,v] : graph->get_adj()) {
            
            this->node_to_graph[k] = graph;
            for (string& child : v) {
                this->prerequisites_remaining[child]++;
            }
        }

        
        // add these starting nodes to queue but irst get a lock
        unique_lock<mutex> lock(this->mtx);
        for (Task* t : starting_tasks) {
            this->tasks.push(t);
            t->set_state(t->get_queued_state());
        }
        lock.unlock();

        // after releasing the lock needed to modify the shared queue(tasks), notify the worker threads
        this->cv.notify_one();
    }

    void log_task_status(string task_id) {
        if (this->node_to_graph.find(task_id) == this->node_to_graph.end()) {
            Logger::get_instance()->log_message_on_terminal("Task: " + task_id + " is not present in consideration...");
            return;
        }
        this->node_to_graph[task_id]->get_task_map()->get_task(task_id)->LOG_STATUS();
    }

    void add_task(string task_id) {
        unique_lock<mutex> lock(this->mtx);
        Task* t = this->node_to_graph[task_id]->get_task_map()->get_task(task_id);
        this->tasks.push(t);
        t->set_state(t->get_queued_state());
        lock.unlock();

        // notify worker
        this->cv.notify_one();

    }

};

void Standard_Scheduler::CONSUME() {
        while (1) {
            unique_lock<mutex> lock(this->mtx);

            // sleep until a task is pushed in pq or stop is awakened or deadline is met
            string dag_id = "";
            this->cv.wait_for(lock, chrono::milliseconds(this->next_wake_up_wait_time),[this] {
                return check();
            });

            if (this->stop) {
                // means no more scheduled task and it is time to get over
                return;
            }

            // else there is a task just pushed in pq so wee need to set the timer for thread to wake up again and push the earliest upcoming task to tool.s function
            else if (!this->stop && this->just_task_pushed) {
                
                auto wall_now = chrono::steady_clock::now();
                long long time_ms = chrono::duration_cast<chrono::milliseconds>(wall_now.time_since_epoch()).count();
                long long scheduled_timing = -this->scheduled_time.top().first;
                long long wait_gap = scheduled_timing - time_ms;
                this->next_wake_up_wait_time = wait_gap;
                this->just_task_pushed = false;

                if (this->next_wake_up_wait_time <= 0) {
                    // time occured on insertion only
                    dag_id = this->scheduled_time.top().second;
                    this->scheduled_time.pop();

                    if (this->scheduled_time.size()>0) {

                        auto wall_now = chrono::steady_clock::now();
                        long long time_ms = chrono::duration_cast<chrono::milliseconds>(wall_now.time_since_epoch()).count();
                        long long scheduled_timing = -this->scheduled_time.top().first;
                        long long wait_gap = scheduled_timing - time_ms;
                        this->next_wake_up_wait_time = max(0LL, wait_gap);
                    } else {
                        
                        this->next_wake_up_wait_time = 1e5;
                    }
                }
            }

            // else the last cause of its wake up is that time met for the scheduled task
            else if (!this->stop && this->scheduled_time.size()>0 &&
    -this->scheduled_time.top().first <=
    chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now().time_since_epoch()
    ).count()) {
                // pop the earliest dag
                dag_id = this->scheduled_time.top().second;
                this->scheduled_time.pop();
                // update time of next pop
                if (this->scheduled_time.size()>0) {

                    auto wall_now = chrono::steady_clock::now();
                    long long time_ms = chrono::duration_cast<chrono::milliseconds>(wall_now.time_since_epoch()).count();
                    long long scheduled_timing = -this->scheduled_time.top().first;
                    long long wait_gap = scheduled_timing - time_ms;
                    this->next_wake_up_wait_time = max(0LL, wait_gap);
                } else {
                    
                    this->next_wake_up_wait_time = 1e5;
                }
                
            }

            // check if there is a task available for to be passed to tool
            if (dag_id == "" && !this->scheduled_time.empty()) {
                auto wall_now = chrono::steady_clock::now();
                long long time_ms = chrono::duration_cast<chrono::milliseconds>(
                    wall_now.time_since_epoch()).count();

                this->next_wake_up_wait_time =
                    max(0LL, -this->scheduled_time.top().first - time_ms);
            }
            if (dag_id=="") {
                continue;
            }
            DAG* graph = this->id_to_graph_map[dag_id];
            this->id_to_graph_map.erase(dag_id);
            lock.unlock();

            // if there is a graph then push it
            this->tool->add_DAG(graph);

        }
    }


void Standard_Execution_Handler::execute_task(Task* task) {
    task->set_state(task->get_running_state());
    Response response = task->EXECUTE();
    if (response == Response::SUCCESS) {
        task->set_state(task->get_success_state());
        // means the task was successfully executed
        // now traverse to its child nodes and push them to queue
        Logger::get_instance()->log_message_on_terminal("Task: " + task->get_task_id() + " is successfully completed.");

        // reduce its child prerequisite by 1
        unique_lock<mutex> lock(this->tool->get_pm());
        for (string& child : task->get_dependencies()) {
            if (this->tool->get_prerequisite_map().find(child)!=this->tool->get_prerequisite_map().end()) {

                this->tool->get_prerequisite_map()[child]--;
            }
        
        }
        lock.unlock();

        // now check if child count reaches 0, then add it in queue
        for (string& child : task->get_dependencies()) {
            if ((this->tool->get_prerequisite_map().find(child)!=this->tool->get_prerequisite_map().end()) && this->tool->get_prerequisite_map()[child] == 0 && (this->tool->get_node_to_graph_map()[child]->get_task_map()->get_task(child)->GET_STATUS()==Response::PENDING)) {
                // all requirements met for this child task
                this->tool->add_task(child);
            }
        
        }
    } else if (response == Response::FAILED) {
        // now as this is standard mechanism, no retry will be given and all its child task will be sent to skipped states
        task->set_state(task->get_failed_state());

        // now put all its child in skipped states and remove them from prerequisite map as they now cannot be completed
        unique_lock<mutex> lock(this->tool->get_pm());
        for (string& child : task->get_dependencies()) {
            if (this->tool->get_prerequisite_map().find(child)!=this->tool->get_prerequisite_map().end()) {

                
                this->tool->get_prerequisite_map().erase(child);
            }
            Task* child_task = this->tool->get_node_to_graph_map()[child]->get_task_map()->get_task(child);
            child_task->set_state(child_task->get_skipped_state());
        
        }
        lock.unlock();
    }
}

void Retry_Execution_Handler::execute_task(Task* task) {
    task->set_state(task->get_running_state());
    this->trial_number[task->get_task_id()]++;
    Response response = task->EXECUTE();
    if (response == Response::SUCCESS) {
        task->set_state(task->get_success_state());
        // means the task was successfully executed
        // now traverse to its child nodes and push them to queue
        Logger::get_instance()->log_message_on_terminal("Task: " + task->get_task_id() + " is successfully completed.");

        // reduce its child prerequisite by 1
        unique_lock<mutex> lock(this->tool->get_pm());
        for (string& child : task->get_dependencies()) {
            if (this->tool->get_prerequisite_map().find(child)!=this->tool->get_prerequisite_map().end()) {

                this->tool->get_prerequisite_map()[child]--;
            }
        
        }
        lock.unlock();

        // now check if child count reaches 0, then add it in queue
        for (string& child : task->get_dependencies()) {
            if ((this->tool->get_prerequisite_map().find(child)!=this->tool->get_prerequisite_map().end()) && this->tool->get_prerequisite_map()[child] == 0 && (this->tool->get_node_to_graph_map()[child]->get_task_map()->get_task(child)->GET_STATUS()==Response::PENDING)) {
                // all requirements met for this child task
                this->tool->add_task(child);
            }
        
        }
    } else if (response == Response::FAILED) {
        // now as this is retry mechanism, so if its chances remain, it will be pushed to tasks queue
        if (this->trial_number[task->get_task_id()] < this->retry_limit) {
            // means it can be retries
            task->set_state(task->get_retried_state());
            this->tool->add_task(task->get_task_id());
        
        }
        else {
            // its limit is exhausted, 
            task->set_state(task->get_failed_state());

            // put all its child to skipped state
            // now put all its child in skipped states and remove them from prerequisite map as they now cannot be completed
            unique_lock<mutex> lock(this->tool->get_pm());
            for (string& child : task->get_dependencies()) {
                if (this->tool->get_prerequisite_map().find(child)!=this->tool->get_prerequisite_map().end()) {

                    
                    this->tool->get_prerequisite_map().erase(child);
                }
                Task* child_task = this->tool->get_node_to_graph_map()[child]->get_task_map()->get_task(child);
                child_task->set_state(child_task->get_skipped_state());
            
            }
            lock.unlock();
        }
    }
}

int main() {
    return 0;
}