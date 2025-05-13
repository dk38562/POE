#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vex.h"

using namespace vex;

// Brain should be defined by default
brain Brain;

// START V5 MACROS
#define waitUntil(condition) \
    do {                     \
        wait(5, msec);       \
    } while (!(condition))

#define repeat(iterations) \
    for (int iterator = 0; iterator < iterations; iterator++)
// END V5 MACROS

// Robot configuration code.
optical rightOptical10 = optical(PORT10);
optical leftOptical1 = optical(PORT1);
limit limitSwitchA = limit(Brain.ThreeWirePort.A);
motor motor20 = motor(PORT20, ratio18_1, false);

pot potentiometerB = pot(Brain.ThreeWirePort.B);
limit limitSwitchC = limit(Brain.ThreeWirePort.C);
#pragma endregion VEXcode Generated Robot Configuration

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Daniel K.                                                 */
/*    Created:      06 May, 2025                                              */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// Include the V5 Library
#include "vex.h"
// Allows for easier use of the VEX Library
using namespace vex;

// 30 - 100 - 210
#define NIGHTTIME_BRIGHTNESS 10
#define HOME 95
#define CW_LIMIT 210
#define CCW_LIMIT 30

double curMotorPosition;
double moveToPosition = HOME;

void checkEmergencyStop() {
    if (limitSwitchC.pressing()) {
        Brain.Screen.setCursor(5, 1);
        Brain.Screen.print("System Halt!!!");
        waitUntil(!limitSwitchC.pressing());
        waitUntil(limitSwitchC.pressing());
        waitUntil(!limitSwitchC.pressing());
        Brain.Screen.clearLine(5);
    }
}

double prevMotorPosition = HOME;
void updateDisplay() {
    curMotorPosition = motor20.position(degrees);

    if (prevMotorPosition != curMotorPosition) {
        Brain.Screen.clearLine(1);
        Brain.Screen.setCursor(1, 1);
        Brain.Screen.print("Current Position: %.2fº",
                           motor20.position(degrees));
    }

    prevMotorPosition = curMotorPosition;
}

double potentiometerAngle;
bool atCWLimit;
bool atCCWLimit;
double desiredMovement;
bool safeToRotate(double desiredPosition) {
    potentiometerAngle = potentiometerB.angle(degrees);

    atCWLimit = (desiredPosition >= CW_LIMIT);
    atCCWLimit = (desiredPosition <= CCW_LIMIT);

    return !(atCWLimit || atCCWLimit);
}

double curLeftOpticalReading;
double curRightOpticalReading;
bool shouldMotorMove;
int main() {
    wait(1000, msec);
    motor20.setPosition(potentiometerB.angle(degrees), degrees);
    motor20.setStopping(brake);  // CHANGE WHEN DONE!!!!

    while (true) {
        wait(100, msec);
        checkEmergencyStop();
        curLeftOpticalReading = leftOptical1.brightness();
        curRightOpticalReading = rightOptical10.brightness();
        curMotorPosition = motor20.position(degrees);

        shouldMotorMove = true;
        if (limitSwitchA.pressing()) {
            motor20.spinToPosition(HOME, degrees, false);
            shouldMotorMove = false;
        } else if ((curLeftOpticalReading <= NIGHTTIME_BRIGHTNESS) &&
                   (curRightOpticalReading <= NIGHTTIME_BRIGHTNESS)) {
            moveToPosition = HOME;

        } else if (curRightOpticalReading > curLeftOpticalReading) {
            moveToPosition = curMotorPosition + 3;
        } else {
            moveToPosition = curMotorPosition - 3;
        }

        if (motor20.isDone() && shouldMotorMove &&
            safeToRotate(moveToPosition)) {
            motor20.spinToPosition(moveToPosition, degrees, false);
        }
        updateDisplay();
    }
}
