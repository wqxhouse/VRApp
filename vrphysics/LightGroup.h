//
//  ObjectGroup.h
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__ObjectGroup__
#define __vrphysics__ObjectGroup__

#include <stdio.h>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include "PointLight.h"

class LightGroup
{
public:
    static const int skNumBoxes = 50;
    static const int skNumLights = 100;
    static const int skRadius = 20;
    static const int skMaxPointLightRadius = 8;
    
    LightGroup();
    ~LightGroup();
    
    void addLight(const osg::Vec3 &position, const osg::Vec3 &color);
    const std::vector<PointLight *> &getPointLightsArrayReference();
    void addRandomLight();
    osg::ref_ptr<osg::Group> getGeomTransformLightGroup();
    int getLightNum() { return _lightNum; };
    
private:
    osg::ref_ptr<osg::Group> _geomTransformLightGroup;
    std::vector<PointLight *> _pointLights;
    float randomf(float a, float b) {
        float random = ((float) rand()) / (float) RAND_MAX;
        float diff = b - a;
        float r = random * diff;
        return a + r;
    }
    
    int _lightNum;
};

#endif /* defined(__vrphysics__ObjectGroup__) */
