# Antikyth
Bullet Physics 3 server bindings for node.js. Designed for multiple realtime clients.

Runs at native C++ speed in a dedicated process. This is _not_ an emscripten Javascript build and does _not_ run in the browser. If that's what you want, you're probably looking for the much slower ammo.js.

## Example

```
const Antikyth = require('.');

// initialize new physics engine

const engine = new Antikyth();

// initialize new world

const world = new Antikyth.World();
engine.add(world);

// add some bodies

const floor = new Antikyth.Plane({
  position: [0, 0, 0],
  dimensions: [0, 1, 0],
  mass: 0,
});
world.add(floor);

const box = new Antikyth.Box({
  position: [0, 2, 0],
  rotation: [Math.PI / 8, 0, 0, 1],
  dimensions: [1, 1, 1],
  mass: 1,
});
world.add(box);

// request engine updates at 60 FPS

setInterval(() => {
  engine.requestUpdate();
}, 1000 / 60);

// log updates to console

box.on('update', ({position: [px, py, pz], rotation: [rx, ry, rz, rw]}) => {
  console.log('box', px, py, pz);
});

// start running

engine.start();
```
