#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> //필요한 모든 OpenGL 헤더를 얻을 수 있는 glad 포함.
#include <glm/glm.hpp>
#include <vector>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdarg.h>

class Shader
{
public:
	// the program ID
	unsigned int ID;

	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		// 1. filePath에서 정점/조각 소스 코드를 검색합니다.
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		// 스트림 개체가 예외를 적용할 수 있는지 확인 :
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// 파일 열기
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// 파일의 버퍼 내용을 스트림으로 읽기
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// 파일 처리기 닫기
			vShaderFile.close();
			fShaderFile.close();
			//스트림을 문자열로 바꾼다.
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (const std::exception&)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. 컴파일 셰이더
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		// vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		//오류가 있을 경우 출력해라.
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		//컴파일 오류가 있을 경우 출력해라.
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// shader 프로그램
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		//링킹 오류가 있을 경우 출력해라.
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		// 셰이더를 지금 프로그램에 연결했으므로 더 이상 필요하지 않습니다.
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	};

	//가변 인자를 사용하여 값을 여러 개 받도록 한다.
	Shader(int args, ...)
	{
		// 1. filePath에서 정점/조각 소스 코드를 검색합니다.
		std::vector<std::string> Code;
		Code.reserve(args);

		va_list ap; //가변 인자 목록 포인터
		va_start(ap, args);

		for (int i = 0; i < args; i++)
		{
			std::ifstream ShaderFile;

			ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				std::string path = va_arg(ap, char*);
				// 파일 열기
				ShaderFile.open(path);

				std::stringstream shaderStream;
				// 파일의 버퍼 내용을 스트림으로 읽기
				shaderStream << ShaderFile.rdbuf();

				// 파일 처리기 닫기
				ShaderFile.close();

				// 스트림을 문자열로 바꾼다.
				Code.push_back(shaderStream.str());

			}
			catch (const std::exception&)
			{
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			}
		}
		va_end(ap);

		std::vector<GLuint> GL_SHADER = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER };

		// 컴파일 셰이더
		std::vector<int> shader;
		int success;
		char infoLog[512];

		for (int i = 0; i < args; i++)
		{
			if (GL_SHADER.size() <= i)
				break;

			const char* shaderCode = Code[i].c_str();
			shader.push_back(glCreateShader(GL_SHADER[i]));
			glShaderSource(shader[i], 1, &shaderCode, NULL);
			glCompileShader(shader[i]);

			//오류가 있을 경우 출력해라.
			glGetShaderiv(shader[i], GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader[i], 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
		}

		// shader 프로그램
		ID = glCreateProgram();
		for (int i = 0; i < shader.size(); i++)
		{
			glAttachShader(ID, shader[i]);
		}
		glLinkProgram(ID);

		// 링킹 오류가 있을 경우 출력해라
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		// 셰이더를 지금 프로그램에 연결했으므로 더 이상 필요하지 않습니다.
		for (int i = 0; i < args; i++)
		{
			glDeleteShader(shader[i]);
		}
	}

	// use/activate the shader
	void use()
	{
		glUseProgram(ID);
	};
	// utility uniform functions
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	};
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	};
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	};
	void setFloat4(const std::string& name, float value1, float value2, float value3, float value4) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3, value4);
	};
	void setMat4(const std::string& name, glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	};
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	};
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	};
};

#endif // !SHADER_H
