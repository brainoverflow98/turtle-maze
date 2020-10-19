#ifndef TURTLE_H
#define TURTLE_H

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <model.h>

// Defines several possible options for player movement. Used as abstraction to stay away from window-system specific input methods
enum class Turtle_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	PAGE_UP,
	PAGE_DOWN
};


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Turtle
{
public:
	Model model;
	// Position Attributes
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scaler;

	glm::mat4 rot;

	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	
	float Yaw;
	float Pitch;

	float ground_position;
	// Turtle options
	float MovementSpeed;
	float RotationSensitivity;	

	//Sound Engine
	ISoundEngine *SoundEngine;

	//Player variables
	int points = 0;

	// Constructor
	Turtle(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scaler = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		Position = position;
		Rotation = rotation;
		Scaler = scaler;

		rot = glm::mat4(1.0f);

		Pitch = 0.0f;
		Yaw = 90.0f;

		ground_position = position.y;

		Front = glm::vec3(0.0f, 0.0f, -1.0f);
		WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		MovementSpeed = 2.5f;
		RotationSensitivity = 100.0f;		

		updateTurtleVectors();
	}

	
	bool stepup = true;		
	void AnimateWalk()
	{
		if (!stepup)
		{
			float time = 16.0f;
			float position_begin = ground_position;
			float position_end = ground_position + 0.02f;
			
			Position.y += (position_end - position_begin) / time;			

			if (Position.y >= position_end)
			{
				stepup = true;								
			}			
		}
		else if (stepup)
		{
			float time = 16.0f;
			float position_begin = ground_position + 0.02f;
			float position_end = ground_position;

			Position.y += (position_end - position_begin) / time;
			
			if (Position.y <= position_end)
			{
				SoundEngine->play2D("sound/grass_walk.wav", GL_FALSE);
				stepup = false;
			}
		}
	}


	void Draw(Shader program, glm::mat4 view_matrix, glm::mat4 projection_matrix)
	{
		glm::mat4 model_matrix = glm::mat4(1.0f);

		if (Position != glm::vec3(0.0f, 0.0f, 0.0f))
		{
			model_matrix = glm::translate(model_matrix, glm::vec3(Position.x, Position.y, Position.z));
		}

		model_matrix = model_matrix * rot;

		if (Rotation.x != 0.0f)
		{
			model_matrix = glm::rotate(model_matrix, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
		}

		if (Rotation.y != 0.0f)
		{
			model_matrix = glm::rotate(model_matrix, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
		}

		if (Rotation.z != 0.0f)
		{
			model_matrix = glm::rotate(model_matrix, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
		}

		if (Scaler != glm::vec3(1.0f, 1.0f, 1.0f))
		{
			model_matrix = glm::scale(model_matrix, glm::vec3(Scaler.x, Scaler.y, Scaler.z));
		}

		//glm::mat4 MVP = projection_matrix * view_matrix * model_matrix;
		program.setMat4("model", model_matrix);
		program.setMat4("view", view_matrix);
		program.setMat4("projection", projection_matrix);
		// material properties
		program.setFloat("material.shininess", 40.0f);
		program.setInt("material.diffuse", 1);
		program.setInt("material.specular", 1);

		model.Draw(program);
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

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Turtle_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime / 2000;
		if (direction == Turtle_Movement::FORWARD)
		{
			Position += Front * velocity;
			AnimateWalk();
		}
		if (direction == Turtle_Movement::BACKWARD)
		{
			Position -= Front * velocity;
			AnimateWalk();
		}
		if (direction == Turtle_Movement::LEFT)
		{			
			Rotation.z += velocity * RotationSensitivity;
			Yaw -= velocity * RotationSensitivity;
			Yaw = NormalizeAngle(Yaw);
			updateTurtleVectors();
		}
		if (direction == Turtle_Movement::RIGHT)
		{			
			Rotation.z -= velocity * RotationSensitivity;
			Yaw += velocity * RotationSensitivity;
			Yaw = NormalizeAngle(Yaw);
			updateTurtleVectors();
		}
	}	


private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateTurtleVectors()
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