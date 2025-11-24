#include <math.h>
typedef struct Point {
    float x;
    float y;
    float z;
} Point;
typedef struct MatrixRow {
    float col0;
    float col1;
    float col2;
} MatrixRow;
typedef struct Matrix{
    MatrixRow row0;
    MatrixRow row1;
    MatrixRow row2;
} Matrix;

Point newPoint(float x, float y, float z) {
    Point p = {.x=x,.y=y,.z=z};
    return p;
}
Point vectorSub(Point p1, Point p2) {
    Point result = {
        .x = p1.x - p2.x, 
        .y = p1.y - p2.y, 
        .z = p1.z - p2.z
    };
    return result;
}

Point vectorAdd(Point p1, Point p2) {
    Point result = {
        .x = p1.x + p2.x, 
        .y = p1.y + p2.y, 
        .z = p1.z + p2.z
    };
    return result;
}

Point vectorScale(float scale, Point p) {
        Point result = {
        .x = p.x * scale, 
        .y = p.y * scale, 
        .z = p.z * scale
    };
    return result;
}

Point unitVector(Point p) {
    float magnitude = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    return vectorScale(1/magnitude, p);
}
/**Returns the distance of seperation of two points */
float seperation(Point p1, Point p2) {
    float deltaX = (p1.x - p2.x);
    float deltaY = (p1.y - p2.y);
    float deltaZ = (p1.z - p2.z);
    return sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
}
float dotProduct(Point p1, Point p2) {
    return  (p1.x * p2.x) + (p1.y * p2.y) + (p1.z * p2.z) ;
}

Point crossProduct(Point p1, Point p2) {
    Point res = {
        .x= p1.y * p2.z - p1.z * p2.y,
        .y= p1.z * p2.x - p1.x * p2.z,
        .z= p1.x * p2.y - p1.y * p2.x,
    };
    return res;
}

float distance2d(float x, float y){
    return sqrt(x * x + y * y);
}

Point transformMatrix(Matrix m, Point p) {
    Point result = {
        .x=m.row0.col0 * p.x  + m.row0.col1 * p.y + m.row0.col2 * p.z,
        .y=m.row1.col0 * p.x  + m.row1.col1 * p.y + m.row1.col2 * p.z,
        .z=m.row2.col0 * p.x  + m.row2.col1 * p.y + m.row2.col2 * p.z,
    };
    return result;
}

Point rotateX(Point point, Point pivot, float theta) {
    Matrix transform = {
        { 1,          0,           0},
        { 0, cos(theta), -sin(theta)},
        { 0, sin(theta),  cos(theta)}
    };
    Point translated = vectorSub(point, pivot);
    Point rotated = transformMatrix(transform,translated);
    return vectorAdd(rotated,pivot);
}

Point rotateY(Point point, Point pivot, float theta) {
    Matrix transform = {
        {  cos(theta), 0, sin(theta)},
        {           0, 1,          0},
        { -sin(theta), 0, cos(theta)}
    };
    Point translated = vectorSub(point, pivot);
    Point rotated = transformMatrix(transform,translated);
    return vectorAdd(rotated,pivot);
}

Point rotateZ(Point point, Point pivot, float theta) {
    Matrix transform = {
        { cos(theta), -sin(theta), 0},
        {sin(theta),  cos(theta), 0},
        {         0,            0, 1}
    };
    Point translated = vectorSub(point, pivot);
    Point rotated = transformMatrix(transform,translated);
    return vectorAdd(rotated,pivot);
}