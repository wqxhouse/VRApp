//
//  GBuffer.h
//  vrphysics
//
//  Created by Robin Wu on 11/6/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__GBuffer__
#define __vrphysics__GBuffer__

#include <stdio.h>
#include <osg/Shader>
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osg/TextureRectangle>
#include <osg/Texture2D>

class GBuffer {
    
public:
    enum GBuffer_texture_type
    {
        kGBuffer_texture_diffuse,
        kGBuffer_texture_normal,
        kGBuffer_texture_light,
        kGBuffer_texture_num
    };
    
    GBuffer();
    ~GBuffer();
    
    void init(const osg::ref_ptr<osg::Group> &scene, int win_with, int win_height);
    
    void bindGeometry(float near, float far);
    void unbindGeometry();
    
    void bindLight(float near, float far);
    void unbindLight();
    
    void resetLightPass();
    
    osg::ref_ptr<osg::Group> getRoot();
    
    void drawDebug(int x, int y);
    osg::ref_ptr<osg::Texture> getTexture(GBuffer_texture_type type);
    
private:
    int _w;
    int _h;
    
    osg::ref_ptr<osg::Texture> _diffuse_tex;
    osg::ref_ptr<osg::Texture> _normal_tex;
    osg::ref_ptr<osg::Texture> _light_tex;
    
    osg::ref_ptr<osg::Group> _scene;
    osg::ref_ptr<osg::Group> _root;
    
    osg::ref_ptr<osg::Camera> _geomPassCamera;
    osg::ref_ptr<osg::Camera> _lightPassCamera;
    
    bool _geomRendering;
    bool _lightRendering;
    
    void loadPassesCameras();
    osg::ref_ptr<osg::TextureRectangle> createTextureRectangle();
    osg::ref_ptr<osg::Texture2D> createTextureImage(const char *imageName);
    osg::ref_ptr<osg::Camera> createRTTCamera(osg::Camera::BufferComponent buffer,
                                              osg::Texture *tex,
                                              bool isAbsolute);
    
    osg::ref_ptr<osg::Camera> geomPassCamera();
    osg::ref_ptr<osg::Camera> lightPassCamera();
    
};

#endif /* defined(__vrphysics__GBuffer__) */
