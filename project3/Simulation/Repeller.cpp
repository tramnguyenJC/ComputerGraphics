#include "Repeller.h"


//////////////////////////////////////////////////////////////////////////////
// @brief: Default constructor
Repeller::Repeller(glm::vec3 pos, float range, float strength){
	this->pos = pos;
	this->range = range;
	this->strength = strength;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: retrieve force of repeller acting on particle
// @param p: given particle
glm::vec3 
Repeller::getForce(shared_ptr<Particle> p){
	glm::vec3 force = glm::vec3(0,0,0);
	if(!activated)
		return force;
	glm::vec3 pPos = p->getCurrentState().pos;
	glm::vec3 dir = pPos - pos;
	if(glm::length(dir) < range){
		force += (dir*(strength));
	}
	return force;
}