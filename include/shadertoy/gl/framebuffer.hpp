#ifndef _SHADERTOY_GL_FRAMEBUFFER_HPP_
#define _SHADERTOY_GL_FRAMEBUFFER_HPP_

#include "shadertoy/gl/resource.hpp"

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null framebuffer.
	 */
		class shadertoy_EXPORT null_framebuffer_error : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the null_framebuffer_error class.
		 */
		explicit null_framebuffer_error();
	};

	/**
	 * @brief Represents an OpenGL framebuffer object
	 */
	class shadertoy_EXPORT framebuffer : public resource<
		framebuffer,
		multi_allocator<&glCreateFramebuffers, &glDeleteFramebuffers>,
		null_framebuffer_error>
	{
	public:
		/**
		 * @brief glBindFramebuffer
		 *
		 * @param target Target to bind this framebuffer to
		 *
		 * @throws opengl_error
		 */
		void bind(GLenum target);

		/**
		 * @brief glNamedFramebufferTexture
		 *
		 * @param attachment Framebuffer attachment
		 * @param texture    Texture object to attach
		 * @param level      Texture level to attach
		 *
		 * @throws opengl_error
		 * @throws null_texture_error
		 */
		void texture(GLenum attachment, texture &texture, GLint level);
	};
}
}

#endif /* _SHADERTOY_GL_FRAMEBUFFER_HPP_ */
