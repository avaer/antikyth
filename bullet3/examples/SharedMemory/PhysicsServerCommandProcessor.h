#ifndef PHYSICS_SERVER_COMMAND_PROCESSOR_H
#define PHYSICS_SERVER_COMMAND_PROCESSOR_H

#include "LinearMath/btVector3.h"

#include "PhysicsCommandProcessorInterface.h"

struct SharedMemLines
{
	btVector3 m_from;
	btVector3 m_to;
	btVector3 m_color;
};



///todo: naming. Perhaps PhysicsSdkCommandprocessor?
class PhysicsServerCommandProcessor : public PhysicsCommandProcessorInterface
{

	struct PhysicsServerCommandProcessorInternalData* m_data;

	//todo: move this to physics client side / Python
	void createDefaultRobotAssets();

protected:




	bool loadSdf(const char* fileName, char* bufferServerToClient, int bufferSizeInBytes, bool useMultiBody);

	bool loadUrdf(const char* fileName, const class btVector3& pos, const class btQuaternion& orn,
		bool useMultiBody, bool useFixedBase, int* bodyUniqueIdPtr, char* bufferServerToClient, int bufferSizeInBytes);

	bool	supportsJointMotor(class btMultiBody* body, int linkIndex);

	int createBodyInfoStream(int bodyUniqueId, char* bufferServerToClient, int bufferSizeInBytes);
	void deleteCachedInverseDynamicsBodies();

public:
	PhysicsServerCommandProcessor();
	virtual ~PhysicsServerCommandProcessor();

	void	createJointMotors(class btMultiBody* body);

	virtual void createEmptyDynamicsWorld();
	virtual void deleteDynamicsWorld();

	virtual bool connect()
	{
		return true;
	};

	virtual void disconnect() {}

	virtual bool isConnected() const
	{
		return true;
	}



	virtual bool processCommand(const struct SharedMemoryCommand& clientCmd, struct SharedMemoryStatus& serverStatusOut, char* bufferServerToClient, int bufferSizeInBytes);

	virtual bool receiveStatus(struct SharedMemoryStatus& serverStatusOut, char* bufferServerToClient, int bufferSizeInBytes)
	{
		return false;
	};

	virtual void renderScene();
	virtual void   physicsDebugDraw(int debugDrawFlags);
	virtual void setGuiHelper(struct GUIHelperInterface* guiHelper);
	
	//@todo(erwincoumans) Should we have shared memory commands for picking objects?
	///The pickBody method will try to pick the first body along a ray, return true if succeeds, false otherwise
	virtual bool pickBody(const btVector3& rayFromWorld, const btVector3& rayToWorld);
	virtual bool movePickedBody(const btVector3& rayFromWorld, const btVector3& rayToWorld);
	virtual void removePickingConstraint();
	
	void enableCommandLogging(bool enable, const char* fileName);
	void replayFromLogFile(const char* fileName);
	void replayLogCommand(char* bufferServerToClient, int bufferSizeInBytes );
	void stepSimulationRealTime(double dtInSec);
	void applyJointDamping(int bodyUniqueId);
};

#endif //PHYSICS_SERVER_COMMAND_PROCESSOR_H
