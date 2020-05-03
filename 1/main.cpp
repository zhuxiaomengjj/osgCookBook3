#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgUtil/CullVisitor>
#include <osgViewer/Viewer>

//#include <osgUtil/PrintVisitor>
//#include <iostream>

class BillboardCallback : public osg::NodeCallback
{
public:
	BillboardCallback(osg::MatrixTransform* billboard)
		: _billboardNode(billboard) {}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osgUtil::CullVisitor* cv =
			dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (_billboardNode.valid() && cv)
		{
			osg::Vec3d translation, scale;
			osg::Quat rotation, so;
			cv->getModelViewMatrix()->decompose(translation, rotation,
				scale, so);
			osg::Matrixd matrix(rotation.inverse());
			_billboardNode->setMatrix(matrix);
		}
		std::cout << node->libraryName() << "::"
			<< node->className() << std::endl;
		traverse(node, nv);
	}
protected:
	osg::observer_ptr<osg::MatrixTransform> _billboardNode;
};

int main(int argc, char *argv[])
{
	osg::ref_ptr<osg::MatrixTransform> billboardNode =
		new osg::MatrixTransform;
	billboardNode->addChild(osgDB::readNodeFile("cessna.osg"));

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(billboardNode.get());
	root->addChild(osgDB::readNodeFile("lz.osg"));
	root->addCullCallback(
		new BillboardCallback(billboardNode.get()));

	//std::cout << "DFS Visitor traversal: " << std::endl;
	//osgUtil::PrintVisitor pv(std::cout);
	//root->accept(pv);
	//std::cout << std::endl;

	osgViewer::Viewer viewer;
	viewer.setSceneData(root.get());
	return viewer.run();
}