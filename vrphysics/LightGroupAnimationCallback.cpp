//
//  LightGroupAnimationCallback.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/14/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "LightGroupAnimationCallback.h"

#include <vector>
#include "PointLight.h"
#include "Utils.h"

LightGroupAnimationCallback::LightGroupAnimationCallback(LightGroup *group)
{
    _group = group;
}

void LightGroupAnimationCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    _angle += osg::DegreesToRadians(1.0f);
    int count = 0;
    const osg::FrameStamp *fs = nv->getFrameStamp();
    if(!fs) return;
    float time = fs->getReferenceTime();
    std::vector<PointLight *> &ptArr = _group->getPointLightsArrayReference();
    for(std::vector<PointLight *>::iterator it = ptArr.begin(); it != ptArr.end(); it++)
    {
        PointLight *pt = *it;
        if(!pt->isAnimated())
        {
            continue;
        }
        
        float percent = count / (float) ptArr.size();
        osg::Vec3 original = pt->getPosition();
        osg::Matrix mat;
        mat.makeRotate(percent * osg::DegreesToRadians(0.2f) + osg::DegreesToRadians(0.1f), pt->getAnimOrbitAxis());
        pt->setPosition(original * mat);
        
        pt->setIntensity(0.5f + 0.25f * (1.0f + cosf(time + percent* osg::PI)));
        ++count;
    }
}