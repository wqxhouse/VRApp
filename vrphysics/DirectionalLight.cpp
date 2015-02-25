//
//  DirectionalLight.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/27/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "DirectionalLight.h"
DirectionalLight::DirectionalLight() :
intensity(1.0f), _isAnimated(false)
{
    memset(ambient, 0, sizeof(ambient));
    memset(diffuse, 0, sizeof(diffuse));
    memset(specular, 0, sizeof(specular));
    genGeometry();
    
    _light_geom_radius = 0.25;
    _id = _highest_id++;
}

int DirectionalLight::_highest_id;