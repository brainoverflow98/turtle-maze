#ifndef CABBAGE_H
#define CABBAGE_H

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <model.h>
#include <turtle.h>

class Cabbage
{

public:
	Model model;
	// Position Attributes
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scaler;
	glm::mat4 rot;

	//Sound Engine
	ISoundEngine *SoundEngine;

	bool collected = false;

	// Constructor
	Cabbage(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scaler = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		Position = position;
		Rotation = rotation;
		Scaler = scaler;
		rot = glm::mat4(1.0f);
	}

	bool CheckCollision(Turtle &turtle)
	{
		if (!collected)
		{
			float box = 0.2f;			
			glm::vec3 tpos = turtle.Position;
			if (tpos.x > Position.x - box && tpos.x < Position.x + box && tpos.z > Position.z - box && tpos.z < Position.z + box)
			{
				SoundEngine->play2D("sound/pickup.wav", GL_FALSE);
				turtle.points += 1;
				collected = true;
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	void Draw(Shader program, glm::mat4 view_matrix, glm::mat4 projection_matrix)
	{
		if (!collected)
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
			program.setFloat("material.shininess", 64.0f);
			program.setInt("material.diffuse", 0);
			program.setInt("material.specular", 1);
			model.Draw(program);
		}
	}

};
#endif