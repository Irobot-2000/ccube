#include <stdio.h>
#include <wchar.h>
#include <locale.h>

#include "timing.c"
#include "3d.c"
#define NUM_FACES_IN_CUBE 6
Quadrilateral  const unitCube[] = {
    { { {-0.5, -0.5, -0.5}, {-0.5, 0.5, -0.5}, {0.5, 0.5, -0.5}, {0.5, -0.5, -0.5} } },
    {{ {-0.5, -0.5, 0.5}, {-0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, {0.5, -0.5, 0.5} } },
    {{ {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}, {-0.5, -0.5, 0.5} } },
    {{ {-0.5, 0.5, -0.5}, {0.5, 0.5, -0.5}, {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} } },
    {{ {-0.5, -0.5, -0.5}, {-0.5, 0.5, -0.5}, {-0.5, 0.5, 0.5}, {-0.5, -0.5, 0.5} } },
    {{ {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {0.5, 0.5, 0.5}, {0.5, -0.5, 0.5} } },
};


void printCharacter(ProjectedQuad projectedMesh[], size_t numfaces, size_t row, size_t col, float xScale, float yScale);

int main(void) {
    Point const focalPoint = newPoint(0, 0, 4);
    Point const screenCenter = newPoint(0, 0, 16);
    Point const screenXDirection = newPoint(1, 0, 0);
    Point const screenYDirection = newPoint(0, 1, 0);
    Point const rotationCenter = newPoint(0, 0, 0);
    float const screenWidth = 10;
    float const screenHeight = 7;
    size_t const numCharRows = 30;
    size_t const numCharCols = 120;
    float const xScale = 0.09;
    float const yScale = 0.2;

    float xRotationFreqency = 0.055;
    float yRotationFreqency = 0.07;
    float zRotationFreqency = 0.04;
    //allow printing unicode characters
    setlocale(LC_ALL, "en_US.UTF-8");
    ProjectedQuad projectedCube[6];
    Screen screen = newScreen(
                        screenWidth,
                        screenHeight,
                        screenCenter,
                        focalPoint,
                        screenXDirection,
                        screenYDirection
                    );
    for(size_t i = 0; i < NUM_FACES_IN_CUBE; i++) {
        Quadrilateral face = unitCube[i];
        float angleX = (2 * 3.142  * xRotationFreqency / 1000) * getFrame(1000, 1000/xRotationFreqency) ;
        float angleY = (2 * 3.142  * yRotationFreqency / 1000) * getFrame(1000, 1000/yRotationFreqency) ;
        float angleZ = (2 * 3.142 * zRotationFreqency / 1000) * getFrame(1000, 1000/zRotationFreqency) ;
        for(size_t i = 0; i < NUM_VERTICES_IN_QUAD; i++) {
            face.vertices[i] = rotateX(face.vertices[i], rotationCenter, angleX);
            face.vertices[i] = rotateY(face.vertices[i], rotationCenter, angleY);
            face.vertices[i] = rotateZ(face.vertices[i], rotationCenter, angleZ);
        }

        projectedCube[i] = projectQuadrilaterel(face, screen);;
        //printQuadrilateral(&face);
    }
    printf("\n");
/*
    while(0) {
        char input[21] = {0};
        char * end;
        printf("Enter x value");
        fgets(input, 20, stdin);
        if(input[0] == '\0') {
            break;
        }
        float x = strtof(input, &end);
        printf("Enter y value");
        fgets(input, 20, stdin);
        float y = strtof(input, &end);
        Pixel p = rayPixelFromProjectedFace(x, y, &(projectedCube[0]), 0);
        printf("%f %d \n", p.distanceFromSource, p.color);
    }

    */

    for(size_t row = 0; row < numCharRows; row++) {
        for(size_t col = 0; col < numCharCols; col ++) {
            printCharacter(projectedCube, NUM_FACES_IN_CUBE, row, col, xScale, yScale);
        }
        printf("\n");
    }
}


void printCharacter(ProjectedQuad projectedMesh[], size_t numfaces, size_t row, size_t col, float xScale, float yScale) {
    #define NUM_SUBROWS  4
    #define NUM_SUBCOLS 2
    int subPixels[NUM_SUBROWS][NUM_SUBCOLS];
    for(int subRow = 0; subRow < NUM_SUBROWS; subRow++) {
        for(int subCol = 0; subCol < NUM_SUBCOLS; subCol++) {
            float x = col * xScale + subCol * xScale / NUM_SUBCOLS;
            float y = row * yScale + subRow * yScale / NUM_SUBROWS;
            //Pixel p = rayPixelFromProjectedFace(x, y, &(projectedCube[0]), 3);

            Pixel p = {.distanceFromSource = INFINITY, .color = BLACK};
            for(size_t i = 0; i < numfaces; i++) {
                ProjectedQuad face = projectedMesh[i];
                Pixel newP = rayPixelFromProjectedFace(x, y, &face, 0.002);
                if(newP.distanceFromSource < p.distanceFromSource) {
                    p = newP;
                }
            }
            if(p.color == WHITE) {
                subPixels[subRow][subCol] = 1;
            } else {
                subPixels[subRow][subCol] = 0;
            } 
            
        }
    }

            //convert the subPixels to the corresponding braille charcode
        wchar_t charCode = L'⠀' +
        subPixels[0][0] * 1 +
        subPixels[1][0] * 2 +
        subPixels[2][0] * 4 +
        subPixels[0][1] * 8 +
        subPixels[1][1] * 16 +
        subPixels[2][1] * 32 +
        subPixels[3][0] * 64 +
        subPixels[3][1] * 128;

        printf("%lc", charCode);


}