#include "Attractor.h"

Attractor::Attractor(glm::vec3 pos, float range, float strength){
	this->pos = pos;
	this->range = range;
	this->strength = strength;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: retrieve force of attractor acting on particle
// @param p: given particle
glm::vec3 
Attractor::getForce(shared_ptr<Particle> p){
	glm::vec3 force = glm::vec3(0,0,0);
	if(!activated)
		return force;
	glm::vec3 pPos = p->getCurrentState().pos;
	glm::vec3 dir = pos - pPos;
	if(glm::length(dir) < range){
		force += (dir*(strength));
	}
	return force;
}