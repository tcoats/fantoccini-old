hub = require 'odo-hub'
three = require 'three'
require './ammo'
ammo = Ammo
stats = require 'stats.js'

# Physics variables
physicsWorld = null
rigidBodies = []
hinge = null
rope = null
margin = 0.05

createRigidBody = (threeObject, physicsShape, mass, pos, quat) ->
  threeObject.position.copy pos
  threeObject.quaternion.copy quat
  transform = new ammo.btTransform
  transform.setIdentity()
  transform.setOrigin new ammo.btVector3 pos.x, pos.y, pos.z
  transform.setRotation new ammo.btQuaternion quat.x, quat.y, quat.z, quat.w
  motionState = new ammo.btDefaultMotionState transform
  localInertia = new ammo.btVector3 0, 0, 0
  physicsShape.calculateLocalInertia mass, localInertia
  rbInfo = new ammo.btRigidBodyConstructionInfo mass, motionState, physicsShape, localInertia
  body = new ammo.btRigidBody rbInfo
  threeObject.userData.physicsBody = body
  scene.add threeObject
  if mass > 0
    rigidBodies.push threeObject
    body.setActivationState 4
  physicsWorld.addRigidBody body

createParalellepiped = (sx, sy, sz, mass, pos, quat, material) ->
  threeObject = new three.Mesh(new three.BoxGeometry(sx, sy, sz, 1, 1, 1), material)
  shape = new ammo.btBoxShape(new ammo.btVector3(sx * 0.5, sy * 0.5, sz * 0.5))
  shape.setMargin margin
  createRigidBody threeObject, shape, mass, pos, quat
  threeObject

createObjects = ->
  pos = new three.Vector3()
  quat = new three.Quaternion()
  # Ground
  pos.set 0, -0.5, 0
  quat.set 0, 0, 0, 1
  ground = createParalellepiped(40, 1, 40, 0, pos, quat, new three.MeshPhongMaterial(color: 0xFFFFFF))
  ground.castShadow = yes
  ground.receiveShadow = yes
  # Ball
  ballMass = 1.2
  ballRadius = 0.6
  ball = new three.Mesh(new three.SphereGeometry(ballRadius, 20, 20), new three.MeshPhongMaterial(color: 0x202020))
  ball.castShadow = yes
  ball.receiveShadow = yes
  ballShape = new ammo.btSphereShape ballRadius
  ballShape.setMargin margin
  pos.set -3, 2, 0
  quat.set 0, 0, 0, 1
  createRigidBody ball, ballShape, ballMass, pos, quat
  ball.userData.physicsBody.setFriction 0.5
  # Wall
  brickMass = 0.5
  brickLength = 1.2
  brickDepth = 0.6
  brickHeight = brickLength * 0.5
  numBricksLength = 6
  z0 = -numBricksLength * brickLength * 0.5
  pos.set 0, brickHeight * 0.5, z0
  quat.set 0, 0, 0, 1
  for j in [0...8]
    oddRow = j % 2 == 1
    pos.z = z0
    if oddRow
      pos.z -= 0.25 * brickLength
    nRow = if oddRow then numBricksLength + 1 else numBricksLength
    for i in [0...nRow]
      brickLengthCurrent = brickLength
      brickMassCurrent = brickMass
      if oddRow and (i is 0 or i is nRow - 1)
        brickLengthCurrent *= 0.5
        brickMassCurrent *= 0.5
      brick = createParalellepiped brickDepth, brickHeight, brickLengthCurrent, brickMassCurrent, pos, quat, new three.MeshPhongMaterial color: 0x660000
      brick.castShadow = yes
      brick.receiveShadow = yes
      if oddRow and (i == 0 or i == nRow - 2)
        pos.z += 0.75 * brickLength
      else
        pos.z += brickLength
    pos.y += brickHeight
  # The rope
  # Rope graphic object
  ropeNumSegments = 10
  ropeLength = 4
  ropeMass = 3
  ropePos = ball.position.clone()
  ropePos.y += ballRadius
  segmentLength = ropeLength / ropeNumSegments
  ropeGeometry = new three.BufferGeometry
  ropeMaterial = new three.LineBasicMaterial(color: 0x000000)
  ropePositions = []
  ropeIndices = []
  for i in [0...ropeNumSegments + 1]
    ropePositions.push ropePos.x, ropePos.y + i * segmentLength, ropePos.z
  for i in [0...ropeNumSegments]
    ropeIndices.push i, i + 1
  ropeGeometry.setIndex new three.BufferAttribute(new Uint16Array(ropeIndices), 1)
  ropeGeometry.addAttribute 'position', new three.BufferAttribute(new Float32Array(ropePositions), 3)
  ropeGeometry.computeBoundingSphere()
  rope = new three.LineSegments(ropeGeometry, ropeMaterial)
  rope.castShadow = yes
  rope.receiveShadow = yes
  scene.add rope
  # Rope physic object
  softBodyHelpers = new ammo.btSoftBodyHelpers
  ropeStart = new ammo.btVector3(ropePos.x, ropePos.y, ropePos.z)
  ropeEnd = new ammo.btVector3(ropePos.x, ropePos.y + ropeLength, ropePos.z)
  ropeSoftBody = softBodyHelpers.CreateRope(physicsWorld.getWorldInfo(), ropeStart, ropeEnd, ropeNumSegments - 1, 0)
  sbConfig = ropeSoftBody.get_m_cfg()
  sbConfig.set_viterations 10
  sbConfig.set_piterations 10
  ropeSoftBody.setTotalMass ropeMass, no
  ammo.castObject(ropeSoftBody, ammo.btCollisionObject).getCollisionShape().setMargin margin * 3
  physicsWorld.addSoftBody ropeSoftBody, 1, -1
  rope.userData.physicsBody = ropeSoftBody
  # Disable deactivation
  ropeSoftBody.setActivationState 4
  # The base
  armMass = 2
  armLength = 3
  pylonHeight = ropePos.y + ropeLength
  baseMaterial = new three.MeshPhongMaterial color: 0x606060
  pos.set ropePos.x, 0.1, ropePos.z - armLength
  quat.set 0, 0, 0, 1
  base = createParalellepiped 1, 0.2, 1, 0, pos, quat, baseMaterial
  base.castShadow = yes
  base.receiveShadow = yes
  pos.set ropePos.x, 0.5 * pylonHeight, ropePos.z - armLength
  pylon = createParalellepiped 0.4, pylonHeight, 0.4, 0, pos, quat, baseMaterial
  pylon.castShadow = yes
  pylon.receiveShadow = yes
  pos.set ropePos.x, pylonHeight + 0.2, ropePos.z - (0.5 * armLength)
  arm = createParalellepiped 0.4, 0.4, armLength + 0.4, armMass, pos, quat, baseMaterial
  arm.castShadow = yes
  arm.receiveShadow = yes
  # Glue the rope extremes to the ball and the arm
  influence = 1
  ropeSoftBody.appendAnchor 0, ball.userData.physicsBody, yes, influence
  ropeSoftBody.appendAnchor ropeNumSegments, arm.userData.physicsBody, yes, influence
  # Hinge constraint to move the arm
  pivotA = new ammo.btVector3 0, pylonHeight * 0.5, 0
  pivotB = new ammo.btVector3 0, -0.2, -armLength * 0.5
  axis = new ammo.btVector3 0, 1, 0
  hinge = new ammo.btHingeConstraint pylon.userData.physicsBody, arm.userData.physicsBody, pivotA, pivotB, axis, axis, yes
  physicsWorld.addConstraint hinge, yes

# graphics
camera = new three.PerspectiveCamera 60, window.innerWidth / window.innerHeight, 0.2, 2000
scene = new three.Scene()
camera.position.x = -7
camera.position.y = 5
camera.position.z = 8
# controls = new three.OrbitControls(camera)
# controls.target.y = 2
renderer = new three.WebGLRenderer()
renderer.setClearColor 0xbfd1e5
renderer.setPixelRatio window.devicePixelRatio
renderer.setSize window.innerWidth, window.innerHeight
renderer.shadowMap.enabled = yes
ambientLight = new three.AmbientLight 0x404040
scene.add ambientLight
light = new three.DirectionalLight 0xffffff, 1
light.position.set -10, 10, 5
light.castShadow = yes
d = 10
light.shadow.camera.left = -d
light.shadow.camera.right = d
light.shadow.camera.top = d
light.shadow.camera.bottom = -d
light.shadow.camera.near = 2
light.shadow.camera.far = 50
light.shadow.mapSize.x = 1024
light.shadow.mapSize.y = 1024
scene.add light
document.body.appendChild renderer.domElement
fps = new stats()
document.body.appendChild fps.domElement

window.addEventListener 'resize', ->
  camera.aspect = window.innerWidth / window.innerHeight
  camera.updateProjectionMatrix()
  renderer.setSize window.innerWidth, window.innerHeight

# physics
gravityConstant = -9.8
collisionConfiguration = new ammo.btSoftBodyRigidBodyCollisionConfiguration()
dispatcher = new ammo.btCollisionDispatcher collisionConfiguration
broadphase = new ammo.btDbvtBroadphase()
solver = new ammo.btSequentialImpulseConstraintSolver()
softBodySolver = new ammo.btDefaultSoftBodySolver()
physicsWorld = new ammo.btSoftRigidDynamicsWorld dispatcher, broadphase, solver, collisionConfiguration, softBodySolver
physicsWorld.setGravity new ammo.btVector3 0, gravityConstant, 0
physicsWorld.getWorldInfo().set_m_gravity new ammo.btVector3 0, gravityConstant, 0

createObjects()

armMovement = 0
window.addEventListener 'keydown', (e) ->
  switch e.keyCode
    # Q
    when 81
      armMovement = 1
    # A
    when 65
      armMovement = -1
window.addEventListener 'keyup', (e) ->
  armMovement = 0

transformAux1 = new ammo.btTransform()
updatePhysics = (deltaTime) ->
  # Hinge control
  hinge.enableAngularMotor yes, 1.5 * armMovement, 50
  # Step world
  physicsWorld.stepSimulation deltaTime, 10
  # Update rope
  softBody = rope.userData.physicsBody
  ropePositions = rope.geometry.attributes.position.array
  numVerts = ropePositions.length / 3
  nodes = softBody.get_m_nodes()
  indexFloat = 0
  for i in [0...numVerts]
    node = nodes.at(i)
    nodePos = node.get_m_x()
    ropePositions[indexFloat++] = nodePos.x()
    ropePositions[indexFloat++] = nodePos.y()
    ropePositions[indexFloat++] = nodePos.z()
  rope.geometry.attributes.position.needsUpdate = yes
  # Update rigid bodies
  for objThree in rigidBodies
    objPhys = objThree.userData.physicsBody
    ms = objPhys.getMotionState()
    continue if !ms
    ms.getWorldTransform transformAux1
    p = transformAux1.getOrigin()
    q = transformAux1.getRotation()
    objThree.position.set p.x(), p.y(), p.z()
    objThree.quaternion.set q.x(), q.y(), q.z(), q.w()

clock = new three.Clock()
time = 0
animate = ->
  requestAnimationFrame animate
  fps.begin()
  deltaTime = clock.getDelta()
  updatePhysics deltaTime
  #controls.update deltaTime
  renderer.render scene, camera
  time += deltaTime
  fps.end()
animate()
