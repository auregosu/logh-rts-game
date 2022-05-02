#ifndef TIMESTEP_HPP
#define TIMESTEP_HPP
#include <chrono>
#include <thread>

float idealFramerate = 60;
float framerate = idealFramerate;
float idealUpdateTime = 1/framerate;
float deltaTime = idealUpdateTime;
float timeScale = 1;

#endif