//
//  GBuffer.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/6/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "GBuffer.h"
#include "LightPassCameraCallback.h"
#include "Shared.h"

GBuffer::GBuffer(): _w(0), _h(0), _geomRendering(false), _lightRendering(false) {}
GBuffer::~GBuffer() {}

void GBuffer::init(const osg::ref_ptr<osg::Group> &scene, int win_with, int win_height)
{
    _scene = scene;
    _w = win_with;
    _h = win_height;
    _root = osg::ref_ptr<osg::Group>(new osg::Group);
    
    loadPassesCameras();
}

void GBuffer::loadPassesCameras()
{
    _geomPassCamera = geomPassCamera();
    _lightPassCamera = lightPassCamera();
}


static osg::ref_ptr<osg::Texture2D> createRenderTexture(int width, int height)
{
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
    texture->setTextureSize(width,height);
    
    texture->setInternalFormat(GL_RGBA16F_ARB);
    
    texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_EDGE);
    texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
    texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
    
    texture->setSourceType(GL_FLOAT);
    
//    osg::ref_ptr<osg::TextureRectangle> tex2D(new osg::TextureRectangle);
//    tex2D->setTextureSize(_w, _h);
//    tex2D->setInternalFormat(GL_RGBA16F_ARB);
//    tex2D->setSourceFormat(GL_RGBA);
//    tex2D->setSourceType(GL_FLOAT);
//    return tex2D;
    
    return texture;
}

osg::ref_ptr<osg::Camera> GBuffer::geomPassCamera()
{
    osg::ref_ptr<osg::Program> geometry_pass_shader(new osg::Program);
    geometry_pass_shader->addShader(osgDB::readShaderFile("gbuffer.vert"));
    geometry_pass_shader->addShader(osgDB::readShaderFile("gbuffer.frag"));
    
    auto diffuse_tex_rec = createTextureRectangle();
   // auto diffuse_tex_rec = createRenderTexture(_w, _h);
    diffuse_tex_rec->setDataVariance(osg::Object::DYNAMIC);
    
    _diffuse_tex = diffuse_tex_rec;
    
    // Bind diffuse_tex to gl_FragData[0];
    auto diffuse_cam = createRTTCamera(osg::Camera::COLOR_BUFFER0, diffuse_tex_rec, false);
    diffuse_cam->setRenderOrder(osg::Camera::PRE_RENDER, 0);
    auto ss = diffuse_cam->getOrCreateStateSet();
    ss->setAttributeAndModes(geometry_pass_shader, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    
    auto diffuse_tex_image = createTextureImage("random.png");
    ss->setTextureAttributeAndModes(0, diffuse_tex_image);
    ss->addUniform(new osg::Uniform("u_texture", 0));
    
    auto normal_tex_rc = createTextureRectangle();
    normal_tex_rc->setDataVariance(osg::Object::DYNAMIC);
    _normal_tex = normal_tex_rc;
    
    // Bind normal_tex to gl_FragData[1];
    diffuse_cam->attach(osg::Camera::COLOR_BUFFER1, _normal_tex);
    
    diffuse_cam->addChild(_scene);
    
    return diffuse_cam;
}


osg::ref_ptr<osg::Camera> GBuffer::lightPassCamera()
{
    osg::ref_ptr<osg::Program> lighting_pass_shader(new osg::Program);
    lighting_pass_shader->addShader(osgDB::readShaderFile("pointLightPass.vert"));
    lighting_pass_shader->addShader(osgDB::readShaderFile("pointLightPass.frag"));
    
    auto light_tex_rec = createTextureRectangle();
    _light_tex = light_tex_rec;
    
    auto light_cam = createRTTCamera(osg::Camera::COLOR_BUFFER0, light_tex_rec, false);
    light_cam->setRenderOrder(osg::Camera::PRE_RENDER, 1);
    auto ss = light_cam->getOrCreateStateSet();
    
    ss->setAttributeAndModes(lighting_pass_shader, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    
    ss->addUniform(new osg::Uniform("u_albedoTex", 0));
    ss->addUniform(new osg::Uniform("u_normalAndDepthTex", 1));
    ss->addUniform(new osg::Uniform("u_inverseScreenSize", osg::Vec2f(1.0f/_w, 1.0f/_h)));
    
    ss->setTextureAttributeAndModes(0, _diffuse_tex);
    ss->setTextureAttributeAndModes(1, _normal_tex);
    
    light_cam->addChild(_scene);
    light_cam->setUpdateCallback(new LightPassCameraCallback());
    
    return light_cam;
}


void GBuffer::bindGeometry(float near, float far)
{
    if(_geomPassCamera)
    {
        _geomPassCamera->getOrCreateStateSet()->addUniform(new osg::Uniform("u_farDistance", far));
        
        if(!_geomRendering)
        {
            _root->addChild(_geomPassCamera);
            _geomRendering = true;
        }
    }
}

void GBuffer::unbindGeometry()
{
    if(_geomPassCamera && _geomRendering)
    {
        _root->removeChild(_geomPassCamera);
        _geomRendering = false;
    }
}

void GBuffer::bindLight(float near, float far)
{
    if(_lightPassCamera)
    {
        auto ss = _lightPassCamera->getOrCreateStateSet();
        ss->addUniform(new osg::Uniform("u_numLights", Shared::light_num));
        ss->addUniform(new osg::Uniform("u_farDistance", far));
        
        osg::Matrix mainCameraModelViewMatrix = Shared::viewer.getCamera()->getViewMatrix();
        for (std::vector<PointLight>::iterator it = Shared::pointLights.begin(); it != Shared::pointLights.end(); it++)
        {
            float radius = it->intensity * Shared::skMaxPointLightRadius;
            
            // SHADING/LIGHTING CALCULATION
            // this pass draws the spheres representing the area of influence each light has
            // in a fragment shader, only the pixels that are affected by the drawn geometry are processed
            // drawing light volumes (spheres for point lights) ensures that we're only running light calculations on
            // the areas that the spheres affect
            osg::Vec3f lightPosInViewSpace = it->getPosition() * mainCameraModelViewMatrix;
            
            ss->addUniform(new osg::Uniform("u_lightPosition", lightPosInViewSpace));
            ss->addUniform(new osg::Uniform("u_lightAmbient", it->ambient));
            ss->addUniform(new osg::Uniform("u_lightDiffuse", it->diffuse));
            ss->addUniform(new osg::Uniform("u_lightSpecular", it->specular));
            ss->addUniform(new osg::Uniform("u_lightIntensity", it->intensity));
            ss->addUniform(new osg::Uniform("u_lightAttenuation", it->attenuation));
            
            ss->addUniform(new osg::Uniform("u_lightRadius", radius));
        }
        
        
        if(!_lightRendering)
        {
            _root->addChild(_lightPassCamera);
            _lightRendering = true;
        }
    }
}

void GBuffer::unbindLight()
{
    if(_lightPassCamera && _lightRendering)
    {
        _root->removeChild(_lightPassCamera);
        _lightRendering = false;
    }
}

void GBuffer::resetLightPass()
{
    
}

void GBuffer::drawDebug(int x, int y)
{
    
}

osg::ref_ptr<osg::Group> GBuffer::getRoot()
{
    return _root;
}

osg::ref_ptr<osg::Texture> GBuffer::getTexture(GBuffer_texture_type type)
{
    switch (type) {
        case kGBuffer_texture_diffuse:
            return _diffuse_tex;
            break;
        case kGBuffer_texture_light:
            return _light_tex;
            break;
        case kGBuffer_texture_normal:
            return _normal_tex;
        default:
            break;
    }
    return 0;
}

osg::ref_ptr<osg::TextureRectangle> GBuffer::createTextureRectangle()
{
    osg::ref_ptr<osg::TextureRectangle> tex2D(new osg::TextureRectangle);
    tex2D->setTextureSize(_w, _h);
    tex2D->setInternalFormat(GL_RGBA16F_ARB);
    tex2D->setSourceFormat(GL_RGBA);
    tex2D->setSourceType(GL_FLOAT);
    
    // why adding the below works???
//    tex2D->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
//    tex2D->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
//    tex2D->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
//    tex2D->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    
    return tex2D;
}

osg::ref_ptr<osg::Texture2D> GBuffer::createTextureImage(const char *imageName)
{
    osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
    texture->setImage(osgDB::readImageFile(imageName));
    texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    return texture;
}

osg::ref_ptr<osg::Camera> GBuffer::createRTTCamera(osg::Camera::BufferComponent buffer,
                                                   osg::Texture *tex,
                                                   bool isAbsolute)
{
    osg::ref_ptr<osg::Camera> camera(new osg::Camera);
    camera->setClearColor(osg::Vec4());
    camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    camera->setRenderOrder(osg::Camera::PRE_RENDER);
    camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    if (tex)
    {
        tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
        camera->setViewport(0, 0, tex->getTextureWidth(), tex->getTextureHeight());
        camera->attach(buffer, tex);
    }
    if (isAbsolute)
    {
        camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0));
        camera->setViewMatrix(osg::Matrix::identity());
        //camera->addChild(createScreenQuad(1.0f, 1.0f));
    }
    return camera;
}
