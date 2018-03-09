#include <epoxy/gl.h>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl/renderbuffer.hpp"

using namespace shadertoy::gl;

null_renderbuffer_error::null_renderbuffer_error()
	: shadertoy_error("An attempt was made to dereference a null renderbuffer")
{
}

void renderbuffer::bind(GLenum target)
{
	gl_call(glBindRenderbuffer, target, GLuint(*this));
}

void renderbuffer::storage(GLenum internalFormat, GLsizei width, GLsizei height)
{
	gl_call(glNamedRenderbufferStorage, GLuint(*this), internalFormat, width, height);
}