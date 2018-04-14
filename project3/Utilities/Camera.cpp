#include "Camera.h"

//////////////////////////////////////////////////////////////////////////////
// @brief: Rotate camera to the left on the xz plane, with reference to y axis
void
Camera::configureParams(ifstream& inputFileStream){
	while (inputFileStream.good()){
        // Read each line
		string input;
		getline(inputFileStream, input);

		if(input == "end")
			break;
		
		if(input == "")
            continue;
        else if (input[0] == '#')
            continue;

        // Skip to next line if line is empty or starts with pound sign
		if(input == "")
			continue;
		else if (input[0] == '#')
			continue;

		// Read in parameters for camera
		vector<string> tokens;
		Model::tokenize(input, " ", tokens);
		if(tokens[0] == "position")
			eyePos = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
		if(tokens[0] ==  "lookingDirection")
			dir = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
		if(tokens[0] == "viewDist")
			viewDist = atof(tokens[1].c_str());
		if(tokens[0] == "panDist")
			panDist = atof(tokens[1].c_str());
		if(tokens[0] == "moveSpeed")
			moveSpeed = atof(tokens[1].c_str());
		
		lookAt = eyePos + dir*viewDist;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Rotate camera to the left on the xz plane, with reference to y axis
void
Camera::rotateLeft(){
	dir = glm::rotateY(dir,rotateAngle);
	dir = glm::normalize(dir);
	lookAt = eyePos + dir*viewDist;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Rotate camera to the right on the xz plane, with reference to y axis
void
Camera::rotateRight(){
	dir = glm::rotateY(dir,-rotateAngle);
	dir = glm::normalize(dir);
	lookAt = eyePos + dir*viewDist;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Rotate camera downwards on the xy plane, with reference to z axis
void
Camera::rotateUp(){
	dir = glm::rotateZ(dir,rotateAngle);
	dir = glm::normalize(dir);
	lookAt = eyePos + dir*viewDist;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Rotate camera downwards on the xy plane, with reference to z axis
void
Camera::rotateDown(){
	dir = glm::rotateZ(dir,-rotateAngle);
	dir = glm::normalize(dir);
	lookAt = eyePos + dir*viewDist;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Move camera to the left, perpendicular to the dir direction
void
Camera::moveLeft(){
	glm::vec3 dirHorizontal = glm::rotateY(dir,glm::radians(90.f));
	dirHorizontal = glm::normalize(glm::vec3(dirHorizontal.x, 0, dirHorizontal.z));
	eyePos = eyePos + dirHorizontal* moveSpeed;
	lookAt = eyePos + dir*viewDist;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Move camera to the right, perpendicular to the dir direction
void
Camera::moveRight(){
	glm::vec3 dirHorizontal = glm::rotateY(dir,glm::radians(-90.f));
	dirHorizontal = glm::normalize(glm::vec3(dirHorizontal.x, 0, dirHorizontal.z));
	eyePos = eyePos + dirHorizontal* moveSpeed;
	lookAt = eyePos + dir*viewDist;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Move camera forward along the dir direction
void
Camera::moveForward(){
	// dir is normalized, so when moveSpeed = k, camera moves forward k unit of distance
	// across the dir vector
	glm::vec3 dirTemp = glm::normalize(glm::vec3(dir.x, 0, dir.z));
	eyePos = eyePos + dirTemp * moveSpeed;
	lookAt = eyePos + dir*viewDist;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Move camera backwards along the dir direction				
void
Camera::moveBackwards(){
	// dir is normalized, so when moveSpeed = k, camera moves forward k unit of distance
	// across the dir vector
	glm::vec3 dirTemp = glm::normalize(glm::vec3(dir.x, 0, dir.z));
	eyePos = eyePos - dirTemp* moveSpeed;
	lookAt = eyePos + dir*viewDist;
}

void
Camera::setPosition(glm::vec3 eyepos, glm::vec3 dir){
	eyePos = eyepos;
	dir = glm::normalize(dir);
    lookAt = eyePos + dir*viewDist;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: pan Camera leftwards with reference to y-axis around lookat point
void
Camera::panLeft(){
	glm::vec3 negDir = dir * (-1.f);
	negDir = glm::rotateY(negDir,-rotateAngle);
	negDir = glm::normalize(negDir);
	eyePos = lookAt + negDir*panDist;
	dir = glm::normalize(lookAt - eyePos);
}

//////////////////////////////////////////////////////////////////////////////
// @brief: pan Camera rightwards with reference to y-axis around lookat point
void
Camera::panRight(){
	glm::vec3 negDir = dir * (-1.f);
	negDir = glm::rotateY(negDir,rotateAngle);
	negDir = glm::normalize(negDir);
	eyePos = lookAt + negDir*panDist;
	dir = glm::normalize(lookAt - eyePos);
}

