#pragma once
#include <string>
#include <unordered_map>

#include <unordered_map>
#include <glm/glm.hpp>

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader {
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);
	void SetUniform1f(const std::string& name, float f);
	void SetUniform1i(const std::string& name, int i);
	void SetUniformMat4f(const std::string name, glm::mat4& matrix);
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;


	int GetUniformLocation(const std::string& name) const;
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	ShaderProgramSource ParseShader(const std::string& filepath);
};