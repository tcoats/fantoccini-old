// Generated by CoffeeScript 1.10.0
var cannon, three;

three = require('three');

cannon = require('cannon');

module.exports = function(camera, cannonBody) {
  var PI_2, canJump, contactNormal, euler, eyeYPos, inputVelocity, jumpVelocity, moveBackward, moveForward, moveLeft, moveRight, onKeyDown, onKeyUp, onMouseMove, pitchObject, quat, scope, upAxis, velocity, velocityFactor, yawObject;
  eyeYPos = 2;
  velocityFactor = 0.2;
  jumpVelocity = 20;
  scope = this;
  pitchObject = new three.Object3D;
  pitchObject.add(camera);
  yawObject = new three.Object3D;
  yawObject.position.y = 2;
  yawObject.add(pitchObject);
  quat = new three.Quaternion;
  moveForward = false;
  moveBackward = false;
  moveLeft = false;
  moveRight = false;
  canJump = false;
  contactNormal = new cannon.Vec3();
  upAxis = new cannon.Vec3(0, 1, 0);
  cannonBody.addEventListener('collide', function(e) {
    var contact;
    contact = e.contact;
    if (contact.bi.id === cannonBody.id) {
      contact.ni.negate(contactNormal);
    } else {
      contactNormal.copy(contact.ni);
    }
    if (contactNormal.dot(upAxis) > 0.5) {
      canJump = true;
    }
  });
  velocity = cannonBody.velocity;
  PI_2 = Math.PI / 2;
  onMouseMove = function(event) {
    var movementX, movementY;
    if (scope.enabled === false) {
      return;
    }
    movementX = event.movementX || event.mozMovementX || event.webkitMovementX || 0;
    movementY = event.movementY || event.mozMovementY || event.webkitMovementY || 0;
    yawObject.rotation.y -= movementX * 0.002;
    pitchObject.rotation.x -= movementY * 0.002;
    pitchObject.rotation.x = Math.max(-PI_2, Math.min(PI_2, pitchObject.rotation.x));
  };
  onKeyDown = function(event) {
    switch (event.keyCode) {
      case 38:
      case 87:
        moveForward = true;
        break;
      case 37:
      case 65:
        moveLeft = true;
        break;
      case 40:
      case 83:
        moveBackward = true;
        break;
      case 39:
      case 68:
        moveRight = true;
        break;
      case 32:
        if (canJump === true) {
          velocity.y = jumpVelocity;
        }
        canJump = false;
    }
  };
  onKeyUp = function(event) {
    switch (event.keyCode) {
      case 38:
      case 87:
        moveForward = false;
        break;
      case 37:
      case 65:
        moveLeft = false;
        break;
      case 40:
      case 83:
        moveBackward = false;
        break;
      case 39:
      case 68:
        moveRight = false;
    }
  };
  document.addEventListener('mousemove', onMouseMove, false);
  document.addEventListener('keydown', onKeyDown, false);
  document.addEventListener('keyup', onKeyUp, false);
  this.enabled = false;
  inputVelocity = new three.Vector3();
  euler = new three.Euler();
  return {
    getObject: function() {
      return yawObject;
    },
    getDirection: function(targetVec) {
      targetVec.set(0, 0, -1);
      quat.multiplyVector3(targetVec);
    },
    update: function(delta) {
      if (!scope.enabled) {
        return;
      }
      delta *= 0.1;
      inputVelocity.set(0, 0, 0);
      if (moveForward) {
        inputVelocity.z = -velocityFactor * delta;
      }
      if (moveBackward) {
        inputVelocity.z = velocityFactor * delta;
      }
      if (moveLeft) {
        inputVelocity.x = -velocityFactor * delta;
      }
      if (moveRight) {
        inputVelocity.x = velocityFactor * delta;
      }
      euler.x = pitchObject.rotation.x;
      euler.y = yawObject.rotation.y;
      euler.order = 'XYZ';
      quat.setFromEuler(euler);
      inputVelocity.applyQuaternion(quat);
      velocity.x += inputVelocity.x;
      velocity.z += inputVelocity.z;
      return yawObject.position.copy(cannonBody.position);
    }
  };
};
