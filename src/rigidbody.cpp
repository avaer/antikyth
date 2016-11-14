
#include "helper.h"
#include "btBulletDynamicsCommon.h"
#include "pointers.h"
#include "rigidbody.h"

Nan::Persistent<v8::Function> mox::physics::RigidBody::constructor;

mox::physics::RigidBody::RigidBody()
{
  m_transform.setIdentity();
  m_mass = 0;
}

mox::physics::RigidBody::~RigidBody()
{

}

void mox::physics::RigidBody::Init(v8::Local<v8::Object> namespc)
{
  DEFINE_FUNCTION_TEMPLATE("RigidBody", tpl);

  Nan::SetMethod(tpl, "make", make);

  tpl->Set(Nan::New("BOX").ToLocalChecked(), Nan::New(BOX));
  tpl->Set(Nan::New("CYLINDER").ToLocalChecked(), Nan::New(CYLINDER));
  tpl->Set(Nan::New("SPHERE").ToLocalChecked(), Nan::New(SPHERE));

  Nan::SetPrototypeMethod(tpl, "getMass", getMass);
  Nan::SetPrototypeMethod(tpl, "setPosition", setPosition);
  Nan::SetPrototypeMethod(tpl, "getPosition", getPosition);

  constructor.Reset(tpl->GetFunction());
  namespc->Set(Nan::New("RigidBody").ToLocalChecked(), tpl->GetFunction());

}

NAN_METHOD(mox::physics::RigidBody::New)
{
  ALLOW_ONLY_CONSTRUCTOR(info);
  RigidBody *obj = new RigidBody();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(mox::physics::RigidBody::make)
{
  CHECK_NUM_ARGUMENTS(info, 1);

  v8::Local<v8::Object> instance = NewInstance();
  RigidBody *nativeInstance = ObjectWrap::Unwrap<RigidBody>(instance);

  v8::Local<v8::String> keyType = Nan::New("type").ToLocalChecked();
  v8::Local<v8::String> keyDimensions = Nan::New("dimensions").ToLocalChecked();
  v8::Local<v8::String> keyPosition = Nan::New("position").ToLocalChecked();
  v8::Local<v8::String> keyMass = Nan::New("mass").ToLocalChecked();

  v8::Local<v8::Object> def = Nan::To<v8::Object>(info[0]).ToLocalChecked();

  // type - decides which kind of collision shape this rigid body has
  MOXCHK(Nan::Has(def, keyType).FromJust());
  nativeInstance->m_type = Nan::To<uint32_t>(
    Nan::Get(def, keyType).ToLocalChecked()).FromJust();

  // mass
  if (Nan::Has(def, keyMass).FromJust()) {
    nativeInstance->m_mass = Nan::To<double>(
      Nan::Get(def, keyMass).ToLocalChecked()).FromJust();
  }

  nativeInstance->m_isDynamic = (nativeInstance->m_mass != 0.0f);

  // position
  v8::Local<v8::Object> position;
  double x=0, y=0, z=0;

  if (Nan::Has(def, keyPosition).FromJust()) {
    position = Nan::To<v8::Object>(Nan::Get(def, keyPosition)
      .ToLocalChecked()).ToLocalChecked();
    x = Nan::To<double>(Nan::Get(position, 0).ToLocalChecked()).FromJust();
    y = Nan::To<double>(Nan::Get(position, 1).ToLocalChecked()).FromJust();
    z = Nan::To<double>(Nan::Get(position, 2).ToLocalChecked()).FromJust();
    nativeInstance->m_transform.setOrigin(btVector3(x, y, z));
  }


  //
  // type-specific construction of rigid body
  //

  v8::Local<v8::Object> dimensions;
  double dx=0, dy=0, dz=0;

  switch (nativeInstance->m_type) {
  case BOX:
    MOXCHK(Nan::Has(def, keyDimensions).FromJust());
    dimensions = Nan::To<v8::Object>(Nan::Get(def, keyDimensions)
      .ToLocalChecked()).ToLocalChecked();
    dx = Nan::To<double>(Nan::Get(dimensions, 0).ToLocalChecked()).FromJust();
    dy = Nan::To<double>(Nan::Get(dimensions, 1).ToLocalChecked()).FromJust();
    dz = Nan::To<double>(Nan::Get(dimensions, 2).ToLocalChecked()).FromJust();
    nativeInstance->m_collisionShape = std::make_shared<btBoxShape>(
      btVector3(btScalar(dx), btScalar(dy), btScalar(dz)));
    break;

  case CYLINDER:
    break;
  case SPHERE:
    break;
  }

  btVector3 localInertia(0, 0, 0);
  if (nativeInstance->m_isDynamic) {
    nativeInstance->m_collisionShape->calculateLocalInertia(
      nativeInstance->m_mass, localInertia);
  }

  nativeInstance->m_motionState = std::make_shared<btDefaultMotionState>(
    nativeInstance->m_transform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(
    nativeInstance->m_mass,
    nativeInstance->m_motionState.get(),
    nativeInstance->m_collisionShape.get(),
    localInertia
  );

  nativeInstance->m_rigidBody = std::make_shared<btRigidBody>(rbInfo);

  info.GetReturnValue().Set(instance);
}

NAN_METHOD(mox::physics::RigidBody::getMass)
{
  GET_SELF(mox::physics::RigidBody, self);
  info.GetReturnValue().Set(Nan::New<v8::Number>(self->m_mass));
}

NAN_METHOD(mox::physics::RigidBody::setPosition)
{
  GET_SELF(mox::physics::RigidBody, self);
  CHECK_NUM_ARGUMENTS(info, 3);
  double x = info[0]->IsUndefined() ? 0 : Nan::To<double>(info[0]).FromJust();
  double y = info[1]->IsUndefined() ? 0 : Nan::To<double>(info[1]).FromJust();
  double z = info[2]->IsUndefined() ? 0 : Nan::To<double>(info[2]).FromJust();
  self->m_transform.setOrigin(btVector3(x, y, z));
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(mox::physics::RigidBody::getPosition)
{
  GET_SELF(mox::physics::RigidBody, self);
  v8::Local<v8::Array> position = Nan::New<v8::Array>(3);

  btTransform xform;
  self->m_motionState->getWorldTransform(xform);
  Nan::Set(position, 0, Nan::New<v8::Number>(xform.getOrigin().getX()));
  Nan::Set(position, 1, Nan::New<v8::Number>(xform.getOrigin().getY()));
  Nan::Set(position, 2, Nan::New<v8::Number>(xform.getOrigin().getZ()));

  info.GetReturnValue().Set(position);
}

v8::Local<v8::Object> mox::physics::RigidBody::NewInstance()
{
  Nan::EscapableHandleScope scope;

  const unsigned argc = 1;
  v8::Local<v8::Value> argv[1] = {Nan::New("xxx").ToLocalChecked()};
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);

  return scope.Escape(instance);
}
