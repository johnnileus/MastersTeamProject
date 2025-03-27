#pragma once
#include"math.h"
using namespace NCL::Maths;
class Constants
{
public:
    Constants() = delete;

    //string
    static constexpr const char* APP_NAME = "MyApp";
    static constexpr const char* VERSION = "1.0.0";

    //float
    static constexpr float SPHERE_DEFAULT_MASS = 100;
    
    
    //bool
    static constexpr bool IS_DEBUG_MODE = true;
};
