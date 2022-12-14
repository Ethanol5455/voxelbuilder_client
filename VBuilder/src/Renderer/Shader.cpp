#include "vbpch.h"
#include "Shader.h"

#include <fstream>
#include <sstream>

Shader::Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath)
	: m_RendererID(0)
{
	ShaderProgramSource source;
	source.VertexSource = ParseShader(vertexFilepath);
	source.FragmentSource = ParseShader(fragmentFilepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::Shader(const std::string& computeFilepath)
	: m_RendererID(0)
{
	std::string computeSource = ParseShader(computeFilepath);
	m_RendererID = CreateShader(computeSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

std::string Shader::ParseShader(const std::string& filePath)
{
	std::ifstream stream(filePath);

	if (!stream.is_open())
	{
		std::cout << "Unable to open shader source at " << filePath << "\n";
	}

	std::string line;
	std::stringstream ss;
	while (getline(stream, line))
	{
		ss << line << '\n';
	}

	return ss.str();
}

ShaderProgramSource Shader::ParseCombinedShader(const std::string& filePath)
{
	std::ifstream stream(filePath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

uint32_t Shader::CompileShader(uint32_t type, const std::string& source)
{
	int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		if (type == GL_COMPUTE_SHADER)
			std::cout << "Failed to compile compute shader!" << '\n';
		else
			std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << '\n';
		std::cout << message << '\n';
		return 0;
	}

	return id;
}

uint32_t Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	uint32_t program = glCreateProgram();
	uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

uint32_t Shader::CreateShader(const std::string& computeShader)
{
	uint32_t program = glCreateProgram();
	uint32_t cs = CompileShader(GL_COMPUTE_SHADER, computeShader);

	glAttachShader(program, cs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(cs);

	return program;
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::DispatchCompute(uint32_t xGroups, uint32_t yGroups, uint32_t zGroups)
{
	glDispatchCompute(xGroups, yGroups, zGroups);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1iv(const std::string& name, uint32_t count, int* values)
{
	glUniform1iv(GetUniformLocation(name), count, values);
}

void Shader::SetUniform1ui(const std::string& name, uint32_t value)
{
	glUniform1ui(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform3f(const std::string& name, glm::vec3 v)
{
	glUniform3f(GetUniformLocation(name), v.x, v.y, v.z);
}

void Shader::SetUniform3i(const std::string& name, glm::ivec3 v)
{
	glUniform3i(GetUniformLocation(name), v.x, v.y, v.z);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform4f(const std::string& name, glm::vec4 v)
{
	glUniform4f(GetUniformLocation(name), v.r, v.g, v.b, v.a);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		VB_WARN("Warning: uniform '{0}' doesn't exist!", name);

	m_UniformLocationCache[name] = location;
	return location;
}