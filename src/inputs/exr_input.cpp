#include <epoxy/gl.h>

#if LIBSHADERTOY_OPENEXR
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImfRgbaFile.h>
#endif /* LIBSHADERTOY_OPENEXR */

#include "shadertoy/pre.hpp"

#include "shadertoy/gl.hpp"
#include "shadertoy/utils/log.hpp"

#include "shadertoy/inputs/exr_input.hpp"

using namespace shadertoy;
using namespace shadertoy::inputs;
using namespace shadertoy::utils;

std::shared_ptr<gl::texture> exr_input::load_file(const std::string &filename, bool vflip)
{
	std::shared_ptr<gl::texture> texture;

#if LIBSHADERTOY_OPENEXR
	Imf::RgbaInputFile in(filename.c_str());

	Imath::Box2i win = in.dataWindow();

	Imath::V2i dim(win.max.x - win.min.x + 1, win.max.y - win.min.y + 1);

	std::vector<Imf::Rgba> pixelBuffer(dim.x * dim.y);

	// Set buffer stride according to reading direction
	if (vflip)
		in.setFrameBuffer(pixelBuffer.data() + (dim.y - 1) * dim.x, 1, -dim.x);
	else
		in.setFrameBuffer(pixelBuffer.data(), 1, dim.x);

	// Read the whole image
	in.readPixels(win.min.y, win.max.y);

	// Create a texture object
	texture = std::make_shared<gl::texture>(GL_TEXTURE_2D);
	texture->image_2d(GL_TEXTURE_2D, 0, GL_RGBA16F, dim.x, dim.y, 0, GL_RGBA, GL_HALF_FLOAT,
					  pixelBuffer.data());
#else  /* LIBSHADERTOY_OPENEXR */
	log::shadertoy()->error("Cannot load {}: OpenEXR support is not enabled", filename);
#endif /* LIBSHADERTOY_OPENEXR */

	return texture;
}

exr_input::exr_input() : file_input() {}

exr_input::exr_input(const std::string &filename) : file_input(filename) {}