//
//  PointLight.h
//  vrphysics
//
//  Created by Robin Wu on 11/7/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__PointLight__
#define __vrphysics__PointLight__

#include <stdio.h>

#include <iostream>
#include <osg/Vec3f>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

class PointLight {
public:
    PointLight() :
    intensity(1.0f)
    {
        memset(ambient, 0, sizeof(ambient));
        memset(diffuse, 0, sizeof(diffuse));
        memset(specular, 0, sizeof(specular));
        memset(attenuation, 1.0f, sizeof(attenuation));
    }
    
    void setAmbient(float r, float g, float b, float a=1.0f) {
        ambient[0] = r;
        ambient[1] = g;
        ambient[2] = b;
        ambient[3] = a;
    }
    
    void setDiffuse(float r, float g, float b, float a=1.0f) {
        diffuse[0] = r;
        diffuse[1] = g;
        diffuse[2] = b;
        diffuse[3] = a;
    }
    
    void setSpecular(float r, float g, float b, float a=1.0f) {
        specular[0] = r;
        specular[1] = g;
        specular[2] = b;
        specular[3] = a;
    }
    
    void setAttenuation(float constant, float linear, float exponential) {
        attenuation[0] = constant;
        attenuation[1] = linear;
        attenuation[2] = exponential;
    }
    
    void setPosition(const osg::Vec3f &pos) {
        position = pos;
    }
    
    osg::Vec3f getPosition() {
        return position;
    }
    
    void genGeometry()
    {
        osg::ref_ptr<osg::Sphere> sphere(new osg::Sphere(osg::Vec3(0, 0, 0), 2.0));
        osg::ref_ptr<osg::ShapeDrawable> shapeDraw(new osg::ShapeDrawable);
        shapeDraw->setShape(sphere);
        osg::ref_ptr<osg::Geode> sphereGeode(new osg::Geode);
        sphereGeode->addDrawable(shapeDraw);
        
        osg::Matrix scale;
        scale.makeScale(osg::Vec3f(0.25f, 0.25f, 0.25f));
        
        osg::Matrix translate;
        translate.makeTranslate(getPosition());
        _rootTransform = new osg::MatrixTransform(translate * scale);
        _rootTransform->addChild(sphereGeode);
    }
    
    osg::ref_ptr<osg::MatrixTransform> getRoot()
    {
        return _rootTransform;
    }
    
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float attenuation[3];
    
    float intensity;
    
    osg::Vec3f orbitAxis;
    osg::Vec3f position;
    
    osg::ref_ptr<osg::MatrixTransform> _rootTransform;
};

#endif /* defined(__deferred__pointlight__) */
