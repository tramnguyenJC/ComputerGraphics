#include "Face.h"

//////////////////////////////////////////////////////////////////////////////
// @brief: Add a new Vertex to the face's vertex lists
// @param vertex the new Vertex
void Face::addVertex(Vertex& vertex){
	faceVertices.push_back(vertex);
}