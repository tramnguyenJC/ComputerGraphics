//////////////////////////////////////////////////////////////////////////////
// @brief: This class represents a model. Upon construction, the model would
// 		   process information about its vertices, normals, textures and faces
//		   through an input file. This class offers functions that process the 
//		   model and display it.
 	
#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <stdlib.h>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
// GLM
#include "../Utilities/glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL 
#include "../Utilities/glm/vec2.hpp" // glm::vec3
#include "../Utilities/glm/vec3.hpp" // glm::vec3
#include "../Utilities/glm/geometric.hpp" // For normalize, cross, dot product, scalar multiply, etc.
#include "../Utilities/glm/gtx/vector_angle.hpp" // For calculating angle between vectors

#if   defined(OSX)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <GLUT/glut.h>
#elif defined(LINUX)
#include <GL/glut.h>

#endif

#include "Face.h"

using namespace std;

// Different types of style
enum Style{
  LINE,
  FILL,
  POINT
};

class Model {
private:
	vector<glm::vec3> points;	        //< Vector of the model's vertices 
	vector<glm::vec3> normals;			//< Vector of the model's normals
	vector<glm::vec3> textures;		    //< Vector of the model's textures
	vector<shared_ptr<Face>> facesTriangle;	  	 //< Vector of the model's triangular faces
	vector<shared_ptr<Face>> facesQuad;	   		 //< Vector of the model's quadrilateral faces

	string name;						//< name of model
	Style style{FILL};				    //< style the model is drawn in
	glm::vec3 color{0.6f, 0.6f, 0.6f};  //< Color of the model, default to grey
	glm::vec3 pos;						//< position of model in the world
	glm::vec3 orientation;				//< orientation of model
	glm::vec3 scale;					//< factor to scale by
	glm::vec3 centerOfMass;				//< To scale then translate correctly

	void getTokens(string input);
	void storeValue(string input, vector<glm::vec3>& vec);
	void readFace(string input);
	void drawFace(shared_ptr<Face> face) const;
	void changeOrientation();

public:

	Model(string inputFile, string _name, glm::vec3 _pos, glm::vec3 _orientation, glm::vec3  scale, glm::vec3 _color);
	void draw();
	static void parseInModels(string inputFile, vector<shared_ptr<Model>>& models);
	void setColor(glm::vec3 _color){ color = _color;}
	static void tokenize(string input, string split, vector<string>& vec);

	// Getter methods
	const vector<glm::vec3>& getPoints() { return points; }
	const vector<glm::vec3>& getNormals() { return normals; }
	const vector<glm::vec3>& getTextures() { return textures; }

};

#if   defined(OSX)
#pragma clang diagnostic pop
#endif
#endif