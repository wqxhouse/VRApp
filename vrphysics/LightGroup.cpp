//
//  ObjectGroup.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "LightGroup.h"
#include "LightGroupAnimationCallback.h"

LightGroup::LightGroup() : _lightNum(0)
{
    _geomTransformLightGroup = new osg::Group;
    _geomTransformLightGroup->setUpdateCallback(new LightGroupAnimationCallback(this));
}
LightGroup::~LightGroup() {}

void LightGroup::addLight(const osg::Vec3 &position, const osg::Vec3 &color)
{
    PointLight *light = new PointLight();
    light->setPosition(position);
    light->setDiffuse(color.x(), color.y(), color.z());
    light->setSpecular(color.x(), color.y(), color.z());
    light->setAttenuation(0.0f, 0.0f, 0.2f); // set constant, linear, and exponential attenuation
    light->intensity = 0.8f;
    light->genGeomTransform(0.25);
    _geomTransformLightGroup->addChild(light->_lightGeomTransform);
    light->orbitAxis = osg::Vec3(0, 0, 1);
    
    _pointLights.push_back(light);
    _lightNum++;
}

void LightGroup::addRandomLightWithBoundingSphere(const osg::BoundingSphere &boundSphere)
{
    // create a random light that is positioned on bounding sphere of scene (skRadius)
    PointLight *l = new PointLight;
    osg::Vec3f posOnSphere(randomf(-1.0f, 1.0f), randomf(-1.0f, 1.0f), randomf(-1.0f, 1.0f));
    posOnSphere.normalize();
    
    posOnSphere *= randomf(0.95f, 1.05f);
    
    osg::Vec3f orbitAxis(randomf(0.0f, 1.0f), randomf(0.0f, 1.0f), randomf(0.0f, 1.0f));
    orbitAxis.normalize();
    l->orbitAxis = orbitAxis;
   
    if(boundSphere.valid())
    {
        float radius = boundSphere.radius();
        posOnSphere *= radius;
    }
    else
    {
        posOnSphere *= skRadius-1;
    }
    
    l->setPosition(posOnSphere);
    l->setAmbient(0.0f, 0.0f, 0.0f);
    
    // osg::Vec3f col(randomf(0.3f, 0.5f), randomf(0.2f, 0.4f), randomf(0.7f, 1.0f));
    
    // randomize color
    osg::Vec3 col = osg::Vec3(randomf(0.4f, 1.0f), randomf(0.1f, 1.0f), randomf(0.3f, 1.0f));
    
    l->setDiffuse(col.x(), col.y(), col.z());
    l->setSpecular(col.x(), col.y(), col.z());
    l->setAttenuation(0.0f, 0.0f, 0.2f); // set constant, linear, and exponential attenuation
    l->intensity = 0.8f;
    l->genGeomTransform(0.25);
    
    _geomTransformLightGroup->addChild(l->_lightGeomTransform);
    _pointLights.push_back(l);
    _lightNum++;
}

void LightGroup::addRandomLight()
{
    osg::BoundingSphere sp;
    sp.radius() = -1.0f; // invalid bsphere
    addRandomLightWithBoundingSphere(sp);
}


osg::ref_ptr<osg::Group> LightGroup::getGeomTransformLightGroup()
{
    return _geomTransformLightGroup;
}

std::vector<PointLight *> & LightGroup::getPointLightsArrayReference()
{
    return _pointLights;
}
