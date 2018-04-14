#include <fstream>
#include <cstdlib>

#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#define GLM_ENABLE_EXPERIMENTAL 
#include "glm/geometric.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtx/vector_angle.hpp"
#include <iostream>

#include "../Models/Model.h"

using namespace std;

class Camera {
private:
	float rotateAngle{0.02f};

	glm::vec3 prevLookAt;
	glm::vec3 prevEyePos;
	glm::vec3 refForPanning;	//< hacky, @TODO
	
	glm::vec3 lookAt;			//< The position of the look at point
	glm::vec3 dir;				//< Normalized direction from eye position to look at position
	glm::vec3 eyePos;			//< The position of the camera
	glm::vec3 upVec;			//< The direction of the up vector
	float viewDist;			    //< Length of view distance from eyePos to lookAt
	float panDist;				//<
	float moveSpeed;			//< Speed of moving when controlled camera's eyePos movement

public:

	glm::vec3 getEyePos(){ return eyePos;}
	glm::vec3 getLookAt(){ return lookAt;}
	glm::vec3 getDir(){ return dir;}

	void setRotateAngle(float angle){ rotateAngle = angle; }
	void setMoveSpeed(float speed){ moveSpeed = speed; }
	void setPosition(glm::vec3 eyepos, glm::vec3 lookat);
	void configureParams(ifstream& inputFileStream);

	void rotateLeft();			//< Camera rotates left 
	void rotateRight();			//< Camera rotates right
	void rotateUp();			//< Camera rotates up
	void rotateDown();			//< Camera rotates down

	void moveLeft();			//< Camera's eye position moves left
	void moveRight();			//< Camera's eye position moves right
	void moveForward();			//< Camera's eye position moves forward
	void moveBackwards();		//< Camera's eye position moves backwards

	void panLeft();				//< Eye point moves left
	void panRight();			//< Eye point moves right

};