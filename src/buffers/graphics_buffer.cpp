#include <epoxy/gl.h>

#include "shadertoy/buffers/graphics_buffer.hpp"

using namespace shadertoy::buffers;

buffer_output::buffer_output(std::string name, GLint location, GLenum type)
: name(name), location(location), type(type)
{
}

graphics_buffer::graphics_buffer(std::string id)
	: basic_buffer(id)
{
}

std::optional<std::vector<buffer_output>> graphics_buffer::get_buffer_outputs() const
{
	return std::nullopt;
}