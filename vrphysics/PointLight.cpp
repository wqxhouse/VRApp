//
//  PointLight.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "PointLight.h"
#include <osgDB/ReadFile>

PointLight::PointLight() :
intensity(1.0f), _animated(false)
{
    genGeometry();
    
    memset(ambient, 0, sizeof(ambient));
    memset(diffuse, 0, sizeof(diffuse));
    memset(specular, 0, sizeof(specular));
    
//    _light_effective_radius = intensity * _light_max_effective_radius;
    
    _animated = false;
    setAttenuation(1, 0, 0.6);
  
    _id = _highest_id++;
}


osg::ref_ptr<osg::MatrixTransform> PointLight::getGeomTransformNode()
{
    return _lightGeomTransform;
}

osg::ref_ptr<osg::MatrixTransform> PointLight::getLightSphereTransformNode()
{
    return _lightSphereTransform;
}

float PointLight::calcRadiusByAttenuation()
{
    float MaxChannel = fmax(fmax(diffuse[0], diffuse[1]), diffuse[2]);
    if(MaxChannel == 0.0f)
    {
        return 0.0f;
    }
    
    float &c = attenuation[0];
    float &l = attenuation[1];
    float &e = attenuation[2];
  
    float dist = 0.0f;
    if(e == 0.0f)
    {
        dist = (256 * MaxChannel * intensity - c) / l;
    }
    else
    {
        float sqrtInner = l*l - 4 * e * (c - 256 * MaxChannel * intensity);
        float sqrt = sqrtf(sqrtInner);
        dist = (-l + sqrt) / (2 * e);
    }
    
    return dist;
}

int PointLight::_highest_id;
osg::ref_ptr<osg::Geode> PointLight::_s_lightSphere = osgDB::readNodeFile("lightSphere.obj")->asGroup()->getChild(0)->asGeode();
