//
//  ShadowCallback.cpp
//  vrphysics
//
//  Created by Robin Wu on 1/18/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "ShadowCallback.h"

void ShadowCallback::setPointLight(PointLight *pl)
{
    _pointLight = pl;
    _dirLight = NULL;
}

void ShadowCallback::setDirectionalLight(DirectionalLight *dirLight)
{
    _dirLight = dirLight;
    _pointLight = NULL;
}

std::pair<float, float> ShadowCallback::getNearFar()
{
    float dummy;
    float near;
    float far;
    _shadowProjection.getOrtho(dummy, dummy, dummy, dummy, near, far);
   
    return std::make_pair(near, far);
}

osg::Matrix ShadowCallback::calcProjectionMatrix(const osg::Matrix &viewMatrix)
{
    float xMin, xMax, yMin, yMax, zMin, zMax;
    xMin = 1000000000;
    xMax = -1000000000;
    yMin = 1000000000;
    yMax = -1000000000;
    zMin = 1000000000;
    zMax = -1000000000;
   
    for(int i = 0; i < 8; i++)
    {
        osg::Vec3 v = _sceneAABB.corner(i);
        osg::Vec3 viewV = v * viewMatrix;
        
        if(viewV.x() > xMax) xMax = viewV.x();
        if(viewV.x() < xMin) xMin = viewV.x();
        
        if(viewV.y() > yMax) yMax = viewV.y();
        if(viewV.y() < yMin) yMin = viewV.y();
        if(viewV.z() > zMax) zMax = viewV.z();
        if(viewV.z() < zMin) zMin = viewV.z();
    }
    osg::Matrix proj;
    float near, far;
    if(zMax < 0 && zMin < 0)
    {
        near = -zMax;
        far = -zMin;
    }
    else // zMax > 0 && zMin > 0 || zMax > 0 zMin < 0
    {
        near = zMin;
        far = zMax;
    }
    
    proj.makeOrtho(xMin, xMax, yMin, yMax, near, far);
    return proj;
}

void ShadowCallback::operator()(osg::StateSet *ss, osg::NodeVisitor* nv)
{
    osg::Matrixf shadowMV;
    osg::Matrixf shadowViewInverse;
    if(_dirLight != NULL)
    {
        shadowMV.makeLookAt(_dirLight->getPosition(), _dirLight->getLookAt(), osg::Vec3(0, 0, 1));
        shadowViewInverse = osg::Matrixf::inverse(shadowMV);
    }

    _shadowProjection = calcProjectionMatrix(shadowMV);
    osg::Matrixf shadowMVP = shadowMV * _shadowProjection;
    // TODO: pass in near/ far plane
    ss->getUniform("u_lightViewMatrix")->set(shadowMV);
//    ss->getUniform("u_lightViewProjectionMatrix")->set(shadowMVP);
    _lightCamera->setViewMatrix(shadowMV);
    _lightCamera->setProjectionMatrix(_shadowProjection);
    
    ss->getUniform("u_lightViewInverseMatrix")->set(shadowViewInverse);
    
    ss->getUniform("u_lightPos")->set(_dirLight->getPosition());
   
    std::pair<float, float> nearFar = getNearFar();
    ss->getUniform("u_zLength")->set(nearFar.second - nearFar.first);
    ss->getUniform("u_nearDistance_s")->set(nearFar.first);
    
    _dirLight->_lightNearDistance = nearFar.first;
    _dirLight->_lightFarDistance = nearFar.second;
    
    _dirLight->_lightProjectionMatrix = _shadowProjection;
    _dirLight->_lightViewMatrix = shadowMV;
    _dirLight->_lightMVP = shadowMVP;
}
