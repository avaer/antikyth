#ifndef PHYSICS_DIRECT_H
#define PHYSICS_DIRECT_H

//#include "SharedMemoryCommands.h"


#include "PhysicsClient.h"
#include "LinearMath/btVector3.h"

///PhysicsDirect executes the commands directly, without transporting them or having a separate server executing commands
class PhysicsDirect : public PhysicsClient 
{
protected:

	struct PhysicsDirectInternalData* m_data;

	bool processDebugLines(const struct SharedMemoryCommand& orgCommand);

	bool processCamera(const struct SharedMemoryCommand& orgCommand);

    bool processContactPointData(const struct SharedMemoryCommand& orgCommand);

	bool processOverlappingObjects(const struct SharedMemoryCommand& orgCommand);

	bool processVisualShapeData(const struct SharedMemoryCommand& orgCommand);
	
    void processBodyJointInfo(int bodyUniqueId, const struct SharedMemoryStatus& serverCmd);
    
	void postProcessStatus(const struct SharedMemoryStatus& serverCmd);

public:

	PhysicsDirect(class PhysicsCommandProcessorInterface* physSdk);
    
    virtual ~PhysicsDirect();

	// return true if connection succesfull, can also check 'isConnected'
	//it is OK to pass a null pointer for the gui helper
    virtual bool connect();
	
	////todo: rename to 'disconnect'
    virtual void disconnectSharedMemory();

    virtual bool isConnected() const;

    // return non-null if there is a status, nullptr otherwise
    virtual const  SharedMemoryStatus* processServerStatus();

    virtual  SharedMemoryCommand* getAvailableSharedMemoryCommand();

    virtual bool canSubmitCommand() const;

    virtual bool submitClientCommand(const struct SharedMemoryCommand& command);

	virtual int getNumBodies() const;

	virtual int getBodyUniqueId(int serialIndex) const;

	virtual bool getBodyInfo(int bodyUniqueId, struct b3BodyInfo& info) const;

    virtual int getNumJoints(int bodyIndex) const;

    virtual bool getJointInfo(int bodyIndex, int jointIndex, struct b3JointInfo& info) const;

	///todo: move this out of the
    virtual void setSharedMemoryKey(int key);

    void uploadBulletFileToSharedMemory(const char* data, int len);

    virtual int getNumDebugLines() const;

    virtual const float* getDebugLinesFrom() const;
    virtual const float* getDebugLinesTo() const;
    virtual const float* getDebugLinesColor() const;

	virtual void getCachedCameraImage(b3CameraImageData* cameraData);

    virtual void getCachedContactPointInformation(struct b3ContactInformation* contactPointData);

	virtual void getCachedOverlappingObjects(struct b3AABBOverlapData* overlappingObjects);

	virtual void getCachedVisualShapeInformation(struct b3VisualShapeInformation* visualShapesInfo);
	

	//those 2 APIs are for internal use for visualization
	virtual bool connect(struct GUIHelperInterface* guiHelper);
	virtual void renderScene();
	virtual void debugDraw(int debugDrawMode);

};

#endif //PHYSICS_DIRECT_H
