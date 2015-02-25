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
: _group(NULL), _dirLightGroup(NULL)
{
    _group = group;
}

LightGroupAnimationCallback::LightGroupAnimationCallback(DirectionalLightGroup *group)
: _group(NULL), _dirLightGroup(NULL)
{
    _dirLightGroup = group;
}

void LightGroupAnimationCallback::animateDirLights(int count, float secPerFrame)
{
    if(_dirLightGroup == NULL) return;
    
    std::vector<DirectionalLight *> &ptArr = _dirLightGroup->getDirectionalLightsReference();
    for(std::vector<DirectionalLight *>::iterator it = ptArr.begin(); it != ptArr.end(); it++)
    {
        DirectionalLight *dl = *it;
        if(!dl->isAnimated())
        {
            continue;
        }
        
        // float percent = count / (float) ptArr.size();
        osg::Vec3 original = dl->getPosition();
        original -= dl->getLookAt();
        osg::Matrix mat;
        mat.makeRotate(osg::DegreesToRadians(10 * secPerFrame), dl->getAnimOrbitAxis());
        
        osg::Vec3 newPos = original * mat;
        newPos += dl->getLookAt();
        dl->setPosition(newPos);
        
        ++count;
    }
}

void LightGroupAnimationCallback::animatePointLights(int count, float time)
{
    if(_group == NULL) return;
    
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

void LightGroupAnimationCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    _angle += osg::DegreesToRadians(1.0f);
    int count = 0;
    const osg::FrameStamp *fs = nv->getFrameStamp();
    if(!fs) return;
    float time = fs->getReferenceTime();
    int frameNum = fs->getFrameNumber();
    float secPerFrame = time / frameNum;
    
    animateDirLights(count, secPerFrame);
    animatePointLights(count, time);
}