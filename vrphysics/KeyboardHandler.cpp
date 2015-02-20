//
//  KeyboardHandler.cpp
//  vrphysics
//
//  Created by Robin Wu on 12/9/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "KeyboardHandler.h"
#include "LightGroup.h"

KeyboardHandler::KeyboardHandler(osg::ref_ptr<osg::Group> sceneRoot, osg::ref_ptr<osg::Group> hud, LightGroup *lightGroup)
:_sceneRoot(sceneRoot), _hud(hud), _hudOn(true)
{
    _pointLightGroup = lightGroup;
}

void KeyboardHandler::offsetExpAttenuation(int offset)
{
    std::vector<PointLight *> &group = _pointLightGroup->getPointLightsArrayReference();
    if(group.empty())
    {
        return;
    }
    
    float diff = offset / 100.0;
    
    PointLight *p = group[0];
    osg::Vec3 att = p->getAttenuation();
    att.z() = att.z() + diff;
    printf("exp: %.4f", att.z());
    if(att.z() < 0) return;
    p->setAttenuation(att.x(), att.y(), att.z());
}

bool KeyboardHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::KEYDOWN):
        {
            switch(ea.getKey())
            {
                case 'h':
                    if(_hudOn)
                    {
                        _sceneRoot->removeChild(_hud);
                        _hudOn = false;
                    }
                    else
                    {
                        _sceneRoot->addChild(_hud);
                        _hudOn = true;
                    }
                    return false; // ?
                    break;
                case 'o':
                    offsetExpAttenuation(1);
                    break;
                case 'p':
                    offsetExpAttenuation(-1);
                    break;
                default:
                    return false;
            }
        }
            
        default:
            return false;
    }
}