//
//  LightTrackBallManipulator.h
//  vrphysics
//
//  Created by Robin Wu on 2/17/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__LightTrackBallManipulator__
#define __vrphysics__LightTrackBallManipulator__

#include <stdio.h>
#include <osgGA/TrackballManipulator>
#include "DirectionalLight.h"

class LightTrackBallManipulator : public osgGA::TrackballManipulator
{
public:
    LightTrackBallManipulator() : _mainDirLight(NULL)
    {
        _lightTrackBall = new LightTrackBallManipulator;
    };
    
    virtual ~LightTrackBallManipulator() {} ;
    
    inline void setMainDirLight(DirectionalLight *mainDirLight)
    {
        _mainDirLight = mainDirLight;
        
        // TODO: update home position if light pos change (by another mechnism, like keyboard, physics engine..)
        _lightTrackBall->setHomePosition(_mainDirLight->getPosition(), _mainDirLight->getLookAt(), osg::Vec3(0, 0, 1));
    }
    
protected:
    
    // override
    virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );

    
private:
    DirectionalLight *_mainDirLight;
    
    osg::ref_ptr<LightTrackBallManipulator> _lightTrackBall;
};

#endif /* defined(__vrphysics__LightTrackBallManipulator__) */
