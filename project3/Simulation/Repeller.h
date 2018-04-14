#include "Particle.h"

class Repeller {
public:
	Repeller(glm::vec3 pos, float range, float strength);
	glm::vec3 getForce(shared_ptr<Particle> p);
	glm::vec3 getPos(){ return pos;}
	void toggleActivate(){ activated = !activated;}
private:
	glm::vec3 pos;				///< Position of repeller
	float range;				///< Range repeller can affect other particles
	float strength;				///< Strength of the force from repeller
	bool activated{false};		///< True if activated, false if not
								///< Only serves the purpose of demonstration
};