/* -*-c++-*- OpenSceneGraph Cookbook
 * Chapter 6 Recipe 7
 * Author: Wang Rui <wangray84 at gmail dot com>
 */

#ifndef H_COOKBOOK_CH6_SKYBOX
#define H_COOKBOOK_CH6_SKYBOX

#include <osg/TextureCubeMap>
#include <osg/Transform>
#include <osg/Shape>

class SkyBox
{
public:
    SkyBox(osg::Camera *camera);
    
    void setEnvironmentMap( osg::Image* posX, osg::Image* negX,
                           osg::Image* posY, osg::Image* negY, osg::Image* posZ, osg::Image* negZ );
    void setSceneBound(float radius);
    
    inline osg::ref_ptr<osg::Group> getRootNode()
    {
        return _skyBoxRoot;
    }
    
protected:
    virtual ~SkyBox() {}
    
    osg::ref_ptr<osg::Group> _skyBoxRoot;
    osg::ref_ptr<osg::Sphere> _sphere;
    osg::ref_ptr<osg::MatrixTransform> _matTrans;
    
    osg::ref_ptr<osg::Camera> _mainCamera;
};

class SkyBoxCallback : public osg::StateSet::Callback
{
public:
    SkyBoxCallback(osg::Camera *mainCamera);
    void operator()(osg::StateSet *ss, osg::NodeVisitor* nv);
    
private:
    osg::ref_ptr<osg::Camera> _mainCamera;
};

class SkyBoxPositionCallback : public osg::NodeCallback
{
public:
    SkyBoxPositionCallback(osg::Camera *mainCamera) : _mainCamera(mainCamera) {}
    virtual void operator()( osg::Node* node,osg::NodeVisitor* nv );
private:
    osg::ref_ptr<osg::Camera> _mainCamera;
};

#endif
