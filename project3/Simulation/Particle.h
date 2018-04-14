#ifndef PARTICLE_H
#define PARTICLE_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <stdlib.h>
#include <string>
#include <random>
#include <time.h>    

// GLM
#include "../Utilities/glm/glm.hpp"
#include "../Utilities/glm/geometric.hpp"	// normalize, dot, cross,..
#include "../Utilities/glm/vec3.hpp" // glm::vec3
#define GLM_ENABLE_EXPERIMENTAL // To use glm::rotate
#include "../Utilities/glm/gtx/rotate_vector.hpp" // Rotate


#if   defined(OSX)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <GLUT/glut.h>
#elif defined(LINUX)
#include <GL/glut.h>

#endif
using namespace std;

struct State {
	glm::vec3 acc;			  ///< Acceleration vector of particle
	glm::vec3 pos;			  ///< Position vector of particle
	glm::vec3 vel;			  ///< Velocity vector of particle
};

struct Collision {
	float f = 0;              ///< Fraction of motion to collision
  	glm::vec3 n;    		  ///< Normal for collision response
  	bool none = true;		  ///< True if there was no collision
};

class Particle {
public:
	Particle(glm::vec3 pos, glm::vec3 vel, float mass, int longevity, 
		vector<glm::vec3> colors, float size, float maxVel,
		float elasticity, float friction, float alphaStart, float alphaEnd);
	void update(glm::vec3 force, State& state, float t);
	void draw();
	State& getCurrentState() { return state;}
	const size_t getTimeAlive(){ return timeAlive;}
	const size_t getLongevity(){ return longevity;}
	void resolveCollision(Collision& c);
	bool hasRested() { return rest; }
private:
	State state;			   ///< State of particle, containing information
							   ///< about its acceleration, velocity, position

	float mass;     		   ///< Mass constant, default to 1
	size_t timeAlive {0};	   //< Time particle has been alive	
	size_t longevity;		   ///< How long the particle "lives"
	vector<glm::vec3> colors;  ///< Color transitions
	glm::vec3 color;		   ///< Current color of the particle
	float maxVel;			   ///< Maximum velocity
	float size;		  	 	   ///< Size of particle
	float elasticity; 		   ///< Elasticity constant
	float friction;   		   ///< Friction constant
	float alphaStart;		   ///< Starting alpha of particle
	float alphaEnd;			   ///< Ending alpha of particle
	float alpha;			   ///< Current alpha of particle
	bool rest{false};      	   ///< Particle comes to rest (stop moving, but
							   ///< still displayed, so different from dead)

	void updateColor();
};
#endif