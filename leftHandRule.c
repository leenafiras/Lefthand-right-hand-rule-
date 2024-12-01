#include <stdio.h>
#include <stdlib.h>
#include "API.h"

// Enumeration for movement directions
enum Direction { NORTH, EAST, SOUTH, WEST };

// Current direction the mouse is facing
int currentDirection = NORTH;

// Current position of the mouse
int mouseX = 0, mouseY = 0;

// Dimensions of the maze
int mazeWidth = 0, mazeHeight = 0;

// Logs the mouse's current position for debugging
void logMousePosition() {
    fprintf(stderr, "Mouse Position: (%d, %d)\n", mouseX, mouseY);
    fflush(stderr);
}

// Updates the mouse's position based on its current direction
void updateMousePosition() {
    if (currentDirection == NORTH) mouseY++;
    else if (currentDirection == EAST) mouseX++;
    else if (currentDirection == SOUTH) mouseY--;
    else if (currentDirection == WEST) mouseX--;

    API_setText(mouseX, mouseY, "X"); // Mark position on the maze
    API_setColor(mouseX, mouseY, 'G'); // Highlight the position
    logMousePosition();
}

// Turns the mouse 90 degrees to the right
void turnMouseRight() {
    API_turnRight();
    currentDirection = (currentDirection + 1) % 4; // Update direction
}

// Turns the mouse 90 degrees to the left
void turnMouseLeft() {
    API_turnLeft();
    currentDirection = (currentDirection + 3) % 4; // Wrap around to handle negative values
}

// Moves the mouse forward if no wall is in front
int moveMouseForward() {
    if (API_moveForward()) { // Move if possible
        updateMousePosition();
        return 1;
    }
    return 0; // Can't move forward
}

// Marks walls around the mouse's current position
void markSurroundingWalls() {
    static const char wallLabels[] = {'n', 'e', 's', 'w'}; // Labels for walls

    if (API_wallFront()) { // Mark front wall
        API_setWall(mouseX, mouseY, wallLabels[currentDirection]);
    }
    if (API_wallRight()) { // Mark right wall
        API_setWall(mouseX, mouseY, wallLabels[(currentDirection + 1) % 4]);
    }
    if (API_wallLeft()) { // Mark left wall
        API_setWall(mouseX, mouseY, wallLabels[(currentDirection + 3) % 4]);
    }
}

// Calculates the Manhattan distance to the center of the maze
int calculateHeuristic() {
    int centerX1 = mazeWidth / 2, centerY1 = mazeHeight / 2; // Center coordinates (even-sized maze)
    int centerX2 = (mazeWidth - 1) / 2, centerY2 = (mazeHeight - 1) / 2; // Center coordinates (odd-sized maze)

    int distanceToCenter1 = abs(mouseX - centerX1) + abs(mouseY - centerY1);
    int distanceToCenter2 = abs(mouseX - centerX2) + abs(mouseY - centerY2);

    return (distanceToCenter1 < distanceToCenter2) ? distanceToCenter1 : distanceToCenter2;
}

// Main function to solve the maze
void solveMaze() {
    while (1) {
        // Stop if the mouse reaches the center of the maze
        if (calculateHeuristic() == 0) {
            API_setText(mouseX, mouseY, "C"); // Mark center
            API_setColor(mouseX, mouseY, 'R'); // Highlight center
            fprintf(stderr, "Mouse reached the center at: (%d, %d)\n", mouseX, mouseY);
            break;
        }

        // Analyze and mark walls
        markSurroundingWalls();

        // Move based on wall detection using the left-hand rule
        if (!API_wallLeft()) { // Prefer moving left if no wall
            turnMouseLeft();
            moveMouseForward();
        } else if (!API_wallFront()) { // Move forward if possible
            moveMouseForward();
        } else if (!API_wallRight()) { // Otherwise, move right
            turnMouseRight();
            moveMouseForward();
        } else {
            // Dead end: turn around
            turnMouseRight();
            turnMouseRight();
            fprintf(stderr, "Dead end at (%d, %d). Turning around.\n", mouseX, mouseY);
        }
    }
}

int main() {
    // Initialize maze dimensions
    mazeWidth = API_mazeWidth();
    mazeHeight = API_mazeHeight();

    // Set up the starting position
    API_setText(mouseX, mouseY, "X");
    API_setColor(mouseX, mouseY, 'G');
    logMousePosition();

    API_ackReset(); // Acknowledge maze reset
    solveMaze(); // Start solving the maze

    return 0;
}
