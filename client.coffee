hub = require 'odo-hub'
three = require 'three'
cannon = require 'cannon'
stats = require 'stats.js'
fpscontrols = require './PointerLockControls'

balls = []
ballMeshes = []
boxes = []
boxMeshes = []

# pointer lock
document.addEventListener 'pointerlockchange', (event) ->
  if document.pointerLockElement is document.body
    controls.enabled = yes
  else
    controls.enabled = no
document.addEventListener 'click', (e) -> document.body.requestPointerLock()

# Create world
world = new cannon.World()
world.quatNormalizeSkip = 0
world.quatNormalizeFast = false
world.defaultContactMaterial.contactEquationStiffness = 1e9
world.defaultContactMaterial.contactEquationRelaxation = 4
solver = new cannon.GSSolver()
solver.iterations = 7
solver.tolerance = 0.1
world.solver = new cannon.SplitSolver solver
world.gravity.set 0, -20, 0
world.broadphase = new cannon.NaiveBroadphase()
# Create a slippery material (friction coefficient = 0.0)
physicsMaterial = new cannon.Material 'slipperyMaterial'
physicsContactMaterial = new cannon.ContactMaterial physicsMaterial, physicsMaterial, 0.0, 0.3
# We must add the contact materials to the world
world.addContactMaterial physicsContactMaterial
# Create a sphere
sphereShape = new cannon.Sphere 1.3
sphereBody = new cannon.Body mass: 5
sphereBody.addShape sphereShape
sphereBody.position.set 0, 5, 0
sphereBody.linearDamping = 0.9
world.addBody sphereBody
# Create a plane
groundShape = new cannon.Plane()
groundBody = new cannon.Body mass: 0
groundBody.addShape groundShape
groundBody.quaternion.setFromAxisAngle new cannon.Vec3(1, 0, 0), -Math.PI / 2
world.addBody groundBody

camera = new three.PerspectiveCamera 75, window.innerWidth / window.innerHeight, 0.1, 1000
scene = new three.Scene()
scene.fog = new three.Fog 0x000000, 0, 500
ambient = new three.AmbientLight 0x111111
scene.add ambient
light = new three.SpotLight 0xffffff
light.position.set 10, 30, 20
light.target.position.set 0, 0, 0
light.castShadow = yes
light.shadow.camera.near = 20
light.shadow.camera.far = 50
#camera.far;
light.shadow.camera.fov = 40
light.shadowMapBias = 0.1
light.shadowMapDarkness = 0.7
light.shadow.mapSize.width = 2 * 512
light.shadow.mapSize.height = 2 * 512
#light.shadowCameraVisible = yes;
scene.add light
controls = new fpscontrols camera, sphereBody
scene.add controls.getObject()
# floor
geometry = new three.PlaneGeometry 300, 300, 50, 50
geometry.applyMatrix new three.Matrix4().makeRotationX(-Math.PI / 2)
material = new three.MeshLambertMaterial color: 0xdddddd
mesh = new three.Mesh geometry, material
mesh.castShadow = yes
mesh.receiveShadow = yes
scene.add mesh
renderer = new three.WebGLRenderer()
renderer.shadowMap.enabled = yes
renderer.shadowMapSoft = yes
renderer.setSize window.innerWidth, window.innerHeight
renderer.setClearColor scene.fog.color, 1
renderer.context.getShaderInfoLog = -> ''
document.body.appendChild renderer.domElement
window.addEventListener 'resize', ->
  camera.aspect = window.innerWidth / window.innerHeight
  camera.updateProjectionMatrix()
  renderer.setSize window.innerWidth, window.innerHeight
# Add boxes
halfExtents = new cannon.Vec3 1, 1, 1
boxShape = new cannon.Box halfExtents
boxGeometry = new three.BoxGeometry halfExtents.x * 2, halfExtents.y * 2, halfExtents.z * 2
i = 0
while i < 7
  x = (Math.random() - 0.5) * 20
  y = 1 + (Math.random() - 0.5) * 1
  z = (Math.random() - 0.5) * 20
  boxBody = new cannon.Body(mass: 5)
  boxBody.addShape boxShape
  boxMesh = new (three.Mesh)(boxGeometry, material)
  world.addBody boxBody
  scene.add boxMesh
  boxBody.position.set x, y, z
  boxMesh.position.set x, y, z
  boxMesh.castShadow = true
  boxMesh.receiveShadow = true
  boxes.push boxBody
  boxMeshes.push boxMesh
  i++
# Add linked boxes
size = 0.5
he = new cannon.Vec3(size, size, size * 0.1)
boxShape = new (cannon.Box)(he)
mass = 0
space = 0.1 * size
N = 5
last = undefined
boxGeometry = new (three.BoxGeometry)(he.x * 2, he.y * 2, he.z * 2)
i = 0
while i < N
  boxbody = new cannon.Body mass: mass
  boxbody.addShape boxShape
  boxMesh = new three.Mesh boxGeometry, material
  boxbody.position.set 5, (N - i) * (size * 2 + 2 * space) + size * 2 + space, 0
  boxbody.linearDamping = 0.01
  boxbody.angularDamping = 0.01
  # boxMesh.castShadow = true;
  boxMesh.receiveShadow = true
  world.addBody boxbody
  scene.add boxMesh
  boxes.push boxbody
  boxMeshes.push boxMesh
  if i != 0
    # Connect this body to the last one
    c1 = new cannon.PointToPointConstraint boxbody, new cannon.Vec3(-size, size + space, 0), last, new cannon.Vec3(-size, -size - space, 0)
    c2 = new cannon.PointToPointConstraint boxbody, new cannon.Vec3(size, size + space, 0), last, new cannon.Vec3(size, -size - space, 0)
    world.addConstraint c1
    world.addConstraint c2
  else
    mass = 0.3
  last = boxbody
  i++


time = Date.now()
dt = 1 / 60
animate = ->
  `var i`
  requestAnimationFrame animate
  if controls.enabled
    world.step dt
    # Update ball positions
    i = 0
    while i < balls.length
      ballMeshes[i].position.copy balls[i].position
      ballMeshes[i].quaternion.copy balls[i].quaternion
      i++
    # Update box positions
    i = 0
    while i < boxes.length
      boxMeshes[i].position.copy boxes[i].position
      boxMeshes[i].quaternion.copy boxes[i].quaternion
      i++
  controls.update Date.now() - time
  renderer.render scene, camera
  time = Date.now()
animate()

# Click to fire projectile
ballShape = new (cannon.Sphere)(0.2)
ballGeometry = new (three.SphereGeometry)(ballShape.radius, 32, 32)
shootDirection = new (three.Vector3)
shootVelo = 15
window.addEventListener 'click', (e) ->
  return if !controls.enabled
  x = sphereBody.position.x
  y = sphereBody.position.y
  z = sphereBody.position.z
  ballBody = new cannon.Body mass: 1
  ballBody.addShape ballShape
  ballMesh = new three.Mesh ballGeometry, material
  world.addBody ballBody
  scene.add ballMesh
  ballMesh.castShadow = true
  ballMesh.receiveShadow = true
  balls.push ballBody
  ballMeshes.push ballMesh
  v1 = shootDirection
  shootDirection.set 0, 0, 1
  v1.unproject camera
  ray = new three.Ray sphereBody.position, v1.sub(sphereBody.position).normalize()
  shootDirection.copy ray.direction
  ballBody.velocity.set shootDirection.x * shootVelo, shootDirection.y * shootVelo, shootDirection.z * shootVelo
  # Move the ball outside the player sphere
  x += shootDirection.x * (sphereShape.radius * 1.02 + ballShape.radius)
  y += shootDirection.y * (sphereShape.radius * 1.02 + ballShape.radius)
  z += shootDirection.z * (sphereShape.radius * 1.02 + ballShape.radius)
  ballBody.position.set x, y, z
  ballMesh.position.set x, y, z
