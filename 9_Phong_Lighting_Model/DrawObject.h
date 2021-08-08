#pragma once
class DrawObject
{
public:
	DrawObject();

	void Draw();

	void Release();

private:
	unsigned int VBO = 0, VAO = 0;
};

