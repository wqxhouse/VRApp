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
#include <osg/StencilTwoSided>
#include <osg/CullFace>
#include <osg/Depth>

#include "LightCallback.h"

// protected
LightingPass::LightingPass(osg::Camera *mainCamera, osg::TextureRectangle *position_tex, osg::TextureRectangle *diffuse_tex, osg::TextureRectangle *normal_tex, osg::Texture2D *sharedGeomPassDepthStencilTexture, LightGroup *lightGroup)
: ScreenPass(mainCamera)
{
    _lightGroup = lightGroup;
    osg::Matrix projMatrix = mainCamera->getProjectionMatrix();
    float dummy;
    projMatrix.getFrustum(dummy, dummy, dummy, dummy, _nearPlaneDist, _farPlaneDist);
    
    _diffuse_tex_id = ScreenPass::addInTexture(diffuse_tex);
    _normal_tex_id = ScreenPass::addInTexture(normal_tex);
    _position_tex_id = ScreenPass::addInTexture(position_tex);
    
    _sharedGeomPassDepthStencilTex = sharedGeomPassDepthStencilTexture;
    
    //ScreenPass::setShader("pointLightPass.vert", "pointLightPass.frag");
    _light_shader_id = addShader("pointLightPass.vert", "pointLightPass.frag");
    _depth_fill_shader_id = addShader("pointLightDepthFill.vert", "pointLightDepthFill.frag");
    _stencil_shader_id = addShader("pointLightStencil.vert", "pointLightStencil.frag");
    ScreenPass::setupCamera();
    
    // configDepthFillQuad();
    
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
    _rttCamera->setClearColor(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
    _rttCamera->setClearStencil(0);
    _rttCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //_rttCamera->attach(osg::Camera::PACKED_DEPTH_STENCIL_BUFFER, GL_DEPTH_STENCIL_EXT);
    _rttCamera->attach(osg::Camera::PACKED_DEPTH_STENCIL_BUFFER, _sharedGeomPassDepthStencilTex);

    _rttCamera->addChild(_lightPassGroupNode);
    _rootGroup->addChild(_rttCamera);
}

//void LightingPass::configDepthFillQuad()
//{
//    _depthFillQuad = createTexturedQuad();
//    
//    osg::ref_ptr<osg::Camera> quadCamera(new osg::Camera);
//    quadCamera->addChild(_depthFillQuad);
//    quadCamera->setRenderOrder(osg::Camera::NESTED_RENDER);
//    quadCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
//    quadCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
//    
//    osg::ref_ptr<osg::StateSet> dss = _depthFillQuad->getOrCreateStateSet();
//    dss->setAttributeAndModes(getShader(_depth_fill_shader_id), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
//    dss->addUniform(new osg::Uniform("u_geom_depth", 0));
//    dss->setTextureAttribute(0, _sharedGeomPassDepthStencilTex);
//    dss->setRenderBinDetails(-2, "RenderBin");
//    osg::ref_ptr<osg::Depth> depth(new osg::Depth);
//    depth->setWriteMask(true);
//    dss->setAttribute(depth);
//    depth->setFunction(osg::Depth::ALWAYS);
//    dss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
//    
//    _rttCamera->addChild(quadCamera);
//}

void LightingPass::configStencilPassStateSet()
{
    osg::ref_ptr<osg::Group> stencilGroup(new osg::Group);
    osg::ref_ptr<osg::StateSet> sss = stencilGroup->getOrCreateStateSet();
    osg::ref_ptr<osg::ColorMask> colorMask(new osg::ColorMask);
    colorMask->setMask(false, false, false, false);
    sss->setAttribute(colorMask, osg::StateAttribute::OVERRIDE);

    sss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
    sss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    
    osg::ref_ptr<osg::StencilTwoSided> stencil(new osg::StencilTwoSided);
    stencil->setFunction(osg::StencilTwoSided::FRONT, osg::StencilTwoSided::ALWAYS, 0, 0);
    stencil->setFunction(osg::StencilTwoSided::BACK, osg::StencilTwoSided::ALWAYS, 0, 0);
    stencil->setOperation(osg::StencilTwoSided::FRONT, osg::StencilTwoSided::KEEP,
                          osg::StencilTwoSided::DECR_WRAP, osg::StencilTwoSided::KEEP);
    stencil->setOperation(osg::StencilTwoSided::BACK, osg::StencilTwoSided::KEEP,
                          osg::StencilTwoSided::INCR_WRAP, osg::StencilTwoSided::KEEP);
    
    stencil->setWriteMask(osg::StencilTwoSided::FRONT, 0xFF);
    stencil->setWriteMask(osg::StencilTwoSided::BACK, 0xFF);
    
    sss->setAttributeAndModes(stencil, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    sss->setRenderBinDetails(0, "RenderBin");
    
    sss->setAttributeAndModes(getShader(_stencil_shader_id), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    // under same node, better use different uniform name for the same thing,
    // seems like a bug in osg that same name uniform may have conflicts, overriding each other.
    sss->addUniform(new osg::Uniform("u_projectionMat", osg::Matrixf(_mainCamera->getProjectionMatrix())));
    sss->addUniform(new osg::Uniform("u_viewMat", osg::Matrixf(_mainCamera->getViewMatrix())));
    sss->setUpdateCallback(new LightStencilCallback(_mainCamera));
    
    std::vector<PointLight *> &pointLights = _lightGroup->getPointLightsArrayReference();
    for (std::vector<PointLight *>::iterator it = pointLights.begin(); it != pointLights.end(); it++)
    {
        osg::ref_ptr<osg::MatrixTransform> mt = (*it)->getLightSphereTransformNode();
        stencilGroup->addChild(mt);
    }
    
    _lightPassGroupNode->addChild(stencilGroup);
}

void LightingPass::configPointLightPassStateSet()
{
    osg::ref_ptr<osg::Group> lightRenderGroup(new osg::Group);
    osg::ref_ptr<osg::StateSet> lss = lightRenderGroup->getOrCreateStateSet();
    
    osg::ref_ptr<osg::Stencil> stencil(new osg::Stencil);
    stencil->setFunction(osg::Stencil::NOTEQUAL, 0, 0xFF);
    //stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::KEEP);
    stencil->setWriteMask(0);
    
    //lss->setAttributeAndModes(stencil, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    
    // turn off depth test for correct color blending
    lss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
//    osg::ref_ptr<osg::Depth> depth(new osg::Depth);
//    depth->setWriteMask(true);
//    depth->setFunction(osg::Depth::LESS);
//    lss->setAttribute(depth);
    
    
//    // set for blending
    osg::ref_ptr<osg::BlendFunc> blendFunc(new osg::BlendFunc);
    osg::ref_ptr<osg::BlendEquation> blendEquation(new osg::BlendEquation);
    blendFunc->setFunction(GL_ONE, GL_ONE);
    blendEquation->setEquation(osg::BlendEquation::FUNC_ADD);
    
    lss->setMode(GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    lss->setAttributeAndModes(blendFunc, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    lss->setAttributeAndModes(blendEquation, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    
    // cull front face so that light is also visable when camera is inside the light sphere
    lss->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    osg::ref_ptr<osg::CullFace> cf(new osg::CullFace(osg::CullFace::FRONT));
    lss->setAttribute(cf, osg::StateAttribute::OVERRIDE);
   
    // setup shader
    lss->setAttributeAndModes(getShader(_light_shader_id), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    
    // global uniforms
    lss->addUniform(new osg::Uniform("u_albedoTex", 0));
    lss->addUniform(new osg::Uniform("u_normalAndDepthTex", 1));
    lss->addUniform(new osg::Uniform("u_positionTex", 2));
    lss->addUniform(new osg::Uniform("u_inverseScreenSize", osg::Vec2f(1.0f/_screenWidth, 1.0f/_screenHeight)));
    
    lss->setTextureAttributeAndModes(0, ScreenPass::getInTexture(_diffuse_tex_id));
    lss->setTextureAttributeAndModes(1, ScreenPass::getInTexture(_normal_tex_id));
    lss->setTextureAttributeAndModes(2, ScreenPass::getInTexture(_position_tex_id));
    
//    //debug
//    lss->addUniform(new osg::Uniform("u_debug1", 4));
//    lss->setTextureAttributeAndModes(4, _sharedGeomPassDepthStencilTex);
//    
    lss->setRenderBinDetails(1, "RenderBin");
    
    // specific uniforms and states
    osg::Matrix &mainCameraModelViewMatrix = _mainCamera->getViewMatrix();
    std::vector<PointLight *> &pointLights = _lightGroup->getPointLightsArrayReference();
    for (std::vector<PointLight *>::iterator it = pointLights.begin(); it != pointLights.end(); it++)
    {
        // float radius = (*it)->_light_effective_radius;
        osg::Vec3f lightPosInViewSpace = (*it)->getPosition() * mainCameraModelViewMatrix;
        
        osg::ref_ptr<osg::MatrixTransform> lightModelTransform = (*it)->getLightSphereTransformNode();
        
        // create addition layer so that the state changes won't affect the prev stencil pass
        osg::ref_ptr<osg::Group> lightStateNode(new osg::Group);
        lightStateNode->addChild(lightModelTransform);
        lightRenderGroup->addChild(lightStateNode);
        osg::ref_ptr<osg::StateSet> ss = lightStateNode->getOrCreateStateSet();

        ss->setUpdateCallback(new LightCallback(_mainCamera, (*it)));
        ss->addUniform(new osg::Uniform("u_lightPosition", lightPosInViewSpace));
        ss->addUniform(new osg::Uniform("u_lightAmbient", (*it)->getAmbient()));
        ss->addUniform(new osg::Uniform("u_lightDiffuse", (*it)->getDiffuse()));
        ss->addUniform(new osg::Uniform("u_lightSpecular", (*it)->getSpecular()));
        ss->addUniform(new osg::Uniform("u_lightIntensity", (*it)->getIntensity()));
        ss->addUniform(new osg::Uniform("u_lightAttenuation", (*it)->getAttenuation()));
        // ss->addUniform(new osg::Uniform("u_lightRadius", radius));
        
        ss->addUniform(new osg::Uniform("u_projMatrix", osg::Matrixf(_mainCamera->getProjectionMatrix())));
        ss->addUniform(new osg::Uniform("u_viewMatrix", osg::Matrixf(_mainCamera->getViewMatrix())));
    }
    _lightPassGroupNode->addChild(lightRenderGroup);
}

void LightingPass::configureStateSet()
{
    osg::ref_ptr<osg::Group> lightPassGroup(new osg::Group);
    _lightPassGroupNode = lightPassGroup;
    
    // disable depth write
    osg::ref_ptr<osg::Depth> depth(new osg::Depth);
    depth->setWriteMask(false);
    depth->setFunction(osg::Depth::LESS);
    _lightPassGroupNode->getOrCreateStateSet()->setAttribute(depth);
    
    // enable stencil test for correct light volume bounding
    _lightPassGroupNode->getOrCreateStateSet()->setMode(GL_STENCIL_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    //configStencilPassStateSet();
    configPointLightPassStateSet();
}

int LightingPass::addOutTexture()
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    
    tex->setTextureSize(_screenWidth, _screenHeight);
    tex->setSourceType(GL_UNSIGNED_BYTE);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA);
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}
