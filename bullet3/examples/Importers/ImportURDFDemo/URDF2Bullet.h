#ifndef _URDF2BULLET_H
#define _URDF2BULLET_H
#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btTransform.h"
#include <string>
class btVector3;
class btTransform;
class btMultiBodyDynamicsWorld;
class btTransform;


class URDFImporterInterface;
class MultiBodyCreationInterface;



enum ConvertURDFFlags {
  CUF_USE_SDF = 1,
  // Use inertia values in URDF instead of recomputing them from collision shape.
  CUF_USE_URDF_INERTIA = 2
};

void ConvertURDF2Bullet(const URDFImporterInterface& u2b,
			MultiBodyCreationInterface& creationCallback,
			const btTransform& rootTransformInWorldSpace,
			btMultiBodyDynamicsWorld* world,
			bool createMultiBody,
			const char* pathPrefix,
            int flags = 0);


#endif //_URDF2BULLET_H

