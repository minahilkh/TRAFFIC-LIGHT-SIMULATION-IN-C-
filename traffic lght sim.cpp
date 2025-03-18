#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

enum class LightState { RED, YELLOW, GREEN };

class TrafficLight {
public:
    LightState state;

    TrafficLight() { state = LightState::RED; }

    void simulate() {
        while (true) {
            state = LightState::GREEN;
            cout << "Traffic Light: GREEN (Vehicles moving)\n";
            this_thread::sleep_for(chrono::seconds(5));

            state = LightState::YELLOW;
            cout << "Traffic Light: YELLOW (Prepare to stop)\n";
            this_thread::sleep_for(chrono::seconds(2));

            state = LightState::RED;
            cout << "Traffic Light: RED (Vehicles stopping)\n";
            this_thread::sleep_for(chrono::seconds(5));
        }
    }
};

void vehicleSimulation(queue<int>& vehicles, mutex& mtx) {
    while (true) {
        this_thread::sleep_for(chrono::seconds(3));

        lock_guard<mutex> lock(mtx);
        vehicles.push(1);
        cout << "Vehicle arrived! Queue size: " << vehicles.size() << endl;
    }
}

void processVehicles(queue<int>& vehicles, TrafficLight& light, mutex& mtx) {
    while (true) {
        if (light.state == LightState::GREEN) {
            lock_guard<mutex> lock(mtx);
            while (!vehicles.empty()) {
                vehicles.pop();
                cout << "Vehicle passed! Queue size: " << vehicles.size() << endl;
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    TrafficLight trafficLight;
    queue<int> vehicles;
    mutex mtx;

    thread lightThread(&TrafficLight::simulate, &trafficLight);
    thread vehicleThread(vehicleSimulation, ref(vehicles), ref(mtx));
    thread processThread(processVehicles, ref(vehicles), ref(trafficLight), ref(mtx));

    lightThread.join();
    vehicleThread.join();
    processThread.join();

    return 0;
}