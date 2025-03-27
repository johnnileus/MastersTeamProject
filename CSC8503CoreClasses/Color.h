#pragma once
#include "GameWorld.h"
using namespace NCL;
using namespace CSC8503;

class Color
{
public:
    // Predefined static colors
    static const Vector4 Red;
    static const Vector4 Green;
    static const Vector4 Blue;
    static const Vector4 White;
    static const Vector4 Black;
    static const Vector4 Grey;
};

// Initialize static color members
const Vector4 Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
const Vector4 Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
const Vector4 Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);
const Vector4 Color::White(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
const Vector4 Color::Grey(0.5f, 0.5f, 0.5f, 1.0f);
