//
//  LightingPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "LightingPass.h"
#include <osg/BlendFunc>
#include <osg/BlendEquation>
#include "LightCallback.h"

// protected
LightingPass::LightingPass(osg::Camera *mainCamera, osg::TextureRectangle *position_tex, osg::TextureRectangle *diffuse_tex, osg::TextureRectangle *normal_tex, LightGroup *lightGroup)
: ScreenPass(mainCamera)
{
    _lightGroup = lightGroup;
    osg::Matrix projMatrix = mainCamera->getProjectionMatrix();
    float dummy;
    projMatrix.getFrustum(dummy, dummy, dummy, dummy, _nearPlaneDist, _farPlaneDist);
    
    _diffuse_tex_id = ScreenPass::addInTexture(diffuse_tex);
    _normal_tex_id = ScreenPass::addInTexture(normal_tex);
    _position_tex_id = ScreenPass::addInTexture(position_tex);
    
    ScreenPass::setShader("pointLightPass.vert", "pointLightPass.frag");
    ScreenPass::setupCamera();
    
    configureStateSet();
    configRTTCamera();
}

LightingPass::~LightingPass()
{
}

void LightingPass::configRTTCamera()
{
    _out_lighting_tex_id = ScreenPass::addOutTexture();
    _rttCamera->attach(osg::Camera::BufferComponent(osg::Camera::COLOR_BUFFER0+0),
                       getLightingOutTexture());
    _rttCamera->addChild(_lightPassGroupNode);
    _rootGroup->addChild(_rttCamera);
}

void LightingPass::configureStateSet()
{
    osg::ref_ptr<osg::Group> lightPassGroup(new osg::Group);
    _lightPassGroupNode = lightPassGroup;
    
    auto pointLights = _lightGroup->getPointLightsArrayReference();
    osg::Matrix mainCameraModelViewMatrix = _mainCamera->getViewMatrix();
    
    osg::ref_ptr<osg::BlendFunc> blendFunc(new osg::BlendFunc);
    osg::ref_ptr<osg::BlendEquation> blendEquation(new osg::BlendEquation);
    blendFunc->setFunction(GL_ONE, GL_ONE);
    blendEquation->setEquation(osg::BlendEquation::FUNC_ADD);
    
    for (std::vector<PointLight *>::iterator it = pointLights.begin(); it != pointLights.end(); it++)
    {
        float radius = (*it)->intensity * LightGroup::skMaxPointLightRadius;
        
        // SHADING/LIGHTING CALCULATION
        // this pass draws the spheres representing the area of influence each light has
        // in a fragment shader, only the pixels that are affected by the drawn geometry are processed
        // drawing light volumes (spheres for point lights) ensures that we're only running light calculations on
        // the areas that the spheres affect
        osg::Vec3f lightPosInViewSpace = (*it)->getPosition() * mainCameraModelViewMatrix;
//        printf("%.2f, %.2f, %.2f\n", (*it)->getPosition().x(), (*it)->getPosition().y(), (*it)->getPosition().z());
//        printf("%.2f, %.2f, %.2f\n", lightPosInViewSpace.x(), lightPosInViewSpace.y(), lightPosInViewSpace.z());
        
        // transform light sphere
        (*it)->genLightSphereTransform(radius);
        auto mt = (*it)->_lightSphereTransform;
        lightPassGroup->addChild(mt);
        
        auto ss = mt->getOrCreateStateSet();
        ss->setUpdateCallback(new LightCallback(_mainCamera, (*it)));
        ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
 
        // enable blending
        ss->setMode(GL_BLEND, osg::StateAttribute::ON);
        ss->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);
        ss->setAttributeAndModes(blendEquation, osg::StateAttribute::ON);
        
        // passing uniforms
        ss->setAttributeAndModes(_shaderProgram, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        ss->addUniform(new osg::Uniform("u_lightPosition", lightPosInViewSpace));
        ss->addUniform(new osg::Uniform("u_farDistance", _farPlaneDist));
        
        ss->addUniform(new osg::Uniform("u_albedoTex", 0));
        ss->addUniform(new osg::Uniform("u_normalAndDepthTex", 1));
        ss->addUniform(new osg::Uniform("u_positionTex", 2));
        ss->addUniform(new osg::Uniform("u_inverseScreenSize", osg::Vec2f(1.0f/_screenWidth, 1.0f/_screenHeight)));
        
        ss->setTextureAttributeAndModes(0, ScreenPass::getInTexture(_diffuse_tex_id));
        ss->setTextureAttributeAndModes(1, ScreenPass::getInTexture(_normal_tex_id));
        ss->setTextureAttributeAndModes(2, ScreenPass::getInTexture(_position_tex_id));
        
        ss->addUniform(new osg::Uniform("u_lightAmbient", (*it)->getAmbient()));
        ss->addUniform(new osg::Uniform("u_lightDiffuse", (*it)->getDiffuse()));
        ss->addUniform(new osg::Uniform("u_lightSpecular", (*it)->getSpecular()));
        ss->addUniform(new osg::Uniform("u_lightIntensity", (*it)->intensity));
        ss->addUniform(new osg::Uniform("u_lightAttenuation", (*it)->getAttenuation()));
        ss->addUniform(new osg::Uniform("u_lightRadius", radius));
    }
}

