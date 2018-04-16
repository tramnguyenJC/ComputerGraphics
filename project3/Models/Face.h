#ifndef FACE_H
#define FACE_H
//////////////////////////////////////////////////////////////////////////////
// @brief: This class represents a face of the model. A face can be triangular
// 		   or quadrilateral, and is represented as a vector of Vertex. Each
//		   Vertex contains indices that can be used to retrieve the position,
//		   normals and texture coordinates of each vertex of the face.
#include <iostream>
#include <vector>

using namespace std;

//////////////////////////////////////////////////////////////////////////////
// @brief: Struct vertex. Each vertex contains information about the position,
//		   normal and texture coordinates.
struct Vertex {
	int pointIdx{-1}; 
	int normalIdx{-1};
	int textureIdx{-1};
};

class Face {
private:
	vector<Vertex> faceVertices;

public:
	
	// Getter methods. Return const vector to provide protection against
	// careless modifying. There are add methods to modify these vectors
	// outside of this Face class.
	const vector<Vertex>& getVertices(){ return faceVertices; }
	size_t getNumVertices() { return faceVertices.size(); }

	// Add methods
	void addVertex(Vertex vertex);
};
#endif