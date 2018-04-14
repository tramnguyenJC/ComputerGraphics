#include "ParticleGenerator.h"

//////////////////////////////////////////////////////////////////////////////
// @brief: Constructor
// @param inputFileStream: stream passed in to read inputFile
// @param _timeStep: value of time step
ParticleGenerator::ParticleGenerator(ifstream& inputFileStream, float _timeStep){
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
        if(tokens[0] == "maxParticles")
          maxParticles = atoi(tokens[1].c_str());
        if(tokens[0] == "distribution")
          distribution = tokens[1];
        if(tokens[0] == "minimumLife")
          minimumLife = atoi(tokens[1].c_str());
        if(tokens[0] == "maximumLife")
          maximumLife = atoi(tokens[1].c_str());
        if(tokens[0] == "minNumGenerated")
          minNumGenerated = atoi(tokens[1].c_str());
        if(tokens[0] == "maxNumGenerated")
          maxNumGenerated = atoi(tokens[1].c_str());
        if(tokens[0] == "type")
          populateTypeInfo(tokens);
        if(tokens[0] == "timeIntervalGeneration")
          timeIntervalGeneration = atoi(tokens[1].c_str());
        if(tokens[0] == "elasticity")
          elasticity = atof(tokens[1].c_str());
        if(tokens[0] == "friction")
          friction = atof(tokens[1].c_str());
        if(tokens[0] == "maxVel")
          maxVel = atof(tokens[1].c_str());
        if(tokens[0] == "minVel")
          minVel = atof(tokens[1].c_str());
        if(tokens[0] == "gravity")
          gravity = glm::vec3(atof(tokens[1].c_str()), 
            atof(tokens[2].c_str()), atof(tokens[3].c_str()));
        if(tokens[0] == "color"){
          glm::vec3 color = glm::vec3(atof(tokens[1].c_str()), 
            atof(tokens[2].c_str()), atof(tokens[3].c_str()));
          colors.push_back(color);
        }
        if(tokens[0] == "size")
          size = atof(tokens[1].c_str());
        if(tokens[0] == "sd")
          sd = atof(tokens[1].c_str());
        if(tokens[0] == "forceStrength")
          forceStrength = atof(tokens[1].c_str());
        if(tokens[0] == "alphaStart")
          alphaStart = atof(tokens[1].c_str());
        if(tokens[0] == "alphaEnd")
          alphaEnd = atof(tokens[1].c_str());
        if(tokens[0] == "mass")
          mass = atof(tokens[1].c_str());
      } while(tokens[0] != "end");
      timeStep = _timeStep;
      currTime = timeIntervalGeneration;
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Populate information about the generator type
// @param tokens: the line containing information about the generator type, 
//         broken into tokens
void
ParticleGenerator::populateTypeInfo(vector<string>& tokens){
  string typeName = tokens[1];
  if(typeName == "Point"){
    type = PointType;
    point.pos = glm::vec3(atof(tokens[3].c_str()), atof(tokens[4].c_str()), 
      atof(tokens[5].c_str()));
  } else if (typeName == "Line"){
    type = LineType;
    line.endPoint1 = glm::vec3(atof(tokens[3].c_str()), 
      atof(tokens[4].c_str()), atof(tokens[5].c_str()));
    line.endPoint2 = glm::vec3(atof(tokens[7].c_str()),
      atof(tokens[8].c_str()), atof(tokens[9].c_str()));
    line.normal = glm::vec3(atof(tokens[11].c_str()), 
      atof(tokens[12].c_str()), atof(tokens[13].c_str()));
  } else if (typeName == "Circle"){
    type = CircleType;
    circle.center = glm::vec3(atof(tokens[3].c_str()), 
      atof(tokens[4].c_str()), atof(tokens[5].c_str()));
    circle.radius = atof(tokens[7].c_str());
    circle.normal = glm::vec3(atof(tokens[9].c_str()), 
      atof(tokens[10].c_str()), atof(tokens[11].c_str()));
  } else if (typeName == "Beam"){
    type = BeamType;
    circle.center = glm::vec3(atof(tokens[3].c_str()), 
      atof(tokens[4].c_str()), atof(tokens[5].c_str()));
    circle.radius = atof(tokens[7].c_str());
    circle.normal = glm::vec3(atof(tokens[9].c_str()), 
      atof(tokens[10].c_str()), atof(tokens[11].c_str()));
  } else if (typeName == "Fire"){
    type = FireType;
    circle.center = glm::vec3(atof(tokens[3].c_str()), 
      atof(tokens[4].c_str()), atof(tokens[5].c_str()));
    circle.radius = atof(tokens[7].c_str());
    circle.normal = glm::vec3(atof(tokens[9].c_str()), 
      atof(tokens[10].c_str()), atof(tokens[11].c_str()));
  } 
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Generate forces affecting the particles
// @return: the cumulative force of all the forces affecting the particles
// @TODO: Change all those arbitrary constant
glm::vec3
ParticleGenerator::generateForces(shared_ptr<Particle> particle){
  glm::vec3 f(0,0,0);
  f += gravity*(mass);
  if(type == FireType)
    f += fireForce(particle);

  return(f);
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Generate force for fire particles to keep them around the center 
//         normal axis
// @param particle the given paritcle
glm::vec3 
ParticleGenerator::fireForce(shared_ptr<Particle> particle){
  glm::vec3 force = glm::vec3(0,0,0);
  glm::vec3 pos = particle->getCurrentState().pos;
  glm::vec3 vel = particle->getCurrentState().vel;
    // vec's component will be positive if circle.normal component is negative
  glm::vec3 vec = circle.normal*(glm::dot(circle.normal, glm::vec3(1,1,1)));
    // Thus in here circle.normal*(glm::dot(vec, pos)) would retain the sign of
    // circle.normal
  glm::vec3 center = circle.center - circle.normal*(glm::dot(circle.normal, circle.center));
  glm::vec3 component = circle.center + circle.normal*(glm::dot(vec, pos) + 10);
  // dir should have the non-normal components  (like x,z), but the normal 
  // component (ex: y) which would be 0. Indicates the vector from particle's
  // position to the center of the fire, on the same height (relative to
  // the normal component). Hard to explain ugh.
  glm::vec3 dir = (component - pos);
  // This gives a vector that has non-normal components of velocity, and
  // the normal component is now 0
  glm::vec3 componentVel = vel - circle.normal*(glm::dot(vec, vel));
 
  float ratio = (float)(particle->getTimeAlive())/ particle->getLongevity();
  ratio = forceStrength*ratio;
  force += dir*(ratio);
  return force;

}

//////////////////////////////////////////////////////////////////////////////
// @brief: Parse in the generator input file and create particle generatos
//         accordingly
// @param inputFile filename of the generator input file
// @param generators a vector of particle generators that each newly created
//        particle generator should be added to.
// @param _timeStep: user-specified value for the time step
void ParticleGenerator::parseInGenerators(string inputFile, 
  vector<shared_ptr<ParticleGenerator>>& generators, float _timeStep){
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
    shared_ptr<ParticleGenerator> generator = 
      make_shared<ParticleGenerator>(inputFileStream, _timeStep);
    generators.push_back(generator);
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
// @brief: Generate particles if the user-specified timeInterval for particle
//         generation is reached, and if the number of particles have not reached
//         the user-specified max number of particles for this generator
void
ParticleGenerator::generateParticles(){
  if(particles.size() > maxParticles){
    return;
  }
  if(currTime-- == 0){
    currTime = timeIntervalGeneration;
    shared_ptr<Particle> particle;
    if(distribution == "uniform"){
      switch(type){
        case PointType:
          pointGenerate(sd);
          break;
        case LineType:
          lineGenerate();
          break;
        case CircleType:
          circleGenerate(sd);
          break;
        case BeamType:
          beamGenerate();
          break;
        case FireType:
          fireGenerate(sd);
          break;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Draw all particles owned by the generator
void ParticleGenerator::draw(){
  glPointSize(size);
  glBegin(GL_POINTS);
  for(auto& particle : particles){
    particle->draw();
  }
  glEnd();
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Break lines into tokens based on given identifier, then store them
//       into given vector
// @param input given string
// @param split given identifier to split string by
// @param vec vector to store the tokens into
void ParticleGenerator::tokenize(string input, string split, vector<string>& vec){
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

//////////////////////////////////////////////////////////////////////////////
// @brief: Generate particles based on a uniform distribution, from a point. 
//         All particles share same spawning position, just with different velocities
void ParticleGenerator::pointGenerate(float sd){
  glm::vec3 position;
  glm::vec3 velocity;
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::uniform_int_distribution<int> distributionNumParticles(minNumGenerated, 
    maxNumGenerated);
  int numGenerated = distributionNumParticles(generator);
  std::uniform_int_distribution<int> distributionLongevity(minimumLife, 
    maximumLife);
  // @TODO: Remove hard code variables
  std::uniform_real_distribution<double> distributionReal(-3, 3);

  for(int i = 0; i < numGenerated; i++){
    position = glm::vec3(point.pos.x, point.pos.y, point.pos.z);
    velocity = glm::vec3(distributionReal(generator),
      distributionReal(generator), distributionReal(generator));
    int longevity = distributionLongevity(generator);

    // Cap velocity at the user-specifed maxVel
    if(glm::length(velocity) > maxVel) {
      glm::vec3 normalizedVel = glm::normalize(velocity);
      velocity = (normalizedVel*(maxVel));
    }

    shared_ptr<Particle> particle = make_shared<Particle>(position, velocity,
      mass, longevity, colors, size, maxVel, elasticity, friction,
      alphaStart, alphaEnd);
    particles.push_back(particle);
  }
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Generate particles from a line
//         Their positions vary on the line with a uniform distribution, but
//         their velocities follow a normal distribution with a relatively
//         small standard deviation, and the mean corresponds to the specified
//         normal vector
void ParticleGenerator::lineGenerate(){
  glm::vec3 position;
  glm::vec3 velocity;
  float length = glm::length(line.endPoint2 - line.endPoint1);
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::uniform_int_distribution<int> distributionNumParticles(minNumGenerated, maxNumGenerated);
  std::uniform_int_distribution<int> distributionLongevity(minimumLife, maximumLife);
  std::uniform_real_distribution<double> distributionVelScale(minVel, maxVel*3);
  std::uniform_real_distribution<double> distributionLength(0.0, length);
  std::normal_distribution<float> distributionVelDirX(line.normal.x,0.05);
  std::normal_distribution<float> distributionVelDirY(line.normal.x,0.05);
  std::normal_distribution<float> distributionVelDirZ(line.normal.x,0.05);

  int numGenerated = distributionNumParticles(generator);
  for(int i = 0; i < numGenerated; i++){
    // Randomize the value of the distance away from Point1
    float randLength = distributionLength(generator);
    glm::vec3 dir = line.endPoint2 - line.endPoint1;
    dir = glm::normalize(dir);
    position = line.endPoint1 + randLength * dir;

    // Generate a random velocity that follows a normal distribution, with mean
    // corresponding to the normal vector of the line (or the general direction
    // most particles will travel). The magnitude of the velocity is also
    // randomized
    glm::vec3 dirVel = glm::vec3(distributionVelDirX(generator),
      distributionVelDirY(generator),distributionVelDirZ(generator));
    dirVel = glm::normalize(dirVel);
    velocity = glm::vec3(distributionVelScale(generator) * dirVel.x,
      distributionVelScale(generator) * dirVel.y, distributionVelScale(generator)*dirVel.z);
    int longevity = distributionLongevity(generator);

    // Cap velocity at the user-specifed maxVel
    if(glm::length(velocity) > maxVel) {
      glm::vec3 normalizedVel = glm::normalize(velocity);
      velocity = (normalizedVel*(maxVel));
    }

    shared_ptr<Particle> particle = make_shared<Particle>(position, velocity,
      mass, longevity, colors, size, maxVel, elasticity, friction,
      alphaStart, alphaEnd);
    particles.push_back(particle);
  }
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Generate particles from a specified circle
//         Randomize positions by varying angle of circle and vector from center to
//         position. Randomize velocities but keep the same normal component.
void ParticleGenerator::circleGenerate(float sd){
  glm::vec3 position;
  glm::vec3 velocity;

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::uniform_int_distribution<int> distributionNumParticles(minNumGenerated, maxNumGenerated);
  std::uniform_int_distribution<int> distributionLongevity(minimumLife, maximumLife);
  std::uniform_real_distribution<float> distributionVelScale(0.0, maxVel);
  std::uniform_real_distribution<float> distributionLength(0.0, circle.radius);
  std::normal_distribution<float> distributionVelDirX(circle.normal.x, sd);
  std::normal_distribution<float> distributionVelDirY(circle.normal.x, sd);
  std::normal_distribution<float> distributionVelDirZ(circle.normal.x, sd);
  std::uniform_real_distribution<float> distributionAngle(0.0, 360.0);

  int numGenerated = distributionNumParticles(generator);
  for(int i = 0; i < numGenerated; i++){
    float angle = distributionAngle(generator);
    float randLength = distributionLength(generator);

    // Get a vector with 1's in the non-normal direction
    glm::vec3 vRandom = glm::vec3(1,1,1) - 
      circle.normal*(glm::dot(circle.normal, glm::vec3(1,1,1)));

    // Rotate that vector with a randomized angle in relative to the normal vector
    // of the circle, then multiply with the randomized length in (0, radius) to 
    // get a random position
    vRandom = glm::normalize(glm::rotate(vRandom, angle, circle.normal));
    position = circle.center + vRandom*(randLength);
    
    glm::vec3 dirVel = glm::vec3(distributionVelDirX(generator),
      distributionVelDirY(generator),distributionVelDirZ(generator));
    dirVel = glm::normalize(dirVel);
    velocity = glm::vec3(distributionVelScale(generator) * dirVel.x,
      distributionVelScale(generator) * dirVel.y, distributionVelScale(generator)*dirVel.z);

    // Whichever directional component of velocity parallels to the normal vector
    // will be only going in the normal direction
    velocity = velocity - circle.normal*(glm::dot(circle.normal, velocity));
    velocity = velocity + circle.normal*distributionVelScale(generator);

    int longevity = distributionLongevity(generator);

    // Cap velocity at the user-specifed maxVel
    if(glm::length(velocity) > maxVel) {
      glm::vec3 normalizedVel = glm::normalize(velocity);
      velocity = (normalizedVel*(maxVel));
    }

    shared_ptr<Particle> particle = make_shared<Particle>(position, velocity,
      mass, longevity, colors, size, maxVel, elasticity, friction,
      alphaStart, alphaEnd);
    particles.push_back(particle);
  }
}

/////////////////////////////////////////////////////////////////////////////
// @brief: Generate particles from a specified circle
//         Randomize positions by varying angle of circle and vector from center to
//         position. Velocities are set equal to the normal.
void ParticleGenerator::fireGenerate(float sd){
  glm::vec3 position;
  glm::vec3 velocity;

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::uniform_int_distribution<int> distributionNumParticles(minNumGenerated, maxNumGenerated);
  std::uniform_int_distribution<int> distributionLongevity(minimumLife, maximumLife);
  std::uniform_real_distribution<float> distributionVelScale(minVel, maxVel);
  std::uniform_real_distribution<float> distributionLength(0.0, circle.radius);
  std::uniform_real_distribution<float> distributionAngle(0.0, 360.0);
  std::normal_distribution<float> distributionVelDirX(circle.normal.x, sd);
  std::normal_distribution<float> distributionVelDirY(circle.normal.x, sd);
  std::normal_distribution<float> distributionVelDirZ(circle.normal.x, sd);

  int numGenerated = distributionNumParticles(generator);
  for(int i = 0; i < numGenerated; i++){
    float angle = distributionAngle(generator);
    float randLength = distributionLength(generator);

    // Get a vector with 1's in the non-normal direction
    glm::vec3 vRandom = glm::vec3(1,1,1) - 
      circle.normal*(glm::dot(circle.normal, glm::vec3(1,1,1)));

    // Rotate that vector with a randomized angle in relative to the normal vector
    // of the circle, then multiply with the randomized length in (0, radius) to 
    // get a random position
    vRandom = glm::normalize(glm::rotate(vRandom, angle, circle.normal));
    position = circle.center + vRandom*(randLength);
    
    glm::vec3 dirVel = glm::vec3(distributionVelDirX(generator),
      distributionVelDirY(generator),distributionVelDirZ(generator));
    velocity = glm::vec3(distributionVelScale(generator) * dirVel.x,
      distributionVelScale(generator) * dirVel.y, distributionVelScale(generator)*dirVel.z);
    // Whichever directional component of velocity parallels to the normal vector
    // will be only going in the normal direction
    velocity = velocity - circle.normal*(glm::dot(circle.normal, velocity));
    velocity = velocity + circle.normal*distributionVelScale(generator);
    int longevity = distributionLongevity(generator);

    // Cap velocity at the user-specifed maxVel
    if(glm::length(velocity) > maxVel) {
      glm::vec3 normalizedVel = glm::normalize(velocity);
      velocity = (normalizedVel*(maxVel));
    }

    shared_ptr<Particle> particle = make_shared<Particle>(position, velocity,
      mass, longevity, colors, size, maxVel, elasticity, friction,
      alphaStart, alphaEnd);
    particles.push_back(particle);
  }
}

/////////////////////////////////////////////////////////////////////////////
// @brief: Generate particles from a specified circle
//         Randomize positions by varying angle of circle and vector from center to
//         position. Velocities are set equal to the normal.
void ParticleGenerator::beamGenerate(){
  glm::vec3 position;
  glm::vec3 velocity;

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::uniform_int_distribution<int> distributionNumParticles(minNumGenerated, maxNumGenerated);
  std::uniform_int_distribution<int> distributionLongevity(minimumLife, maximumLife);
  std::uniform_real_distribution<float> distributionVelScale(minVel, maxVel);
  std::uniform_real_distribution<float> distributionLength(0.0, circle.radius);
  std::uniform_real_distribution<float> distributionAngle(0.0, 360.0);

  int numGenerated = distributionNumParticles(generator);
  for(int i = 0; i < numGenerated; i++){
    float angle = distributionAngle(generator);
    float randLength = distributionLength(generator);

    // Get a vector with 1's in the non-normal direction
    glm::vec3 vRandom = glm::vec3(1,1,1) - 
      circle.normal*(glm::dot(circle.normal, glm::vec3(1,1,1)));

    // Rotate that vector with a randomized angle in relative to the normal vector
    // of the circle, then multiply with the randomized length in (0, radius) to 
    // get a random position
    vRandom = glm::normalize(glm::rotate(vRandom, angle, circle.normal));
    position = circle.center + vRandom*(randLength);
    
    velocity = circle.normal*(distributionVelScale(generator));
    int longevity = distributionLongevity(generator);

    // Cap velocity at the user-specifed maxVel
    if(glm::length(velocity) > maxVel) {
      glm::vec3 normalizedVel = glm::normalize(velocity);
      velocity = (normalizedVel*(maxVel));
    }

    shared_ptr<Particle> particle = make_shared<Particle>(position, velocity,
      mass, longevity, colors, size, maxVel, elasticity, friction,
      alphaStart, alphaEnd);
    particles.push_back(particle);
  }
}