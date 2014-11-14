//
//  Shared.h
//  vrphysics
//
//  Created by Robin Wu on 11/7/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef vrphysics_Shared_h
#define vrphysics_Shared_h

#include <vector>

#include <osg/Camera>
#include <osgViewer/Viewer>

#include "GBuffer.h"
#include "PointLight.h"

class Shared
{
public:
    static int win_width;
    static int win_height;
    
    static int light_num;
    
    static GBuffer gBuffer;
    static osgViewer::Viewer viewer;
    
    static std::vector<PointLight> pointLights;
    
    static const int skNumBoxes = 50;
    static const int skNumLights = 100;
    static const int skRadius = 20;
    static const int skMaxPointLightRadius = 8;
    
    static float nearPlane;
    static float farPlane;
};

#endif
