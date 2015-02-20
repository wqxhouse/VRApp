//
//  LightTrackBallManipulator.cpp
//  vrphysics
//
//  Created by Robin Wu on 2/17/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "LightTrackBallManipulator.h"

bool LightTrackBallManipulator::performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    if(_mainDirLight == NULL)
    {
        return false;
    }
    
    if(_lightTrackBall->getVerticalAxisFixed() )
    {
        _lightTrackBall->rotateWithFixedVertical( dx, dy );
    }
    else
    {
        _lightTrackBall->rotateTrackball( _ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
                        _ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
                        getThrowScale( eventTimeDelta ) );
    }
    
    const osg::Quat &q = _lightTrackBall->getRotation();
    osg::Matrixf rotMat;
    q.get(rotMat);
    osg::Vec3 newLookAt = _mainDirLight->getLookAt() * rotMat;
    _mainDirLight->setLookAt(newLookAt);
    
    return true;
}
