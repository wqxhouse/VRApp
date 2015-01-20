//
//  FinalPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/13/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "FinalPass.h"

FinalPass::FinalPass(osg::Camera *mainCamera, osg::TextureRectangle *albedoTexture, osg::TextureRectangle *dirLightTexture, osg::TextureRectangle *lightTexture, osg::TextureRectangle *ssaoTexture)
: ScreenPass(mainCamera)
{
    //ScreenPass::setShader("finalPass.vert", "finalPass.frag");
    _final_shader_id = addShader("finalPass.vert", "finalPass.frag");
    _albedo_tex_id = addInTexture(albedoTexture);
    _dirLight_tex_id = addInTexture(dirLightTexture);
    _light_tex_id = addInTexture(lightTexture);
    _ssao_tex_id = addInTexture(ssaoTexture);
    
//    ScreenPass::setupCamera();
    setupCamera();
    configRTTCamera();
    
    configureStateSet();
}

FinalPass::~FinalPass() {}

void FinalPass::configureStateSet()
{
    //_stateSet->setAttributeAndModes(_shaderProgram, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    _stateSet->setAttributeAndModes(getShader(_final_shader_id), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    _stateSet->setTextureAttribute(0, getInTexture(_albedo_tex_id), osg::StateAttribute::ON);
    _stateSet->setTextureAttribute(1, getInTexture(_dirLight_tex_id), osg::StateAttribute::ON);
    _stateSet->setTextureAttribute(2, getInTexture(_light_tex_id), osg::StateAttribute::ON);
    _stateSet->setTextureAttribute(3, getInTexture(_ssao_tex_id), osg::StateAttribute::ON);
    _stateSet->addUniform(new osg::Uniform("u_albedoTex", 0));
    _stateSet->addUniform(new osg::Uniform("u_dirLightPassTex", 1));
    _stateSet->addUniform(new osg::Uniform("u_pointLightPassTex", 2));
    _stateSet->addUniform(new osg::Uniform("u_ssaoPassTex", 3));
    _stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}

void FinalPass::setupCamera()
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

void FinalPass::configRTTCamera()
{
    _out_finalpass_tex_id = ScreenPass::addOutTexture();
    _texQuad = createTexturedQuad();
    _stateSet = _texQuad->getOrCreateStateSet();
    _stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    _rttCamera->addChild(_texQuad);
    _rttCamera->attach(osg::Camera::BufferComponent(osg::Camera::COLOR_BUFFER0+0),
                       getFinalPassTexture());
    _rootGroup->addChild(_rttCamera);

}

int FinalPass::addOutTexture()
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    
    tex->setTextureSize(_screenWidth, _screenHeight);
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA16F_ARB); // float texture for HDR
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}

