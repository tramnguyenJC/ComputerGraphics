//////////////////////////////////////////////////////////////////////////////
// Because of algorithmic complexities of detecting collision between 
// particles and arbitrary surfaces, and the programmer's lack of skills + time,
// this Obstacle class is currently limited to rectangular planes that is parallel
// to the axises. 
#ifndef OBSTACLE_H
#define OBSTACLE_H
#include "Particle.h"

class Obstacle {
public:
	Obstacle(ifstream& inputFileStream);
	Collision checkCollision(State oldState, State newState);
	static void parseInObstacles(string inputFile,
		vector<shared_ptr<Obstacle>>& obstacles);
	void draw();
private:
	glm::vec3 point1;			///< End point 1 of the rectangular plane
	glm::vec3 point2;			///< End point 2 of the rectangular plane
	glm::vec3 point3;			///< End point 3 of the rectangular plane
	glm::vec3 point4;			///< End point 4 of the rectangular plane
	glm::vec3 normal;			///< Normal vector of plane (parallel to axis)
	string style;				///< options: 'invisible', 'line', or 'solid'
	void tokenize(string input, string split, vector<string>& vec);
};
#endif