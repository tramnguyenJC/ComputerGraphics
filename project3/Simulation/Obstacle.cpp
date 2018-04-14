#include "Obstacle.h"

//////////////////////////////////////////////////////////////////////////////
// @brief: Constructor
// @param: inputFileStream a given file stream to continue reading data
Obstacle::Obstacle(ifstream& inputFileStream){
  while (inputFileStream.good()){
    string input;
    getline(inputFileStream, input);

    if(input == "end")
      break;
    if(input == "")
      continue;
    else if (input[0] == '#')
        continue;

    int i = 0;
    vector<string> tokens;
    tokenize(input, " ", tokens);

    if(tokens[0] == "start"){
      do {
        getline(inputFileStream,input);
        tokenize(input, " ", tokens);
        if(tokens[0] == "Point1")
    		point1 = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), 
    		atof(tokens[3].c_str()));
    	if(tokens[0] == "Point2")
    		point2 = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), 
    		atof(tokens[3].c_str()));
    	if(tokens[0] == "Point3")
    		point3 = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), 
    		atof(tokens[3].c_str()));
    	if(tokens[0] == "Point4")
    		point4 = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), 
    		atof(tokens[3].c_str()));
    	if(tokens[0] == "Normal")
    		normal = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), 
    		atof(tokens[3].c_str()));
    	if(tokens[0] == "style")
    		style = tokens[1];
      } while(tokens[0] != "end");
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Parse in the obstacle input file and create obstacles accordingly
// @param inputFile filename of the obstacler input file
// @param obstacles a vector of obstacles that each newly created obstacle 
//		  should be added to.
void Obstacle::parseInObstacles(string inputFile, vector<shared_ptr<Obstacle>>& obstacles){
  ifstream inputFileStream;

  // open file and check that it was successfully opened
  inputFileStream.open(inputFile);
  if (!inputFileStream.is_open()){
   cout << "inputFile: " << inputFile << endl;
   cerr << "Error: Unable to open file " << inputFile << endl;
   exit(1);
  }
  while (inputFileStream.good()){
    // Read each line
    shared_ptr<Obstacle> obstacle = make_shared<Obstacle>(inputFileStream);
    obstacles.push_back(obstacle);
    if(inputFileStream.fail()){
      if (inputFileStream.eof())
        break;
      else{
        cerr << "Error reading data. " << endl;
        exit(1);
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Check if collision happened in the simulation update step
// @param oldState: the old state of the particle
// @param newState: the new state of the particle
// @return null if no collision happened
// 		   a Collision object otherwise
Collision
Obstacle::checkCollision(State& oldState, State& newState) const{
	Collision c;
  float oValue;
  float nValue;
  float planeValue;

  // Without loss of generality, all {point1, point2, point3, point4} have
  // the same value (x), or (y), or (z) value - depending on whichever of x,y,z
  // is the normal direction of the plane, so if normal = (1,0,0) then
  // point1.x = point2.x = point3.x = point4.x
  if(normal.x == 1){
    oValue = oldState.pos.x;
    nValue = newState.pos.x;
    planeValue = point1.x;
  } else if (normal.y == 1){
    oValue = oldState.pos.y;
    nValue = newState.pos.y;
    planeValue = point1.y;
  } else {
    oValue = oldState.pos.z;
    nValue = newState.pos.z;
    planeValue = point1.y;
  }
  if((planeValue - oValue)*(planeValue - nValue) < 0){
    c.f = (planeValue - oValue) / (nValue - oValue);
    c.none = false;
  }
  c.n = normal;
  return c;
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Draw obstacle
void Obstacle::draw(){
  glColor3f(0.7,1.0,1.0);
  if(style == "line"){
    glLineWidth(2);
    glBegin(GL_LINE_STRIP);
      glVertex3f(point1.x, point1.y, point1.z);
      glVertex3f(point2.x, point2.y, point2.z);
      glVertex3f(point3.x, point3.y, point3.z);
      glVertex3f(point4.x, point4.y, point4.z);
    glEnd();
  } else if (style == "solid"){
    glBegin(GL_POLYGON);
    glVertex3f(point1.x, point1.y, point1.z);
      glVertex3f(point2.x, point2.y, point2.z);
      glVertex3f(point3.x, point3.y, point3.z);
      glVertex3f(point4.x, point4.y, point4.z);
    glEnd();
  }
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Break lines into tokens based on given identifier, then store them
//       into given vector
// @param input given string
// @param split given identifier to split string by
// @param vec vector to store the tokens into
void Obstacle::tokenize(string input, string split, vector<string>& vec){
  vec.clear();
  int pos = input.find(split);
  int currPos = 0;
  while(pos != string::npos && currPos < input.length()){
    vec.push_back(input.substr(currPos, pos-currPos));
    currPos = pos + split.length();
    pos = input.find(split, currPos);
  }
  if(currPos < input.length())
    vec.push_back(input.substr(currPos));
}

