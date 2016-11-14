#pragma once

#include <nan.h>
#include "pointers.h"

namespace mox {
  namespace physics {

    class RigidBody : public Nan::ObjectWrap {
    public:
      static void Init(v8::Local<v8::Object> namespc);

      static v8::Local<v8::Object> NewInstance();

      btRigidBodyPtr getRigidBody() { return m_rigidBody;  }

    private:
      explicit RigidBody();
      ~RigidBody();

      uint32_t m_type;
      btCollisionShapePtr m_collisionShape;
      btTransform m_transform;
      btDefaultMotionStatePtr m_motionState;
      btRigidBodyPtr m_rigidBody;
      bool m_isDynamic;
      double m_mass;

      static const uint32_t BOX = 1;
      static const uint32_t CYLINDER = 2;
      static const uint32_t SPHERE = 3;

      static NAN_METHOD(New);
      static NAN_METHOD(make);

      static NAN_METHOD(getMass);
      static NAN_METHOD(setPosition);
      static NAN_METHOD(getPosition);

      static Nan::Persistent<v8::Function> constructor;
    };

  }
}
