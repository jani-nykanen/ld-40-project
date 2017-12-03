/// Mesh (source)
/// (c) 2017 Jani Nykänen

#include "mesh.h"

#include "graphics.h"

#include "stdio.h"

#define ALLOCATE(arr,type,size) arr = (type*)malloc(sizeof(type) *size); if(arr==NULL){printf("Memory allocation error!\n");return NULL;}

#define COPY_ARR(arr1,arr2,size) for(i=0; i < size; i++) arr1[i] = arr2[i];

/// Create a new mesh
MESH* create_mesh(float* vertices, float* normals, int* vindices, int* nindices, Uint8* colors, int vl, int nl, int il)
{
    MESH* m;

    // Allocate memory
    ALLOCATE(m, MESH, 1);
    ALLOCATE(m->vertices,VEC3,vl/3);
    ALLOCATE(m->normals,VEC3,il/3);
    ALLOCATE(m->indices,int,il);
    ALLOCATE(m->colors,Uint8,il/3);

    int i = 0;
    // Copy data
    for(i = 0; i < vl/3; i ++)
    {
        m->vertices[i] = vec3(vertices[i*3],vertices[i*3+1],vertices[i*3+2]);
    }
    COPY_ARR(m->indices,vindices,il);

    if(colors != NULL)
    {
        COPY_ARR(m->colors,colors,il/3);
    }
    else
    {
        for(i=0; i < il/3; i++)
        {
            m->colors[i] = 0b00110101;
        }
    }

    // Copy size values
    m->indexCount = il;
    m->vertexCount = vl/3;
    m->normalCount = il/3;
    m->colorCount = il/3;

    // Compute normals if NULL
    if(normals == NULL)
    {
        VEC3 a,b,c;
        VEC3 n;

        int nindex = 0;
        for(i = 0; i < m->indexCount; i += 3)
        {
            a = (m->vertices[m->indices[i]]);
            b = (m->vertices[m->indices[i+1]]);
            c = (m->vertices[m->indices[i+2]]);

            n = normalize(cross(dec_vec3(b,a),dec_vec3(c,a)));

            m->normals[nindex] = n;
            nindex ++;
        }
    }
    else
    {
        for(i = 0; i < m->normalCount; i ++)
        {
            m->normals[i] = vec3(
                normals[ nindices[i*3]*3    ],
                normals[ nindices[i*3]*3 +1 ],
                normals[ nindices[i*3]*3 +2 ]
            );
        }
    }

    return m;
}

/// Load a mesh
MESH* load_mesh(const char* path)
{
    MESH* m = NULL;

    unsigned int vcount = 0;
    unsigned int icount = 0;
    unsigned int ncount = 0;
    unsigned int uvcount = 0;

    // Open file
    FILE* f = fopen(path,"rb");
    if(f == NULL)
    {
        printf("Could not load a file in %s!\n",path);
        return NULL;
    }

    // Read data

    // Read the magic byte (will be ignored)
    Uint8 b;
    fread(&b,sizeof(Uint8),1,f);

    // Read lengths
    fread(&vcount,sizeof(int),1,f);
    fread(&icount,sizeof(int),1,f);
    fread(&ncount,sizeof(int),1,f);
    fread(&uvcount,sizeof(int),1,f);

    // Allocate memory for integer data
    int* ALLOCATE(iVertices,int,vcount);
    int* ALLOCATE(indices,int,icount);
    int* ALLOCATE(iNormals,int,ncount);
    int* ALLOCATE(iUVs,int,uvcount);

    // Read integer data
    fread(iVertices,sizeof(int),vcount,f);
    fread(indices,sizeof(int),icount,f);
    fread(iNormals,sizeof(int),ncount,f);
    fread(iUVs,sizeof(int),uvcount,f);

    // Allocate memory for actual model data
    float* ALLOCATE(vertices,float,vcount);
    int* ALLOCATE(vindices,int,icount/3);

    float* ALLOCATE(normals,float,ncount);
    int* ALLOCATE(nindices,int,icount/3);

    // Make int be float again
    int i = 0;

    for(i=0; i < vcount; i ++)
    {
        vertices[i] = ( (float) iVertices[i] ) / 1000.0f;
    }
    for(i=0; i < icount / 3; i++)
    {
        vindices[i] = indices[i*3] -1;
        nindices[i] = indices[i*3 +2] -1;
    }
    for(i=0; i < ncount; i++)
    {
        normals[i] = ( (float)iNormals[i] ) / 1000.0f;
        
    }

    m = create_mesh(vertices,normals, vindices, nindices, NULL,vcount,ncount, icount/3);

    free(iVertices);
    free(iNormals);
    free(iUVs);
    free(indices);

    return m;
}

/// Draw mesh
/// < m Mesh to draw
void draw_mesh(MESH* m)
{

}