/*
 *	loadObj.h
 *
 *	Created on: Mar 13, 2011
 *	Author	  : Anoop Haridas
 *
 *	Class to handle OBJ files for OpenGL mesh drawing.
 */

#ifndef LOADOBJ_H_
#define LOADOBJ_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <vector>
#include <QGLWidget>

using namespace std;

#ifndef C_PI
#define C_PI 3.14159265f
#endif

#define OBJM_NONE             (0)          // render with vertices
#define OBJM_FLAT_SHADE       (1 << 0)     // render with face normals
#define OBJM_SMOOTH_SHADE     (1 << 1)     // render with vertex normals
#define OBJM_TEXTURE          (1 << 2)     // render with texture coords
#define OBJM_COLOR            (1 << 3)     // render with colors
#define OBJM_MATERIAL         (1 << 4)     // render with materials

#define T(x) (model->triangles[(x)])	   // This makes pointer usage a little less cumbersome.

// OBJMmaterial: Structure that defines a material in a model.
// Read in from the appropriate lines in the OBJ file, this structure
// contains info that defines how the object appears when lighted
typedef struct _OBJMmaterial
{
	char* name;               // Name of the material
	GLfloat ambient[4];       // Ambient component
	GLfloat diffuse[4];       // Diffuse component
	GLfloat specular[4];      // Specular component
	GLfloat emissive[4];      // Emissive component
	GLfloat shininess;        // Specular exponent
}OBJMmaterial;

// OBJMtriangle: Structure that defines a triangle in a model. All
// the information relevant to a triangle, which is the building block
// of any OBJ mesh. Array of these is used in the OBJMmodel structure.
typedef struct _OBJMtriangle
{
	GLuint vindices[3];       // Array of triangle vertex indices
	GLuint nindices[3];       // Array of triangle normal indices
	GLuint tindices[3];       // Array of triangle texcoord indices
	GLuint findex;            // Triangle facet normal index
}OBJMtriangle;

// OBJMgroup: Structure that defines a group in a model. Large OBJ files
// representing complex meshes have groups in them, which is a means of
// grouping different regions of the mesh. For example, an OBJ mesh of a 
// human might have the triangles for the head, torso, arms & legs in
// different groups.
typedef struct _OBJMgroup
{
	char* name;               // The group name
	GLuint numtriangles;      // The number of triangles in the group
	GLuint* triangles;        // The array of triangle indices
	GLuint material;          // The index of the material for the group
	struct _OBJMgroup* next;  // The pointer to the next group in the model
}OBJMgroup;

// OBJMmodel: Structure that defines a model. It has arrays for all the info
// needed when loading an OBJ file:
//
// 	Vertices
// 	Face normals
// 	Vertex normals
// 	Texture coordinates
// 	Triangles
// 	Materials
//
// It also has a linked list for the groups in the OBJ file.
typedef struct _OBJMmodel
{
	char* pathname;           // Path to the model
	char* mtllibname;         // Name of the material library

	GLuint numvertices;       // Number of vertices in the model
	GLfloat* vertices;        // Array of vertices

	GLuint numfacetnorms;     // Number of facetnorms in the model
	GLfloat* facetnorms;      // Array of facetnorms

	GLuint numvertexnorms;    // Number of vertexnorms in the model
	GLfloat* vertexnorms;     // Array of vertexnorms

	GLuint numtexcoords;      // Number of texcoords in the model
	GLfloat* texcoords;       // Array of texcoords

	GLuint numtriangles;      // Number of triangles in the model
	OBJMtriangle* triangles;  // Array of triangles

	GLuint nummaterials;      // Number of materials in the model
	OBJMmaterial* materials;  // Array of materials

	GLuint numgroups;         // Number of groups in the model
	OBJMgroup* groups;        // Linked list of groups

	GLfloat position[3];      // Position of the model
}OBJMmodel;

// OBJMnode: General purpose node
typedef struct _OBJMnode
{
	GLuint index;
	GLboolean averaged;
	struct _OBJMnode* next;
}OBJMnode;

class loadObj
{

public:

	loadObj();
	~loadObj();

	GLfloat eye[3];
	GLfloat at[3];
	GLfloat up[3];
	GLfloat translation[3];
	GLfloat rotation[4];

	GLdouble projection[16], modelview[16], inverse[16];

	GLboolean swapped;
	GLboolean world_draw;
	OBJMmodel* pmodel;
	GLint selection;
        int drawType;

	// loadObj functions. See .cpp file for individual function
	// documentation.
	
	GLfloat objmUnitize(OBJMmodel* model);
	GLvoid objmDimensions(OBJMmodel* model, GLfloat* dimensions);
	GLvoid objmScale(OBJMmodel* model, GLfloat scale);
	GLvoid objmReverseWinding(OBJMmodel* model);
	GLvoid objmFacetNormals(OBJMmodel* model);
	GLvoid objmVertexNormals(OBJMmodel* model, GLfloat angle);
	GLvoid objmLinearTexture(OBJMmodel* model);
	GLvoid objmSpheremapTexture(OBJMmodel* model);
	GLvoid objmDelete(OBJMmodel* model);
	bool Load(char* filename);
	bool Save(char* filename);
	OBJMmodel* objmReadOBJ(char* filename);
	GLvoid objmWriteOBJ(OBJMmodel* model, char* filename, GLuint mode);
	GLvoid objmDraw(OBJMmodel* model, GLuint mode);
	GLuint objmList(OBJMmodel* model, GLuint mode);
	GLvoid objmWeld(OBJMmodel* model, GLfloat epsilon);
	GLubyte* objmReadPPM(char* filename, int* width, int* height);
	GLfloat objmMax(GLfloat a, GLfloat b);
	GLfloat objmAbs(GLfloat f);
	GLfloat objmDot(GLfloat* a, GLfloat* b);
	GLvoid objmCross(GLfloat* a, GLfloat* b, GLfloat* n);
	GLvoid objmNormalize(GLfloat* v);
	GLboolean objmEqual(GLfloat* a, GLfloat* b, GLfloat epsilon);
	GLfloat* objmWeldVectors(GLfloat* vectors, GLuint* numvectors, GLfloat epsilon);
	OBJMgroup* objmFindGroup(OBJMmodel* model, char* name);
	OBJMgroup* objmAddGroup(OBJMmodel* model, char* name);
	GLuint objmFindMaterial(OBJMmodel* model, char* name);
	char* objmDirName(char* path);
	GLvoid objmReadMTL(OBJMmodel* model, char* name);
	GLvoid objmWriteMTL(OBJMmodel* model, char* modelpath, char* mtllibname);
	GLvoid objmFirstPass(OBJMmodel* model, FILE* file);
	GLvoid objmSecondPass(OBJMmodel* model, FILE* file);
	float normalize(float* v);
	GLboolean invert(GLdouble src[16], GLdouble inverse[16]);
	void identity(GLdouble m[16]);
	void drawmodel();	
	void buildEdgeList(OBJMmodel* model, int numtriangles);

private:

	int xRot;
	int yRot;
	int zRot;

	int drawPointsAlso;
	float nearPLane;
	float farPlane;
	GLfloat curPickDepth;
	GLint minVertIndex;
	GLfloat minVertCoords[3];
	GLfloat distClosestVertex;
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	vector< vector<int> > edgeList;	
};

#endif /* loadObj.h */
