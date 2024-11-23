#pragma once
#include <string>
#include <fstream>
#include "GamesEngineeringBase.h"
#include <iostream>
#include "Vector.h"
#include "camera.h"
#include <vector>

extern bool running;
extern bool aoeTriggered;
extern float heroProjectileSpeed;
extern float npcProjectileSpeed;
extern int aoeNumber;
extern float score;
extern bool saveTriggered;
extern int level;

struct Effect {
    Vector position;
    float duration;
    float elapsedTime;
};

bool fileExists(std::string filename);

int getLevel();

void vecNormalize(Vector& vec);

void saveString(std::ofstream& binout, const std::string& str);

std::string loadString(std::ifstream& infile);

void renderImg(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Image& sprite, Vector& pos, Camera& cm);

bool checkImageCollision(GamesEngineeringBase::Image& img1, unsigned int x1, unsigned int y1, GamesEngineeringBase::Image& img2, unsigned int x2, unsigned int y2);