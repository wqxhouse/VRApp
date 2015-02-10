//
//  SSAOPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "SSAOPass.h"
#include "Utils.h"
#include "LightCallback.h"

SSAOPass::SSAOPass(osg::Camera *mainCamera, osg::TextureRectangle *positionTex, osg::TextureRectangle *normalTex, osg::Texture2D *randomJitterTex, osg::Texture2D *debug)
: ScreenPass(mainCamera)
{
    _ssao_shader_id = addShader("ssao.vert", "ssao.frag");
    _in_position_tex = addInTexture(positionTex);
    _in_normal_tex = addInTexture(normalTex);
    //_in_random_tex = addInTexture(randomJitterTex);
    _randomTexture2D = randomJitterTex;
    
    setSSAOParameters(0.5f, 30.0f, 0.3f, 0.36f);
    
    setupCamera();
    configRTTCamera();
    
    // get matrix and farPlaneDist
    osg::Matrix projMatrix = mainCamera->getProjectionMatrix();
    float dummy;
    // TODO: fix nearPlane
    projMatrix.getFrustum(dummy, dummy, dummy, dummy, dummy, _farPlaneDist);
   
    _debug = debug;
    configureStateSet();
}

SSAOPass::~SSAOPass()
{
}

void SSAOPass::configureStateSet()
{
    osg::Matrix projMatrix = _mainCamera->getProjectionMatrix();
    osg::Matrix inverseProjMat = osg::Matrix::inverse(projMatrix);
    
    _stateSet->setAttributeAndModes(getShader(_ssao_shader_id), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    _stateSet->addUniform(new osg::Uniform("u_occluderBias", _occulderBias));
    _stateSet->addUniform(new osg::Uniform("u_samplingRadius", _samplingRadius));
    _stateSet->addUniform(new osg::Uniform("u_attenuation", osg::Vec2(_constantAttenuation, _linearAttenuation)));
    _stateSet->addUniform(new osg::Uniform("u_farDistance", _farPlaneDist));
    _stateSet->addUniform(new osg::Uniform("u_inverseProjMatrix", inverseProjMat));
    _stateSet->addUniform(new osg::Uniform("u_randomJitterTex", 0));
    _stateSet->addUniform(new osg::Uniform("u_normalAndDepthTex", 1));
    _stateSet->addUniform(new osg::Uniform("u_positionTex", 2));
    _stateSet->addUniform(new osg::Uniform("u_screen_wh", osg::Vec2(_screenWidth, _screenHeight)));
    
//    _stateSet->addUniform(new osg::Uniform("u_debug", 3));
//    _stateSet->setTextureAttributeAndModes(3, _debug);
//    
    _stateSet->setTextureAttribute(0, _randomTexture2D);
    _stateSet->setTextureAttribute(1, getInTexture(_in_normal_tex));
    _stateSet->setTextureAttribute(2, getInTexture(_in_position_tex));
//
//    float w = _mainCamera->getViewport()->width();
//    float h = _mainCamera->getViewport()->height();
//    osg::Vec2 texelSize = osg::Vec2(1.0f/w, 1.0f/h);
//    _stateSet->addUniform(new osg::Uniform("u_texelSize", texelSize));
    
    //_screenQuad->getOrCreateStateSet()->setUpdateCallback(new SSAOStateCallback(_mainCamera, this));
    _stateSet->setUpdateCallback(new SSAOStateCallback(_mainCamera, this));
}

void SSAOPass::setSSAOParameters(float occluderBias, float samplingRadius, float constantAttenuation, float linearAttenuation)
{
    _occulderBias = occluderBias;
    _samplingRadius = samplingRadius;
    _constantAttenuation = constantAttenuation;
    _linearAttenuation = linearAttenuation;
}

// overload
int SSAOPass::addOutTexture()
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    
    // for ssao, set rgb16bit texture
    tex->setTextureSize(_screenWidth, _screenHeight);
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGB);
    tex->setInternalFormat(GL_RGB16F_ARB);
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}

void SSAOPass::setupCamera()
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

void SSAOPass::configRTTCamera()
{
    _out_ssao_tex_id = addOutTexture();
    _stateSet = _rttCamera->getOrCreateStateSet();
    _screenQuad = createTexturedQuad();
    _rttCamera->addChild(_screenQuad);
    _rttCamera->setClearColor(osg::Vec4(0, 0, 0, 1));
    _rttCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _rttCamera->attach(osg::Camera::COLOR_BUFFER0, getOutputTexture(_out_ssao_tex_id));
    
    _rootGroup->addChild(_rttCamera);
}

void SSAOStateCallback::operator()(osg::StateSet *ss, osg::NodeVisitor *nv)
{
    osg::Matrix projMatrix = _mainCamera->getProjectionMatrix();
    osg::Matrix inverseProjMat = osg::Matrix::inverse(projMatrix);
    
    ss->getUniform("u_occluderBias")->set(_ssaoPass->getOcculderBias());
    ss->getUniform("u_samplingRadius")->set(_ssaoPass->getSamplingRadius());
    ss->getUniform("u_attenuation")->set(osg::Vec2(_ssaoPass->getConstantAttenuation(), _ssaoPass->getLinearAttenuation()));
    ss->getUniform("u_inverseProjMatrix")->set(inverseProjMat);

}