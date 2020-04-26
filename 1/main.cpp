/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description: Minimalistic project example that uses both Qt and OpenSceneGraph libraries.
 *
 *        Version:  1.0
 *        Created:  30-Jun-16 10:23:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Victoria Rudakova (vicrucann@gmail.com),
 *   Organization:  vicrucann.github.io
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osg/Camera>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Material>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>

#include <osgText/Font>
#include <osgText/Text>

namespace osgCookBook {
	osg::Camera* createHUDCamera(double left, double right, double
		bottom, double top)
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		camera->setRenderOrder(osg::Camera::POST_RENDER);
		camera->setAllowEventFocus(false);
		camera->setProjectionMatrix(
			osg::Matrix::ortho2D(left, right, bottom, top));
		camera->getOrCreateStateSet()->setMode(
			GL_LIGHTING, osg::StateAttribute::OFF);
		return camera.release();
	}

	osg::ref_ptr<osgText::Font> g_font = osgText::readFontFile("fonts/arial.ttf");
	osgText::Text* createText(const osg::Vec3& pos, const std::string&
		content, float size)
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setDataVariance(osg::Object::DYNAMIC);
		text->setFont(g_font.get());
		text->setCharacterSize(size);
		text->setAxisAlignment(osgText::TextBase::XY_PLANE);
		text->setPosition(pos);
		text->setText(content);
		return text.release();
	}

	class PickHandler : public osgGA::GUIEventHandler
	{
	public:
		// This virtual method must be overrode by subclasses.
		virtual void doUserOperations(
			osgUtil::LineSegmentIntersector::Intersection&) = 0;
		virtual bool handle(const osgGA::GUIEventAdapter& ea,
			osgGA::GUIActionAdapter& aa)
		{
			if (ea.getEventType() != osgGA::GUIEventAdapter::RELEASE
				|| ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON
				|| !(ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL))
				return false;

			osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
			if (viewer)
			{
				osg::ref_ptr<osgUtil::LineSegmentIntersector>
					intersector = new osgUtil::LineSegmentIntersector
					(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
				osgUtil::IntersectionVisitor iv(intersector.get());
				viewer->getCamera()->accept(iv);
				if (intersector->containsIntersections())
				{
					osgUtil::LineSegmentIntersector::Intersection&
						result = const_cast<osgUtil::LineSegmentIntersector::Intersection&>(*(intersector->getIntersections().begin()));
					doUserOperations(result);
				}
			}
			return false;
		}
	};
}

class RemoveShapeHandler : public osgCookBook::PickHandler
{
	virtual void doUserOperations(osgUtil::LineSegmentIntersector::
		Intersection& result)
	{
		if (result.nodePath.size() > 0)
		{
			osg::Geode* geode = dynamic_cast<osg::Geode*>(
				result.nodePath.back());
			if (geode) geode->removeDrawable(
				result.drawable.get());
		}
	}
};

class SetShapeColorHandler : public osgCookBook::PickHandler
{
	virtual void doUserOperations(osgUtil::LineSegmentIntersector
		::Intersection& result)
	{
		osg::ShapeDrawable* shape = dynamic_cast<osg::ShapeDrawable*>
			(result.drawable.get());
		if (shape) shape->setColor(osg::Vec4(
			1.0f, 1.0f, 1.0f, 2.0f) - shape->getColor());
	}
};

osg::Node* createMatrixTransform(osg::Geode* geode,
	const osg::Vec3& pos)
{
	osg::ref_ptr<osg::MatrixTransform> trans =
		new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(pos));
	trans->addChild(geode);
	return trans.release();
}

class ObserveShapeCallback : public osg::NodeCallback
{
public:
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		std::string content;
		if (_drawable1.valid()) content += "Drawable 1; ";
		if (_drawable2.valid()) content += "Drawable 2; ";
		if (_text.valid()) _text->setText(content);
	}
	osg::ref_ptr<osgText::Text> _text;
	osg::observer_ptr<osg::Drawable> _drawable1;
	osg::observer_ptr<osg::Drawable> _drawable2;
};

int main(int argc, char** argv)
{
	osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable(
		new osg::Sphere);
	shape->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	shape->setDataVariance(osg::Object::DYNAMIC);
	shape->setUseDisplayList(false);

	osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
	geode1->addDrawable(shape.get());
	osg::ref_ptr<osg::Geode> geode2 = dynamic_cast<osg::Geode*>(
		geode1->clone(osg::CopyOp::SHALLOW_COPY));
	osg::ref_ptr<osg::Geode> geode3 = dynamic_cast<osg::Geode*>(
		geode1->clone(osg::CopyOp::DEEP_COPY_ALL));
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createMatrixTransform(geode1.get(),
		osg::Vec3(0.0f, 0.0f, 0.0f)));
	root->addChild(createMatrixTransform(geode2.get(),
		osg::Vec3(-2.0f, 0.0f, 0.0f)));
	root->addChild(createMatrixTransform(geode3.get(),
		osg::Vec3(2.0f, 0.0f, 0.0f)));

	osgViewer::Viewer viewer;
	viewer.addEventHandler(new SetShapeColorHandler);
	viewer.setSceneData(root.get());
	return viewer.run();
}
