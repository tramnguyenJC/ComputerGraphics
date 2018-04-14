#include "Simulator.h"

//////////////////////////////////////////////////////////////////////////////
// @brief: Constructor
//		   Read in paremeters for the simulation configurations
//@ param inputFile: name of the given inputFile
Simulator::Simulator(string inputFile){	
	// Initialize variables
	sceneColor = glm::vec3(1.0, 1.0, 1.0);

	// declare an input-file stream variable for reading
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
    string input;
    getline(inputFileStream, input);

          // Skip to next line if line is empty or starts with pound sign
    if(input == "")
      continue;
    else if (input[0] == '#')
      continue;

    vector<string> tokens;
    Model::tokenize(input, " ", tokens);
    if(tokens[0] == "StaticModels")
     Model::parseInModels(tokens[1], staticModels);
    if(tokens[0] == "Camera")
       camera.configureParams(inputFileStream);
    if(tokens[0] == "ParticleGenerators")
      ParticleGenerator::parseInGenerators(tokens[1], generators, timeStep);
    if(tokens[0] == "SceneColor")
      sceneColor = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
    if(tokens[0] == "PlaneColor")
      planeColor = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
    if(tokens[0] == "PlaneDimension")
      planeSize = atof(tokens[1].c_str());
    if(tokens[0] == "TimeStep")
      timeStep = atof(tokens[1].c_str());
    if(tokens[0] == "Obstacles")
      Obstacle::parseInObstacles(tokens[1], obstacles);
    if(tokens[0] == "Repeller"){
      glm::vec3 pos = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
      shared_ptr<Repeller> repeller = make_shared<Repeller>(pos, atof(tokens[4].c_str()), 
        atof(tokens[5].c_str()));
      repellers.push_back(repeller);
    }
    if(tokens[0] == "Attractor"){
      glm::vec3 pos = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
      shared_ptr<Attractor> attractor = make_shared<Attractor>(pos, atof(tokens[4].c_str()), 
        atof(tokens[5].c_str()));
      attractors.push_back(attractor);
    } 
    if(inputFileStream.fail()){
      if (inputFileStream.eof())
        break;
      else{
                // failed for some other reason than reaching eof
                // print a message and exit
        cerr << "Error reading data. " << endl;
        exit(1);
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Draw every simulated objects
void
Simulator::draw(){
	glClearColor(sceneColor.x, sceneColor.y, sceneColor.z, 0.f);
	glm::vec3 eyePos = camera.getEyePos();
  glm::vec3 lookAt = camera.getLookAt();
  gluLookAt(eyePos.x, eyePos.y, eyePos.z,
   lookAt.x, lookAt.y, lookAt.z,
   0.f, 1.f, 0.f);

  glEnable(GL_LIGHTING);
  glColor3f(1.0,0.f,0.f);
  glPointSize(10);
  glBegin(GL_POINTS);
  glVertex3f(lookAt.x, lookAt.y, lookAt.z);
  glColor3f(1.0,1.f,0.f);
  for(auto& attractor : attractors){
    glVertex3f(attractor->getPos().x, attractor->getPos().y, attractor->getPos().z);
  }
  glColor3f(1.0,1.f,1.f);
  for(auto& repeller : repellers){
    glVertex3f(repeller->getPos().x, repeller->getPos().y, repeller->getPos().z);
  }
  glEnd();

  drawPlane();
  for(auto& model : staticModels)
    model->draw();
  glDisable(GL_LIGHTING);
  for(auto& generator : generators)
    generator->draw();
  for(auto& obstacle : obstacles)
    obstacle->draw();
 
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Draw plane and axies
void
Simulator::drawPlane(){
  glBegin(GL_LINES);
  glLineWidth(5.0);
  // Plane
  glColor3f(planeColor.x, planeColor.y, planeColor.z);
  for(int i = (-1)*planeSize; i < planeSize + 1; i+=5){
    glVertex3f(i,0.0,planeSize);
    glVertex3f(i,0.0,-planeSize);
  }
  for(int i = (-1)*planeSize; i < planeSize + 1; i+=5){
    glVertex3f(planeSize, 0.0,i);
    glVertex3f(-planeSize,0.0,i);
  }
  // Axies
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(20.0, 0.0, 0.0);
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 20.0, 0.0);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 20.0);
  glEnd();
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Update each generator by generate particles if appropriate, 
//         delete any particle that has reached their longevities, generate 
//         forces and apply those forces to particles to update 
//         particles' movement, and check for collision.
/// @ ASSUMPTION: that because timeStep is small, the change in position
/// is small enough that a particle can collide with only 1 obstacle in 
/// the list of obstacles in one simulation step. Not robust, @TODO
/// think of a way to address this (sorting will be too expensive?)
void Simulator::update(){
  for(auto& generator : generators){
    generator->generateParticles();
    vector<shared_ptr<Particle>>& particles = generator->getParticles();
    for (auto p = particles.begin(); p != particles.end();) {
      // Delete particle if it reaches the end of its longevity
      if ((*p)->getTimeAlive() >= (*p)->getLongevity()) {
        p = particles.erase(p);
      } else {
        if((*p)->hasRested()){
          ++p;
          continue;
        }
        // Otherwise update the particle's state
        // Set timeRemaining (tr) equal to timeStep
        float tr = timeStep;
        while(tr > 0) {
          State oldState = (*p)->getCurrentState();
          // Resolve inner force from the generator
          glm::vec3 force = generator->generateForces((*p));
          // Resolve forces from repellers and attractors
          for(auto& repeller : repellers)
            force += repeller->getForce((*p));
          for(auto& attractor : attractors)
            force += attractor->getForce((*p));

          // Update state of particle
          (*p)->update(force, oldState, tr);

          // Resolve collision (if there is)
          Collision c;
          for(auto& obstacle : obstacles){
            c = obstacle->checkCollision(oldState, (*p)->getCurrentState());
            if(!c.none){
              (*p)->update(force, oldState, tr*c.f);
              (*p)->resolveCollision(c);
              tr -= tr*c.f;
              break;
            }
          }
          if(c.none){
            tr = 0;
          }
        }
        ++p;
      }
    }
  }
}