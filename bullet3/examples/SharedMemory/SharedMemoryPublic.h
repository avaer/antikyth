#ifndef SHARED_MEMORY_PUBLIC_H
#define SHARED_MEMORY_PUBLIC_H

#define SHARED_MEMORY_KEY 12347

enum EnumSharedMemoryClientCommand
{
    CMD_LOAD_SDF,
	CMD_LOAD_URDF,
    CMD_LOAD_BUNNY,
        CMD_SEND_BULLET_DATA_STREAM,
        CMD_CREATE_BOX_COLLISION_SHAPE,
//      CMD_DELETE_BOX_COLLISION_SHAPE,
      CMD_CREATE_RIGID_BODY,
      CMD_DELETE_RIGID_BODY,
    CMD_CREATE_SENSOR,///enable or disable joint feedback for force/torque sensors
//    CMD_REQUEST_SENSOR_MEASUREMENTS,//see CMD_REQUEST_ACTUAL_STATE/CMD_ACTUAL_STATE_UPDATE_COMPLETED
        CMD_INIT_POSE,
        CMD_SEND_PHYSICS_SIMULATION_PARAMETERS,
        CMD_SEND_DESIRED_STATE,//todo: reconsider naming, for example SET_JOINT_CONTROL_VARIABLE?
        CMD_REQUEST_ACTUAL_STATE,
        CMD_REQUEST_DEBUG_LINES,
    CMD_REQUEST_BODY_INFO,
	CMD_REQUEST_INTERNAL_DATA,
    CMD_STEP_FORWARD_SIMULATION,
    CMD_RESET_SIMULATION,
    CMD_PICK_BODY,
    CMD_MOVE_PICKED_BODY,
    CMD_REMOVE_PICKING_CONSTRAINT_BODY,
    CMD_REQUEST_CAMERA_IMAGE_DATA,
    CMD_APPLY_EXTERNAL_FORCE,
	CMD_CALCULATE_INVERSE_DYNAMICS,
    CMD_CALCULATE_INVERSE_KINEMATICS,
    CMD_CALCULATE_JACOBIAN,
    CMD_CREATE_JOINT,
    CMD_REQUEST_CONTACT_POINT_INFORMATION,
	CMD_REQUEST_AABB_OVERLAP,
	CMD_SAVE_WORLD,
	CMD_REQUEST_VISUAL_SHAPE_INFO,
    CMD_UPDATE_VISUAL_SHAPE,
    CMD_LOAD_TEXTURE,
    //don't go beyond this command!
    CMD_MAX_CLIENT_COMMANDS,
    
};

enum EnumSharedMemoryServerStatus
{
        CMD_SHARED_MEMORY_NOT_INITIALIZED=0,
        CMD_WAITING_FOR_CLIENT_COMMAND,
        //CMD_CLIENT_COMMAND_COMPLETED is a generic 'completed' status that doesn't need special handling on the client
        CMD_CLIENT_COMMAND_COMPLETED,
        //the server will skip unknown command and report a status 'CMD_UNKNOWN_COMMAND_FLUSHED'
        CMD_UNKNOWN_COMMAND_FLUSHED,
		CMD_SDF_LOADING_COMPLETED,
        CMD_SDF_LOADING_FAILED,
        CMD_URDF_LOADING_COMPLETED,
        CMD_URDF_LOADING_FAILED,
		CMD_REQUEST_INTERNAL_DATA_COMPLETED,
		CMD_REQUEST_INTERNAL_DATA_FAILED,
        CMD_BULLET_DATA_STREAM_RECEIVED_COMPLETED,
        CMD_BULLET_DATA_STREAM_RECEIVED_FAILED,
        CMD_BOX_COLLISION_SHAPE_CREATION_COMPLETED,
        CMD_RIGID_BODY_CREATION_COMPLETED,
        CMD_SET_JOINT_FEEDBACK_COMPLETED,
        CMD_ACTUAL_STATE_UPDATE_COMPLETED,
        CMD_ACTUAL_STATE_UPDATE_FAILED,
        CMD_DEBUG_LINES_COMPLETED,
        CMD_DEBUG_LINES_OVERFLOW_FAILED,
        CMD_DESIRED_STATE_RECEIVED_COMPLETED,
        CMD_STEP_FORWARD_SIMULATION_COMPLETED,
        CMD_RESET_SIMULATION_COMPLETED,
        CMD_CAMERA_IMAGE_COMPLETED,
        CMD_CAMERA_IMAGE_FAILED,
        CMD_BODY_INFO_COMPLETED,
        CMD_BODY_INFO_FAILED,
		CMD_INVALID_STATUS,
		CMD_CALCULATED_INVERSE_DYNAMICS_COMPLETED,
		CMD_CALCULATED_INVERSE_DYNAMICS_FAILED,
        CMD_CALCULATED_JACOBIAN_COMPLETED,
        CMD_CALCULATED_JACOBIAN_FAILED,
		CMD_CONTACT_POINT_INFORMATION_COMPLETED,
		CMD_CONTACT_POINT_INFORMATION_FAILED,
		CMD_REQUEST_AABB_OVERLAP_COMPLETED,
		CMD_REQUEST_AABB_OVERLAP_FAILED,
		CMD_CALCULATE_INVERSE_KINEMATICS_COMPLETED,
		CMD_CALCULATE_INVERSE_KINEMATICS_FAILED,
		CMD_SAVE_WORLD_COMPLETED,
		CMD_SAVE_WORLD_FAILED,
        CMD_VISUAL_SHAPE_INFO_COMPLETED,
        CMD_VISUAL_SHAPE_INFO_FAILED,
        CMD_VISUAL_SHAPE_UPDATE_COMPLETED,
        CMD_VISUAL_SHAPE_UPDATE_FAILED,
        CMD_LOAD_TEXTURE_COMPLETED,
        CMD_LOAD_TEXTURE_FAILED,
        //don't go beyond 'CMD_MAX_SERVER_COMMANDS!
        CMD_MAX_SERVER_COMMANDS
};

enum JointInfoFlags
{
    JOINT_HAS_MOTORIZED_POWER=1,
};

enum 
{
	COLLISION_SHAPE_TYPE_BOX=1,
	COLLISION_SHAPE_TYPE_CYLINDER_X,
	COLLISION_SHAPE_TYPE_CYLINDER_Y,
	COLLISION_SHAPE_TYPE_CYLINDER_Z,
	COLLISION_SHAPE_TYPE_CAPSULE_X,
	COLLISION_SHAPE_TYPE_CAPSULE_Y,
	COLLISION_SHAPE_TYPE_CAPSULE_Z,
	COLLISION_SHAPE_TYPE_SPHERE
};

// copied from btMultiBodyLink.h
enum JointType {
    eRevoluteType = 0,
    ePrismaticType = 1,
    eFixedType = 2,
	ePoint2PointType = 3,
};

struct b3JointInfo
{
        char* m_linkName;
        char* m_jointName;
        int m_jointType;
        int m_qIndex;
        int m_uIndex;
        int m_jointIndex;
        int m_flags;
		double m_jointDamping;
		double m_jointFriction;
    double m_parentFrame[7]; // position and orientation (quaternion)
    double m_childFrame[7]; // ^^^
    double m_jointAxis[3]; // joint axis in parent local frame
};

struct b3BodyInfo
{
	const char* m_baseName;
};



struct b3JointSensorState
{
  double m_jointPosition;
  double m_jointVelocity;
  double m_jointForceTorque[6];  /* note to roboticists: this is NOT the motor torque/force, but the spatial reaction force vector at joint */
  double m_jointMotorTorque;
};

struct b3DebugLines
{
    int m_numDebugLines;
    const float*  m_linesFrom;//float x,y,z times 'm_numDebugLines'.
    const float*  m_linesTo;//float x,y,z times 'm_numDebugLines'.
    const float*  m_linesColor;//float red,green,blue times 'm_numDebugLines'.
};

struct b3OverlappingObject
{
	int m_objectUniqueId;
	int m_linkIndex;
};

struct b3AABBOverlapData
{
    int m_numOverlappingObjects;
	struct b3OverlappingObject* m_overlappingObjects;
};

struct b3CameraImageData
{
	int m_pixelWidth;
	int m_pixelHeight;
	const unsigned char* m_rgbColorData;//3*m_pixelWidth*m_pixelHeight bytes
	const float* m_depthValues;//m_pixelWidth*m_pixelHeight floats
	const int* m_segmentationMaskValues;//m_pixelWidth*m_pixelHeight ints
};


struct b3ContactPointData
{
//todo: expose some contact flags, such as telling which fields below are valid
    int m_contactFlags;
    int m_bodyUniqueIdA;
    int m_bodyUniqueIdB;
    int m_linkIndexA;
    int m_linkIndexB;
    double m_positionOnAInWS[3];//contact point location on object A, in world space coordinates
    double m_positionOnBInWS[3];//contact point location on object A, in world space coordinates
    double m_contactNormalOnBInWS[3];//the separating contact normal, pointing from object B towards object A
    double m_contactDistance;//negative number is penetration, positive is distance.
    
    double m_normalForce;

//todo: expose the friction forces as well
//    double m_linearFrictionDirection0[3];
//    double m_linearFrictionForce0;
//    double m_linearFrictionDirection1[3];
//    double m_linearFrictionForce1;
//    double m_angularFrictionDirection[3];
//    double m_angularFrictionForce;
};

enum 
{
	CONTACT_QUERY_MODE_REPORT_EXISTING_CONTACT_POINTS = 0,
	CONTACT_QUERY_MODE_COMPUTE_CLOSEST_POINTS = 1,
};



struct b3ContactInformation
{
	int m_numContactPoints;
	struct b3ContactPointData* m_contactPointData;
};

#define VISUAL_SHAPE_MAX_PATH_LEN 128

struct b3VisualShapeData
{
	int m_objectUniqueId;
	int m_linkIndex;
	int m_visualGeometryType;//box primitive, sphere primitive, triangle mesh
	double m_dimensions[3];//meaning depends on m_visualGeometryType
	char m_meshAssetFileName[VISUAL_SHAPE_MAX_PATH_LEN];
	double m_localInertiaFrame[7];//pos[3], orn[4]
	//todo: add more data if necessary (material color etc, although material can be in asset file .obj file)
};

struct b3VisualShapeInformation
{
	int m_numVisualShapes;
	struct b3VisualShapeData* m_visualShapeData;
};

///b3LinkState provides extra information such as the Cartesian world coordinates
///center of mass (COM) of the link, relative to the world reference frame.
///Orientation is a quaternion x,y,z,w
///Note: to compute the URDF link frame (which equals the joint frame at joint position 0)
///use URDF link frame = link COM frame * inertiaFrame.inverse()
struct b3LinkState
{
    double m_worldPosition[3];
    double m_worldOrientation[4];

    double m_localInertialPosition[3];
    double m_localInertialOrientation[4];
};

//todo: discuss and decide about control mode and combinations
enum {
    //    POSITION_CONTROL=0,
    CONTROL_MODE_VELOCITY=0,
    CONTROL_MODE_TORQUE,
    CONTROL_MODE_POSITION_VELOCITY_PD,
};

///flags for b3ApplyExternalTorque and b3ApplyExternalForce
enum EnumExternalForceFlags
{
    EF_LINK_FRAME=1,
    EF_WORLD_FRAME=2,
};

///flags to pick the renderer for synthetic camera
enum EnumRenderer
{
    ER_TINY_RENDERER=(1<<16),
    ER_BULLET_HARDWARE_OPENGL=(1<<17),
    //ER_FIRE_RAYS=(1<<18),
};

#endif//SHARED_MEMORY_PUBLIC_H
