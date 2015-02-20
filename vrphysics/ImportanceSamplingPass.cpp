//
//  ImportanceSamplingPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 1/24/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "ImportanceSamplingPass.h"
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/ShapeDrawable>
#include <osg/Point>

#include "ShadowGroup.h"
#include "DirectionalLightGroup.h"

ImportanceSamplingPass::ImportanceSamplingPass(osg::Camera *mainCamera, ShadowGroup *sg, DirectionalLightGroup *dlg)
: ScreenPass(mainCamera), _shadowGroup(sg), _dirLightGroup(dlg), _impSampleEnabled(false), _splatsSize(0)
{
    _rsmWidth = sg->getRsmWidth();
    _rsmHeight = sg->getRsmHeight();
    
    _mipMapCameraGroup = new osg::Group;
    _importanceSampleCameraGroup = new osg::Group;
    
    _importanceSamplingShaderId = addShader("importanceSampling.vert", "importanceSampling.frag");
    _mipMapShaderId = addShader("fluxMipmap.vert", "fluxMipmap.frag");
   
    // ! cautious, this call sets the _splatsSize to poisson texture size;
    loadPoissonTexture();
    // generatePoissonTexture();
    // configDebugPoints();
   
    // screen Quad needs to be created before attaching it to the cameras
    _screenQuad = createTexturedQuad();
    
    // enqueue flux images
    std::vector<int> dirLightIds = _dirLightGroup->getAllLightIds();
    
    // TODO: this is hard code. when light group changes, need a way to notify to this class
    // TODO: add omni/spot lights later
    for(int i = 0; i < dirLightIds.size(); i++)
    {
        osg::ref_ptr<osg::TextureRectangle> inFluxTex = _shadowGroup->getDirLightDirFluxTexture( dirLightIds[i] );
        int in_tex_id = addInTexture(inFluxTex);
        _in_flux_mipmap_ids.insert(std::make_pair(dirLightIds[i], in_tex_id));
        
        osg::ref_ptr<osg::Texture2D> impOutTex = createImportanceSampleOutTexture();
        _out_sample_textures.insert(std::make_pair(dirLightIds[i], impOutTex));
        
        
        osg::ref_ptr<osg::Texture2D> mipMapOut = createMipMapTexture();
        _mipMapOut = mipMapOut;
        
        addMipMapCamera(inFluxTex, mipMapOut);
        addImportanceSampleCamera(mipMapOut, impOutTex);
    }
    
    // order dependent
    _rootGroup->addChild(_mipMapCameraGroup);
    _rootGroup->addChild(_importanceSampleCameraGroup);
}

ImportanceSamplingPass::~ImportanceSamplingPass()
{
}

osg::ref_ptr<osg::Texture2D> ImportanceSamplingPass::createImportanceSampleOutTexture()
{
    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
    
    // tex->setTextureSize(_splatsSize, _splatsSize);
    tex->setTextureSize(_splatsSize, _splatsSize);
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA32F_ARB);
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    
    return tex;
}

osg::ref_ptr<osg::Texture2D> ImportanceSamplingPass::createMipMapTexture()
{
    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
    
    tex->setTextureSize(_rsmWidth, _rsmHeight);
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGB8);
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::NEAREST_MIPMAP_LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    
    return tex;
}

void ImportanceSamplingPass::configureStateSet()
{
}

void ImportanceSamplingPass::configRTTCamera()
{
}

void ImportanceSamplingPass::addImportanceSampleCamera(osg::Texture2D *mipMapIn, osg::Texture2D *outTex)
{
    osg::Camera *cam = new osg::Camera;
    cam->addChild(_screenQuad);
    // cam->addChild(_debugPoints);
    
    cam->setClearColor(osg::Vec4(1, 1, 1, 1));
    cam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    cam->setRenderOrder(osg::Camera::PRE_RENDER);
    
    cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    cam->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
    
    // cam->setViewport(0, 0, _splatsSize, _splatsSize);
    cam->setViewport(0, 0, _splatsSize, _splatsSize);
    
    cam->attach(osg::Camera::COLOR_BUFFER0, outTex);
    _importanceSampleCameraGroup->addChild(cam);
    
    // set stateset
    osg::StateSet *impSS = cam->getOrCreateStateSet();
    impSS->setAttributeAndModes(getShader(_importanceSamplingShaderId), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    impSS->addUniform(new osg::Uniform("u_mipMapFlux", 0));
    impSS->addUniform(new osg::Uniform("u_mipMapSize", osg::Vec2(_rsmWidth, _rsmHeight)));
    impSS->addUniform(new osg::Uniform("u_splatSize", _splatsSize));
    impSS->addUniform(new osg::Uniform("u_poissowTex", 1));
    impSS->setTextureAttribute(0, mipMapIn);
    impSS->setTextureAttribute(1, _poissowTex);
}

void ImportanceSamplingPass::addMipMapCamera(osg::TextureRectangle *fluxMap, osg::Texture2D *mipTexOut)
{
    osg::Camera *mipMapCamera = new osg::Camera;
    mipMapCamera->addChild(_screenQuad);
   
    mipMapCamera->setClearColor(osg::Vec4());
    mipMapCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mipMapCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    mipMapCamera->setRenderOrder(osg::Camera::PRE_RENDER);
   
    mipMapCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    mipMapCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
   
    mipMapCamera->setViewport(0, 0, _rsmWidth, _rsmHeight);
 
    mipMapCamera->attach(osg::Camera::COLOR_BUFFER0, mipTexOut);
    _mipMapCameraGroup->addChild(mipMapCamera);
   
    // set stateset
    osg::StateSet *mipSS = mipMapCamera->getOrCreateStateSet();
    mipSS->setAttributeAndModes(getShader(_mipMapShaderId), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    mipSS->addUniform(new osg::Uniform("u_rsmFlux", 0));
    mipSS->setTextureAttributeAndModes(0, fluxMap);
}

void ImportanceSamplingPass::loadPoissonTexture()
{
    _poissowTex = new osg::Texture2D;
    _poissowTex->setSourceFormat(GL_FLOAT);
    _poissowTex->setSourceType(GL_FLOAT);
    _poissowTex->setSourceFormat(GL_RGB);
    _poissowTex->setInternalFormat(GL_RGB32F_ARB);
  
//    _poissowTex->setImage(osgDB::readImageFile("poissow.tga"));
    _poissowTex->setWrap(osg::TextureRectangle::WRAP_S, osg::TextureRectangle::CLAMP_TO_EDGE);
    _poissowTex->setWrap(osg::TextureRectangle::WRAP_T, osg::TextureRectangle::CLAMP_TO_EDGE);
    _poissowTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    _poissowTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
   
    std::vector<osg::Vec3> texcoord;
    std::string path = osgDB::findDataFile("poisson.txt");
    std::ifstream infile(path.c_str());
    if(infile.fail())
    {
        fprintf(stderr, "poisson sample file fails to open\n");
        exit(0);
    }
    float x, y;
    int count = 0;
    while(infile >> x >> y)
    {
        osg::Vec3 v(x, y, 0);
        texcoord.push_back(v);
        count++;
    }
    
    infile.close();
    
    int size = ceilf(sqrtf(count));
    
    // !IMPORTANT HIGHLY COUPLED
    _splatsSize = size;
    // ! =======================
    
    osg::ref_ptr<osg::Image> poissonImage(new osg::Image);
    poissonImage->allocateImage(size, size, 1, GL_RGB, GL_FLOAT);
    poissonImage->setInternalTextureFormat(GL_RGB32F_ARB);
    
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            osg::Vec3 *v = (osg::Vec3 *)poissonImage->data(j, i);
            int index = i * size + j;
            *v = texcoord[index];
        }
    }
   
    _poissowTex->setTextureSize(size, size);
    _poissowTex->setImage(poissonImage);
}

void ImportanceSamplingPass::generatePoissonTexture()
{
    float taps[64*64][3];
    
    int nTaps = 0;
    
    float max = 0.0f;
    srand(time(NULL));
    taps[ nTaps ][ 0 ] = rand() / 32767.0f;
    taps[ nTaps ][ 1 ] = rand() / 32767.0f;
    taps[ nTaps ][ 2 ] = 1.0f;
    nTaps++;
    while ( nTaps < 64 * 64 )
    {
        float bestDist = 0.0f, bestX = 0.0f, bestY = 0.0f, minDist;
        
        for ( int i = 0; i < 60; i++ )
        {
            float x = rand() / 32767.0f, y = rand() / 32767.0f;
            
            minDist = 1e37f;
            for ( int j = 0; j < nTaps; j++ ) {
                float dist = fmax( 0.0f, sqrtf( ( taps[ j ][ 0 ] - x ) * ( taps[ j ][ 0 ] - x ) +
                                              ( taps[ j ][ 1 ] - y ) * ( taps[ j ][ 1 ] - y ) ) );
                
                minDist = fmin( minDist, dist );
            }
            if ( minDist > bestDist ) {
                bestDist = minDist; bestX = x; bestY = y;
            }
        }
        if(bestX > max) max = bestX;
        if(bestY > max) max = bestY;
        
        taps[ nTaps ][ 0 ] = bestX;	// x
        taps[ nTaps ][ 1 ] = bestY;	// y
        taps[ nTaps ][ 2 ] = 1.0f;	// weight
        nTaps ++;
    }
    
    for(int i = 0; i < nTaps; i++)
    {
        taps[i][0] /= max;
        taps[i][1] /= max;
    }
    
    osg::ref_ptr<osg::Image> possiowTexImg(new osg::Image);
    possiowTexImg->allocateImage(64, 64, 1, GL_RGBA, GL_FLOAT);
    possiowTexImg->setInternalTextureFormat(GL_RGBA32F_ARB);
    for(int i = 0; i < 64; i++)
    {
        for(int j = 0; j < 64; j++)
        {
            osg::Vec4 *data = (osg::Vec4 *)possiowTexImg->data(j, i);
            int index = i * 64 + j;
            osg::Vec4 v = osg::Vec4(taps[index][0], taps[index][1], taps[index][2], 1.0);
            *data = v;
        }
    }
    osg::ref_ptr<osg::Image> _test(new osg::Image);
    _test = osgDB::readImageFile("tablewood.jpg");
    
    _poissowTex = new osg::Texture2D;
    _poissowTex->setSourceFormat(GL_FLOAT);
    _poissowTex->setSourceType(GL_FLOAT);
    _poissowTex->setSourceFormat(GL_RGBA);
    _poissowTex->setInternalFormat(GL_RGBA32F_ARB);
    _poissowTex->setTextureSize(64, 64);
    
    _poissowTex->setImage(possiowTexImg);
    _poissowTex->setWrap(osg::TextureRectangle::WRAP_S, osg::TextureRectangle::CLAMP_TO_EDGE);
    _poissowTex->setWrap(osg::TextureRectangle::WRAP_T, osg::TextureRectangle::CLAMP_TO_EDGE);
    _poissowTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    _poissowTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
}

std::vector<int> ImportanceSamplingPass::getSampledLightIds()
{
    std::vector<int> res;
    res.resize(getNumOutImportanceSampleTextures());
    int count = 0;
    for(std::map<int, osg::ref_ptr<osg::Texture2D> >::iterator it = _out_sample_textures.begin(); it != _out_sample_textures.end(); ++it)
    {
        res[count] = it->first;
        count++;
    }
    return res;
}

void ImportanceSamplingPass::configDebugPoints()
{
    _debugPoints = new osg::Group;
    osg::ref_ptr<osg::Geode> geode(new osg::Geode);

    osg::ref_ptr<osg::Vec3Array> pointCoords = new osg::Vec3Array; // vertex coords
    for(int i = 0; i < _splatsSize; i++)
    {
        for(int j = 0; j < _splatsSize; j++)
        {
//            printf("%.4f, %.4f\n", i/(float)_splatsSize, j/(float)_splatsSize);
            pointCoords->push_back(osg::Vec3( float(j) / _splatsSize,
                                              float(i) / _splatsSize,
                                              0.0f) );
        }
    }
    
    osg::ref_ptr<osg::Geometry> quad_geom = new osg::Geometry;
    osg::ref_ptr<osg::DrawArrays> quad_da = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, (int)pointCoords->size());
    
    quad_geom->setVertexArray(pointCoords.get());
    quad_geom->addPrimitiveSet(quad_da.get());
    
    auto _StateSet = quad_geom->getOrCreateStateSet();
    _StateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    _StateSet->setAttribute( new osg::Point( 1.0f ), osg::StateAttribute::ON);
    
    geode->addDrawable(quad_geom.get());
    _debugPoints->addChild(geode);
}