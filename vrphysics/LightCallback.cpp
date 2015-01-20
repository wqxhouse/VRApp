//
//  LightCallback.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/13/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "LightCallback.h"

void LightCallback::operator()(osg::StateSet *ss, osg::NodeVisitor *nv)
{
    // update uniform and other data to the light shader
//    float radius = _light->intensity * LightGroup::skMaxPointLightRadius;
    float radius = _light->_light_effective_radius;
    
    osg::Vec3f lightPosInViewSpace = _light->getPosition() * _mainCamera->getViewMatrix();
    
    // transform light sphere
    _light->genLightSphereTransform(radius);
    
//    auto m = _mainCamera->getViewMatrix();
//    Utils::print(m);
    
    // passing uniforms
    ss->getUniform("u_lightPosition")->set(lightPosInViewSpace);
    //        ss->getUniform("u_lightPosition")->set(_light->getPosition());
    
    ss->getUniform("u_lightAmbient")->set(_light->getAmbient());
    ss->getUniform("u_lightDiffuse")->set(_light->getDiffuse());
    ss->getUniform("u_lightSpecular")->set(_light->getSpecular());
    ss->getUniform("u_lightIntensity")->set(_light->intensity);
    ss->getUniform("u_lightAttenuation")->set(_light->getAttenuation());
    ss->getUniform("u_lightRadius")->set(radius);
}