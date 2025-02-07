#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <random>
#include <mutex>
#include "horse.h"

std::mutex coutMutex;

Horse::Horse(std::string name, int finalDistance) 
    : name(name), 
      distance(0),
      finalDistance(finalDistance), 
      rng(std::random_device{}()), 
      distRun(1, 10), 
      distRest(1, 3) {}

std::string Horse::getName() {
    return name;
}

void Horse::run() {
    while (distance < finalDistance) {        
        choice = distRun(rng) % 2; // Random choice between 0 and 1
        if (choice == 0) {
            HorseRest();
        } else {
            HorseRun();
            HasFinished();
        }
    }
}

void Horse::reset() {
    distance = 0;
}

int Horse::getDistance() {
    if (distance >= finalDistance) {
        return finalDistance;
    }
    return distance;
}

void Horse::HorseRest(){
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << name + " is resting" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(distRest(rng))); 
}

void Horse::HorseRun() {
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << name + " is running" << std::endl;
    }

    while (distance < finalDistance) {
        distance += distRun(rng);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << name + " has run " << distance << " meters" << std::endl;
        }
    }
    distance = finalDistance;
    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << name + " has finished" << std::endl;
}

bool Horse::HasFinished() {
    return distance >= finalDistance;
}