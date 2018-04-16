#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <stdlib.h>
#include <string>
#include <memory>

// #include "../Models/Model.h" Camera already includes Model.h
#include "Attractor.h"
#include "../Utilities/Camera.h"
#include "ParticleGenerator.h"
#include "Repeller.h"
#include "Obstacle.h"
class Simulator {

public:
	Simulator(string inputFile);
	void draw();
	void generateScene();
	void update();
	vector<shared_ptr<Repeller>>& getRepellers(){ return repellers; }
	vector<shared_ptr<Attractor>>& getAttractors(){ return attractors; }
	vector<shared_ptr<ParticleGenerator>>& getGenerators() { return generators; }
	Camera& getCamera() {return camera;}

private:
	vector<shared_ptr<Model>> staticModels;
	vector<shared_ptr<ParticleGenerator>> generators;
	vector<shared_ptr<Obstacle>> obstacles;
	vector<shared_ptr<Repeller>> repellers;
	vector<shared_ptr<Attractor>> attractors;

	Camera camera;

	// Scene parameters
	glm::vec3 sceneColor;
	glm::vec3 planeColor;
	int planeSize;
	float timeStep;
	string staticModelFile;
	glm::vec3 cameraPos;
	glm::vec3 cameraDir;
	void drawPlane();

};
#endif