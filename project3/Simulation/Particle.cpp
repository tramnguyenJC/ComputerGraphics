#include "Particle.h"

//////////////////////////////////////////////////////////////////////////////
// @brief: Constructor for particle
// @param _pos: starting position of particle
// @param _vel: starting velocity of particle
// @param mass: the mass of particle
// @param longevity: the longevity of particle
// @param colorStart: color of particle when it's spawned
// @param colorEnd: color of particle when it's reaching its longevity
// @param size: size of particle
// @param maxVel: specified max velocity particle can have
Particle::Particle(glm::vec3 pos, glm::vec3 vel, float mass, int longevity, 
		vector<glm::vec3> colors, float size, float maxVel,
		float elasticity, float friction, float alphaStart, float alphaEnd){
	state.pos = pos;
	state.vel = vel;
	state.acc = glm::vec3(0,0,0);
	this->mass = mass;
	this->longevity = longevity;
	this->colors = colors;
	this->size = size;
	this->maxVel = maxVel;
	this->elasticity = elasticity;
	this->friction = friction;
	this->alphaStart = alphaStart;
	this->alphaEnd = alphaEnd;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Draw particle
void
Particle::draw(){
	glColor4f(color.x, color.y, color.z, alpha);
  	glVertex3f(state.pos.x, state.pos.y, state.pos.z);
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Resolve the given collision
// @param c given collision
void Particle::resolveCollision(Collision& c){
	// Dot equation a · b   =   a.x*b.x + a.y*b.y + a.z*b.z
	// so for example, if c.n = (0,1,0), then c.n dot vel gives vel.y,and 
	// vn = (0, vel.y, 0);
	glm::vec3 vn = c.n*(glm::dot(c.n, state.vel));
	// vt = (vel.x, 0, vel.z)
	glm::vec3 vt = state.vel - vn;
	// vn = (0, -vel.y * elasticity, 0)
	// Negate y to simulate reflection off surface, with regard to the normal dir 
	// of the surface (etc. bouncing back up for y). Elasticity is sth like 0.9, 
	// to slow the particle down after each bouncing
	vn *= (-elasticity);
	// vt = (1-f)*vt, friction is small, to also simulate friction for x,z
	vt *= (1 - friction);
	// velNew = (velNew.x, 0, velNew.z) + (0, velNew.y, 0);
	state.vel = vn + vt;

	//TODO remove hard constant for resting distance
	if(glm::length(state.vel) < 0.5)
		rest = true;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Update particle's current color based on the specified color
// 		   transitions and the time particle has been alive
void
Particle::updateColor(){
	float fractionAlive = (float)(timeAlive)/(float)longevity;
	glm::vec3 colorStart;
	glm::vec3 colorEnd;
	for(int i = 0; i < colors.size() - 1; i++){
		if(i * (1.f/(colors.size()-1)) < fractionAlive && 
			fractionAlive <= (i+1)*(1.f/(colors.size() -1))){
			colorStart = colors[i];
			colorEnd = colors[i+1];
			break;
		}
	}

	float colorX = colorStart.x + fractionAlive*(colorEnd.x - colorStart.x);
	float colorY = colorStart.y + fractionAlive*(colorEnd.y - colorStart.y);
	float colorZ = colorStart.z + fractionAlive*(colorEnd.z - colorStart.z);
	alpha = alphaStart + fractionAlive*(alphaEnd - alphaStart);
	color = glm::vec3(colorX, colorY, colorZ);
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Update particle's state with Euler's integration
// @param force: the cumulative forces affecting the particle
// @param s: the current state of particle
// @param t: timestep value
void
Particle::update(glm::vec3 force, State& s, float t){
	timeAlive++;

	updateColor();
	
	// Movement of particle
	s.acc = force*(1/mass); //F = m*a meaning a = F/m
	glm::vec3 velocity = s.vel + (s.acc*(t));
	if(glm::length(velocity) > maxVel) {
      glm::vec3 normalizedVel = glm::normalize(velocity);
      velocity = (normalizedVel*(maxVel));
    }
	s.vel = velocity;
	s.pos += (s.vel*(t));
	state = s;
	// cout << "state" << state.acc.x << " " << state.acc.y << " " << state.acc.z << endl;
	// cout << "FinalVelocity " << state.vel.x << " " << state.vel.y << " " << state.vel.z << endl;
	// cout << "position " << state.pos.x << " " << state.pos.y << " " << state.pos.z << endl;
}