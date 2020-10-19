#ifndef MAZE_H
#define MAZE_H

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <model.h>


class Maze
{
public:
	Model model;
	// Position Attributes
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scaler;
	

	// Constructor
	Maze(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scaler = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		Position = position;
		Rotation = rotation;
		Scaler = scaler;
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
		program.setFloat("material.shininess", 18.0f);
		program.setInt("material.diffuse", 0);
		program.setInt("material.specular", 0);
		model.Draw(program);
	}

};
#endif