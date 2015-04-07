/* -*-c++-*- OpenSceneGraph Cookbook
 * Chapter 6 Recipe 7
 * Author: Wang Rui <wangray84 at gmail dot com>
 */

#include <vector>
#include <osg/Depth>
#include <osgUtil/CullVisitor>
#include <osgDB/ReadFile>
#include <osg/Image>

#include "Skybox.h"
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

SkyBox::SkyBox(osg::Camera *mainCamera)
: _mainCamera(mainCamera)
{
//    setReferenceFrame( osg::Transform::ABSOLUTE_RF );
//    setCullingActive( false );
//    
//    osg::StateSet* ss = getOrCreateStateSet();
//    ss->setAttributeAndModes( new osg::Depth(osg::Depth::LEQUAL, 1.0f, 1.0f) );
//    ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
//    ss->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
//    ss->setRenderBinDetails( 5, "RenderBin" );
    
    _skyBoxRoot = new osg::Group;
    _matTrans = new osg::MatrixTransform;
    
    osg::ref_ptr<osg::StateSet> ss = _skyBoxRoot->getOrCreateStateSet();
    osg::ref_ptr<osg::Program> skyBoxShader = new osg::Program();
    skyBoxShader->addShader(osgDB::readShaderFile("skybox.vert"));
    skyBoxShader->addShader(osgDB::readShaderFile("skybox.frag"));
    ss->setAttributeAndModes(skyBoxShader, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        ss->setAttributeAndModes( new osg::Depth(osg::Depth::LEQUAL, 1.0f, 1.0f) );
        ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
        ss->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
        ss->setRenderBinDetails( 5, "RenderBin" );
    
    // uniforms
    ss->addUniform(new osg::Uniform("u_viewProjMat", osg::Matrixf()));
    ss->addUniform(new osg::Uniform("u_CubemapTexture", 0));
    
    _skyBoxRoot->setCullingActive(false);
    ss->setUpdateCallback(new SkyBoxCallback(_mainCamera));
    
    // set skydome
    osg::ref_ptr<osg::Geode> skyDomeGeode = new osg::Geode;
    osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable;
    _sphere = new osg::Sphere();
    _sphere->setRadius(10.0);
    sd->setShape(_sphere);
    skyDomeGeode->addDrawable(sd);
   
    _matTrans->addChild(skyDomeGeode);
    _matTrans->setUpdateCallback(new SkyBoxPositionCallback(_mainCamera));
    
    _skyBoxRoot->addChild(_matTrans);
}

void SkyBox::setSceneBound(float radius)
{
    _sphere->setRadius(radius);
}

void SkyBox::setEnvironmentMap(osg::Image* posX, osg::Image* negX,
                               osg::Image* posY, osg::Image* negY, osg::Image* posZ, osg::Image* negZ )
{
    if ( posX && posY && posZ && negX && negY && negZ )
    {
        osg::ref_ptr<osg::TextureCubeMap> cubemap = new osg::TextureCubeMap;
        cubemap->setImage( osg::TextureCubeMap::POSITIVE_X, posX );
        cubemap->setImage( osg::TextureCubeMap::NEGATIVE_X, negX );
        cubemap->setImage( osg::TextureCubeMap::POSITIVE_Y, posY );
        cubemap->setImage( osg::TextureCubeMap::NEGATIVE_Y, negY );
        cubemap->setImage( osg::TextureCubeMap::POSITIVE_Z, posZ );
        cubemap->setImage( osg::TextureCubeMap::NEGATIVE_Z, negZ );
        
        cubemap->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
        cubemap->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
        cubemap->setWrap( osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE );
        cubemap->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
        cubemap->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
        cubemap->setResizeNonPowerOfTwoHint( false );
        _skyBoxRoot->getOrCreateStateSet()->setTextureAttributeAndModes(0, cubemap.get() );
    }
}

SkyBoxCallback::SkyBoxCallback(osg::Camera *mainCamera) : _mainCamera(mainCamera)
{
}

void SkyBoxCallback::operator()(osg::StateSet *ss, osg::NodeVisitor *nv)
{
    osg::Matrixf viewProjMat = _mainCamera->getViewMatrix() * _mainCamera->getProjectionMatrix();
    ss->getUniform("u_viewProjMat")->set(viewProjMat);
}

void SkyBoxPositionCallback::operator()( osg::Node* node,osg::NodeVisitor* nv )
{
    osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
   
    // eye pos
    osg::Matrix eyeTrans;
    osg::Vec3 eye;
    osg::Vec3 dummy;
    _mainCamera->getViewMatrixAsLookAt(eye, dummy, dummy);
    eyeTrans.makeTranslate(eye);
    //mt->setMatrix(eyeTrans);
    
    traverse(node, nv);
}

