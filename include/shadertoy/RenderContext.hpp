#ifndef _SHADERTOY_RENDER_CONTEXT_HPP_
#define _SHADERTOY_RENDER_CONTEXT_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief      Represents a context for rendering a specific ShaderToy program.
 */
class shadertoy_EXPORT RenderContext
{
protected:
	/// Last rendered-to texture
	std::weak_ptr<gl::texture> lastTexture;

	/// Program for screen quad
	gl::program screenProg;

private:
	/// Config reference
	ContextConfig &config;

	/// Vertex shader for screen quad
	gl::shader screenVs;

	/// Fragment shader for screen quad
	gl::shader screenFs;

	/// Vertex buffer for screen quad
	gl::buffer screenQuadCorners;
	/// Index buffer for screen quad
	gl::buffer screenQuadIndices;

	/// Screen quad source texture
	/// Note that this texture will only be allocated if needed
	std::shared_ptr<gl::texture> screenQuadTexture;

	/// Input texture engine
	std::shared_ptr<TextureEngine> textureEngine;

	/// Aux buffers
	std::map<std::string, std::shared_ptr<ToyBuffer>> buffers;

	// Cache for sources
	std::map<std::string, std::string> sourceCache;

	// Cache for define wrapper
	std::string defineWrapper;

	/// Raw frame count
	int frameCount;

	/// Uniform state
	ShaderInputsType state;

	// Callbacks

	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             initializing a custom texture engine instead of the default
	 *             one. Note that this field is invoked after all other member
	 *             fields of this class have been initialized, including the
	 *             config reference.
	 *
	 *             The derived class may also invoke this method to allocate a
	 *             texture engine with `buffer` texture type support, and add
	 *             its own handlers on the allocated instance.
	 *
	 * @return     A pointer to a TextureEngine derived instance, which will be
	 *             used by this instance.
	 */
	virtual std::shared_ptr<TextureEngine> BuildTextureEngine();

	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             executing code before the ShaderToy buffers are compiled and
	 *             initialized.
	 */
	virtual void PreInitializeBuffers();

	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             executing code after the ShaderToy buffers have been compiled
	 *             and initialized.
	 */
	virtual void PostInitializeBuffers();

	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             providing supplementary sources to insert in individual
	 *             buffer fragment shaders.
	 *
	 * @param      sources  List which should hold the sources to be inserted.
	 */
	virtual void LoadBufferSources(std::vector<std::pair<std::string, std::string>> &sources);

	/**
	 * @brief      When implemented in a derived class, provides a callback for
	 *             executing code after an auxiliary buffer has been rendered,
	 *             and its input and output textures swapped.
	 *
	 *             The latest rendered result of this buffer will thus be in the
	 *             *source* texture of the buffer.
	 *
	 * @param[in]  name    Name of the current buffer
	 * @param      buffer  Buffer object
	 */
	virtual void PostBufferRender(const std::string &name,
								  std::shared_ptr<ToyBuffer> &buffer);

	/**
	 * @brief      When implemented in a dervied class, provides a callback for
	 *             adding supplementary inputs to be bound to a given program.
	 *
	 * @param      inputs   List which should hold the bound inputs to be
	 *                      applied to the target program.
	 * @param      program  Target shader program.
	 */
	virtual void BindInputs(std::vector<std::shared_ptr<BoundInputsBase>> &inputs,
							gl::program &program);

public:
	/**
	 * @brief      Create a new render context.
	 *
	 * @param      config  Configuration for this context.
	 */
	RenderContext(ContextConfig &config);

	/**
	 * @brief      Initialize a new render context
	 */
	void Initialize();

	/**
	 * @brief      Initializes the ShaderToy-like buffers
	 */
	void InitializeBuffers();

	/**
	 * @brief      Allocates textures based on the current config width and
	 *             height. This can be useful for implementing framebuffer
	 *             resize, but is more lightweight than calling Initialize,
	 *             which recompiles shaders.
	 */
	void AllocateTextures();

	/**
	 * @brief      Clears all caches and buffers, call InitializeBuffers for
	 *             reloading.
	 */
	void ClearState();

	/**
	 * @brief      Render the current frame into the current OpenGL context
	 */
	void Render();

	/**
	 * @brief      Retrieves the texture ids to allow an operation which uses
	 *             the texIn texture as an input, and writes to the texOut
	 *             texture. The caller is executing the operation.
	 *
	 * @param[in]  texIn   Input texture id for outside operation
	 * @param[in]  texOut  Output texture id for outside operation
	 */
	void DoReadWriteCurrentFrame(GLuint &texIn, GLuint &texOut);

	/**
	 * @brief      Read the currently rendered result.
	 *
	 * @param      texIn  Input texture
	 */
	void DoReadCurrentFrame(GLuint &texIn);

	/**
	 * @brief      Get a reference to the texture engine of this render context.
	 *
	 * @return     The texture engine.
	 */
	inline TextureEngine &GetTextureEngine()
	{ return *textureEngine; }

	/**
	 * @brief      Compiles a fragment shader for use in a ToyBuffer.
	 *
	 * @param[in]  id  Buffer configuration identifier.
	 * @param      fs  Fragment shader object to compile to.
	 */
	void BuildBufferShader(const std::string &id,
						   gl::shader &fs);

	/**
	 * @brief      Loads a shader source by its path.
	 *
	 * @param[in]  path  Path to the shader file.
	 *
	 * @return     Contents of the shader source.
	 */
	const GLchar *LoadShaderSource(const boost::filesystem::path &path) throw(std::runtime_error);

	/**
	 * @brief      Get the define wrapper for fragment shaders.
	 *
	 * @return     Source code of the preprocessor definition wrapper.
	 */
	const GLchar *GetDefineWrapper() const;

	/**
	 * @brief      Gets a reference to the uniform state container
	 */
	inline ShaderInputsType &GetState()
	{ return state; }

	/**
	 * @brief      Gets a reference to the configuration objet for this context.
	 */
	inline ContextConfig &GetConfig()
	{ return config; }

	/**
	 * @brief      Binds uniforms to an actual program, returning the handle object to these bound uniforms.
	 *
	 * @param program Program to bind to
	 * @return
	 */
	std::vector<std::shared_ptr<BoundInputsBase>> GetBoundInputs(gl::program &program);

	/**
	 * @brief      Resets the current context target using the default viewport size
	 *
	 * @param[in]  level       Gray-level of the output
	 */
	void Clear(float level);

	/**
	 * @brief      Render a screen quad using the current context
	 */
	void RenderScreenQuad();

	/**
	 * @brief      Render a screen quad using the current context
	 *
	 * @param timerQuery Query object to use for measuring the runtime of the
	 *                   draw call.
	 */
	void RenderScreenQuad(gl::query &timerQuery);

	/**
	 * @brief      Binds the texture containing the shadertoy result as well as
	 *             a program which renders this texture to the viewport. Useful
	 *             for rendering to screen by calling
	 *             RenderContext::RenderScreenQuad().
	 */
	void BindResult();

	/**
	 * @brief      Get the default screen quad vertex shader
	 */
	gl::shader &GetScreenQuadVertexShader();

	/**
	 * @brief Obtains the buffer object for the given name.
	 *
	 * @param  name [optional] Name of the buffer object to obtain. If empty,
	 *              returns the image buffer.
	 * @return      Pointer to the buffer object, or a null pointer if no such
	 *              buffer exists.
	 */
	std::shared_ptr<ToyBuffer> GetBufferByName(const std::string &name = std::string());
};

}

#endif /* _SHADERTOY_RENDER_CONTEXT_HPP_ */
