const physics = require('/home/k/physics/build/Release/physics.node');

const FPS = 120;
const STEP_SECONDS = 1 / FPS;
const STEP_MILLISECONDS = 1000 / FPS;

const worlds = new Map();
const bodies = new Map();

class BodyRecord {
  constructor(world, body) {
    this.world = world;
    this.body = body;
  }
}

/* const world = new physics.World();

const floor = physics.RigidBody.make({
  type: physics.RigidBody.PLANE,
  dimensions: [0, 1, 0],
  mass: 0,
});
floor.setPosition(0, 0, 0);

const box1 = physics.RigidBody.make({
  type: physics.RigidBody.BOX,
  dimensions: [1, 1, 1],
  mass: 1,
});
box1.setPosition(0, 2, 0);
box1.setRotation(Math.PI / 8, 0, 0, 1);

const sphere1 = physics.RigidBody.make({
  type: physics.RigidBody.SPHERE,
  size: 1,
  mass: 1,
});
sphere1.setPosition(10, 2, 10);

world.addRigidBody(floor);
world.addRigidBody(box1);
world.addRigidBody(sphere1); */

/* for (let i = 0; i < FPS * TIME; i++) {
  world.stepSimulation(STEP_SECONDS, 1, STEP_SECONDS);
  const rotation = box1.getRotation();
  console.log(rotation[0], rotation[1], rotation[2], rotation[3]);
} */

const _requestUpdate = () => {
  const updates = bodies.map((bodyRecord, bodyId) => {
    const {body} = bodyRecord;
    const position = body.getPosition();
    const rotation = body.getRotation();
    const linearVelocity = body.getLinearVelocity();
    const angularVelocity = body.getAngularVelocity();

    return {
      id: bodyId,
      position,
      rotation,
      linearVelocity,
      angularVelocity,
    };
  });

  send('update', updates);
};
let interval = null;
const _start = () => {
  _stop();

  interval = setInterval(() => {
    worlds.forEach(world => {
      world.stepSimulation(STEP_SECONDS, 1, STEP_SECONDS);
    });
  }, STEP_MILLISECONDS);
};
const _stop = () => {
  if (interval) {
    clearInterval(interval);
    interval = null;
  }
};
const _addWorld = ({id}) => {
  const world = new physics.World();
  worlds.set(id, world);
};
const _removeWorld = ({id}) => {
  worlds.remove(id);
};
const _addBody = ({worldId, body: bodySpec}) => {
  const world = worlds.get(worldId);
  const body = _makeBody(bodySpec);

  world.addRigidBody(body);

  const {id: bodyId} = bodySpec;
  const bodyRecord = new BodyRecord(world, body);
  bodies.set(bodyId, bodyRecord);
};
const _removeBody = ({bodyId}) => {
  const bodyRecord = bodies.get(bodyId);
  const {world, body} = bodyRecord;

  world.removeRigidBody(body);

  bodies.delete(bodyId);
};
const _setPosition = ({bodyId, position}) => {
  const bodyRecord = bodies.get(bodyId);
  const {body} = bodyRecord;

  const [x, y, z] = position;
  body.setPosition(x, y, z);
};
const _setRotation = ({bodyId, rotation}) => {
  const bodyRecord = bodies.get(bodyId);
  const {body} = bodyRecord;

  const [x, y, z, w] = rotation;
  body.setRotation(x, y, z, w);
};

const _makeBody = bodySpec => {
  const {type} = bodySpec;

  switch (type) {
    case 'plane': {
      const {dimensions, mass, position, rotation} = bodySpec;

      const plane = physics.RigidBody.make({
        type: physics.RigidBody.PLANE,
        dimensions,
        mass,
      });
      if (position) {
        plane.setPosition(position[0], position[1], position[2]);
      }
      if (rotation) {
        plane.setRotation(rotation[0], rotation[1], rotation[2], rotation[3]);
      }

      return plane;
    }
    case 'box': {
      const {dimensions, mass, position, rotation} = bodySpec;

      const box = physics.RigidBody.make({
        type: physics.RigidBody.BOX,
        dimensions,
        mass,
      });
      if (position) {
        box.setPosition(position[0], position[1], position[2]);
      }
      if (rotation) {
        box.setRotation(rotation[0], rotation[1], rotation[2], rotation[3]);
      }

      return box;
    }
    case 'sphere': {
      const {size, mass, position, rotation} = bodySpec;

      const sphere = physics.RigidBody.make({
        type: physics.RigidBody.SPHERE,
        size,
        mass,
      });
      if (position) {
        sphere.setPosition(position[0], position[1], position[2]);
      }
      if (rotation) {
        sphere.setRotation(rotation[0], rotation[1], rotation[2], rotation[3]);
      }

      return sphere;
    }
    default:
      return null;
  }
};

process.on('message', m => {
  const {type} = m;

  switch (type) {
    case 'requestUpdate':
      _requestUpdate();
      break;
    case 'start':
      _start();
      break;
    case 'stop':
      _stop();
    case 'addWorld': {
      const {args: {id}} = m;
      _addWorld({id});
      break;
    }
    case 'removeWorld': {
      const {id} = m;
      _removeWorld({id});
      break;
    }
    case 'addBody': {
      const {args: {worldId, body}} = m;
      _addBody({worldId, body});
      break;
    }
    case 'removeBody': {
      const {args: {bodyId}} = m;
      _removeBody({bodyId});
      break;
    }
    case 'setPosition': {
      const {args: {bodyId, position}} = m;
      _setPosition({bodyId, position});
      break;
    }
    case 'setRotation': {
      const {args: {bodyId, rotation}} = m;
      _setRotation({bodyId, rotation});
      break;
    }
    default:
      console.warn('unknown message type:', JSON.stringify(type));
      break;
  }
});

const send = (type, data) => {
  process.send({
    type,
    data,
  });
};

// console.log(physics);
