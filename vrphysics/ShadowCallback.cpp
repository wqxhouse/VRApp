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
    
void ShadowCallback::operator()(osg::StateSet *ss, osg::NodeVisitor* nv)
{
    osg::Matrixf shadowMV;
    if(_dirLight != NULL)
    {
        shadowMV.makeLookAt(_dirLight->getPosition(), _dirLight->getLookAt(), osg::Vec3(0, 0, 1));
    }

    osg::Matrixf shadowMVP = shadowMV * _shadowProjection;
    // TODO: pass in near/ far plane
    ss->getUniform("u_lightViewMatrix")->set(shadowMV);
    ss->getUniform("u_lightViewProjectionMatrix")->set(shadowMVP);
    
    _dirLight->_lightProjectionMatrix = _shadowProjection;
    _dirLight->_lightViewMatrix = shadowMV;
    _dirLight->_lightMVP = shadowMVP;
}
