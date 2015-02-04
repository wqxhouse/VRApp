////
////  ImportanceSamplingPass.cpp
////  vrphysics
////
////  Created by Robin Wu on 1/24/15.
////  Copyright (c) 2015 WSH. All rights reserved.
////
//
//#include "ImportanceSamplingPass.h"
//
//ImportanceSamplingPass::ImportanceSamplingPass(osg::Camera *mainCamera, osg::TextureRectangle *fluxTex, int rsmWidth, int rsmHeight)
//: ScreenPass(mainCamera), _rsmWidth(rsmWidth), _rsmHeight(rsmHeight)
//{
//    _splatsSize = 64;
//    _mipMapCamera = new osg::Camera;
//    _tapTexture = new osg::Texture2D;
//    _in_flux_tex_id = addInTexture(fluxTex);
//    _out_importance_sample_tex_id = addOutTexture();
//   
//    _rootGroup->addChild(_mipMapCamera);
//    _rootGroup->addChild(_rttCamera);
//    
//    _screenQuad = createTexturedQuad();
//    
//    configMipMapCamera();
//    configRTTCamera();
//    configureStateSet();
//}
//
//ImportanceSamplingPass::~ImportanceSamplingPass()
//{
//}
//
//// overriding
//int ImportanceSamplingPass::addOutTexture()
//{
//    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
//    
//    tex->setTextureSize(_rsmWidth, _rsmHeight);
//    tex->setSourceType(GL_FLOAT);
//    tex->setSourceFormat(GL_RGBA);
//    tex->setInternalFormat(GL_RGBA8);
//    
//    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
//    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR_MIPMAP_LINEAR);
//    
//    _screenOutTexture.push_back(tex);
//    
//    return (int)_screenOutTexture.size() - 1;
//}
//
//osg::ref_ptr<osg::Texture2D> ImportanceSamplingPass::createMipMapTexture()
//{
//    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
//    
//    tex->setTextureSize(_splatsSize, _splatsSize);
//    tex->setSourceType(GL_FLOAT);
//    tex->setSourceFormat(GL_RGBA);
//    tex->setInternalFormat(GL_RGBA32F_ARB);
//    
//    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
//    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
//    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
//    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
//}
//
//void ImportanceSamplingPass::configureStateSet()
//{
//    
//}
//
//void ImportanceSamplingPass::configRTTCamera()
//{
//    _rttCamera->addChild(_screenQuad);
//    
//    _rttCamera->setClearColor(osg::Vec4());
//    _rttCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    _rttCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
//    _rttCamera->setRenderOrder(osg::Camera::PRE_RENDER);
//    
//    _rttCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
//    _rttCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
//    _rttCamera->setViewMatrix(osg::Matrix::identity());
//    
//    _rttCamera->setViewport(0, 0, _rsmWidth, _rsmHeight);
//    
//    _rttCamera->attach(osg::Camera::COLOR_BUFFER0, getOutputTexture(_out_importance_sample_tex_id), 
//}
//
//void ImportanceSamplingPass::configMipMapCamera()
//{
//    
//}