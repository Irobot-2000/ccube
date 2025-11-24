#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "vectorMath.c"


#define NUM_VERTICES_IN_QUAD 4

typedef  struct Quadrilateral {
    Point vertices[NUM_VERTICES_IN_QUAD];
} Quadrilateral;

typedef  struct ProjectedQuad {
    Point vertices[NUM_VERTICES_IN_QUAD];
    Point planeCoefficents;
    float planeConstant;
} ProjectedQuad;

typedef enum Color {
    BLACK = 0,
    WHITE = 1,
} Color;
typedef struct Pixel {
    float distanceFromSource;
    Color color;
} Pixel;

typedef struct Screen{
    Point focal;
    Point center; //The point on the plane closest to the focal
    Point xUnitDirection;
    Point yUnitDirection; 
    Point planeCoefficents;
    float planeConstant;
    size_t width;
    size_t height;
} Screen;

void printQuadrilateral(ProjectedQuad * q);

/** 
Create new screen
center coresponds to the point closest to the focal on the plane.
*/ 
Screen newScreen(size_t width, size_t height, Point center, Point focal, Point xDirection, Point yDirection ) 
{
    Screen screen = {.width=width,.height=height,.center=center, .focal=focal, .xUnitDirection=unitVector(xDirection), .yUnitDirection=unitVector(yDirection),  .planeCoefficents = {0}, .planeConstant=0};
    //finding the equation of the plane passing through the origin and center, perpendicular to the line passing through the center and the focal point;
    Point perpendicularDirection = vectorSub(focal, center);
    //the constant is the dot product of the perendicular direction and a point on the plane
    screen.planeConstant = dotProduct(center,perpendicularDirection);
    //the plane coefficents are equal to the direction vector of the perpendicular
    screen.planeCoefficents = perpendicularDirection;
    return screen;
}
/** 
    Project quadrilateral onto 2D screen
    This function modifies the contents of quadrilateral
*/
ProjectedQuad projectQuadrilaterel(Quadrilateral  quad, Screen screen) {
    ProjectedQuad projection;
    for (size_t i = 0; i < NUM_VERTICES_IN_QUAD; i++ ) {
        Point rayDirection = vectorSub(screen.focal, quad.vertices[i]);
        // screen.focal + t * rayDirection is the ray line.
        //finding t at point of intersection
        // screen.planeConstant == screen.planeCoefficents . (screen.focal + t * rayDirection)
        // => screen.planeConstant == screen.planeCoefficents . (screen.focal) + screen.planeCoefficents . (rayDirection) * t
        float t = (screen.planeConstant - dotProduct(screen.planeCoefficents, screen.focal)) / dotProduct(screen.planeCoefficents, rayDirection);
        if (t == INFINITY || t == -INFINITY) {
            fputs("Error, focal point colliding with object, adjust focal point\n",stderr);
            exit(1);
        }
        Point pointOnPlane = vectorAdd(screen.focal, vectorScale(t, rayDirection));
        float x2dCoord = dotProduct(vectorSub(pointOnPlane, screen.center), screen.xUnitDirection);
        //flip x coord to account for the flip during projection
        x2dCoord = screen.width / 2 - x2dCoord;

        float y2dCoord = dotProduct(vectorSub(pointOnPlane, screen.center), screen.yUnitDirection);
        //flip y coord to account for the flip during projection
        y2dCoord = screen.height / 2 - y2dCoord;

        projection.vertices[i].z = seperation(pointOnPlane, quad.vertices[i]);
        projection.vertices[i].x = x2dCoord;
        projection.vertices[i].y = y2dCoord;
    }
    //calculate the equation of the plane of the quadrilateral, such that dotProduct(r,planeCoeff) == planeConst when r is a point on the plane
    Point planeCoeff = crossProduct( vectorSub(projection.vertices[0], projection.vertices[1]) , vectorSub(projection.vertices[0], projection.vertices[2]) );
    float planeConst = dotProduct(planeCoeff, projection.vertices[0]);
    projection.planeCoefficents = planeCoeff;
    projection.planeConstant = planeConst;
    return projection;
}

void printQuadrilateral(ProjectedQuad * q){
    for (size_t i = 0; i< NUM_VERTICES_IN_QUAD; i++) {
        if (i != 0) {
            printf(",");
        }
        printf("(%f, %f, %f) ", q->vertices[i].x, q->vertices[i].y, q->vertices[i].z);
    }
    printf(", ");
}
/**
    Returns a Pixel struct such that
    distanceFromSource = INFINITY if the point is outside the quadrilateral / distanceFromSource otherwise
    color = BLACK if near the outline, white otherwise 
*/
Pixel rayPixelFromProjectedFace(float x, float y, ProjectedQuad * face, float lineOutline) {
    int numIntersections = 0;
    Pixel pixel  = {.color=WHITE, .distanceFromSource=INFINITY};
    //printQuadrilateral(face);
    for (int i = 0; i < 4; i++) {
        //construct lines between each of the four vertices
        Point startVertex = face->vertices[i];
        // %4 wraps around to 0
        Point endVertex = face->vertices[(i + 1) % 4];
        Point lineCoefficients = vectorSub(endVertex, startVertex);

        //draw a horisontal line from  (x, y) to (inf, y) and check for intersections;
        // (x,y) = startVertex + t.lineCoefficients
        
        float t = (y - startVertex.y ) / lineCoefficients.y;
        float xIntersection = startVertex.x + t * lineCoefficients.x;
        //printf("%f,%f | %f,%f %d %d\n", startVertex.x, startVertex.y,endVertex.x, endVertex.y, (i + 1) % 4, i);
        //if tntersectio takes place
        //printf("%d ,%f \n", t , startVertex.x);
        //and intersection is to the right
        if (t >= 0 && t <= 1 && xIntersection > x) {
                numIntersections++;
        }
        else
        //if not already dertermined to be part of the outline
        if (pixel.color != BLACK) {
            //check also if the point is close to a line, ie part of the outline
            float t2 = (x - startVertex.x ) / lineCoefficients.x;
            float yIntersection = startVertex.y + t2 * lineCoefficients.y;
            //printf("x: %f,y %f\n",xIntersection,yIntersection);
            float distanceToLine;
            //handle division by zero
            if (t == -INFINITY || t == INFINITY) {
                distanceToLine = yIntersection - y;
            }
            else if (t2 == -INFINITY || t2 == INFINITY) {
                distanceToLine = xIntersection - x;
            }
            else{
            float distanceBetweenIntersections = distance2d(xIntersection, yIntersection);
            distanceToLine = (xIntersection - x) * (yIntersection - y) / distanceBetweenIntersections;
            }
            if (fabs(distanceToLine) < lineOutline) {
                pixel.color = BLACK;
            }
            //printf(">>>>%f\n", distanceToLine);
        }
    }
    //even number of intersections => outside quad
    if (numIntersections % 2 == 0) {
        pixel.color = BLACK;
        pixel.distanceFromSource = INFINITY;
        return pixel;
    }

    //calculate distance from source
    //find plane equation

    pixel.distanceFromSource = (face->planeConstant - x * face->planeCoefficents.x - y  * face->planeCoefficents.y ) / face->planeCoefficents.z;
    if (face->planeCoefficents.z == 0) {
        pixel.distanceFromSource = 0;
    }
    return pixel;
}