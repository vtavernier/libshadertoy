#ifndef _SHADERTOY_OPENGL_PROGRAM_HPP_
#define _SHADERTOY_OPENGL_PROGRAM_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class NullProgramError : public shadertoy::ShadertoyError
	{
	public:
		explicit NullProgramError();
	};

	class Shader;

	class UniformLocation
	{
	public:
		UniformLocation(const Program &program, GLint location);

		bool IsActive() const;

		// glProgramUniform
		// return true if the value was set
		bool SetValue(const GLint &v0);
		bool SetValue(const GLfloat &v0);
		bool SetValue(const glm::vec2 &v);
		bool SetValue(const glm::vec3 &v);
		bool SetValue(const glm::vec4 &v);

		bool SetValue(size_t count, const GLint *v0);
		bool SetValue(size_t count, const GLfloat *v0);
		bool SetValue(size_t count, const glm::vec2 *v);
		bool SetValue(size_t count, const glm::vec3 *v);
		bool SetValue(size_t count, const glm::vec4 *v);

	private:
		const GLuint _program;
		const GLint _location;
	};

	class Program : public Resource<Program, SingleDeleter<&glDeleteProgram>, NullProgramError>
	{
	public:
		// glLinkProgram
		void Link();

		// glUseProgram
		void Use();

		// glGetUniformLocation
		UniformLocation GetUniformLocation(const GLchar *name);

		// glAttachShader
		void AttachShader(const Shader &shader);
	};
}
}

#endif /* _SHADERTOY_OPENGL_PROGRAM_HPP_ */
