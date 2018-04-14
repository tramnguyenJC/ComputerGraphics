//////////////////////////////////////////////////////////////////////////////
// @brief: This class represents a particle generator that generates particles
//		   at randomized positions and randomized initial velocities
#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H
#include "Particle.h"
enum GeneratorType {PointType, LineType, CircleType, BeamType, FireType};

//////////////////////////////////////////////////////////////////////////////
// @brief: Point Generator - all particles share same spawning position,
//		   just with different velocities
struct Point {
	glm::vec3 pos;
	glm::vec3 normal{glm::vec3(0,0,0)}; ///< only used for fireworks
};

//////////////////////////////////////////////////////////////////////////////
// @brief: Line Generator - all particles spawn along the line (specified by 2 
//		   endpoints) with a random uniform/normal distribution, and their 
//		   velocities are in the direction of the specified normal vector
struct Line { 
	glm::vec3 endPoint1;
	glm::vec3 endPoint2;
	glm::vec3 normal;
};

//////////////////////////////////////////////////////////////////////////////
// @brief: Circle Generator - all particles spawn inside the circle surface
//		   with a random uniform/normal distribution, and their velocities are
//		   in the direction of the specified normal vector
struct Circle{
	glm::vec3 center;
	float radius;
	glm::vec3 normal;
};

class ParticleGenerator {
public:
	
	ParticleGenerator(ifstream& inputFileStream, float timeStep);
	static void parseInGenerators(string inputFile,
		vector<shared_ptr<ParticleGenerator>>& generators, float timeStep);
	void generateParticles();
	void draw();
	glm::vec3 generateForces(shared_ptr<Particle> particle);
	vector<shared_ptr<Particle>>& getParticles() { return particles; }
	float getSize() { return size; }

private:
	int maxParticles;		   //< Maximum number of particles in system
	vector<shared_ptr<Particle>> particles;	  //< List of particles in the system
	string distribution;	   		//< Can have 2 values: "normal" or "uniform"
	// To create variance in randomly generated particles, some particle attributes
	// have a random value in between a specified min and max time values
	int minimumLife;		   		//< Minimum longevity of particles (in second)
	int maximumLife;		   		//< Maximum longevity of particles (in second)
	int minNumGenerated;	   		//< Minimum number of particles generated each
							   		//< time generateParticles() is called
	int maxNumGenerated;	  		//< Maximum number of particles generated each
							   		//< time generateParticles() is called
	int timeIntervalGeneration;		//< Time in between generations
	int currTime;		  			//< Current time in system
	float elasticity; 		   		//< Elasticity constant
	float friction;   		   		//< Friction constant
	float maxVel;			   		//< Max velocity
	float minVel;			   		//< Min velocity
	glm::vec3 gravity;		   		//< Gravity force
	vector<glm::vec3> colors;	 	//< Color stages of particle
	float size;						//< size of particles
	float mass;						//< mass of particles
	float timeStep;					//< timeStep value
	float alphaStart;				//< starting alpha value of particle
	float alphaEnd;					//< ending alpha value of particle
	float sd {0.5};					//< Standard deviation (for some generator this
									//< variable is needed)
	float forceStrength;			//< Needed for fire generator
	////////////////////////////////////////////////////////////////////////////
	// Generator type
	GeneratorType type;		   		//< Type of generator
	Line line;				   		//< Line generator
	Point point;			   		//< Point generator
	Circle circle;		       		//< Circle generator
	void pointGenerate(float sd);   //< Generate particles in uniform distribution
									//< with pointGenerator
	void lineGenerate();			//< Generate particles in uniform distribution
									//< with lineGenerator
	void circleGenerate(float sd);	//< Generate particles in uniform distribution
									//< with circleGenerator
	void beamGenerate();			//< Generate particles in uniform distribution
									//< with  beamGenerator
	void fireGenerate(float sd);	//< Generate particles in uniform distribution
									//< with  beamGenerator
	//< Generate force for fire particles
	glm::vec3 fireForce(shared_ptr<Particle> particle);

	////////////////////////////////////////////////////////////////////////////
	// Helper methods
	void tokenize(string input, string split, vector<string>& vec);
	void populateTypeInfo(vector<string>& tokens);
};


#if   defined(OSX)
#pragma clang diagnostic pop
#endif
#endif