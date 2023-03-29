#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader {
    private:
	uint32_t m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;

    public:
	Shader(const std::string &vertexFilepath,
	       const std::string &fragmentFilepath);
	Shader(const std::string &computeFilepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void DispatchCompute(uint32_t xGroups = 1, uint32_t yGroups = 1,
			     uint32_t zGroups = 1);

	// Set uniforms
	void SetUniform1i(const std::string &name, int value);
	void SetUniform1iv(const std::string &name, uint32_t count,
			   int *values);
	void SetUniform1ui(const std::string &name, uint32_t value);
	void SetUniform1f(const std::string &name, float value);
	void SetUniform3f(const std::string &name, float v0, float v1,
			  float v2);
	void SetUniform3f(const std::string &name, glm::vec3 v);
	void SetUniform3i(const std::string &name, glm::ivec3 v);
	void SetUniform4f(const std::string &name, float v0, float v1, float v2,
			  float v3);
	void SetUniform4f(const std::string &name, glm::vec4 v);
	void SetUniformMat4f(const std::string &name, const glm::mat4 &matrix);

    private:
	std::string ParseShader(const std::string &filePath);
	ShaderProgramSource ParseCombinedShader(const std::string &filePath);
	uint32_t CompileShader(uint32_t type, const std::string &source);
	uint32_t CreateShader(const std::string &vertexShader,
			      const std::string &fragmentShader);
	uint32_t CreateShader(const std::string &computeShader);
	int GetUniformLocation(const std::string &name);
};