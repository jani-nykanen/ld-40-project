/// Mesh (header)
/// (c) 2017 Jani Nykänen

#ifndef __MESH__
#define __MESH__

#include "SDL2/SDL.h"

#include "vector.h"

/// Mesh type
typedef struct
{
    VEC3* vertices; /// An array of vertices
    VEC3* normals; /// An array of normals
    int* indices; /// An array of indices
    Uint8* colors; /// An array of colors

    int vertexCount; /// Amount of vertices
    int normalCount; /// Amount of normals
    int indexCount; /// Amount of indices
    int colorCount; /// Amount of colors

}MESH;

/// Create a new mesh
/// < vertices Vertices (note: float array, not VEC3)
/// < normals Normals (can be NULL)
/// < vindices Vertex indices
/// < nindices Normal indices
/// < colors Colors
/// < vl Vertex array size
/// < nl Normal array size
/// < il Index array size
/// > A new mesh
MESH* create_mesh(float* vertices, float* normals, int* vindices, int* nindices, Uint8* colors, int vl, int nl, int il);

/// Load a mesh
/// < path Mesh path
/// > A new mesh
MESH* load_mesh(const char* path);

/// Draw mesh
/// < m Mesh to draw
void draw_mesh(MESH* m);

#endif // __MESH__