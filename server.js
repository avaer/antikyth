const Antikyth = require('antikyth');

const OPEN = 1; // ws.OPEN

class World {
  constructor(name) {
    this._name = name;

    const engine = new Antikyth();
    engine.start();
    this.engine = engine;

    this.objects = new Map();
  }

  create(type, opts) {
    const object = (() => {
      switch (type) {
        case 'world': return new Antikyth.World(opts);
        case 'plane': return new Antikyth.Plane(opts);
        case 'box': return new Antikyth.Box(opts);
        case 'sphere': return new Antikyth.Sphere(opts);
        default: return null;
      }
    })();

    if (object) {
      this.objects.set(id, object);

      return object.id;
    } else {
      return null;
    }
  }

  destroy(id) {
    this.objects.delete(id);
  }

  add(parentId, childId) {
    const {objects} = this;

    const parent = objects.get(parentId);
    const child = objects.get(childId);
    parent.add(child);
  }

  remove(parentId, childId) {
    const {objects} = this;

    const parent = objects.get(parentId);
    const child = objects.get(childId);
    parent.remove(child);
  }

  getUpdate(cb) {
    this.engine.requestUpdate();

    this.engine.once('update', updates => {
      cb(null, updates);
    });
  }
}

const server = ({wss}) => ({
  mount() {
    const worlds = new Map();

    const connections = [];

    wss.on('connection', c => {
      const {url} = c.upgradeReq;

      const match = url.match(/\/archae\/antikythWs\/(.*)/);
      if (match) {
        const worldName = match[1];

        let world = worlds.get(worldName);
        if (!world) {
          world = new World();
          worlds.set(worldName, world);
        }

        c.on('message', s => {
          const m = JSON.parse(s);
          if (typeof m === 'object' && m && typeof m.method === 'string' && typeof m.id === 'string' && Array.isArray(m.args)) {
            const {method, id, args} = m;

            const cb = (err = null, result = null) => {
              if (c.readyState === OPEN) {
                const e = {
                  id: id,
                  error: err,
                  result: result,
                };
                const es = JSON.stringify(e);
                c.send(es);
              }
            };

            if (method === 'create') {
              const [type, opts] = args;
              const id = world.create(type, opts);

              cb(null, id);
            } else if (method === 'destroy') {
              const [id] = args;
              world.destroy(id);

              cb();
            } else if (method === 'add') {
              const [parentId, childId] = args;
              world.add(parentId, childId);

              cb();
            } else if (method === 'remove') {
              const [parentId, childId] = args;
              world.remove(parentId, childId);

              cb();
            } else if (method === 'update') {
              world.getUpdate(updates => {
                if (live) {
                  cb(null, updates);
                }
              });

              cb();
            } else {
              const err = new Error('no such method:' + JSON.stringify(method));
              cb(err.stack);
            }
          }
        });
        c.on('close', () => {
          connections.splice(connections.indexOf(c), 1);
        });

        connections.push(c);
      }
    });

    let live = true;
    this._cleanup = () => {
      for (let i = 0; i < connections.length; i++) {
        const connection = connections[i];
        connection.close();
      }
      connections = [];

      live = false;
    };
  },
  unmount() {
    this._cleanup();
  },
});

module.exports = server;
