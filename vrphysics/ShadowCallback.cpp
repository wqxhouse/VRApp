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

std::pair<float, float> ShadowCallback::getNearFarPlane()
{
    float dummy;
    float near;
    float far;
    _shadowProjection.getOrtho(dummy, dummy, dummy, dummy, near, far);
    return std::make_pair(near, far);
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

    osg::Matrixf shadowMVP = shadowMV * _shadowProjection;
    // TODO: pass in near/ far plane
    ss->getUniform("u_lightViewMatrix")->set(shadowMV);
    ss->getUniform("u_lightViewProjectionMatrix")->set(shadowMVP);
    ss->getUniform("u_lightViewInverseMatrix")->set(shadowViewInverse);
    
    ss->getUniform("u_lightPos")->set(_dirLight->getPosition());
   
    std::pair<float, float> nearFar = getNearFarPlane();
    ss->getUniform("u_farDistance_s")->set(nearFar.second);
    ss->getUniform("u_nearDistance_s")->set(nearFar.first);
    
    _dirLight->_lightProjectionMatrix = _shadowProjection;
    _dirLight->_lightViewMatrix = shadowMV;
    _dirLight->_lightMVP = shadowMVP;
}
