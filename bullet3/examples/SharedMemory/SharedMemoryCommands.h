
#ifndef SHARED_MEMORY_COMMANDS_H
#define SHARED_MEMORY_COMMANDS_H

//this is a very experimental draft of commands. We will iterate on this API (commands, arguments etc)

#include "SharedMemoryPublic.h"

#ifdef __GNUC__
	#include <stdint.h>
	typedef int32_t smInt32_t;
	typedef int64_t smInt64_t;
	typedef uint32_t smUint32_t;
	typedef uint64_t smUint64_t;
#elif defined(_MSC_VER)
	typedef __int32 smInt32_t;
	typedef __int64 smInt64_t;
	typedef unsigned __int32 smUint32_t;
	typedef unsigned __int64 smUint64_t;
#else
	typedef int smInt32_t;
	typedef long long int smInt64_t;
	typedef unsigned int smUint32_t;
	typedef unsigned long long int smUint64_t;
#endif

#define SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE (512*1024)

#define SHARED_MEMORY_SERVER_TEST_C
#define MAX_DEGREE_OF_FREEDOM 128
#define MAX_NUM_SENSORS 256
#define MAX_URDF_FILENAME_LENGTH 1024
#define MAX_SDF_FILENAME_LENGTH 1024
#define MAX_FILENAME_LENGTH MAX_URDF_FILENAME_LENGTH
#define MAX_NUM_LINKS MAX_DEGREE_OF_FREEDOM
#define MAX_SDF_BODIES 500

struct TmpFloat3 
{
    float m_x;
    float m_y;
    float m_z;
};

#ifdef _WIN32
__inline
#else
inline
#endif
TmpFloat3 CreateTmpFloat3(float x, float y, float z) 
{
    TmpFloat3 tmp;
    tmp.m_x = x;
    tmp.m_y = y;
    tmp.m_z = z;
    return tmp;
}

enum EnumSdfArgsUpdateFlags
{
	SDF_ARGS_FILE_NAME=1,
};

struct SdfArgs
{
	char m_sdfFileName[MAX_URDF_FILENAME_LENGTH];
    int m_useMultiBody;
};

enum EnumUrdfArgsUpdateFlags
{
	URDF_ARGS_FILE_NAME=1,
	URDF_ARGS_INITIAL_POSITION=2,
	URDF_ARGS_INITIAL_ORIENTATION=4,
	URDF_ARGS_USE_MULTIBODY=8,
	URDF_ARGS_USE_FIXED_BASE=16,
};


struct UrdfArgs
{
	char m_urdfFileName[MAX_URDF_FILENAME_LENGTH];
	double m_initialPosition[3];
	double m_initialOrientation[4];
	int m_useMultiBody;
	int m_useFixedBase;
};


struct BulletDataStreamArgs
{
	char m_bulletFileName[MAX_FILENAME_LENGTH];
	int m_bodyUniqueId;
};

struct SetJointFeedbackArgs
{
	int m_bodyUniqueId;
	int m_linkId;
	int m_isEnabled;
};

enum EnumInitPoseFlags
{
    INIT_POSE_HAS_INITIAL_POSITION=1,
    INIT_POSE_HAS_INITIAL_ORIENTATION=2,
    INIT_POSE_HAS_JOINT_STATE=4
};


///InitPoseArgs is mainly to initialize (teleport) the robot in a particular position
///No motors or controls are needed to initialize the pose. It is similar to
///moving a robot to a starting place, while it is switched off. It is only called
///at the start of a robot control session. All velocities and control forces are cleared to zero.
struct InitPoseArgs
{
	int m_bodyUniqueId;
	int m_hasInitialStateQ[MAX_DEGREE_OF_FREEDOM];
	double m_initialStateQ[MAX_DEGREE_OF_FREEDOM];
};


struct RequestDebugLinesArgs
{
	int m_debugMode;
    int m_startingLineIndex;
};

struct RequestPixelDataArgs
{
	float m_viewMatrix[16];
	float m_projectionMatrix[16];
	int m_startPixelIndex;
	int m_pixelWidth;
	int m_pixelHeight;
};

enum EnumRequestPixelDataUpdateFlags
{
	REQUEST_PIXEL_ARGS_HAS_CAMERA_MATRICES=1,
	REQUEST_PIXEL_ARGS_SET_PIXEL_WIDTH_HEIGHT=4,
	//don't exceed (1<<15), because this enum is shared with EnumRenderer in SharedMemoryPublic.h
	
};

enum EnumRequestContactDataUpdateFlags
{
	CMD_REQUEST_CONTACT_POINT_HAS_QUERY_MODE=1,
	CMD_REQUEST_CONTACT_POINT_HAS_CLOSEST_DISTANCE_THRESHOLD=2,
};

struct RequestContactDataArgs
{
    int m_startingContactPointIndex;
    int m_objectAIndexFilter;
	int m_objectBIndexFilter;
	double m_closestDistanceThreshold;
	int m_mode;
};

struct RequestOverlappingObjectsArgs
{
	int m_startingOverlappingObjectIndex;
	double m_aabbQueryMin[3];
	double m_aabbQueryMax[3];
};

struct RequestVisualShapeDataArgs
{
	int m_bodyUniqueId;
	int m_startingVisualShapeIndex;
};

struct UpdateVisualShapeDataArgs
{
    int m_bodyUniqueId;
    int m_jointIndex;
    int m_shapeIndex;
    int m_textureUniqueId;
};

struct LoadTextureArgs
{
    char m_textureFileName[MAX_FILENAME_LENGTH];
};

struct SendVisualShapeDataArgs
{
	int m_bodyUniqueId;
    int m_startingVisualShapeIndex;
    int m_numVisualShapesCopied;
    int m_numRemainingVisualShapes;
};



struct SendDebugLinesArgs
{
    int m_startingLineIndex;
	int m_numDebugLines;
    int m_numRemainingDebugLines;
};

struct SendPixelDataArgs
{
	int m_imageWidth;
	int m_imageHeight;

    int m_startingPixelIndex;
    int m_numPixelsCopied;
    int m_numRemainingPixels;
};

struct PickBodyArgs
{
    double m_rayFromWorld[3];
    double m_rayToWorld[3];
};


///Controlling a robot involves sending the desired state to its joint motor controllers.
///The control mode determines the state variables used for motor control.
struct SendDesiredStateArgs
{
	int m_bodyUniqueId;
	int m_controlMode;

	//PD parameters in case m_controlMode == CONTROL_MODE_POSITION_VELOCITY_PD
	double m_Kp[MAX_DEGREE_OF_FREEDOM];//indexed by degree of freedom, 6 for base, and then the dofs for each link
	double m_Kd[MAX_DEGREE_OF_FREEDOM];//indexed by degree of freedom, 6 for base, and then the dofs for each link

    int m_hasDesiredStateFlags[MAX_DEGREE_OF_FREEDOM];
    
	//desired state is only written by the client, read-only access by server is expected

	//m_desiredStateQ is indexed by position variables, 
	//starting with 3 base position variables, 4 base orientation variables (quaternion), then link position variables
    double m_desiredStateQ[MAX_DEGREE_OF_FREEDOM];
    

	//m_desiredStateQdot is index by velocity degrees of freedom, 3 linear and 3 angular variables for the base and then link velocity variables
    double m_desiredStateQdot[MAX_DEGREE_OF_FREEDOM];
	
	//m_desiredStateForceTorque is either the actual applied force/torque (in CONTROL_MODE_TORQUE) or
	//or the maximum applied force/torque for the PD/motor/constraint to reach the desired velocity in CONTROL_MODE_VELOCITY and CONTROL_MODE_POSITION_VELOCITY_PD mode
	//indexed by degree of freedom, 6 dof base, and then dofs for each link
    double m_desiredStateForceTorque[MAX_DEGREE_OF_FREEDOM];
    
};

enum EnumSimDesiredStateUpdateFlags
{
	SIM_DESIRED_STATE_HAS_Q=1,
	SIM_DESIRED_STATE_HAS_QDOT=2,
	SIM_DESIRED_STATE_HAS_KD=4,
	SIM_DESIRED_STATE_HAS_KP=8,
	SIM_DESIRED_STATE_HAS_MAX_FORCE=16,
};


enum EnumSimParamUpdateFlags
{
	SIM_PARAM_UPDATE_DELTA_TIME=1,
	SIM_PARAM_UPDATE_GRAVITY=2,
	SIM_PARAM_UPDATE_NUM_SOLVER_ITERATIONS=4,	
	SIM_PARAM_UPDATE_NUM_SIMULATION_SUB_STEPS=8,
	SIM_PARAM_UPDATE_REAL_TIME_SIMULATION = 16,
	SIM_PARAM_UPDATE_DEFAULT_CONTACT_ERP=32,
	SIM_PARAM_UPDATE_INTERNAL_SIMULATION_FLAGS=64
};

enum EnumLoadBunnyUpdateFlags
{
    LOAD_BUNNY_UPDATE_SCALE=1,
    LOAD_BUNNY_UPDATE_MASS=2,
    LOAD_BUNNY_UPDATE_COLLISION_MARGIN=4
};

enum EnumSimParamInternalSimFlags
{
	SIM_PARAM_INTERNAL_CREATE_ROBOT_ASSETS=1,
};


///Controlling a robot involves sending the desired state to its joint motor controllers.
///The control mode determines the state variables used for motor control.
struct SendPhysicsSimulationParameters
{
	double m_deltaTime;
	double m_gravityAcceleration[3];
	int m_numSimulationSubSteps;
	int m_numSolverIterations;
	bool m_allowRealTimeSimulation;
	int m_internalSimFlags;
	double m_defaultContactERP;
};

struct LoadBunnyArgs
{
    double m_scale;
    double m_mass;
    double m_collisionMargin;
};

struct RequestActualStateArgs
{
	int m_bodyUniqueId;
};




struct SendActualStateArgs
{
	int m_bodyUniqueId;
	int m_numDegreeOfFreedomQ;
	int m_numDegreeOfFreedomU;

    double m_rootLocalInertialFrame[7];
    
	  //actual state is only written by the server, read-only access by client is expected
    double m_actualStateQ[MAX_DEGREE_OF_FREEDOM];
    double m_actualStateQdot[MAX_DEGREE_OF_FREEDOM];

    //measured 6DOF force/torque sensors: force[x,y,z] and torque[x,y,z]
    double m_jointReactionForces[6*MAX_DEGREE_OF_FREEDOM];

    double m_jointMotorForce[MAX_DEGREE_OF_FREEDOM];
    
    double m_linkState[7*MAX_NUM_LINKS];
    double m_linkLocalInertialFrames[7*MAX_NUM_LINKS];
};

enum EnumSensorTypes
{
    SENSOR_FORCE_TORQUE=1,
    SENSOR_IMU=2,
};

struct CreateSensorArgs
{
    int m_bodyUniqueId;
    int m_numJointSensorChanges;
    int m_sensorType[MAX_DEGREE_OF_FREEDOM];
    
///todo: clean up the duplication, make sure no-one else is using those members directly (use C-API header instead)
    int m_jointIndex[MAX_DEGREE_OF_FREEDOM];
    int m_enableJointForceSensor[MAX_DEGREE_OF_FREEDOM];

    int m_linkIndex[MAX_DEGREE_OF_FREEDOM];
    int m_enableSensor[MAX_DEGREE_OF_FREEDOM];
    
};

typedef  struct SharedMemoryCommand SharedMemoryCommand_t;

enum EnumBoxShapeFlags
{
    BOX_SHAPE_HAS_INITIAL_POSITION=1,
    BOX_SHAPE_HAS_INITIAL_ORIENTATION=2,
    BOX_SHAPE_HAS_HALF_EXTENTS=4,
	BOX_SHAPE_HAS_MASS=8,
	BOX_SHAPE_HAS_COLLISION_SHAPE_TYPE=16,
	BOX_SHAPE_HAS_COLOR=32,
};
///This command will be replaced to allow arbitrary collision shape types
struct CreateBoxShapeArgs
{
    double m_halfExtentsX;
    double m_halfExtentsY;
    double m_halfExtentsZ;

	double m_mass;
	int m_collisionShapeType;//see SharedMemoryPublic.h

    double m_initialPosition[3];
	double m_initialOrientation[4];
	double m_colorRGBA[4];
};

struct SdfLoadedArgs
{
    int m_numBodies;
    int m_bodyUniqueIds[MAX_SDF_BODIES];
    
    ///@todo(erwincoumans) load cameras, lights etc
    //int m_numCameras; 
    //int m_numLights; 
};


struct SdfRequestInfoArgs
{
    int m_bodyUniqueId;
};

///flags for b3ApplyExternalTorque and b3ApplyExternalForce
enum EnumExternalForcePrivateFlags
{
//    EF_LINK_FRAME=1,
//    EF_WORLD_FRAME=2,
    EF_TORQUE=4,
    EF_FORCE=8,
};



struct ExternalForceArgs
{
    int m_numForcesAndTorques;
    int m_bodyUniqueIds[MAX_SDF_BODIES];
    int m_linkIds[MAX_SDF_BODIES];
    double m_forcesAndTorques[3*MAX_SDF_BODIES];
    double m_positions[3*MAX_SDF_BODIES];
    int m_forceFlags[MAX_SDF_BODIES];
};

enum EnumSdfRequestInfoFlags
{
    SDF_REQUEST_INFO_BODY=1,
    //SDF_REQUEST_INFO_CAMERA=2,
};


struct CalculateInverseDynamicsArgs
{
	int m_bodyUniqueId;

	double m_jointPositionsQ[MAX_DEGREE_OF_FREEDOM];
	double m_jointVelocitiesQdot[MAX_DEGREE_OF_FREEDOM];
	double m_jointAccelerations[MAX_DEGREE_OF_FREEDOM];
};

struct CalculateInverseDynamicsResultArgs
{
	int m_bodyUniqueId;
	int m_dofCount;
	double m_jointForces[MAX_DEGREE_OF_FREEDOM];
};

struct CalculateJacobianArgs
{
    int m_bodyUniqueId;
    int m_linkIndex;
    double m_localPosition[3];
    double m_jointPositionsQ[MAX_DEGREE_OF_FREEDOM];
    double m_jointVelocitiesQdot[MAX_DEGREE_OF_FREEDOM];
    double m_jointAccelerations[MAX_DEGREE_OF_FREEDOM];
};

struct CalculateJacobianResultArgs
{
    int m_dofCount;
    double m_linearJacobian[3*MAX_DEGREE_OF_FREEDOM];
    double m_angularJacobian[3*MAX_DEGREE_OF_FREEDOM];
};

enum EnumCalculateInverseKinematicsFlags
{
    IK_HAS_TARGET_POSITION=1,
	IK_HAS_TARGET_ORIENTATION=2,
    IK_HAS_NULL_SPACE_VELOCITY=4,
    //IK_HAS_CURRENT_JOINT_POSITIONS=8,//not used yet
};

struct CalculateInverseKinematicsArgs
{
	int m_bodyUniqueId;
//	double m_jointPositionsQ[MAX_DEGREE_OF_FREEDOM];
	double m_targetPosition[3];
	double m_targetOrientation[4];//orientation represented as quaternion, x,y,z,w
	int m_endEffectorLinkIndex;
    double m_lowerLimit[MAX_DEGREE_OF_FREEDOM];
    double m_upperLimit[MAX_DEGREE_OF_FREEDOM];
    double m_jointRange[MAX_DEGREE_OF_FREEDOM];
    double m_restPose[MAX_DEGREE_OF_FREEDOM];
};

struct CalculateInverseKinematicsResultArgs
{
	int m_bodyUniqueId;
	int m_dofCount;
	double m_jointPositions[MAX_DEGREE_OF_FREEDOM];
};

struct CreateJointArgs
{
    int m_parentBodyIndex;
    int m_parentJointIndex;
    int m_childBodyIndex;
    int m_childJointIndex;
    double m_parentFrame[7];
    double m_childFrame[7];
    double m_jointAxis[3];
    int m_jointType;
};

struct SharedMemoryCommand
{
	int m_type;
	smUint64_t	m_timeStamp;
	int	m_sequenceNumber;
	
	//m_updateFlags is a bit fields to tell which parameters need updating
    //for example m_updateFlags = SIM_PARAM_UPDATE_DELTA_TIME | SIM_PARAM_UPDATE_NUM_SOLVER_ITERATIONS;
    int m_updateFlags;

    union
    {
        struct UrdfArgs m_urdfArguments;
		struct SdfArgs m_sdfArguments;
		struct SdfRequestInfoArgs m_sdfRequestInfoArgs;
		struct InitPoseArgs m_initPoseArgs;
		struct SendPhysicsSimulationParameters m_physSimParamArgs;
		struct BulletDataStreamArgs	m_dataStreamArguments;
		struct SendDesiredStateArgs m_sendDesiredStateCommandArgument;
		struct RequestActualStateArgs m_requestActualStateInformationCommandArgument;
        struct CreateSensorArgs m_createSensorArguments;
        struct CreateBoxShapeArgs m_createBoxShapeArguments;
		struct RequestDebugLinesArgs m_requestDebugLinesArguments;
		struct RequestPixelDataArgs m_requestPixelDataArguments;
		struct PickBodyArgs m_pickBodyArguments;
        struct ExternalForceArgs m_externalForceArguments;
		struct CalculateInverseDynamicsArgs m_calculateInverseDynamicsArguments;
        struct CalculateJacobianArgs m_calculateJacobianArguments;
        struct CreateJointArgs m_createJointArguments;
        struct RequestContactDataArgs m_requestContactPointArguments;
		struct RequestOverlappingObjectsArgs m_requestOverlappingObjectsArgs;
        struct RequestVisualShapeDataArgs m_requestVisualShapeDataArguments;
        struct UpdateVisualShapeDataArgs m_updateVisualShapeDataArguments;
        struct LoadTextureArgs m_loadTextureArguments;
		struct CalculateInverseKinematicsArgs m_calculateInverseKinematicsArguments;
        struct LoadBunnyArgs m_loadBunnyArguments;
    };
};

struct RigidBodyCreateArgs
{
	int m_bodyUniqueId; 
};

struct SendContactDataArgs
{
    int m_startingContactPointIndex;
    int m_numContactPointsCopied;
    int m_numRemainingContactPoints;
};

struct SendOverlappingObjectsArgs
{
	int m_startingOverlappingObjectIndex;
	int m_numOverlappingObjectsCopied;
	int m_numRemainingOverlappingObjects;
};

struct SharedMemoryStatus
{
	int m_type;
	
	smUint64_t	m_timeStamp;
	int	m_sequenceNumber;
	
	//m_streamBytes is only for internal purposes
	int		m_numDataStreamBytes;
	char*	m_dataStream;

	union
	{
		struct BulletDataStreamArgs	m_dataStreamArguments;
		struct SdfLoadedArgs m_sdfLoadedArgs;
		struct SendActualStateArgs m_sendActualStateArgs;
		struct SendDebugLinesArgs m_sendDebugLinesArgs;
		struct SendPixelDataArgs m_sendPixelDataArguments;
		struct RigidBodyCreateArgs m_rigidBodyCreateArgs;
		struct CalculateInverseDynamicsResultArgs m_inverseDynamicsResultArgs;
        struct CalculateJacobianResultArgs m_jacobianResultArgs;
		struct SendContactDataArgs m_sendContactPointArgs;
		struct SendOverlappingObjectsArgs m_sendOverlappingObjectsArgs;
		struct CalculateInverseKinematicsResultArgs m_inverseKinematicsResultArgs;
		struct SendVisualShapeDataArgs m_sendVisualShapeArgs;
	};
};

typedef  struct SharedMemoryStatus SharedMemoryStatus_t;



#endif //SHARED_MEMORY_COMMANDS_H
