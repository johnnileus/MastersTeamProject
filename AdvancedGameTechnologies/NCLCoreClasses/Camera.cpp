#include "Camera.h"
#include "Window.h"

using namespace NCL;

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float dt) {
	if (!activeController) {
		return;
	}

	//Update the mouse by how much
	pitch	-= activeController->GetNamedAxis("YLook");
	yaw		-= activeController->GetNamedAxis("XLook");

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw <0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	float frameSpeed = speed * dt;

	Matrix3 yawRotation = Matrix::RotationMatrix3x3(yaw, Vector3(0, 1, 0));

	position += yawRotation * Vector3(0, 0, -activeController->GetNamedAxis("Forward")) * frameSpeed;
	position += yawRotation * Vector3(activeController->GetNamedAxis("Sidestep"), 0, 0) * frameSpeed;

	position.y += activeController->GetNamedAxis("UpDown") * frameSpeed;
	
}

void Camera::LookAt(const Vector3& target) {
	// 计算指向目标的方向
	Vector3 direction = target - position;
	if (Vector::LengthSquared(direction) < 1e-6f) {
		return;
	}
	Vector::Normalise(direction); 

	// 计算 pitch，直接根据方向的 y 分量得到
	pitch = Maths::RadiansToDegrees(asin(direction.y));

	// 计算水平（XZ平面）分量
	Vector3 horizontalDir = direction;
	horizontalDir.y = 0; // 忽略竖直分量

	// 当水平分量过小时（目标几乎在正上或正下方），直接设 yaw 为 0
	if (Vector::LengthSquared(horizontalDir) < 1e-6f) {
		yaw = 0;
	} else {
		Vector::Normalise(horizontalDir);
		// 根据约定，当水平分量为 (0,0,-1) 时，yaw 应为 0，
		// 而 horizontalDir = (-sin(yaw), 0, -cos(yaw))，因此有：
		yaw = Maths::RadiansToDegrees(atan2(-horizontalDir.x, -horizontalDir.z));
	}
}


/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix() const {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix::Translation(-position);
};

Matrix4 PerspectiveCamera::BuildProjectionMatrix(float currentAspect) const {
	return Matrix::Perspective(nearPlane, farPlane, currentAspect, fov);
}

Matrix4 OrhographicCamera::BuildProjectionMatrix(float currentAspect) const {
	return Matrix::Orthographic(left, right, bottom, top, nearPlane, farPlane);
}