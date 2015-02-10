//
//  LightCallback.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/13/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "LightCallback.h"
#include <osgDB/WriteFile>

void LightStencilCallback::operator()(osg::StateSet *ss, osg::NodeVisitor *nv)
{
    ss->getUniform("u_viewMat")->set(osg::Matrixf(_mainCamera->getViewMatrix()));
    ss->getUniform("u_projectionMat")->set(osg::Matrixf(_mainCamera->getProjectionMatrix()));
}

void LightCallback::operator()(osg::StateSet *ss, osg::NodeVisitor *nv)
{
    // update uniform and other data to the light shader
    float radius = _light->_light_effective_radius;
    
    osg::Vec3f lightPosInViewSpace = _light->getPosition() * _mainCamera->getViewMatrix();
    
    // transform light sphere  -- TODO: fix this ugly design (possibly integrate genLightSphereTransform to setPosition())
    _light->genLightSphereTransform(radius);
    
    // passing uniforms
    ss->getUniform("u_lightPosition")->set(lightPosInViewSpace);
    ss->getUniform("u_lightAmbient")->set(_light->getAmbient());
    ss->getUniform("u_lightDiffuse")->set(_light->getDiffuse());
    ss->getUniform("u_lightSpecular")->set(_light->getSpecular());
    ss->getUniform("u_lightIntensity")->set(_light->intensity);
    ss->getUniform("u_lightAttenuation")->set(_light->getAttenuation());
    ss->getUniform("u_lightRadius")->set(radius);
    
    ss->getUniform("u_viewMatrix")->set(osg::Matrixf(_mainCamera->getViewMatrix()));
    ss->getUniform("u_projMatrix")->set(osg::Matrixf(_mainCamera->getProjectionMatrix()));
}