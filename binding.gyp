{
  "targets": [
    {
      "target_name": "physics",
      "include_dirs" : [
        "<!(node -e \"require('nan')\")",
          "<(BULLET_PHYSICS_ROOT)/include/bullet",
          "inc"
      ],
      "libraries" : [
        "<(BULLET_PHYSICS_ROOT)/lib/BulletDynamics_Debug.lib",
        "<(BULLET_PHYSICS_ROOT)/lib/BulletCollision_Debug.lib",
        "<(BULLET_PHYSICS_ROOT)/lib/LinearMath_Debug.lib"
      ],
      "sources": [
        "<!@(node -e \"console.log(require('fs').readdirSync('./src').map(f=>'src/'+f).join(' '))\")",
        "<!@(node -e \"console.log(require('fs').readdirSync('./inc').map(f=>'inc/'+f).join(' '))\")"
      ]
    }
  ]
}
