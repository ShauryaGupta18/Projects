#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>
using namespace std;

// ---------------- CONFIG ----------------
int NUM_THREADS = 4;
const int OPS = 10000;

// ---------------- SHARED STATE ----------------
atomic<int> shared_state(0);   // queue size simulation

atomic<int> push_count(0);
atomic<int> pop_count(0);

mutex mtx;

// ---------------- MODE ----------------
enum Mode { NO_SYNC, MUTEX, OPTIMIZED };

// ======================================================
// NO SYNC (RACE CONDITION SIMULATION - INTENTIONALLY WEAK)
// ======================================================
void worker_no_sync(int id) {

    for (int i = 0; i < OPS; i++) {

        if (i % 3000 == 0)
            cout << "Thread " << id << " at step " << i << endl;

        // ❌ race condition: read-modify-write without lock
        int temp = shared_state.load();

        temp++;            // push
        push_count++;

        // probabilistic pop → simulates inconsistency
        if (temp > 0 && (rand() % 3 == 0)) {
            temp--;
            pop_count++;
        }

        shared_state.store(temp);
    }

    cout << "Thread " << id << " finished\n";
}

// ======================================================
// MUTEX (CORRECT VERSION)
// ======================================================
void worker_mutex(int id) {

    for (int i = 0; i < OPS; i++) {

        if (i % 3000 == 0)
            cout << "Thread " << id << " at step " << i << endl;

        lock_guard<mutex> lock(mtx);

        shared_state++;
        push_count++;

        if (shared_state > 0) {
            shared_state--;
            pop_count++;
        }
    }

    cout << "Thread " << id << " finished\n";
}

// ======================================================
// OPTIMIZED (REDUCED LOCK SCOPE)
// ======================================================
void worker_optimized(int id) {

    for (int i = 0; i < OPS; i++) {

        if (i % 3000 == 0)
            cout << "Thread " << id << " at step " << i << endl;

        {
            lock_guard<mutex> lock(mtx);
            shared_state++;
        }
        push_count++;

        bool popped = false;

        {
            lock_guard<mutex> lock(mtx);
            if (shared_state > 0) {
                shared_state--;
                popped = true;
            }
        }

        if (popped) pop_count++;
    }

    cout << "Thread " << id << " finished\n";
}

// ======================================================
// RUN
// ======================================================
void run(Mode mode) {

    shared_state = 0;
    push_count = 0;
    pop_count = 0;

    vector<thread> threads;

    cout << "\n========================\n";

    if (mode == NO_SYNC) cout << "NO SYNCHRONIZATION\n";
    if (mode == MUTEX) cout << "MUTEX\n";
    if (mode == OPTIMIZED) cout << "OPTIMIZED\n";

    cout << "========================\n";

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_THREADS; i++) {

        if (mode == NO_SYNC)
            threads.emplace_back(worker_no_sync, i);
        else if (mode == MUTEX)
            threads.emplace_back(worker_mutex, i);
        else
            threads.emplace_back(worker_optimized, i);
    }

    for (auto &t : threads)
        t.join();

    auto end = chrono::high_resolution_clock::now();

    double time = chrono::duration<double>(end - start).count();

    int expected = NUM_THREADS * OPS;

    // ======================================================
    // 🔥 FINAL CORRECT METRIC: SYSTEM INTEGRITY ERROR (SIE)
    // ======================================================
    int sie = abs(push_count.load() - (pop_count.load() + shared_state.load()));

    double loss_ratio = (double)sie / expected;
    double throughput = (push_count + pop_count) / time;

    if (mode == NO_SYNC) cout << "\n--RESULT FOR NO SYNCHRONIZATION--\n";
    if (mode == MUTEX) cout << "\n--RESULT FOR MUTEX--\n";
    if (mode == OPTIMIZED) cout << "\n--RESULT FOR OPTIMIZED\n";
    cout << "Expected Ops:        " << expected << endl;
    cout << "Pushes:              " << push_count << endl;
    cout << "Pops:                " << pop_count << endl;
    cout << "Final State:         " << shared_state.load() << endl;

    cout << "Latency:             " << time << " sec\n";
    cout << "Throughput:          " << throughput << " ops/sec\n";

    cout << "System Integrity Error (SIE): " << sie << endl;
    cout << "Loss Ratio:                 " << loss_ratio << endl;
}

// ======================================================
// MAIN
// ======================================================
int main() {

    srand(time(0));

    run(NO_SYNC);
    run(MUTEX);
    run(OPTIMIZED);

    return 0;
}