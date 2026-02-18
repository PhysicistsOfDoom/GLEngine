#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>
#include <iostream>

class Buffer
{
public:
	unsigned int VBO, VAO;

	Buffer(float* vertices, size_t size)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO); // Reserves space but doesn't allocate it yet.

		glBindVertexArray(VAO); // START VAO 

		glBindBuffer(GL_ARRAY_BUFFER, VBO); // Allocates space
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW); // Fills the space

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(0));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); // STOP VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void bind()
	{
		glBindVertexArray(VAO);
	}
};

#endif // !BUFFER_H