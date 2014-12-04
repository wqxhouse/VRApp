//
//  ScreenPass.h
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__ScreenPass__
#define __vrphysics__ScreenPass__

#include <stdio.h>
#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Camera>
#include <osg/MatrixTransform>
#include <osg/Projection>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osgDB/ReadFile>

class ScreenPass {
public:
    ScreenPass(osg::Camera *camera);
    
    virtual ~ScreenPass();
    virtual osg::ref_ptr<osg::Group> getRoot() { return _rootGroup; }
    virtual osg::ref_ptr<osg::TextureRectangle> getOutputTexture(int _id) { return _screenOutTexture[_id]; }
    // virtual void setShader(std::string vertex, std::string fragment);
   
    virtual int addInTexture(osg::TextureRectangle *texture);
    virtual int addOutTexture();
    virtual osg::ref_ptr<osg::TextureRectangle> getInTexture(int _id);
    virtual osg::ref_ptr<osg::TextureRectangle> getOutTexture(int _id);
    
    virtual int addShader(const char *vert, const char *frag);
    virtual osg::Program *getShader(int _id);
    
protected:
    virtual void configureStateSet() = 0;
    
    virtual osg::ref_ptr<osg::Group> createTexturedQuad();
    virtual void setupCamera();
    
    osg::ref_ptr<osg::Group> _rootGroup;
    osg::ref_ptr<osg::Camera> _rttCamera;
    std::vector<osg::ref_ptr<osg::TextureRectangle> > _screenInTextures;
    std::vector<osg::ref_ptr<osg::TextureRectangle> > _screenOutTexture;
    int _screenWidth;
    int _screenHeight;
    //osg::ref_ptr<osg::Program> _shaderProgram;
    osg::ref_ptr<osg::StateSet> _stateSet;
    osg::ref_ptr<osg::Camera> _mainCamera;
    
    std::vector<osg::ref_ptr<osg::Program> > _shaderPrograms;
};

#endif /* defined(__vrphysics__ScreenPass__) */
