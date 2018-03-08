#include <algorithm>
#include <sstream>
#include <vector>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/ShadertoyError.hpp"
#include "shadertoy/OpenGL.hpp"

#include "shadertoy/ShaderCompiler.hpp"

using namespace std;
using namespace shadertoy;

ShaderCompiler::ShaderCompiler()
	: namedSources()
{
}

void ShaderCompiler::Compile(OpenGL::Shader &shader)
{
	// Transform pairs into list of C strings
	vector<std::string> sources(namedSources.size());
	transform(namedSources.begin(), namedSources.end(), sources.begin(),
		[] (const pair<string, string> &namedSource) {
			return namedSource.second;
		});

	// Also, build a line count
	vector<int> lineCounts(namedSources.size());
	transform(namedSources.begin(), namedSources.end(), lineCounts.begin(),
		[] (const pair<string, string> &namedSource) {
			return count(namedSource.second.begin(),
						 namedSource.second.end(),
						 '\n');
		});

	// Load sources in fragment shader and compile
	try
	{
		shader.Source(sources);
		shader.Compile();
	}
	catch (OpenGL::ShaderCompilationError &ex)
	{
		// Reparse log and replace sources with their actual names
		std::stringstream is(ex.log()), os;
		std::string msg;

		while (!is.eof())
		{
			char c, d;
			size_t pid, pline;

			// Read one message
			getline(is, msg);

			// Try parsing message
			std::stringstream msgis(msg);

			// Matches ID(LINE.
			//         ID:LINE.
			msgis >> pid >> c >> pline >> d;

			if (msgis.fail())
			{
				// Parsing failed
				os << msg << endl;
			}
			else
			{
				// Parsing succeeded, find the source part which contains the
				// actual error
				size_t lc, lcn, li = 0;
				for (lc = 0, lcn = 0;
					 li < lineCounts.size();
					 li++)
				{
					lc = lcn;
					lcn += lineCounts.at(li);
					if (lcn > pline) break;
				}

				// If li exceeds known programs, an error occurred, pass the
				// message verbatim
				if (li >= lineCounts.size())
				{
					os << msg << endl;
					continue;
				}

				// Output a formatted message with the error
				os << namedSources.at(li).first
				   << c
				   << (pline - lc)
				   << d
				   << msgis.rdbuf()
				   << endl;
			}
		}

		// Update exception and rethrow
		throw OpenGL::ShaderCompilationError(GLuint(shader), os.str());
	}
}
