#include "Model.h"

//////////////////////////////////////////////////////////////////////////////
// @brief: Process input file to retrieve information about the Model, including
//		   points, normals, textures, and faces; and position and orientation
//		   of the model.
// @param inputFile given name of the input file
// @param _name given name
// @param _pos given position 
// @param _orientation given orientation
// @param _scale: vector to scale by
// @param _color: color of model
Model::Model(string inputFile, string _name, glm::vec3 _pos, glm::vec3 _orientation,
	glm::vec3 _scale, glm::vec3 _color){
	pos = _pos;
	name = _name;
	orientation = _orientation;
	scale = _scale;
	color = _color;

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

        getTokens(input);
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
    centerOfMass/=(points.size());
    inputFileStream.close();
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Read input file, then add models and information about each model's
//         position, orientation, color, scale.
// @param input: given string for input file name
// @param models: vector of Model pointers
void
Model::parseInModels(string input, vector<shared_ptr<Model>>& models){
    //declare an input-file stream variable for reading
    ifstream inputFileStream;
    // open file and check that it was successfully opened
    inputFileStream.open(input);
    if (!inputFileStream.is_open()){
        cerr << "Error: Unable to open file " << input << endl;
        exit(1);
    }

    while (inputFileStream.good()){
        string input;
        getline(inputFileStream, input);

         // Skip to next line if line is empty or starts with pound sign
        if(input == "")
            continue;
        else if (input[0] == '#')
            continue;

        vector<string> tokens;
        string file, name;
        glm::vec3 pos, orientation, color;
        glm::vec3 scale(1.0, 1.0, 1.0);
        tokenize(input, " ", tokens);
        if(tokens[0] == "start"){
          do{
            getline(inputFileStream,input);
            tokenize(input, " ", tokens);
            if(tokens[0] == "file")
              file = tokens[1];
            if(tokens[0] == "name")
              name = tokens[1];
            if(tokens[0] == "pos")
              pos = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
            if(tokens[0] == "orientation")
              orientation = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
            if(tokens[0] == "color")
              color = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
            if(tokens[0] == "scale")
              scale = glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
          }
          while(tokens[0] != "end");
        }

        models.push_back(make_shared<Model>(file, name, pos, orientation, scale, color));

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
    inputFileStream.close();
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Read input line to call corresponding functions based on the 
//		   identifier "v", "vt", "vn", or "f"
// @param input the line given to extract information from
void Model::getTokens(string input){
	string identifier = input.substr(0,2);
	if(identifier == "v ")
		storeValue(input, points);
	else if (identifier == "vt")
		storeValue(input, textures);
	else if (identifier == "vn")
		storeValue(input, normals);
	else if (identifier == "f ")
		readFace(input);
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Read information of a glm::vec3's x, y, z coordinates
// 		   Add newly read glm::vec3'to the given vector<glm::vec3> (can be
//	       points, textures, normals)
// @param input the line given to extract information from
// @param vec given vector<glm::vec3> for the new vec3 to be added to
void Model::storeValue(string input, vector<glm::vec3>& vec){
	// extract x, y, z values
	vector<string> tokens;
	tokenize(input, " ", tokens);
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());
	float z = 0;
	if(tokens.size() > 3){
    	z = atof(tokens[3].c_str());
    } 
	glm::vec3 point(x, y, z);
	centerOfMass = centerOfMass + point;
	vec.push_back(point);
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Break lines into tokens based on given identifier, then store them
//		   into given vector
// @param input given string
// @param split given identifier to split string by
// @param vec vector to store the tokens into
void Model::tokenize(string input, string split, vector<string>& vec){
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
// @brief: Read information of face.
// 		   Add newly read face to the model's face vector.
//		   Flexible for triangular and quadrilateral faces;
//		   Support these styles: f v v v 
//								 f v//vn v//vn v//vn
//								 f v/vt/vn v/vt/vn v/vt/vn
// @param input the line given to extract information from
void Model::readFace(string input){
	vector<string> tokens;
	tokenize(input.substr(2), " ", tokens);
	size_t numVertices = 0;
	shared_ptr<Face> face = make_shared<Face>();
	for(auto& token : tokens){
		Vertex vertex;
		vector<string> triples;
		if(input.find("//") != string::npos){
			tokenize(token, "//", triples);
			vertex.pointIdx = stoi(triples[0]);
			vertex.normalIdx = stoi(triples[1]);
		} 
		else if (input.find("/") != string::npos){
			tokenize(token, "/", triples);
      vertex.pointIdx = stoi(triples[0]);
      vertex.textureIdx = stoi(triples[1]);
      vertex.normalIdx = stoi(triples[2]);
		}
		else
			vertex.pointIdx = stoi(token);
		numVertices++;
		face->addVertex(vertex);
	}

	if(face->getNumVertices() == 3)
		facesTriangle.push_back(face);
	else if(face->getNumVertices() == 4)
		facesQuad.push_back(face);
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Draw face
// @param face Face to be drawn
void Model::drawFace(shared_ptr<Face> face) const{
	vector<Vertex> vertices = face->getVertices();
	for (auto& vertex : vertices){
		if(vertex.normalIdx != 0){
			glm::vec3 normal = normals[vertex.normalIdx - 1];
			glNormal3f(normal.x, normal.y, normal.z);
		}
		// If the OBJ line provides textures
		if(vertex.textureIdx  != 0){
			glm::vec3 tex = textures[vertex.textureIdx - 1];
			glTexCoord2f(tex.x, tex.y);
		}
		glm::vec3 vec = points[vertex.pointIdx - 1];
		glVertex3f(vec.x,vec.y,vec.z);

		// Since some OBJ lines have format "f v v v" or "f v//vn v//vn v//vn",
		// check if OBJ line provides normals or textures before incorporating them
		// If the OBJ line provides normals
	}
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Draw model
//		   Flexible for both triangular and quadrilateral faces
void Model::draw(){
	// Style and color
  	glColor3f(color.x, color.y, color.z);
  	if(style == FILL)
  		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  	else if(style == POINT)
  		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
  	else if(style == LINE)
  		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


  	// Transformations
  	glPushMatrix();
  	glTranslatef(pos.x, pos.y, pos.z);
  	glScalef(scale.x, scale.y, scale.z);
  	changeOrientation();
  	// Translated back to origin before any non-translation transformation
  	glTranslatef(-centerOfMass.x, -centerOfMass.y, -centerOfMass.z);

  	// Draw
  	glBegin(GL_TRIANGLES);
  	for(auto& face : facesTriangle){
  		drawFace(face);
  	}
  	glEnd();
  	glBegin(GL_QUADS);
  	for(auto& face : facesQuad){
  		drawFace(face);
  	}
  	glEnd();
  	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Change orientation of model.
// @note: I could not find what functions of OpenGL can change 3D orientation, so
// I devised my own method, though I think it's messy and not 100% sure if it's correct.
void Model::changeOrientation(){
	// Change the xz-orientation of model
	glm::vec3 vec1 = glm::normalize(glm::vec3(orientation.x, 0, orientation.z));
	// Default xz orientation of model (without any orientation input) is (0,0,1)
	// Find the angle to rotate the model from default orientation to desired orientation,
	// with reference to the y-axis
	// Note that argument to glm::orientedAngle need to be normalized
	float angle = glm::orientedAngle(glm::vec3(0,0,1), vec1, glm::vec3(0,1,0));
	// convert angle from radians to degrees then rotate model with reference to y-axis
  	glRotatef(angle/M_PI * 180, 0, 1, 0);

  	// Change the y-orientation of model
  	glm::vec3 vec2(0, orientation.y, 1);
  	// Find the angle to rotate the model from y = 0 to y = orientation.y, keeping xz orientation
  	// the same
  	float angle2 = glm::orientedAngle(glm::vec3(0,0,1),
  		glm::normalize(glm::vec3(0,orientation.y,1)), glm::vec3(1,0,0));
   	glRotatef(angle2/M_PI * 180, 1, 0, 0); 
}