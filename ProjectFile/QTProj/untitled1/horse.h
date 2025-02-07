#ifndef HORSE_H
#define HORSE_H

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <random>
#include <vector>
#include <mutex>

class Horse {
    std::string name;
    int distance;
    int finalDistance;
    int choice;
    std::mt19937 rng; // Mersenne Twister random number generator
    std::uniform_int_distribution<int> distRun; // Distribution for running
    std::uniform_int_distribution<int> distRest; // Distribution for resting

public:
    Horse(std::string name, int finalDistance);

    std::string getName();

    void run();
    bool HasFinished();
    int getDistance();
    void reset();

private:
    void HorseRest();
    void HorseRun();
};

#endif // HORSE_H