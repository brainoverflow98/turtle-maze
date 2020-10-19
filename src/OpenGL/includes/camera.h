#ifndef CAMERA_H
#define CAMERA_H

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <turtle.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	PAGE_UP,
	PAGE_DOWN,
	BIRDSEYE
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	Turtle &Follow;
	glm::vec3 PositionOffset;
	glm::vec2 RotationOffset;

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float RotationSensitivity;
	float Zoom;

	bool FREE_MODE = false;

	// Constructor with vectors
	Camera(Turtle &follow, glm::vec3 position_offset = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2 rotation_offset = glm::vec2(0.0f, 0.0f) ) : Follow(follow)
	{		
		PositionOffset = position_offset;
		RotationOffset = rotation_offset;
		Position = follow.Position + (position_offset.z * follow.Front) + (position_offset.y * follow.Up) + (position_offset.x * follow.Right);
		WorldUp = follow.Up;
		Yaw = follow.Yaw + RotationOffset.y;
		Pitch = follow.Pitch + RotationOffset.x;
		MovementSpeed = 2.5f;
		MouseSensitivity = 0.3f;
		Zoom = 45.0f;
		RotationSensitivity = 50.0f;
		updateCameraVectors();
	}
	
	float NormalizeAngle(float angle)
	{
		if (angle > 360)
			return angle - 360;
		else if (angle < 0)
			return angle + 360;
		else
			return angle;
	}

	float NormalizeRotation(float angle)
	{
		if (angle > 180)
			return angle - 360;
		else if (angle < -180)
			return NormalizeAngle(angle + 360);
		else
			return angle;
	}

	int birdseye = 0;
	glm::vec3 birdseye_position;
	glm::vec3 ground_position;
	void AnimateBirdsEye()
	{
		if (birdseye == 2)
		{
			float time = 70.0f;
			glm::vec3 position_begin = ground_position;
			glm::vec3 position_end = position_begin + glm::vec3(0.0f,27.0f,0.0f);

			float rotationX_begin = Follow.Pitch + RotationOffset.x;
			float rotationX_end = -89.9f;

			float rotationY_begin = Follow.Yaw + RotationOffset.y;
			float rotationY_end = 89.9f;			
			
			Pitch += (rotationX_end - rotationX_begin) / time;
			Position += (position_end - position_begin) / time;
			Yaw += NormalizeRotation(rotationY_end - rotationY_begin) / time;

			if (Pitch <= rotationX_end)
			{				
				birdseye = 1;
				Pitch = rotationX_end;
				Position = position_end;
				Yaw = rotationY_end;
			}
			

			updateCameraVectors();
		}
		else if (birdseye == 3)
		{
			float time = 70.0f;
			glm::vec3 position_begin = birdseye_position;
			glm::vec3 position_end = ground_position;

			float rotationX_begin = -89.9f;
			float rotationX_end = Follow.Pitch + RotationOffset.x;

			float rotationY_begin = 89.9f;
			float rotationY_end = Follow.Yaw + RotationOffset.y;

			Pitch += (rotationX_end - rotationX_begin) / time;
			Position += (position_end - position_begin) / time;
			Yaw += NormalizeRotation(rotationY_end - rotationY_begin) / time;

			if (Pitch >= rotationX_end)
			{
				birdseye = 0;
				Pitch = rotationX_end;
				Position = position_end;
				Yaw = rotationY_end;
			}		

			updateCameraVectors();
		}
	}

	

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		AnimateBirdsEye();
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{				
		float velocity = MovementSpeed * deltaTime / 1000;
		
		if (direction == Camera_Movement::FORWARD)
		{
			if (birdseye == 0)
			{
				if(!FREE_MODE)
					Position = Follow.Position + (PositionOffset.z * Follow.Front) + (PositionOffset.y * Follow.Up) + (PositionOffset.x * Follow.Right);
				else
					Position += Front * velocity;
			}
			else if (birdseye == 1)
			{
				Position += Up * velocity * 12.0f;
			}
		}
		if (direction == Camera_Movement::BACKWARD)
		{
			if (birdseye == 0)
			{
				if (!FREE_MODE)
					Position = Follow.Position + (PositionOffset.z * Follow.Front) + (PositionOffset.y * Follow.Up) + (PositionOffset.x * Follow.Right);
				else
					Position -= Front * velocity;
			}
			else if (birdseye == 1)
			{
				Position -= Up * velocity * 12.0f;
			}
		}
		if (direction == Camera_Movement::LEFT)
		{
			if (birdseye == 0)
			{				
				if (!FREE_MODE)
				{
					Yaw = Follow.Yaw + RotationOffset.y;
					Yaw = NormalizeAngle(Yaw);
					Position = Follow.Position + (PositionOffset.z * Follow.Front) + (PositionOffset.y * Follow.Up) + (PositionOffset.x * Follow.Right);
				}
				else
					Position -= Right * velocity;
			}
			else if (birdseye == 1)
			{
				Position -= Right * velocity * 12.0f;
			}
		}
		if (direction == Camera_Movement::RIGHT)
		{
			if (birdseye == 0)
			{
				if (!FREE_MODE)
				{
					Yaw = Follow.Yaw + RotationOffset.y;
					Yaw = NormalizeAngle(Yaw);
					Position = Follow.Position + (PositionOffset.z * Follow.Front) + (PositionOffset.y * Follow.Up) + (PositionOffset.x * Follow.Right);
				}
				else
					Position += Right * velocity;
			}
			else if (birdseye == 1)
			{
				Position += Right * velocity * 12.0f;
			}
		}
		if (direction == Camera_Movement::BIRDSEYE)
		{
			if (birdseye == 0)
			{
				ground_position = Follow.Position + (PositionOffset.z * Follow.Front) + (PositionOffset.y * Follow.Up) + (PositionOffset.x * Follow.Right);
				birdseye = 2;
			}
			else if (birdseye == 1)
			{
				birdseye_position = Position;
				birdseye = 3;
			}
		}

		updateCameraVectors();
				
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);		
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif