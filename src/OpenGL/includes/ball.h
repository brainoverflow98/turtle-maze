#ifndef BALL_H
#define BALL_H

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <turtle.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class Ball_Movement {
	FORWARD,
	BACKWARD	
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Ball
{
public:	
	Model model;
	// Position Attributes
	Turtle &Follow;
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scaler;
	glm::vec3 PositionOffset;
	

	// Constructor with vectors
	Ball(Turtle &follow, glm::vec3 position_offset = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scaler = glm::vec3(1.0f, 1.0f, 1.0f)) : Follow(follow)
	{		
		Rotation = rotation;
		Scaler = scaler;
		PositionOffset = position_offset;
		Position = follow.Position + (position_offset.z * follow.Front) + (position_offset.y * follow.Up) + (position_offset.x * follow.Right);		
	}	

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Ball_Movement direction, float deltaTime)
	{		
		if (direction == Ball_Movement::FORWARD)
		{
			Position = Follow.Position + (PositionOffset.z * Follow.Front) + (PositionOffset.y * Follow.Up) + (PositionOffset.x * Follow.Right);		
		}
		if (direction == Ball_Movement::BACKWARD)
		{
			Position = Follow.Position + (PositionOffset.z * Follow.Front) + (PositionOffset.y * Follow.Up) + (PositionOffset.x * Follow.Right);			
		}						
	}


	void Draw(Shader program, glm::mat4 view_matrix, glm::mat4 projection_matrix)
	{
		glm::mat4 model_matrix = glm::mat4(1.0f);

		if (Position != glm::vec3(0.0f, 0.0f, 0.0f))
		{
			model_matrix = glm::translate(model_matrix, glm::vec3(Position.x, Position.y, Position.z));
		}

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
		program.setFloat("material.shininess", 64.0f);
		program.setInt("material.diffuse", 0);
		program.setInt("material.specular", 1);
		model.Draw(program);
	}

};
#endif