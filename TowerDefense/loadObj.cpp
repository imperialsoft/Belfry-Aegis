/*
 *	loadObj.cpp
 *
 *	Created on: Mar 13, 2011
 *	Author	  : Anoop Haridas
 */

#include "loadObj.h"

loadObj::loadObj()
{

}

loadObj::~loadObj()
{

}

// Loads a specified OBJ file. Can be used either
// alternatively or together with drawmodel() [see
// below] which loads a default model and draws it. 
bool loadObj::Load(char* filename)
{
	pmodel = objmReadOBJ(filename);
	if (!pmodel) return false;
	objmUnitize(pmodel);
	objmFacetNormals(pmodel);
	objmVertexNormals(pmodel, 90.0);
	if (!pmodel) return false;
	return true;
}

// Saves a loaded OBJ file. The file is saved with 
// smooth normals, and if material info is present, 
// a function is called by the writer below to write 
// out a separate material (.mtl) file. See function
// objmWriteOBJ().
bool loadObj::Save(char* filename)
{
	if (pmodel)
	{
		this->objmWriteOBJ(pmodel, filename, OBJM_SMOOTH_SHADE | OBJM_MATERIAL);
		return true;
	}

	return false;
}

// Loads a default OBJ file. The path to the file may be changed 
// as desired. Loads up material from the approp. material file if 
// so specifed in the OBJ file. See function objmDraw().
void loadObj::drawmodel()
{
	if (!pmodel)
	{
            pmodel = objmReadOBJ("/Users/Curtis/Documents/OpenGL/project3/data/al.obj");
	    if (!pmodel) exit(0);
	    objmUnitize(pmodel);
	    objmFacetNormals(pmodel);
	    objmVertexNormals(pmodel, 90.0);
	}

	objmDraw(pmodel, OBJM_SMOOTH_SHADE | OBJM_MATERIAL);
}

// Scales the model to fit within a cube of dimensions 2 x 2 x 2. This way, the model is 
// centered at (0, 0, 0) and the 'imaginary bounding cube' has corners at (1, 1, 1) and
// at (-1, -1, -1). This is necessary if a single scene and camera model are to be used 
// for any loaded object, since objects have different sizes.
GLfloat loadObj::objmUnitize(OBJMmodel* model)
{
    GLuint  i;
    GLfloat maxx, minx, maxy, miny, maxz, minz;
    GLfloat cx, cy, cz, w, h, d;
    GLfloat scale;

    assert(model);
    assert(model->vertices);

    // Get the maximum/minimums
    maxx = minx = model->vertices[3 + 0];
    maxy = miny = model->vertices[3 + 1];
    maxz = minz = model->vertices[3 + 2];

    for (i = 1; i <= model->numvertices; i++)
	{
        if (maxx < model->vertices[3 * i + 0])
            maxx = model->vertices[3 * i + 0];
        if (minx > model->vertices[3 * i + 0])
            minx = model->vertices[3 * i + 0];

        if (maxy < model->vertices[3 * i + 1])
            maxy = model->vertices[3 * i + 1];
        if (miny > model->vertices[3 * i + 1])
            miny = model->vertices[3 * i + 1];

        if (maxz < model->vertices[3 * i + 2])
            maxz = model->vertices[3 * i + 2];
        if (minz > model->vertices[3 * i + 2])
            minz = model->vertices[3 * i + 2];
    }

    // Calculate the model width, height, and depth
    w = objmAbs(maxx) + objmAbs(minx);
    h = objmAbs(maxy) + objmAbs(miny);
    d = objmAbs(maxz) + objmAbs(minz);

    // Calculate the center of the model
    cx = (maxx + minx) / 2.0;
    cy = (maxy + miny) / 2.0;
    cz = (maxz + minz) / 2.0;

    // Calculate the unitizing scale factor
    scale = 2.0 / objmMax(objmMax(w, h), d);

    // Translate to the center then scale
    for (i = 1; i <= model->numvertices; i++)
	{
        model->vertices[3 * i + 0] -= cx;
        model->vertices[3 * i + 1] -= cy;
        model->vertices[3 * i + 2] -= cz;
        model->vertices[3 * i + 0] *= scale;
        model->vertices[3 * i + 1] *= scale;
        model->vertices[3 * i + 2] *= scale;
    }

    return scale;
}

// Gets the dimensions of the object - l, b, h. The final values
// in the dimensions array represent the bounding box for the object.
GLvoid loadObj::objmDimensions(OBJMmodel* model, GLfloat* dimensions)
{
    GLuint i;
    GLfloat maxx, minx, maxy, miny, maxz, minz;

    assert(model);
    assert(model->vertices);
    assert(dimensions);

    // get the maximum/minimums
    maxx = minx = model->vertices[3 + 0];
    maxy = miny = model->vertices[3 + 1];
    maxz = minz = model->vertices[3 + 2];

    for (i = 1; i <= model->numvertices; i++)
	{
        if (maxx < model->vertices[3 * i + 0])
            maxx = model->vertices[3 * i + 0];
        if (minx > model->vertices[3 * i + 0])
            minx = model->vertices[3 * i + 0];

        if (maxy < model->vertices[3 * i + 1])
            maxy = model->vertices[3 * i + 1];
        if (miny > model->vertices[3 * i + 1])
            miny = model->vertices[3 * i + 1];

        if (maxz < model->vertices[3 * i + 2])
            maxz = model->vertices[3 * i + 2];
        if (minz > model->vertices[3 * i + 2])
            minz = model->vertices[3 * i + 2];
    }

    /* calculate model width, height, and depth */
    dimensions[0] = objmAbs(maxx) + objmAbs(minx);
    dimensions[1] = objmAbs(maxy) + objmAbs(miny);
    dimensions[2] = objmAbs(maxz) + objmAbs(minz);
}

// Scales the model by the passed in scale factor
GLvoid loadObj::objmScale(OBJMmodel* model, GLfloat scale)
{
    GLuint i;

    for (i = 1; i <= model->numvertices; i++)
	{
        model->vertices[3 * i + 0] *= scale;
        model->vertices[3 * i + 1] *= scale;
        model->vertices[3 * i + 2] *= scale;
    }
}

// Reverses the winding of the triangle faces of the model. This
// is done by changing the vertex order - 1,2,3 becomes 3,2,1; this
// effectively flips the face when CCW orientation is used. The normals
// are pointed in the opposite direction. Thus, the net effect is that
// the model is turned 'inside out'.
GLvoid loadObj::objmReverseWinding(OBJMmodel* model)
{
    GLuint i, swap;

    assert(model);

    // Reverse the winding of the faces
    for (i = 0; i < model->numtriangles; i++)
	{
        swap = T(i).vindices[0];
        T(i).vindices[0] = T(i).vindices[2];
        T(i).vindices[2] = swap;

        if (model->numvertexnorms)
		{
            swap = T(i).nindices[0];
            T(i).nindices[0] = T(i).nindices[2];
            T(i).nindices[2] = swap;
        }

        if (model->numtexcoords)
		{
            swap = T(i).tindices[0];
            T(i).tindices[0] = T(i).tindices[2];
            T(i).tindices[2] = swap;
        }
    }

    // Reverse the facet normals
    for (i = 1; i <= model->numfacetnorms; i++)
	{
        model->facetnorms[3 * i + 0] = -model->facetnorms[3 * i + 0];
        model->facetnorms[3 * i + 1] = -model->facetnorms[3 * i + 1];
        model->facetnorms[3 * i + 2] = -model->facetnorms[3 * i + 2];
    }

    // Reverse the vertex normals
    for (i = 1; i <= model->numvertexnorms; i++)
	{
        model->vertexnorms[3 * i + 0] = -model->vertexnorms[3 * i + 0];
        model->vertexnorms[3 * i + 1] = -model->vertexnorms[3 * i + 1];
        model->vertexnorms[3 * i + 2] = -model->vertexnorms[3 * i + 2];
    }
}

// Get the face normals from the OBJ file and normalize them.
// Normalization is necessary when lighting needs to be done on 
// the model.
GLvoid loadObj::objmFacetNormals(OBJMmodel* model)
{
    GLuint  i;
    GLfloat u[3];
    GLfloat v[3];

    assert(model);
    assert(model->vertices);

    // Free any old facetnormals
    if (model->facetnorms)
        free(model->facetnorms);

    // Allocate memory for the new facet normals
    model->numfacetnorms = model->numtriangles;
    model->facetnorms = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (model->numfacetnorms + 1));

    for (i = 0; i < model->numtriangles; i++)
	{
        model->triangles[i].findex = i+1;

        u[0] = model->vertices[3 * T(i).vindices[1] + 0] - model->vertices[3 * T(i).vindices[0] + 0];
        u[1] = model->vertices[3 * T(i).vindices[1] + 1] - model->vertices[3 * T(i).vindices[0] + 1];
        u[2] = model->vertices[3 * T(i).vindices[1] + 2] - model->vertices[3 * T(i).vindices[0] + 2];

        v[0] = model->vertices[3 * T(i).vindices[2] + 0] - model->vertices[3 * T(i).vindices[0] + 0];
        v[1] = model->vertices[3 * T(i).vindices[2] + 1] - model->vertices[3 * T(i).vindices[0] + 1];
        v[2] = model->vertices[3 * T(i).vindices[2] + 2] - model->vertices[3 * T(i).vindices[0] + 2];

        objmCross(u, v, &model->facetnorms[3 * (i+1)]);
        objmNormalize(&model->facetnorms[3 * (i+1)]);
    }
}

// Computes the normals at every vertex. See the comments below for explanation.
GLvoid loadObj::objmVertexNormals(OBJMmodel* model, GLfloat angle)
{
    OBJMnode* node;
    OBJMnode* tail;
    OBJMnode** members;
    GLfloat* normals;
    GLuint numnormals;
    GLfloat average[3];
    GLfloat dot, cos_angle;
    GLuint i, avg;

    assert(model);
    assert(model->facetnorms);

    // Calculate the cosine of the angle (in degrees)
    cos_angle = cos(angle * C_PI / 180.0);

    // Free any previous normals
    if (model->vertexnorms)
        free(model->vertexnorms);

    // Allocate space for the new normals
    model->numvertexnorms = model->numtriangles * 3;               /* 3 normals per triangle */
    model->vertexnorms = (GLfloat*)malloc(sizeof(GLfloat)* 3 * (model->numvertexnorms+1));

    // Allocate a structure that will hold a linked list of triangle indices for each vertex
    members = (OBJMnode**)malloc(sizeof(OBJMnode*) * (model->numvertices + 1));
    for (i = 1; i <= model->numvertices; i++)
        members[i] = NULL;

    // For every triangle, create a node for each vertex in it
    for (i = 0; i < model->numtriangles; i++)
	{
        node = (OBJMnode*)malloc(sizeof(OBJMnode));
        node->index = i;
        node->next  = members[T(i).vindices[0]];
        members[T(i).vindices[0]] = node;

        node = (OBJMnode*)malloc(sizeof(OBJMnode));
        node->index = i;
        node->next  = members[T(i).vindices[1]];
        members[T(i).vindices[1]] = node;

        node = (OBJMnode*)malloc(sizeof(OBJMnode));
        node->index = i;
        node->next  = members[T(i).vindices[2]];
        members[T(i).vindices[2]] = node;
    }

    // Calculate the average normal for each vertex
    numnormals = 1;
    for (i = 1; i <= model->numvertices; i++)
	{
		// Calculate an average normal for this vertex by averaging the
		// facet normal of every triangle this vertex is in
        node = members[i];

        if (!node)
            fprintf(stderr, "objmVertexNormals(): vertex w/o a triangle\n");

        average[0] = 0.0; average[1] = 0.0; average[2] = 0.0;
        avg = 0;

        while (node)
		{
			// only average if the dot product of the angle between the two
            // facet normals is greater than the cosine of the threshold
            // angle -- or, said another way, the angle between the two
            // facet normals is less than (or equal to) the threshold angle

            dot = objmDot(&model->facetnorms[3 * T(node->index).findex],&model->facetnorms[3 * T(members[i]->index).findex]);
            if (dot > cos_angle)
			{
                node->averaged = GL_TRUE;
                average[0] += model->facetnorms[3 * T(node->index).findex + 0];
                average[1] += model->facetnorms[3 * T(node->index).findex + 1];
                average[2] += model->facetnorms[3 * T(node->index).findex + 2];

                avg = 1;            // At least one normal averaged
            }
			else
			{
                node->averaged = GL_FALSE;
            }
            node = node->next;
        }

        if (avg)
		{
            // normalize the averaged normal
            objmNormalize(average);

            // add the normal to the vertex normals list
            model->vertexnorms[3 * numnormals + 0] = average[0];
            model->vertexnorms[3 * numnormals + 1] = average[1];
            model->vertexnorms[3 * numnormals + 2] = average[2];
            avg = numnormals;
            numnormals++;
        }

        // Set the normal of this vertex in each triangle it is in
        node = members[i];

        while (node)
		{
            if (node->averaged)
			{
                // If this node was averaged, use the average normal
                if (T(node->index).vindices[0] == i)
                    T(node->index).nindices[0] = avg;
                else if (T(node->index).vindices[1] == i)
                    T(node->index).nindices[1] = avg;
                else if (T(node->index).vindices[2] == i)
                    T(node->index).nindices[2] = avg;
            }
			else
			{
                // If this node wasn't averaged, use the facet normal
                model->vertexnorms[3 * numnormals + 0] = model->facetnorms[3 * T(node->index).findex + 0];
                model->vertexnorms[3 * numnormals + 1] = model->facetnorms[3 * T(node->index).findex + 1];
                model->vertexnorms[3 * numnormals + 2] = model->facetnorms[3 * T(node->index).findex + 2];

                if (T(node->index).vindices[0] == i)
                    T(node->index).nindices[0] = numnormals;
                else if (T(node->index).vindices[1] == i)
                    T(node->index).nindices[1] = numnormals;
                else if (T(node->index).vindices[2] == i)
                    T(node->index).nindices[2] = numnormals;
                numnormals++;
            }
            node = node->next;
        }
    }

    model->numvertexnorms = numnormals - 1;

    // Free the member information
    for (i = 1; i <= model->numvertices; i++)
	{
        node = members[i];

        while (node)
		{
            tail = node;
            node = node->next;
            free(tail);
        }
    }
    free(members);

    // pack the normals array (we previously allocated the maximum
    // number of normals that could possibly be created (numtriangles *
    // 3), so get rid of some of them (usually alot unless none of the
    // facet normals were averaged))

    normals = model->vertexnorms;
    model->vertexnorms = (GLfloat*)malloc(sizeof(GLfloat)* 3 * (model->numvertexnorms+1));

    for (i = 1; i <= model->numvertexnorms; i++)
	{
        model->vertexnorms[3 * i + 0] = normals[3 * i + 0];
        model->vertexnorms[3 * i + 1] = normals[3 * i + 1];
        model->vertexnorms[3 * i + 2] = normals[3 * i + 2];
    }
    free(normals);
}

// Generates linear texture information given the texture 
// coordinates in the OBJ file. Gets texture u,v coordinates
GLvoid loadObj::objmLinearTexture(OBJMmodel* model)
{
    OBJMgroup *group;
    GLfloat dimensions[3];
    GLfloat x, y, scalefactor;
    GLuint i;

    assert(model);

    if (model->texcoords)
        free(model->texcoords);

    model->numtexcoords = model->numvertices;
    model->texcoords = (GLfloat*)malloc(sizeof(GLfloat)*2*(model->numtexcoords+1));

    objmDimensions(model, dimensions);
    scalefactor = 2.0 / objmAbs(objmMax(objmMax(dimensions[0], dimensions[1]), dimensions[2]));

    // Do the calculations
    for(i = 1; i <= model->numvertices; i++)
	{
        x = model->vertices[3 * i + 0] * scalefactor;
        y = model->vertices[3 * i + 2] * scalefactor;

        model->texcoords[2 * i + 0] = (x + 1.0) / 2.0;
        model->texcoords[2 * i + 1] = (y + 1.0) / 2.0;
    }

    // Go through and put texture coordinate indices in all the triangles */
    group = model->groups;
    while(group)
	{
        for(i = 0; i < group->numtriangles; i++)
		{
            T(group->triangles[i]).tindices[0] = T(group->triangles[i]).vindices[0];
            T(group->triangles[i]).tindices[1] = T(group->triangles[i]).vindices[1];
            T(group->triangles[i]).tindices[2] = T(group->triangles[i]).vindices[2];
        }
        group = group->next;
    }

#if 0
    printf("glmLinearTexture(): generated %d linear texture coordinates\n",model->numtexcoords);
#endif
}

// Generates spherical texture information given the texture 
// coordinates in the OBJ file. Gets spherical coordinates for the texture.
GLvoid loadObj::objmSpheremapTexture(OBJMmodel* model)
{
    OBJMgroup* group;
    GLfloat theta, phi, rho, x, y, z, r;
    GLuint i;

    assert(model);
    assert(model->vertexnorms);

    if (model->texcoords)
        free(model->texcoords);

    model->numtexcoords = model->numvertexnorms;
    model->texcoords=(GLfloat*)malloc(sizeof(GLfloat)*2*(model->numtexcoords+1));

    for (i = 1; i <= model->numvertexnorms; i++)
	{
        z = model->vertexnorms[3 * i + 0];  // Re-arrange for pole distortion
        y = model->vertexnorms[3 * i + 1];
        x = model->vertexnorms[3 * i + 2];
        r = sqrt((x * x) + (y * y));
        rho = sqrt((r * r) + (z * z));

        if(r == 0.0)
		{
            theta = 0.0;
            phi = 0.0;
        }
		else
		{
            if(z == 0.0)
                phi = 3.14159265 / 2.0;
            else
                phi = acos(z / rho);

            if(y == 0.0)
                theta = 3.14159265 / 2.0;
            else
                theta = asin(y / r) + (3.14159265 / 2.0);
        }

        model->texcoords[2 * i + 0] = theta / 3.14159265;
        model->texcoords[2 * i + 1] = phi / 3.14159265;
    }

    // Go through and put texcoord indices in all the triangles
    group = model->groups;
    while(group)
	{
        for (i = 0; i < group->numtriangles; i++)
		{
            T(group->triangles[i]).tindices[0] = T(group->triangles[i]).nindices[0];
            T(group->triangles[i]).tindices[1] = T(group->triangles[i]).nindices[1];
            T(group->triangles[i]).tindices[2] = T(group->triangles[i]).nindices[2];
        }
        group = group->next;
    }
}

// Delete the model. Ensures there are NO memory leaks!!
GLvoid loadObj::objmDelete(OBJMmodel* model)
{
    OBJMgroup* group;
    GLuint i;

    assert(model);

    if (model->pathname)
		free(model->pathname);
    if (model->mtllibname)
		free(model->mtllibname);
    if (model->vertices)
		free(model->vertices);
    if (model->vertexnorms)
		free(model->vertexnorms);
    if (model->texcoords)
		free(model->texcoords);
    if (model->facetnorms)
		free(model->facetnorms);
    if (model->triangles)
		free(model->triangles);
    if (model->materials)
	{
        for (i = 0; i < model->nummaterials; i++)
            free(model->materials[i].name);
    }
    free(model->materials);                        
    while(model->groups)
	{
        group = model->groups;
        model->groups = model->groups->next;
        free(group->name);
        free(group->triangles);
        free(group);
    }

    free(model);
}

// Reads the OBJ file and loads it into the OBJMmodel container / 'object'.
// Does this in two passes. The first pass goes thru 
// the OBJ file and gets the model statistics needed for memory
// allocation of the various arrays and linked list. The second
// pass then reads the OBJ info into the allocated data structures.
OBJMmodel* loadObj::objmReadOBJ(char* filename)
{
    OBJMmodel* model;
    FILE* file;

    // Open the file for reading
    file = fopen(filename, "r");
    if (!file)
	{
        fprintf(stderr, "objmReadOBJ() failed: can't open data file \"%s\".\n",filename);
        exit(1);
    }

    // Allocate a new model
    model = (OBJMmodel*)malloc(sizeof(OBJMmodel));
    model->pathname = strdup(filename);

    model->mtllibname = NULL;

    model->numvertices = 0;
    model->vertices = NULL;

    model->numvertexnorms = 0;
    model->vertexnorms = NULL;

    model->numtexcoords = 0;
    model->texcoords = NULL;

    model->numfacetnorms = 0;
    model->facetnorms = NULL;

    model->numtriangles = 0;
    model->triangles = NULL;

    model->nummaterials = 0;
    model->materials = NULL;

    model->numgroups = 0;
    model->groups = NULL;

    model->position[0] = 0.0;
    model->position[1] = 0.0;
    model->position[2] = 0.0;

    // Make a first pass through the file to get the model statistics
    objmFirstPass(model, file);

    // Allocate memory with data from first pass
    model->vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (model->numvertices + 1));
    model->triangles = (OBJMtriangle*)malloc(sizeof(OBJMtriangle) * model->numtriangles);
    if (model->numvertexnorms)
	{
        model->vertexnorms = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (model->numvertexnorms + 1));
    }
    if (model->numtexcoords)
	{
        model->texcoords = (GLfloat*)malloc(sizeof(GLfloat) * 2 * (model->numtexcoords + 1));
    }

    // Rewind to beginning of file and read in the data in the second pass
    rewind(file);

    objmSecondPass(model, file);

    // Close the file
    fclose(file);

    return model;
}

// Writes out the data present in parameter 'model' to the OBJ file 
// specified by 'filename'. If material info is present, the material 
// writer function is called. Parameter 'mode' is an OR combination of
// different flags that determine what gets written. See below for details.
GLvoid loadObj::objmWriteOBJ(OBJMmodel* model, char* filename, GLuint mode)
{
    GLuint  i;
    FILE*   file;
    OBJMgroup* group;

    assert(model);

    // Perform warning for contradicting modes

    if (mode & OBJM_FLAT_SHADE && !model->facetnorms)
	{
        printf("objmWriteOBJ() warning: flat normal output requested with no facet normals defined.\n");
        mode &= ~OBJM_FLAT_SHADE;
    }
    if (mode & OBJM_SMOOTH_SHADE && !model->vertexnorms)
	{
        printf("objmWriteOBJ() warning: smooth normal output requested with no normals defined.\n");
        mode &= ~OBJM_SMOOTH_SHADE;
    }
    if (mode & OBJM_TEXTURE && !model->texcoords)
	{
        printf("objmWriteOBJ() warning: texture coordinate output requested with no texture coordinates defined.\n");
        mode &= ~OBJM_TEXTURE;
    }
    if (mode & OBJM_FLAT_SHADE && mode & OBJM_SMOOTH_SHADE)
	{
        printf("objmWriteOBJ() warning: flat normal output requested and smooth normal output requested (using smooth).\n");
        mode &= ~OBJM_FLAT_SHADE;
    }
    if (mode & OBJM_COLOR && !model->materials)
	{
        printf("objmWriteOBJ() warning: color output requested with no colors (materials) defined.\n");
        mode &= ~OBJM_COLOR;
    }
    if (mode & OBJM_MATERIAL && !model->materials)
	{
        printf("objmWriteOBJ() warning: material output requested with no materials defined.\n");
        mode &= ~OBJM_MATERIAL;
    }
    if (mode & OBJM_COLOR && mode & OBJM_MATERIAL)
	{
        printf("objmWriteOBJ() warning: color and material output requested; outputting only materials.\n");
        mode &= ~OBJM_COLOR;
    }


    // Open the file for writing
    file = fopen(filename, "w");
    if (!file)
	{
        fprintf(stderr, "objmWriteOBJ() failed: can't open file \"%s\" to write.\n",filename);
        exit(1);
    }

    // Generate a header
    fprintf(file, "#  \n");
    fprintf(file, "#  Wavefront OBJ generated by OBJM library\n");
    fprintf(file, "#  \n");
    fprintf(file, "#  OBJM library\n");
    fprintf(file, "#  Anoop Haridas\n");
    fprintf(file, "#  \n");

    if (mode & OBJM_MATERIAL && model->mtllibname)
	{
        fprintf(file, "\nmtllib %s\n\n", model->mtllibname);
        objmWriteMTL(model, filename, model->mtllibname);
    }

    // Generate the vertices
    fprintf(file, "\n");
    fprintf(file, "# %d vertices\n", model->numvertices);
    for (i = 1; i <= model->numvertices; i++)
	{
        fprintf(file, "v %f %f %f\n",model->vertices[3 * i + 0],model->vertices[3 * i + 1],model->vertices[3 * i + 2]);
    }

    // Generate the smooth/flat normals
    if (mode & OBJM_SMOOTH_SHADE)
	{
        fprintf(file, "\n");
        fprintf(file, "# %d normals\n", model->numvertexnorms);
        for (i = 1; i <= model->numvertexnorms; i++)
		{
            fprintf(file, "vn %f %f %f\n",model->vertexnorms[3 * i + 0],model->vertexnorms[3 * i + 1],model->vertexnorms[3 * i + 2]);
        }
    }
	else if (mode & OBJM_FLAT_SHADE)
	{
        fprintf(file, "\n");
        fprintf(file, "# %d normals\n", model->numfacetnorms);
        for (i = 1; i <= model->numvertexnorms; i++)
		{
            fprintf(file, "vn %f %f %f\n",model->facetnorms[3 * i + 0],model->facetnorms[3 * i + 1],model->facetnorms[3 * i + 2]);
        }
    }

    // Generate the texture coordinates
    if (mode & OBJM_TEXTURE)
	{
        fprintf(file, "\n");
        fprintf(file, "# %d texcoords\n", model->numtexcoords);
        for (i = 1; i <= model->numtexcoords; i++)
		{
            fprintf(file, "vt %f %f\n", model->texcoords[2 * i + 0],model->texcoords[2 * i + 1]);
        }
    }

    fprintf(file, "\n");
    fprintf(file, "# %d groups\n", model->numgroups);
    fprintf(file, "# %d faces (triangles)\n", model->numtriangles);
    fprintf(file, "\n");

    group = model->groups;
    while(group)
	{
        fprintf(file, "g %s\n", group->name);
        if (mode & OBJM_MATERIAL)
            fprintf(file, "usemtl %s\n", model->materials[group->material].name);

        for (i = 0; i < group->numtriangles; i++)
		{
            if (mode & OBJM_SMOOTH_SHADE && mode & OBJM_TEXTURE)
			{
                fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).tindices[0],
                    T(group->triangles[i]).nindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).tindices[1],
                    T(group->triangles[i]).nindices[1],
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).tindices[2],
                    T(group->triangles[i]).nindices[2]);
            }
			else if (mode & OBJM_FLAT_SHADE && mode & OBJM_TEXTURE)
			{
                fprintf(file, "f %d/%d %d/%d %d/%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).findex);
            }
			else if (mode & OBJM_TEXTURE)
			{
                fprintf(file, "f %d/%d %d/%d %d/%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).tindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).tindices[1],
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).tindices[2]);
            }
			else if (mode & OBJM_SMOOTH_SHADE)
			{
                fprintf(file, "f %d//%d %d//%d %d//%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).nindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).nindices[1],
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).nindices[2]);
            }
			else if (mode & OBJM_FLAT_SHADE)
			{
                fprintf(file, "f %d//%d %d//%d %d//%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).findex);
            }
			else
			{
                fprintf(file, "f %d %d %d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).vindices[2]);
            }
        }
        fprintf(file, "\n");
        group = group->next;
    }

    fclose(file);
}

// Draws the model using the info in parameter 'model'.
GLvoid loadObj::objmDraw(OBJMmodel* model, GLuint mode)
{
    static GLuint i;
    static OBJMgroup* group;
    static OBJMtriangle* triangle;
    static OBJMmaterial* material;

    assert(model);
    assert(model->vertices);

    // Perform warning about contradictory modes

    if (mode & OBJM_FLAT_SHADE && !model->facetnorms)
	{
        printf("objmDraw() warning: flat render mode requested with no facet normals defined.\n");
        mode &= ~OBJM_FLAT_SHADE;
    }
    if (mode & OBJM_SMOOTH_SHADE && !model->vertexnorms)
	{
        printf("objmDraw() warning: smooth render mode requested with no vertex normals defined.\n");
        mode &= ~OBJM_SMOOTH_SHADE;
    }
    if (mode & OBJM_TEXTURE && !model->texcoords)
	{
        printf("objmDraw() warning: texture render mode requested with no texture coordinates defined.\n");
        mode &= ~OBJM_TEXTURE;
    }
    if (mode & OBJM_FLAT_SHADE && mode & OBJM_SMOOTH_SHADE)
	{
        printf("objmDraw() warning: flat render mode requested and smooth render mode requested (using smooth).\n");
        mode &= ~OBJM_FLAT_SHADE;
    }
    if (mode & OBJM_COLOR && !model->materials)
	{
        printf("objmDraw() warning: color render mode requested with no materials defined.\n");
        mode &= ~OBJM_COLOR;
    }
    if (mode & OBJM_MATERIAL && !model->materials)
	{
        printf("objmDraw() warning: material render mode requested with no materials defined.\n");
        mode &= ~OBJM_MATERIAL;
    }
    if (mode & OBJM_COLOR && mode & OBJM_MATERIAL)
	{
        printf("objmDraw() warning: color and material render mode requested using only material mode.\n");
        mode &= ~OBJM_COLOR;
    }

    if (mode & OBJM_COLOR)
        glEnable(GL_COLOR_MATERIAL);
    else if (mode & OBJM_MATERIAL)
        glDisable(GL_COLOR_MATERIAL);

    group = model->groups;
    while (group)
	{
        if (mode & OBJM_MATERIAL)
		{
            material = &model->materials[group->material];
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
        }

        if (mode & OBJM_COLOR)
		{
            glColor3fv(material->diffuse);
        }

        if (this->drawType == 0)	glBegin(GL_POINTS);
        else if (this->drawType == 1)	glBegin(GL_LINES);
        else if (this->drawType == 2)	glBegin(GL_TRIANGLES);

        for (i = 0; i < group->numtriangles; i++)
		{
            triangle = &T(group->triangles[i]);

            if (mode & OBJM_FLAT_SHADE)
                glNormal3fv(&model->facetnorms[3 * triangle->findex]);
            if (mode & OBJM_SMOOTH_SHADE)
                glNormal3fv(&model->vertexnorms[3 * triangle->nindices[0]]);
            if (mode & OBJM_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[0]]);

            glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);

            if (mode & OBJM_SMOOTH_SHADE)
                glNormal3fv(&model->vertexnorms[3 * triangle->nindices[1]]);
            if (mode & OBJM_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[1]]);

            glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);

            if (mode & OBJM_SMOOTH_SHADE)
                glNormal3fv(&model->vertexnorms[3 * triangle->nindices[2]]);
            if (mode & OBJM_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[2]]);

            glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
        }
        glEnd();

        if (this->drawPointsAlso == 1 && this->drawType == 2)
        {
        	glPointSize(2.0);
        	glEnable(GL_COLOR_MATERIAL);
        	glColor3f(1.0, 1.0, 1.0);
                glBegin(GL_POINTS);
        	for (i = 0; i < group->numtriangles; i++)
        	{
        		triangle = &T(group->triangles[i]);
        		glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
        		glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
        		glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
        	}
        	glEnd();
        	glDisable(GL_COLOR_MATERIAL);
        	glPointSize(1.0);
        }

        group = group->next;
    }
}

// Creates an OpenGL display list for the model
GLuint loadObj::objmList(OBJMmodel* model, GLuint mode)
{
    GLuint list;

    list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    objmDraw(model, mode);
    glEndList();

    return list;
}

GLvoid loadObj::objmWeld(OBJMmodel* model, GLfloat epsilon)
{
    GLfloat* vectors;
    GLfloat* copies;
    GLuint numvectors;
    GLuint i;

    // Vertices
    numvectors = model->numvertices;
    vectors  = model->vertices;
    copies = objmWeldVectors(vectors, &numvectors, epsilon);

#if 0
    printf("glmWeld(): %d redundant vertices.\n",model->numvertices - numvectors - 1);
#endif

    for (i = 0; i < model->numtriangles; i++)
	{
        T(i).vindices[0] = (GLuint)vectors[3 * T(i).vindices[0] + 0];
        T(i).vindices[1] = (GLuint)vectors[3 * T(i).vindices[1] + 0];
        T(i).vindices[2] = (GLuint)vectors[3 * T(i).vindices[2] + 0];
    }

    // Free space for the old vertices
    free(vectors);

    // Allocate space for the new vertices
    model->numvertices = numvectors;
    model->vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (model->numvertices + 1));

    // Copy the optimized vertices into the actual vertex list
    for (i = 1; i <= model->numvertices; i++)
	{
        model->vertices[3 * i + 0] = copies[3 * i + 0];
        model->vertices[3 * i + 1] = copies[3 * i + 1];
        model->vertices[3 * i + 2] = copies[3 * i + 2];
    }

    free(copies);
}

// Reads texture info from a .ppm image file
GLubyte* loadObj::objmReadPPM(char* filename, int* width, int* height)
{
    FILE* fp;
    int i, w, h, d;
    unsigned char* image;
    char head[70];          // Max line length <= 70 in PPM (per spec).

    fp = fopen(filename, "rb");
    if (!fp)
	{
        perror(filename);
        return NULL;
    }

    // Get the first two chars of the file and make sure that it has the
    // correct magic cookie for a raw PPM file.
    fgets(head, 70, fp);
    if (strncmp(head, "P6", 2))
	{
        fprintf(stderr, "%s: Not a raw PPM file\n", filename);
        return NULL;
    }

    // Get the three elements in the header (width, height, maxval).
    i = 0;
    while(i < 3)
	{
        fgets(head, 70, fp);
        if (head[0] == '#')     // Skip the comments.
            continue;
        if (i == 0)
            i += sscanf(head, "%d %d %d", &w, &h, &d);
        else if (i == 1)
            i += sscanf(head, "%d %d", &h, &d);
        else if (i == 2)
            i += sscanf(head, "%d", &d);
    }

    // Get all the image data at once
    image = (unsigned char*)malloc(sizeof(unsigned char)*w*h*3);
    fread(image, sizeof(unsigned char), w*h*3, fp);
    fclose(fp);

    *width = w;
    *height = h;
    return image;
}

// STATIC FNS. START HERE
// objmMax: Returns the maximum of two floats
GLfloat loadObj::objmMax(GLfloat a, GLfloat b)
{
	if (a > b)
		return a;
	return b;
}

// objmAbs: Returns the absolute value of a float
GLfloat loadObj::objmAbs(GLfloat f)
{
	if (f < 0)
		return -f;
	return f;
}

// objmDot: Computes the dot product of two vectors
GLfloat loadObj::objmDot(GLfloat* a, GLfloat* b)
{
	assert(a);
	assert(b);

	return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
}

// objmCross: Computes the cross product of two vectors
GLvoid loadObj::objmCross(GLfloat* a, GLfloat* b, GLfloat* n)
{
	assert(a);
	assert(b);
	assert(n);

	n[0] = a[1]*b[2] - a[2]*b[1];
	n[1] = a[2]*b[0] - a[0]*b[2];
	n[2] = a[0]*b[1] - a[1]*b[0];
}

// objmNormalize: Normalize a vector
GLvoid loadObj::objmNormalize(GLfloat* v)
{
	GLfloat x;

	assert(v);

	x = (GLfloat)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] /= x;
	v[1] /= x;
	v[2] /= x;
}

// objmEqual: Compares 2 vectors; returns GL_TRUE if they are equal
// (within a threshold epsilon) & GL_FALSE if they are not
GLboolean loadObj::objmEqual(GLfloat* a, GLfloat* b, GLfloat epsilon)
{
	if (objmAbs(a[0] - b[0]) < epsilon &&
		objmAbs(a[1] - b[1]) < epsilon &&
		objmAbs(a[2] - b[2]) < epsilon)
	{
		return GL_TRUE;
	}
	return GL_FALSE;
}
// STATICS END HERE

// objmWeldVectors: Eliminate vectors that are within epsilon of each other
GLfloat* loadObj::objmWeldVectors(GLfloat* vectors, GLuint* numvectors, GLfloat epsilon)
{
	GLfloat* copies;
	GLuint copied;
	GLuint i,j;

	copies = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (*numvectors + 1));
	memcpy(copies,vectors,(sizeof(GLfloat) * 3 * (*numvectors + 1)));

	copied = 1;

	for (i=1;i<=*numvectors;i++)
	{
		for (j=1;j<=copied;j++)
		{
			if (objmEqual(&vectors[3*i],&copies[3*j],epsilon))

				// Set 1st vector component to point to correct index
				vectors[3*i+0] = (GLfloat)j;                            // goto here
		}

		// must not be any duplicates - add to the copies array
		copies[3*copied + 0] = vectors[3*i + 0];
		copies[3*copied + 1] = vectors[3*i + 1];
		copies[3*copied + 2] = vectors[3*i + 2];

		// pass for below
		j = copied;
		copied++;
	}

	*numvectors = copied - 1;
	return copies;
}

// objmFindGroup: Find a group in the model
OBJMgroup* loadObj::objmFindGroup(OBJMmodel* model, char* name)
{
	OBJMgroup* group;

	assert(model);

	group = model->groups;
	while (group)
	{
		if (!strcmp(name,group->name))     // DOUBT HERE
			break;
		group = group->next;
	}
	return group;
}

// objmAddGroup: Add a group to the model
OBJMgroup* loadObj::objmAddGroup(OBJMmodel* model, char* name)
{
	OBJMgroup* group;

	group = objmFindGroup(model,name);
	if (!group)
	{
		group = (OBJMgroup*)malloc(sizeof(OBJMgroup));
		group->name = strdup(name);
		group->numtriangles = 0;
		group->material = 0;
		group->triangles = NULL;
		group->next = model->groups;
		model->groups = group;
		model->numgroups++;
	}

	return group;
}

// objmFindMaterial: Find a material in the model
GLuint loadObj::objmFindMaterial(OBJMmodel* model, char* name)
{
	GLuint i;

	for (i=0;i < model->nummaterials;i++)
	{
		if (!strcmp(model->materials[i].name,name))            // DOUBT HERE
			goto found;                                             // goto here
	}

	printf("objmFindMaterial(): Can't find material \"%s\". \n", name);
	i = 0;

found:
	return i;
}

// STATIC FUNCTIONS BEGIN
// objmDirName: Return the directory given the path
// path - Filesystem path
char* loadObj::objmDirName(char* path)
{
	char* dir;
	char* s;

	dir = strdup(path);

	s = strrchr(dir, '/');
	if (s)
		s[1] = '\0';
	else
		dir[0] = '\0';

	return dir;
}

// objmReadMTL: Read a Wavefront material library file
// name - Name of the material library
GLvoid loadObj::objmReadMTL(OBJMmodel* model, char* name)
{
	FILE* file;
	char* dir;
	char* filename;
	char buf[128];
	GLuint nummaterials,i;

	dir = objmDirName(model->pathname);
	filename = (char*)malloc(sizeof(char) * (strlen(dir)+strlen(name)+1));
	strcpy(filename,dir);
	strcat(filename,name);
	free(dir);

	// Open the material file for reading
	file = fopen(filename,"r");
	if (!file)
	{
		fprintf(stderr,"objmReadMTL() failed: Can't open material file %s.\n",filename);
		exit(1);
	}
	free(filename);

	// Count the number of materials in the file
	nummaterials = 1;
	while (fscanf(file,"%s",buf) != EOF)
	{
		switch(buf[0])
		{
		case '#':			                          // Comment
			// Simply get the rest of the line
			fgets(buf,sizeof(buf),file);
			break;
		case 'n':                                     // New Material
			// Get reat of the line and increment nummaterials
			fgets(buf,sizeof(buf),file);
			nummaterials++;
			sscanf(buf,"%s %s",buf,buf);
			break;
		default:
			// Simply get the rest of the line
			fgets(buf,sizeof(buf),file);
			break;
		}
	}

	rewind(file);

	// Allocate memory for materials
	model->materials = (OBJMmaterial*)malloc(sizeof(OBJMmaterial)*nummaterials);
	model->nummaterials = nummaterials;

	// Set the default material
	for (i=0;i<nummaterials;i++)
	{
		model->materials[i].name = NULL;
		model->materials[i].shininess = 70.0;
		model->materials[i].diffuse[0] = 0.8;
		model->materials[i].diffuse[1] = 0.8;
		model->materials[i].diffuse[2] = 0.8;
		model->materials[i].diffuse[3] = 1.0;
		model->materials[i].ambient[0] = 0.2;
		model->materials[i].ambient[1] = 0.2;
		model->materials[i].ambient[2] = 0.2;
		model->materials[i].ambient[3] = 1.0;
		model->materials[i].specular[0] = 0.0;
		model->materials[i].specular[1] = 0.0;
		model->materials[i].specular[2] = 0.0;
		model->materials[i].specular[3] = 1.0;
	}
	model->materials[0].name = strdup("default");

	// Read in the material data
	nummaterials = 0;
	while (fscanf(file,"%s",buf) != EOF)
	{
		switch(buf[0])
		{
		case '#':									                      // comment
			// Simply get rest of the line
			fgets(buf,sizeof(buf),file);
			break;
		case 'n':									                      // New material
			fgets(buf,sizeof(buf),file);
			sscanf(buf,"%s %s",buf,buf);
			nummaterials++;
			model->materials[nummaterials].name = strdup(buf);
			break;
		case 'N':									                      // Specular exponent
			fscanf(file,"%f",&model->materials[nummaterials].shininess);
			// Wavefront shininess is from [0,1000], scale for OpenGL
			model->materials[nummaterials].shininess /= 1000.0;
			model->materials[nummaterials].shininess *= 128.0;
			break;
		case 'K':
			switch(buf[1])
			{
			case 'd':													  // Diffuse comp
				fscanf(file,"%f %f %f",
					&model->materials[nummaterials].diffuse[0],
					&model->materials[nummaterials].diffuse[1],
					&model->materials[nummaterials].diffuse[2]);
				break;
			case 's':													  // Specular comp
				fscanf(file,"%f %f %f",
					&model->materials[nummaterials].specular[0],
					&model->materials[nummaterials].specular[1],
					&model->materials[nummaterials].specular[2]);
				break;
			case 'a':													  // Ambient comp
				fscanf(file,"%f %f %f",
					&model->materials[nummaterials].ambient[0],
					&model->materials[nummaterials].ambient[1],
					&model->materials[nummaterials].ambient[2]);
				break;
			default:
				// Simply get the rest of the line
				fgets(buf,sizeof(buf),file);
				break;
			}
		default:
			// Simply get the rest of the line
			fgets(buf,sizeof(buf),file);
			break;
		}
	}
}

// objmWriteMTL: Write a Wavefront material library file
// modelpath  - Path of the model being written
// mtllibname - Name of the material library to be written
GLvoid loadObj::objmWriteMTL(OBJMmodel* model, char* modelpath, char* mtllibname)
{
	FILE* file;
	char* dir;
	char* filename;
	OBJMmaterial* material;
	GLuint i;

	dir = objmDirName(modelpath);
	filename = (char*)malloc(sizeof(char)*(strlen(dir) + strlen(mtllibname)));
	strcpy(filename,dir);
	strcat(filename,mtllibname);
	free(dir);

	// Open the file for writing
	file = fopen(filename,"w");
	if (!file)
	{
		fprintf(stderr,"objmWriteMTL() failed: Can't open file \"%s\".\n",filename);
		exit(1);
	}
	free(filename);

	// Output a header
	fprintf(file, "#  \n");
    fprintf(file, "#  Wavefront MTL generated by OBJM library\n");
    fprintf(file, "#  \n");
    fprintf(file, "#  OBJM library\n");
    fprintf(file, "#  Anoop Haridas\n");
    fprintf(file, "#  \n\n");

	// Write the material data to the file
	for (i=0;i < model->nummaterials;i++)
	{
		material = &model->materials[i];

		fprintf(file,"newmtl %s\n",material->name);
		fprintf(file,"Ka %f %f %f\n",material->ambient[0],material->ambient[1],material->ambient[2]);
		fprintf(file,"Kd %f %f %f\n",material->diffuse[0],material->diffuse[1],material->diffuse[2]);
		fprintf(file,"Ks %f %f %f\n",material->specular[0],material->specular[1],material->specular[2]);
		fprintf(file,"Ns %f\n",material->shininess*128.0/1000.0);
		fprintf(file,"\n");
	}
}

// objmFirstPass: First pass through the Wavefront .obj file that gets
// all the statistics of the model (#vertices,#normals,#texcoords,#triangles)
// file: fopen()d file descriptor
GLvoid loadObj::objmFirstPass(OBJMmodel* model, FILE* file)
{
	GLuint numvertices;       // Number of vertices in the model
	GLuint numnormals;		  // Number of normals in the model
	GLuint numtexcoords;	  // Number of texcoords in the model
	GLuint numtriangles;	  // Number of trianlges in the model
	OBJMgroup* group;		  // The current group
	unsigned int v,n,t;
	char buf[128];

	// Make a default group
	group = objmAddGroup(model,"default");

	// Initialize the counters
	numvertices = numtriangles = numnormals = numtexcoords = 0;

	while (fscanf(file,"%s",buf) != EOF)
	{
		switch(buf[0])
		{
		case '#':			  // Comment
			// Simply get the rest of line
			fgets(buf,sizeof(buf),file);
			break;
		case 'v':			  // v, vn, vt
			switch(buf[1])
			{
			case '\0':		  // vertex
				// Get the rest of line & increment vertex counter
				fgets(buf,sizeof(buf),file);
				numvertices++;
				break;
			case 'n':		  // normal
				// Get the rest of line & increment normal counter
				fgets(buf,sizeof(buf),file);
				numnormals++;
				break;
			case 't':		  // texcoord
				// Get the rest of line & increment texcoord counter
				fgets(buf,sizeof(buf),file);
				numtexcoords++;
				break;
			default:
                printf("glmFirstPass(): Unknown token \"%s\".\n", buf);
                exit(1);
                break;
			}
			break;
		case 'm':
            fgets(buf, sizeof(buf), file);
            sscanf(buf, "%s %s", buf, buf);
            model->mtllibname = strdup(buf);
            objmReadMTL(model,buf);
            break;
		case 'u':
            // Simply get the rest of line
            fgets(buf, sizeof(buf), file);
            break;
		case 'g':             // group
            fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
            sscanf(buf, "%s", buf);
#else
            buf[strlen(buf)-1] = '\0';
#endif
            group = objmAddGroup(model, buf);
            break;
        case 'f':             // face
            v = n = t = 0;
            fscanf(file, "%s", buf);

            // can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d
            if (strstr(buf, "//"))
			{
				// v//n
                sscanf(buf, "%d//%d", &v, &n);
                fscanf(file, "%d//%d", &v, &n);
                fscanf(file, "%d//%d", &v, &n);
                numtriangles++;
                group->numtriangles++;
                while(fscanf(file, "%d//%d", &v, &n) > 0)
				{
					numtriangles++;
                    group->numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
			{
				// v/t/n
                fscanf(file, "%d/%d/%d", &v, &t, &n);
                fscanf(file, "%d/%d/%d", &v, &t, &n);
                numtriangles++;
                group->numtriangles++;
                while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0)
				{
					numtriangles++;
                    group->numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d", &v, &t) == 2)
			{
				// v/t
                fscanf(file, "%d/%d", &v, &t);
                fscanf(file, "%d/%d", &v, &t);
                numtriangles++;
                group->numtriangles++;
                while(fscanf(file, "%d/%d", &v, &t) > 0)
				{
					numtriangles++;
                    group->numtriangles++;
				}
			}
			else
			{
				// v
                fscanf(file, "%d", &v);
                fscanf(file, "%d", &v);
                numtriangles++;
                group->numtriangles++;
                while(fscanf(file, "%d", &v) > 0)
				{
					numtriangles++;
					group->numtriangles++;
				}
			}
            break;
        default:
			// Simply get the rest of line
            fgets(buf, sizeof(buf), file);
            break;
        }
	 }

    // Set the statistics in the model structure
    model->numvertices  = numvertices;
    model->numvertexnorms   = numnormals;
    model->numtexcoords = numtexcoords;
    model->numtriangles = numtriangles;

    // Allocate memory for the triangles in each group
    group = model->groups;
    while(group)
	{
		group->triangles = (GLuint*)malloc(sizeof(GLuint) * group->numtriangles);
        group->numtriangles = 0;
		group = group->next;
	}

    // Allocate the edgeList
    edgeList.clear();
    for(unsigned int i = 0; i < model->numvertices; i++)
    	edgeList.push_back(vector<int>());
}

// objmSecondPass: Second pass through a Wavefront .obj file that gets all the data
GLvoid loadObj::objmSecondPass(OBJMmodel* model, FILE* file)
{
	GLuint numvertices;
	GLuint numnormals;
	GLuint numtexcoords;
	GLuint numtriangles;

	GLfloat* vertices;		  // Array of vertices
	GLfloat* normals;		  // Array of normals
	GLfloat* texcoords;		  // Array of texture coords
	OBJMgroup* group;		  // Current group ptr

	GLuint material;		  // Current material
	GLuint v,n,t;
	char buf[128];

	// Set the pointer shortcuts
	vertices = model->vertices;
	normals = model->vertexnorms;
	texcoords = model->texcoords;
	group = model->groups;

	// Read the data into the allocated arrays
	numvertices = numnormals = numtexcoords = 1;
	numtriangles = material = 0;

	while(fscanf(file, "%s", buf) != EOF)
	{
        switch(buf[0])
		{
        case '#':             // comment
            fgets(buf, sizeof(buf), file);
            break;
        case 'v':             // v, vn, vt
            switch(buf[1])
			{
            case '\0':          // vertex
                fscanf(file, "%f %f %f",
                    &vertices[3 * numvertices + 0],
                    &vertices[3 * numvertices + 1],
                    &vertices[3 * numvertices + 2]);
                numvertices++;
                break;
            case 'n':           // normal
                fscanf(file, "%f %f %f",
                    &normals[3 * numnormals + 0],
                    &normals[3 * numnormals + 1],
                    &normals[3 * numnormals + 2]);
                numnormals++;
                break;
            case 't':           // texcoord
                fscanf(file, "%f %f",
                    &texcoords[2 * numtexcoords + 0],
                    &texcoords[2 * numtexcoords + 1]);
                numtexcoords++;
                break;
            }
            break;
        case 'u':
            fgets(buf, sizeof(buf), file);
            sscanf(buf, "%s %s", buf, buf);
            group->material = material = objmFindMaterial(model, buf);
            break;
        case 'g':               // group
			fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
                sscanf(buf, "%s", buf);
#else
                buf[strlen(buf)-1] = '\0';
#endif
            group = objmFindGroup(model, buf);
            group->material = material;
            break;
        case 'f':               // face
            v = n = t = 0;
            fscanf(file, "%s", buf);

            // can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d
            if (strstr(buf, "//"))
			{
				// v//n
                sscanf(buf, "%d//%d", &v, &n);
                T(numtriangles).vindices[0] = v;
                T(numtriangles).nindices[0] = n;
                fscanf(file, "%d//%d", &v, &n);
                T(numtriangles).vindices[1] = v;
                T(numtriangles).nindices[1] = n;
                fscanf(file, "%d//%d", &v, &n);
                T(numtriangles).vindices[2] = v;
                T(numtriangles).nindices[2] = n;
                group->triangles[group->numtriangles++] = numtriangles;
                this->buildEdgeList(model, numtriangles);
                numtriangles++;
                while(fscanf(file, "%d//%d", &v, &n) > 0)
				{
					T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                    T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
                    T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                    T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
                    T(numtriangles).vindices[2] = v;
                    T(numtriangles).nindices[2] = n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    this->buildEdgeList(model, numtriangles);
                    numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
			{
				// v/t/n
                T(numtriangles).vindices[0] = v;
                T(numtriangles).tindices[0] = t;
                T(numtriangles).nindices[0] = n;
                fscanf(file, "%d/%d/%d", &v, &t, &n);
                T(numtriangles).vindices[1] = v;
                T(numtriangles).tindices[1] = t;
                T(numtriangles).nindices[1] = n;
                fscanf(file, "%d/%d/%d", &v, &t, &n);
                T(numtriangles).vindices[2] = v;
                T(numtriangles).tindices[2] = t;
                T(numtriangles).nindices[2] = n;
                group->triangles[group->numtriangles++] = numtriangles;
                this->buildEdgeList(model, numtriangles);
                numtriangles++;
                while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0)
				{
					T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                    T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
                    T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
                    T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                    T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
                    T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
                    T(numtriangles).vindices[2] = v;
                    T(numtriangles).tindices[2] = t;
                    T(numtriangles).nindices[2] = n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    this->buildEdgeList(model, numtriangles);
                    numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d", &v, &t) == 2)
			{
				// v/t
                T(numtriangles).vindices[0] = v;
                T(numtriangles).tindices[0] = t;
                fscanf(file, "%d/%d", &v, &t);
                T(numtriangles).vindices[1] = v;
                T(numtriangles).tindices[1] = t;
                fscanf(file, "%d/%d", &v, &t);
                T(numtriangles).vindices[2] = v;
                T(numtriangles).tindices[2] = t;
                group->triangles[group->numtriangles++] = numtriangles;
                this->buildEdgeList(model, numtriangles);
                numtriangles++;
                while(fscanf(file, "%d/%d", &v, &t) > 0)
				{
					T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                    T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
                    T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                    T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
                    T(numtriangles).vindices[2] = v;
                    T(numtriangles).tindices[2] = t;
                    group->triangles[group->numtriangles++] = numtriangles;
                    this->buildEdgeList(model, numtriangles);
                    numtriangles++;
				}
			}
			else
			{
				// v
                sscanf(buf, "%d", &v);
                T(numtriangles).vindices[0] = v;
                fscanf(file, "%d", &v);
                T(numtriangles).vindices[1] = v;
                fscanf(file, "%d", &v);
                T(numtriangles).vindices[2] = v;
                group->triangles[group->numtriangles++] = numtriangles;
                this->buildEdgeList(model, numtriangles);
                numtriangles++;
                while(fscanf(file, "%d", &v) > 0)
				{
					T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                    T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                    T(numtriangles).vindices[2] = v;
                    group->triangles[group->numtriangles++] = numtriangles;
                    this->buildEdgeList(model, numtriangles);
                    numtriangles++;
				}
			}
			break;
        default:
			fgets(buf, sizeof(buf), file);
            break;
        }
    }

	for (unsigned int i = model->numvertices - 10; i < model->numvertices; i++)
	{
		if (this->edgeList[i].size() > 0)
		{
			//qDebug()<<"ENTER "<<i;
			printf("%d -> ", i);
			for (unsigned int j = 0; j < this->edgeList[i].size(); j++)
				printf("%d ",this->edgeList[i].at(j));

			printf("\n");
		}
	}

#if 0
  // Announce the memory requirements
  printf(" Memory: %d bytes\n",
      numvertices  * 3*sizeof(GLfloat) +
      numnormals   * 3*sizeof(GLfloat) * (numnormals ? 1 : 0) +
      numtexcoords * 3*sizeof(GLfloat) * (numtexcoords ? 1 : 0) +
      numtriangles * sizeof(OBJMtriangle));
#endif
}

// Builds the edge list for the model
void loadObj::buildEdgeList(OBJMmodel* model, int numtriangles)
{
	int v1 = T(numtriangles).vindices[0];
	int v2 = T(numtriangles).vindices[1];
	int v3 = T(numtriangles).vindices[2];
	unsigned int i;

	// v1, neighb v2
	for (i = 0; i < edgeList[v1 - 1].size(); i++)
	{
		if (this->edgeList[v1 - 1].size() == 0)
			break;

		if (this->edgeList[v1 - 1].at(i) == (v2))
			break;
	}
	if (i == this->edgeList[v1 - 1].size())
		edgeList[v1 - 1].push_back(v2);

	// v1, neighb v3
	for (i = 0; i < edgeList[v1 - 1].size(); i++)
	{
		if (this->edgeList[v1 - 1].size() == 0)
			break;

		if (this->edgeList[v1 - 1].at(i) == (v3))
			break;
	}
	if (i == this->edgeList[v1 - 1].size())
		edgeList[v1 - 1].push_back(v3);

	// v2, neighb v1
	for (i = 0; i < edgeList[v2 - 1].size(); i++)
	{
		if (this->edgeList[v2 - 1].size() == 0)
			break;

		if (this->edgeList[v2 - 1].at(i) == (v1))
			break;
	}
	if (i == this->edgeList[v2 - 1].size())
		edgeList[v2 - 1].push_back(v1);

	// v2, neighb v3
	for (i = 0; i < edgeList[v2 - 1].size(); i++)
	{
		if (this->edgeList[v2 - 1].size() == 0)
			break;

		if (this->edgeList[v2 - 1].at(i) == (v3))
			break;
	}
	if (i == this->edgeList[v2 - 1].size())
		edgeList[v2 - 1].push_back(v3);

	// v3, neighb v1
	for (i = 0; i < edgeList[v3 - 1].size(); i++)
	{
		if (this->edgeList[v3 - 1].size() == 0)
			break;

		if (this->edgeList[v3 - 1].at(i) == (v1))
			break;
	}
	if (i == this->edgeList[v3 - 1].size())
		edgeList[v3 - 1].push_back(v1);

	// v3, neighb v2
	for (i = 0; i < edgeList[v3 - 1].size(); i++)
	{
		if (this->edgeList[v3 - 1].size() == 0)
			break;

		if (this->edgeList[v3 - 1].at(i) == (v2))
			break;
	}
	if (i == this->edgeList[v3 - 1].size())
		edgeList[v3 - 1].push_back(v2);
}

float loadObj::normalize(float* v)
{
	float length;

	length = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] /= length;
	v[1] /= length;
	v[2] /= length;

	return length;
}

GLboolean loadObj::invert(GLdouble src[16], GLdouble inverse[16])
{
		double t;
	    int i, j, k, swap;
	    GLdouble tmp[4][4];

	    identity(inverse);

	    for (i = 0; i < 4; i++) {
	        for (j = 0; j < 4; j++) {
	            tmp[i][j] = src[i*4+j];
	        }
	    }

	    for (i = 0; i < 4; i++) {
	        /* look for largest element in column. */
	        swap = i;
	        for (j = i + 1; j < 4; j++) {
	            if (fabs(tmp[j][i]) > fabs(tmp[i][i])) {
	                swap = j;
	            }
	        }

	        if (swap != i) {
	            /* swap rows. */
	            for (k = 0; k < 4; k++) {
	                t = tmp[i][k];
	                tmp[i][k] = tmp[swap][k];
	                tmp[swap][k] = t;

	                t = inverse[i*4+k];
	                inverse[i*4+k] = inverse[swap*4+k];
	                inverse[swap*4+k] = t;
	            }
	        }

	        if (tmp[i][i] == 0) {
	        /* no non-zero pivot.  the matrix is singular, which
	        shouldn't happen.  This means the user gave us a bad
	            matrix. */
	            return GL_FALSE;
	        }

	        t = tmp[i][i];
	        for (k = 0; k < 4; k++) {
	            tmp[i][k] /= t;
	            inverse[i*4+k] /= t;
	        }
	        for (j = 0; j < 4; j++) {
	            if (j != i) {
	                t = tmp[j][i];
	                for (k = 0; k < 4; k++) {
	                    tmp[j][k] -= tmp[i][k]*t;
	                    inverse[j*4+k] -= inverse[i*4+k]*t;
	                }
	            }
	        }
	    }
	    return GL_TRUE;
}

void loadObj::identity(GLdouble m[16])
{
	m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
	m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
	m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
	m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}
