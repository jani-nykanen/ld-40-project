/// Vectors (header)
/// (c) 2017 Jani Nykänen

#ifndef __VECTOR__
#define __VECTOR__

/// Vector 3 type
typedef struct
{
    float x,y,z;
}
VEC3;

/// Create new vector 3
/// < x X component
/// < y Y component
/// < z Z component
/// > A new vector 3
#define vec3(x,y,z) (VEC3){x,y,z}

/// Vector 2 type
typedef struct
{
    float x,y;
}
VEC2;

/// Point
typedef struct
{
    int x,y;
}
POINT;

/// Create new vector 2
/// < x X component
/// < y Y component
/// > A new vector 2
#define vec2(x,y) (VEC2){x,y}

/// Cross product
/// < A Vector 1
/// < B Vector 2
/// > A new vector
VEC3 cross(VEC3 A, VEC3 B);

/// Addition operator
/// < A Vector 1
/// < B Vector 2
/// > A new vector
VEC3 add_vec3(VEC3 A, VEC3 B);

/// Decrease operator
/// < A Vector 1
/// < B Vector 2
/// > A new vector
VEC3 dec_vec3(VEC3 A, VEC3 B);

/// Normalize
/// < A vector to normalize
/// > Normalized vector
VEC3 normalize(VEC3 A);


#endif // __VECTOR__