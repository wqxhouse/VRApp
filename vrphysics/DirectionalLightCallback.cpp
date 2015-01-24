//
//  DirectionalLightCallback.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/27/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "DirectionalLightCallback.h"
#include "Utils.h"

DirectionalLightCallback::DirectionalLightCallback(osg::Camera *mainCamera, DirectionalLight *light)
: _light(light), _mainCamera(mainCamera)
{
}


void DirectionalLightCallback::operator()(osg::StateSet *ss, osg::NodeVisitor* nv)
{
    // update uniform and other data to the light shader
    osg::Vec3f lightPosInViewSpace = _light->getPosition() * _mainCamera->getViewMatrix();
    osg::Vec3f lookAtInViewSpace = _light->getLookAt() * _mainCamera->getViewMatrix();
    osg::Vec3f lightDir = lightPosInViewSpace - lookAtInViewSpace;
    
//    auto m = _mainCamera->getViewMatrix();
    ss->getUniform("u_viewMatrixInverse")->set(osg::Matrixf(_mainCamera->getInverseViewMatrix()));
    ss->getUniform("u_lightViewMatrix")->set(_light->_lightViewMatrix);
    ss->getUniform("u_lightProjectionMatrix")->set(_light->_lightProjectionMatrix);
    
    //ss->getUniform("u_lightPosition")->set(lightPosInViewSpace);
    ss->getUniform("u_lightDir")->set(lightDir);
    
    ss->getUniform("u_lightAmbient")->set(_light->getAmbient());
    ss->getUniform("u_lightDiffuse")->set(_light->getDiffuse());
    ss->getUniform("u_lightSpecular")->set(_light->getSpecular());
    ss->getUniform("u_lightIntensity")->set(_light->intensity);
    
    ss->getUniform("u_lightNearDistance")->set(_light->_lightNearDistance);
    ss->getUniform("u_lightFarDistance")->set(_light->_lightFarDistance);
}