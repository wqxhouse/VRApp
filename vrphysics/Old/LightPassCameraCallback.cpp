//
//  LightPassCameraCallback.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/6/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "LightPassCameraCallback.h"
#include <stdio.h>

#include "Shared.h"

LightPassCameraCallback::LightPassCameraCallback() {};
LightPassCameraCallback::~LightPassCameraCallback() {};

void LightPassCameraCallback::operator()(osg::Node *node, osg::NodeVisitor *nv)
{
    //Shared::gBuffer.bindLight(0, 100);
    
}