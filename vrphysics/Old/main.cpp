#include <stdio.h>
#include <vector>

#include <osgViewer/Viewer>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Camera>
#include <osg/ref_ptr>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>

#include "GBuffer.h"
#include "Shared.h"
#include "PointLight.h"

osg::Camera *createHUDCamera(double left,
                             double right,
                             double bottom,
                             double top)
{
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    camera->setRenderOrder(osg::Camera::POST_RENDER);
    camera->setAllowEventFocus(false);
    camera->setProjectionMatrix(osg::Matrix::ortho2D(left, right, bottom, top));
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    return camera.release();
}

osg::Geode *createScreenQuad(float width,
                             float height,
                             float scale,
                             osg::Vec3 corner)
{
    osg::Geometry* geom = osg::createTexturedQuadGeometry(
                                                          corner,
                                                          osg::Vec3(width, 0, 0),
                                                          osg::Vec3(0, height, 0),
                                                          0,
                                                          0,
                                                          scale,
                                                          scale);
    osg::ref_ptr<osg::Geode> quad = new osg::Geode;
    quad->addDrawable(geom);
    int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
    quad->getOrCreateStateSet()->setAttribute(
                                              new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,
                                                                   osg::PolygonMode::FILL),
                                              values);
    quad->getOrCreateStateSet()->setMode(GL_LIGHTING, values);
    return quad.release();
}

osg::ref_ptr<osg::Camera> createTextureDisplayQuad(
                                                   const osg::Vec3 &pos,
                                                   osg::StateAttribute *tex,
                                                   float scale,
                                                   float width,
                                                   float height)
{
    osg::ref_ptr<osg::Camera> hc = createHUDCamera(0,1,0,1);
    hc->addChild(createScreenQuad(width, height, scale, pos));
    hc->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
    return hc;
}

osg::ref_ptr<osg::LightSource> createLight(const osg::Vec3 &pos)
{
    osg::ref_ptr<osg::LightSource> light = new osg::LightSource;
    light->getLight()->setPosition(osg::Vec4(pos.x(), pos.y(), pos.z(), 1));
    light->getLight()->setAmbient(osg::Vec4(0.2, 0.2, 0.2, 1));
    light->getLight()->setDiffuse(osg::Vec4(0.8, 0.8, 0.8, 1));
    return light;
}

float randomf(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void addRandomLight()
{
    // create a random light that is positioned on bounding sphere of scene (skRadius)
    PointLight l;
    osg::Vec3f posOnSphere(randomf(-1.0f, 1.0f), randomf(-1.0f, 1.0f), randomf(-1.0f, 1.0f));
    posOnSphere.normalize();
    posOnSphere *= randomf(0.95f, 1.05f);
    
    osg::Vec3f orbitAxis(randomf(0.0f, 1.0f), randomf(0.0f, 1.0f), randomf(0.0f, 1.0f));
    orbitAxis.normalize();
    l.orbitAxis = orbitAxis;
    
    posOnSphere *= (Shared::skRadius-1);
    
    l.setPosition(posOnSphere);
    l.setAmbient(0.0f, 0.0f, 0.0f);
    
    osg::Vec3f col(randomf(0.3f, 0.5f), randomf(0.2f, 0.4f), randomf(0.7f, 1.0f));
    l.setDiffuse(col.x(), col.y(), col.z());
    l.setSpecular(col.x(), col.y(), col.z());
    l.setAttenuation(0.0f, 0.0f, 0.2f); // set constant, linear, and exponential attenuation
    l.intensity = 0.8f;
    
    Shared::pointLights.push_back(l);
}


void setupLights()
{
    for (unsigned int i=0; i<Shared::light_num; i++)
    {
        addRandomLight();
    }
}

void randomizeLightColors()
{
    for (std::vector<PointLight>::iterator it = Shared::pointLights.begin(); it != Shared::pointLights.end(); it++) {
        osg::Vec3f col(randomf(0.4f, 1.0f), randomf(0.1f, 1.0f), randomf(0.3f, 1.0f));
        it->setDiffuse(col.x(), col.y(), col.z());
    }
}

void initShared()
{
    Shared::light_num = 100;
    Shared::win_height = 800;
    Shared::win_width = 600;
    
    osg::Camera *cam = Shared::viewer.getCamera();
    osg::Matrix projMatrix = cam->getProjectionMatrix();
    float near;
    float far;
    float dummy;
    projMatrix.getFrustum(dummy, dummy, dummy, dummy, near, far);
    Shared::nearPlane = near;
    Shared::farPlane = far;
}

void addLightModels(osg::ref_ptr<osg::Group> sceneRoot)
{
    osg::ref_ptr<osg::Sphere> sphere(new osg::Sphere(osg::Vec3(0, 0, 0), 2.0));
    osg::ref_ptr<osg::ShapeDrawable> shapeDraw(new osg::ShapeDrawable);
    shapeDraw->setShape(sphere);
    osg::ref_ptr<osg::Geode> sphereGeode(new osg::Geode);
//    osg::ref_ptr<osg::PolygonMode> polyMode(new osg::PolygonMode);
//    polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
//    boxGeode->getOrCreateStateSet()->setAttribute(polyMode);
    sphereGeode->addDrawable(shapeDraw);
    
    
    osg::Matrix scale;
    scale.makeScale(osg::Vec3f(0.25f, 0.25f, 0.25f));
    
    osg::ref_ptr<osg::MatrixTransform> transform(new osg::MatrixTransform(scale));
    for (std::vector<PointLight>::iterator it = Shared::pointLights.begin();
         it != Shared::pointLights.end(); it++)
    {
        osg::Matrix translate;
        translate.makeTranslate(it->getPosition());
        osg::ref_ptr<osg::MatrixTransform> transform(new osg::MatrixTransform(translate * scale));
        sceneRoot->addChild(transform);
        transform->addChild(sphereGeode);
    }
}

void buildInterior(osg::ref_ptr<osg::Group> sceneRoot)
{
//
//    osg::ref_ptr<osg::Plane> plane(new osg::Plane(osg::Vec4f(0, 0, 0, 1)));
//    osg::ref_ptr<osg::ShapeDrawable> shapeDraw(new osg::ShapeDrawable);
//    osg::ref_ptr<osg::Geode> sphereGeode(new osg::Geode);

}

int main()
{
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("cessna.osg");
    osg::ref_ptr<osg::Group> sceneRoot(new osg::Group);
    initShared();
    setupLights();
    
    // add models
    sceneRoot->addChild(loadedModel);
    addLightModels(sceneRoot);
    
    Shared::gBuffer.init(sceneRoot, 800, 600);
    Shared::gBuffer.bindGeometry(Shared::nearPlane, Shared::farPlane);
    Shared::gBuffer.bindLight(Shared::nearPlane, Shared::farPlane);
    
    Shared::viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

    
    osg::ref_ptr<osg::Camera> qTexN =
    createTextureDisplayQuad(osg::Vec3(0, 0.7, 0),
                             Shared::gBuffer.getTexture(GBuffer::kGBuffer_texture_diffuse),
                             1024, 0.3, 0.4);
    Shared::gBuffer.getRoot()->addChild(qTexN);
    
    osg::ref_ptr<osg::Camera> qTexP =
    createTextureDisplayQuad(osg::Vec3(0.2, 0.0, 0),
                             Shared::gBuffer.getTexture(GBuffer::kGBuffer_texture_light),
                             1024, 0.8, 0.8);
    Shared::gBuffer.getRoot()->addChild(qTexP);
    
    Shared::viewer.setSceneData(Shared::gBuffer.getRoot());
    Shared::viewer.setUpViewInWindow(0, 0, 800, 600);
    Shared::viewer.realize();

    Shared::viewer.run();

    return 0;
}