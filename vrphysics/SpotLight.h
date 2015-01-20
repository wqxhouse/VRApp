//
//  SpotLight.h
//  vrphysics
//
//  Created by Robin Wu on 1/6/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__SpotLight__
#define __vrphysics__SpotLight__

#include <stdio.h>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/ref_ptr>
#include <osg/Geode>

class SpotLight
{
public:
    inline osg::Vec3 getPosition()
    {
        return _pos;
    }
    
    inline void setPosition(const osg::Vec3 &v)
    {
        _pos = v;
    }
    
    inline osg::Vec3 getDirection()
    {
        return _dir;
    }
    
    inline void setDirection(const osg::Vec3 &v)
    {
        _dir = v;
    }
    
    inline osg::Vec3 getConeAngle()
    {
        return _coneAngle;
    }
    
    inline void setConeAngle(const osg::Vec3 &coneAngle)
    {
        _coneAngle = coneAngle;
    }
    
    inline osg::Vec3 getSpotExponent()
    {
        return _spotExp;
    }
    
    inline void setSpotExponent(const osg::Vec3 &spotExp)
    {
        _spotExp = spotExp;
    }
    
    inline osg::Vec3 getAttenuation()
    {
        return _attenuation;
    }
    
    inline void setAttenuation(const osg::Vec3 &att)
    {
        _attenuation = att;
    }
    
//    osg::ref_ptr<osg::Geode> getRoot()
//    {
//        return _sphereGeode;
//    }
//    
    osg::Vec4 getAmbient()
    {
        return osg::Vec4(ambient[0], ambient[1], ambient[2], ambient[3]);
    }
    
    osg::Vec4 getDiffuse()
    {
        return osg::Vec4(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
    }
    
    osg::Vec4 getSpecular()
    {
        return osg::Vec4(specular[0], specular[1], specular[2], specular[3]);
    }
    

private:
    osg::Vec3 _pos;
    osg::Vec3 _dir;
    osg::Vec3 _coneAngle;
    osg::Vec3 _spotExp;
    osg::Vec3 _attenuation;
    
    float ambient[4];
    float diffuse[4];
    float specular[4];
    
};

#endif /* defined(__vrphysics__SpotLight__) */
