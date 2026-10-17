/*
    lock_guard<mutex> lock(mtx)
    {
        this is automatically unlocked when all lines of that block is finished

    }


    unique_lock<mutex> lock(mtx)

    {
        we can unlock this at will
        using: lock.unlock()
        
    }

    Granularity of locking:
    make the lock region only on critical section
    and not on many unnecessary lined of code



    (VERY IMPORTANT ⭐⭐) SHARED OR READ WRITE LOCKS
    IF WE HAVE A WRITE AND A READ FUNCTION, WE CAN ALLOW MULTIPLE READS TO HAPPEN TOGETHER, BUT WHEN A WRITE IS HAPPENING WE DONT WANT ANY THREAD TO READ OR WRITE

    =>MUTUALLY EXCLUSIVE LOCKING ON WRITE
    =>SHARED LOCK ON READ

    void write(...) {
        unique_lock<shared_mutex> lock(shared_mtx);
        ...
        ...
        ...
        lock.unlock();
    }

    void read(...) {
        shared_lock<shared_mutex> lock(shared_mtx);
        ...
        ...
        ...
    }

    REMEMBER : ALL READS CAN BE PARALLELY EXECUTED,
    BUT WRITE REQUEST WILL HAVE MUTUALLY EXCLUSIVE LOCK AND HENCE NO OTHER READ OR WRITE CAN HAPPEN BECAUSE WE ARE USING SAME MUTEX SHARED

    we will test different scenarios below
*/

// this_thread::sleep_for(chrono::milliseconds(300));
#include <bits/stdc++.h>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;


shared_mutex s_mtx;
// adds 1 to index of vector a
void write(vector<int>& a, int index) {
    unique_lock<shared_mutex> lock(s_mtx);
    cout << "==== thread_no : " << this_thread::get_id() <<" ====" << endl; 
    cout << "prev value at index " << index << " " << a[index] << endl;
    this_thread::sleep_for(chrono::milliseconds(1000));
    a[index]+=2;
    cout << "new value at index " << index << " " << a[index] << endl;
    cout << "=======================" << endl;
    lock.unlock();
}

void read(vector<int>& a, int index) {
    shared_lock<shared_mutex> lock(s_mtx);
    cout << "==== thread_no : " << this_thread::get_id() <<" ====" << endl; 
    cout << "value at index " << index << " : " << a[index] << endl;
    this_thread::sleep_for(chrono::milliseconds(2000));
    cout << "=======================" << endl;
    
    
}

int main() {
    vector<int> a = {3, 4, 5, 6, 7, 8};
    
    // thread 1 : peforms a read
    thread t1(read, ref(a), 2);
    thread t2(read, ref(a), 1);

    // write threads
    thread t3(write, ref(a), 3);

    // read
    thread t4(read, ref(a), 3);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    return 0;
};
