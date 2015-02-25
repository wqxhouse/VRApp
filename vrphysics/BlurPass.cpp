//
//  BlurPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 2/24/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "BlurPass.h"

BlurPass::BlurPass(osg::Camera *mainCamera)
: ScreenPass(mainCamera)
{
//    configRTTCamera();
}

BlurPass::~BlurPass()
{
    
}

void BlurPass::configureStateSet()
{
    
}