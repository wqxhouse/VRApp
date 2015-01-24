//
//  DirectionalLightingPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/27/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "DirectionalLightingPass.h"
#include <osg/BlendEquation>
#include <osg/BlendFunc>
#include "DirectionalLightCallback.h"

#include "Utils.h"

DirectionalLightingPass::DirectionalLightingPass(osg::Camera *mainCamera, osg::TextureRectangle *position_tex, osg::TextureRectangle *diffuse_tex, osg::TextureRectangle *normal_tex, ShadowGroup *shadowGroup, DirectionalLightGroup *lightGroup)
: ScreenPass(mainCamera), _shadowGroup(shadowGroup)
{
    _lightGroup = lightGroup;
    osg::Matrix projMatrix = mainCamera->getProjectionMatrix();
    float dummy;
    projMatrix.getFrustum(dummy, dummy, dummy, dummy, _nearPlaneDist, _farPlaneDist);
    
    _diffuse_tex_id = ScreenPass::addInTexture(diffuse_tex);
    _normal_tex_id = ScreenPass::addInTexture(normal_tex);
    _position_tex_id = ScreenPass::addInTexture(position_tex);
    
    //ScreenPass::setShader("dirLight.vert", "dirLight.frag");
    _light_shader_id = addShader("dirLight.vert", "dirLight.frag");
    ScreenPass::setupCamera();
    
    configureStateSet();
    configRTTCamera();
}

DirectionalLightingPass::~DirectionalLightingPass()
{
    
}

void DirectionalLightingPass::configRTTCamera()
{
    _out_lighting_tex_id = ScreenPass::addOutTexture();
    _rttCamera->attach(osg::Camera::BufferComponent(osg::Camera::COLOR_BUFFER0+0),
                       getLightingOutTexture());
    _rttCamera->addChild(_lightPassGroupNode);
    _rootGroup->addChild(_rttCamera);
    
    
    _rttCamera->setClearColor(osg::Vec4());
    _rttCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _rttCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    _rttCamera->setRenderOrder(osg::Camera::PRE_RENDER);
    
    _rttCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    _rttCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
    _rttCamera->setViewMatrix(osg::Matrix::identity());
}

void DirectionalLightingPass::configureStateSet()
{
    osg::ref_ptr<osg::Group> lightPassGroup(new osg::Group);
    _lightPassGroupNode = lightPassGroup;
    
    auto directionalLights = _lightGroup->getDirectionalLightsReference();
    osg::Matrix mainCameraModelViewMatrix = _mainCamera->getViewMatrix();
    
    osg::ref_ptr<osg::BlendFunc> blendFunc(new osg::BlendFunc);
    osg::ref_ptr<osg::BlendEquation> blendEquation(new osg::BlendEquation);
    blendFunc->setFunction(GL_ONE, GL_ONE);
    blendEquation->setEquation(osg::BlendEquation::FUNC_ADD);
    
    for (std::vector<DirectionalLight *>::iterator it = directionalLights.begin(); it != directionalLights.end(); it++)
    {
        osg::Vec3f lightPosInViewSpace = (*it)->getPosition() * mainCameraModelViewMatrix;
        osg::Vec3f lookAtInViewSpace = (*it)->getLookAt() * mainCameraModelViewMatrix;
        osg::Vec3f lightDir = lightPosInViewSpace - lookAtInViewSpace;
        
        (*it)->genRenderQuad(_mainCamera->getViewport()->width(), _mainCamera->getViewport()->height());
        auto renderQuad = (*it)->_renderQuad;
        lightPassGroup->addChild(renderQuad);
        
        auto ss = renderQuad->getOrCreateStateSet();
        ss->setUpdateCallback(new DirectionalLightCallback(_mainCamera, (*it)));
        ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
        
        // enable blending
        ss->setMode(GL_BLEND, osg::StateAttribute::ON);
        ss->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);
        ss->setAttributeAndModes(blendEquation, osg::StateAttribute::ON);
        
        // passing uniforms
        //ss->setAttributeAndModes(_shaderProgram, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        ss->setAttributeAndModes(getShader(_light_shader_id), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
//        ss->addUniform(new osg::Uniform("u_lightPosition", lightPosInViewSpace));
        
        ss->addUniform(new osg::Uniform("u_lightDir", lightDir));
        
        ss->addUniform(new osg::Uniform("u_albedoTex", 0));
        ss->addUniform(new osg::Uniform("u_normalAndDepthTex", 1));
        ss->addUniform(new osg::Uniform("u_positionTex", 2));
        
        int light_id = (*it)->_id;
        osg::ref_ptr<osg::TextureRectangle> depthTex = _shadowGroup->getDirLightShadowTexture(light_id);
//        osg::ref_ptr<osg::Texture2D> depthTex = _shadowGroup->getDirLightShadowTexture(light_id);
        ss->addUniform(new osg::Uniform("u_depthMapTex", 3));
       
        osg::Vec2 depthMapSize = osg::Vec2(depthTex->getTextureWidth(), depthTex->getTextureHeight());
        ss->addUniform(new osg::Uniform("u_depthMapSize", depthMapSize));
        ss->addUniform(new osg::Uniform("u_viewMatrixInverse", osg::Matrixf(_mainCamera->getInverseViewMatrix())));
        ss->addUniform(new osg::Uniform("u_lightNearDistance", (*it)->_lightNearDistance));
        ss->addUniform(new osg::Uniform("u_lightFarDistance", (*it)->_lightFarDistance));
        
        ss->setTextureAttributeAndModes(0, ScreenPass::getInTexture(_diffuse_tex_id));
        ss->setTextureAttributeAndModes(1, ScreenPass::getInTexture(_normal_tex_id));
        ss->setTextureAttributeAndModes(2, ScreenPass::getInTexture(_position_tex_id));
        ss->setTextureAttributeAndModes(3, depthTex);
        
        ss->addUniform(new osg::Uniform("u_lightAmbient", (*it)->getAmbient()));
        ss->addUniform(new osg::Uniform("u_lightDiffuse", (*it)->getDiffuse()));
        ss->addUniform(new osg::Uniform("u_lightSpecular", (*it)->getSpecular()));
        ss->addUniform(new osg::Uniform("u_lightIntensity", (*it)->intensity));
        
        ss->addUniform(new osg::Uniform("u_lightViewMatrix", (*it)->_lightViewMatrix));
        ss->addUniform(new osg::Uniform("u_lightProjectionMatrix",(*it)->_lightProjectionMatrix));
        ss->addUniform(new osg::Uniform("u_lightMVP", (*it)->_lightMVP));
    }
}

int DirectionalLightingPass::addOutTexture()
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    
    tex->setTextureSize(_screenWidth, _screenHeight);
//    tex->setSourceType(GL_UNSIGNED_BYTE);
//    tex->setSourceFormat(GL_RGBA);
//    tex->setInternalFormat(GL_RGBA);
    
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA16F_ARB);
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}

