/// Vectors (source)
/// (c) 2017 Jani Nykänen

#include "vector.h"

#include "math.h"

/// Cross product
VEC3 cross(VEC3 A, VEC3 B)
{
    return vec3(
        A.y*B.z - A.z*B.y,
        -(A.x*B.z-A.z*B.x),
        A.x*B.y - A.y*B.x
    );
}

/// Addition operator
VEC3 add_vec3(VEC3 A, VEC3 B)
{
    return vec3(A.x+B.x,A.y+B.y,A.z+B.z);
}

/// Decrease operator
VEC3 dec_vec3(VEC3 A, VEC3 B)
{
    return vec3(A.x-B.x,A.y-B.y,A.z-B.z);
}

/// Normalize
VEC3 normalize(VEC3 A)
{
    float l = sqrt(A.x*A.x + A.y*A.y + A.z*A.z);
    return vec3(A.x/l,A.y/l,A.z/l);
}