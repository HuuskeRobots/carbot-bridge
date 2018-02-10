#pragma once

const int modeStop = 0;
const int modeForward = 1;
const int modeBackward = 2;
const int modeLeft = 3;
const int modeRight = 4;

void setupController();
bool isControllerMode(int);
void setControllerMode(int);
void loopController();