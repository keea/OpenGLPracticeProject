#pragma once

// http://www.songho.ca/opengl/gl_sphere.html
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace sphere
{
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};
}


class DrawSphere
{
public:
	DrawSphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18)
	{
		this->radius = radius;
		this->sectorCount = sectorCount;
		this->stackCount = stackCount;

		SetVerticesData();
		SetIndices();

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, verticesData.size() * sizeof(sphere::Vertex), verticesData.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		//정점 위치
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sphere::Vertex), (void*)0);
		// 정점 법선
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sphere::Vertex), (void*)offsetof(sphere::Vertex, Normal));
		//정점 texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sphere::Vertex), (void*)offsetof(sphere::Vertex, TexCoords));

		glBindVertexArray(0);
	}
	void Draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	void Release()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

private:
	unsigned int VBO = 0, VAO = 0, EBO = 0;
	const float PI = acos(-1);

	//vertices
	std::vector<sphere::Vertex> verticesData;
	std::vector<unsigned int>	indices;

	int radius;
	int sectorCount;
	int stackCount;

	void SetVerticesData()
	{
		//이전 배열의 메모리 지우기
		std::vector<sphere::Vertex>().swap(verticesData);

		float x, y, z, xy; // 정점 위치
		float nx, ny, nz, lengthInv = 1.0f / radius; // 정점 노말
		float s, t; // 정점 텍스처 좌표

		float sectorStep = 2 * PI / sectorCount;
		float stackStep = PI / stackCount;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = PI / 2 - i * stackStep; // pi/2에서 -pi/2로 시작
			xy = radius * cosf(stackAngle);		 // r * cos(u)
			z = radius * sinf(stackAngle);		 // r * sin(u)

			// 스택당 정점 추가 (sectorCount+1)
			// 첫 번째 정점과 마지막 정점은 위치와 법선이 같지만 tex좌표가 다릅니다.
			for (int j = 0; j <= sectorCount; ++j)
			{
				sectorAngle = j * sectorStep;	// 0에서 2pi로 시작

				sphere::Vertex vertexData;

				// 정점 위치(x, y, z)
				x = xy * cosf(sectorAngle);		// r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);		// r * cos(u) * sin(v)
				vertexData.Position = glm::vec3(x, y, z);

				// 정규화된 정점 법선(nx, ny, nz)
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				vertexData.Normal = glm::vec3(nx, ny, nz);

				// 정점 텍스처 좌표(s, t)범위 [0, 1]
				s = (float)j / sectorCount;
				t = (float)i / stackCount;
				vertexData.TexCoords = glm::vec2(s, t);

				verticesData.push_back(vertexData);
			}
		}
	}
	void SetIndices()
	{
		// 구 삼각형의 CCW 인덱스 목록 생성
	// k1 ⟶ k2 ⟶ k1+1
	// k1+1 ⟶ k2 ⟶ k2+1
	// k1--k1+1
	// |  / |
	// | /  |
	// k2--k2+1

		int k1, k2;
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1); // 현재 스택의 시작
			k2 = k1 + sectorCount + 1;	// 다음 스택의 시작

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// 첫 번째 및 마지막 스택을 제외한 섹터당 2개의 삼각형
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (stackCount - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}
	}
};