#ifndef BULLET_URDF_IMPORTER_H
#define BULLET_URDF_IMPORTER_H 

#include "URDFImporterInterface.h"

#include "LinkVisualShapesConverter.h"


///BulletURDFImporter can deal with URDF and (soon) SDF files
class BulletURDFImporter : public URDFImporterInterface
{
    
	struct BulletURDFInternalData* m_data;
    

public:

	BulletURDFImporter(struct GUIHelperInterface* guiHelper, LinkVisualShapesConverter* customConverter);

	virtual ~BulletURDFImporter();

	virtual bool loadURDF(const char* fileName, bool forceFixedBase = false);

    //warning: some quick test to load SDF: we 'activate' a model, so we can re-use URDF code path
    virtual bool loadSDF(const char* fileName, bool forceFixedBase = false);
    virtual int getNumModels() const;
    virtual void activateModel(int modelIndex);
    virtual void setBodyUniqueId(int bodyId);
    virtual int getBodyUniqueId() const;
	const char* getPathPrefix();

	void printTree(); //for debugging
	
	virtual int getRootLinkIndex() const;
    
    virtual void getLinkChildIndices(int linkIndex, btAlignedObjectArray<int>& childLinkIndices) const;

    virtual std::string getLinkName(int linkIndex) const;

	virtual bool getLinkColor(int linkIndex, btVector4& colorRGBA) const;

	virtual bool getLinkContactInfo(int linkIndex, URDFLinkContactInfo& contactInfo ) const;
	
    virtual std::string getJointName(int linkIndex) const;
    
    virtual void  getMassAndInertia(int linkIndex, btScalar& mass,btVector3& localInertiaDiagonal, btTransform& inertialFrame) const;

    virtual bool getJointInfo(int urdfLinkIndex, btTransform& parent2joint, btTransform& linkTransformInWorld, btVector3& jointAxisInJointSpace, int& jointType, btScalar& jointLowerLimit, btScalar& jointUpperLimit, btScalar& jointDamping, btScalar& jointFriction) const;
    
    virtual bool getRootTransformInWorld(btTransform& rootTransformInWorld) const;

    virtual int convertLinkVisualShapes(int linkIndex, const char* pathPrefix, const btTransform& inertialFrame) const;

    virtual void convertLinkVisualShapes2(int linkIndex, const char* pathPrefix, const btTransform& inertialFrame, class btCollisionObject* colObj, int bodyUniqueId) const;

    ///todo(erwincoumans) refactor this convertLinkCollisionShapes/memory allocation
    
	virtual class btCompoundShape* convertLinkCollisionShapes(int linkIndex, const char* pathPrefix, const btTransform& localInertiaFrame) const;
    
    virtual int getNumAllocatedCollisionShapes() const;
    virtual class btCollisionShape* getAllocatedCollisionShape(int index);

    
};


#endif //BULLET_URDF_IMPORTER_H
