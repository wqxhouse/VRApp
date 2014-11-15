//
//  PointLight.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "PointLight.h"

PointLight::PointLight() :
intensity(1.0f)
{
    memset(ambient, 0, sizeof(ambient));
    memset(diffuse, 0, sizeof(diffuse));
    memset(specular, 0, sizeof(specular));
    memset(attenuation, 1.0f, sizeof(attenuation));
    genGeometry();
  
    _light_geom_radius = 0.25;
    _light_max_effective_radius = 8;
    _light_effective_radius = intensity * _light_max_effective_radius;
    _id = _highest_id++;
}

int PointLight::_highest_id;
