#include "../include/sampleLibFile.h"

#include <iostream>

using namespace std;

IThirdEye::~IThirdEye() {
}

ThirdEye::~ThirdEye() {
}

void ThirdEye::uninit() {
  cout << "In sampleLib uinint." << endl;
}