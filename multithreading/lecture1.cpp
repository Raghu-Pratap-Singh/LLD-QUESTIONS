#include <bits/stdc++.h>
#include <thread>
using namespace std;

void helloworld() {
    cout << "Hello World" << " " << this_thread::get_id() << endl;
}

int main() {
    cout << this_thread::get_id() << endl;

    helloworld();

    thread t(helloworld);
    t.join();
    /*
    t.join ensures that the program waits for the mention thread 't' to complete before continuing furthur
    */

    vector<thread> threads;
    for (int i = 0; i<5; i++) {
        
        threads.push_back(thread(helloworld));
        // as soon as we write thread(helloworld) it will start executing
    }

    for (int i = 0; i<5; i++) {
        threads[i].join();
    }

    /*
        these are not actual hardware threads,
        to see actual hardware threads needed to run this program, we use below
    */
    cout << thread::hardware_concurrency() << endl;
    return 0;
}