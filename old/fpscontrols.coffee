three = require 'three'
cannon = require 'cannon'

module.exports = (camera, cannonBody) ->
  eyeYPos = 2
  # eyes are 2 meters above the ground
  velocityFactor = 0.2
  jumpVelocity = 20
  scope = this
  pitchObject = new (three.Object3D)
  pitchObject.add camera
  yawObject = new (three.Object3D)
  yawObject.position.y = 2
  yawObject.add pitchObject
  quat = new (three.Quaternion)
  moveForward = false
  moveBackward = false
  moveLeft = false
  moveRight = false
  canJump = false
  contactNormal = new cannon.Vec3()
  # Normal in the contact, pointing *out* of whatever the player touched
  upAxis = new cannon.Vec3(0, 1, 0)
  cannonBody.addEventListener 'collide', (e) ->
    contact = e.contact
    # contact.bi and contact.bj are the colliding bodies, and contact.ni is the collision normal.
    # We do not yet know which one is which! Let's check.
    if contact.bi.id == cannonBody.id
      contact.ni.negate contactNormal
    else
      contactNormal.copy contact.ni
    # bi is something else. Keep the normal as it is
    # If contactNormal.dot(upAxis) is between 0 and 1, we know that the contact normal is somewhat in the up direction.
    if contactNormal.dot(upAxis) > 0.5
      canJump = true
    return
  velocity = cannonBody.velocity
  PI_2 = Math.PI / 2

  onMouseMove = (event) ->
    if scope.enabled == false
      return
    movementX = event.movementX or event.mozMovementX or event.webkitMovementX or 0
    movementY = event.movementY or event.mozMovementY or event.webkitMovementY or 0
    yawObject.rotation.y -= movementX * 0.002
    pitchObject.rotation.x -= movementY * 0.002
    pitchObject.rotation.x = Math.max(-PI_2, Math.min(PI_2, pitchObject.rotation.x))
    return

  onKeyDown = (event) ->
    switch event.keyCode
      # up
      when 38, 87
        # w
        moveForward = true
      # left
      when 37, 65
        # a
        moveLeft = true
      # down
      when 40, 83
        # s
        moveBackward = true
      # right
      when 39, 68
        # d
        moveRight = true
      when 32
        # space
        if canJump == true
          velocity.y = jumpVelocity
        canJump = false
    return

  onKeyUp = (event) ->
    switch event.keyCode
      # up
      when 38, 87
        # w
        moveForward = false
      # left
      when 37, 65
        # a
        moveLeft = false
      # down
      when 40, 83
        # a
        moveBackward = false
      # right
      when 39, 68
        # d
        moveRight = false
    return

  document.addEventListener 'mousemove', onMouseMove, false
  document.addEventListener 'keydown', onKeyDown, false
  document.addEventListener 'keyup', onKeyUp, false
  @enabled = false

  # Moves the camera to the cannon.js object position and adds velocity to the object if the run key is down
  inputVelocity = new three.Vector3()
  euler = new three.Euler()

  getObject: ->
    yawObject

  getDirection: (targetVec) ->
    targetVec.set 0, 0, -1
    quat.multiplyVector3 targetVec
    return

  update: (delta) ->
    return if !scope.enabled
    delta *= 0.1
    inputVelocity.set 0, 0, 0
    if moveForward
      inputVelocity.z = -velocityFactor * delta
    if moveBackward
      inputVelocity.z = velocityFactor * delta
    if moveLeft
      inputVelocity.x = -velocityFactor * delta
    if moveRight
      inputVelocity.x = velocityFactor * delta
    # Convert velocity to world coordinates
    euler.x = pitchObject.rotation.x
    euler.y = yawObject.rotation.y
    euler.order = 'XYZ'
    quat.setFromEuler euler
    inputVelocity.applyQuaternion quat
    #quat.multiplyVector3(inputVelocity);
    # Add to the object
    velocity.x += inputVelocity.x
    velocity.z += inputVelocity.z
    yawObject.position.copy cannonBody.position
