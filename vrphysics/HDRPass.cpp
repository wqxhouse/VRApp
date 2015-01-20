//
//  HDRPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 12/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "HDRPass.h"

HDRPass::HDRPass(osg::Camera *mainCamera, osg::TextureRectangle *inScreenTex)
: ScreenPass(mainCamera)
{
    _in_screenTex = addInTexture(inScreenTex);
    _out_screenTex = addOutTexture();
    _hdr_shader = addShader("toneMapping.vert", "toneMapping.frag");
    setupCamera();
    configRTTCamera();
    configureStateSet();
}

HDRPass::~HDRPass()
{
    
}

void HDRPass::configureStateSet()
{
    _stateSet = _rttCamera->getOrCreateStateSet();
    _stateSet->setAttributeAndModes(getShader(_hdr_shader), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    _stateSet->addUniform(new osg::Uniform("u_screenTex", 0));
    _stateSet->setTextureAttribute(0, getInTexture(_in_screenTex));
}

void HDRPass::setupCamera()
{
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setClearColor(osg::Vec4());
    camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    camera->setRenderOrder(osg::Camera::PRE_RENDER);
    
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
    camera->setViewMatrix(osg::Matrix::identity());
    _rttCamera = camera;
    
}

void HDRPass::configRTTCamera()
{
    _stateSet = _rttCamera->getOrCreateStateSet();
    _screenQuad = createTexturedQuad();
    _rttCamera->addChild(_screenQuad);
    _rttCamera->attach(osg::Camera::COLOR_BUFFER0, getOutputTexture(_out_screenTex));
    
    _rootGroup->addChild(_rttCamera);
}


int HDRPass::addOutTexture()
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    
    // for ssao, set rgba16bit texture
    tex->setTextureSize(_screenWidth, _screenHeight);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA16);
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}
