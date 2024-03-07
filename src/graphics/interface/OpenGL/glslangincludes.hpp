#pragma once
#include <rsl/logging>
#include <rsl/primitives>

#define ENABLE_HLSL
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Include/ShHandle.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/SPIRV/GLSL.std.450.h>
#include <glslang/SPIRV/doc.h>
#include <glslang/SPIRV/disassemble.h>
#include <glslang/MachineIndependent/localintermediate.h>

#include <glslang/build_info.h>

#include <spirv_cross/spirv_glsl.hpp>

namespace rythe::rendering::internal
{
	enum TOptions : uint64_t {
		EOptionNone = 0,
		EOptionIntermediate = (1ull << 0),
		EOptionSuppressInfolog = (1ull << 1),
		EOptionMemoryLeakMode = (1ull << 2),
		EOptionRelaxedErrors = (1ull << 3),
		EOptionGiveWarnings = (1ull << 4),
		EOptionLinkProgram = (1ull << 5),
		EOptionMultiThreaded = (1ull << 6),
		EOptionDumpConfig = (1ull << 7),
		EOptionDumpReflection = (1ull << 8),
		EOptionSuppressWarnings = (1ull << 9),
		EOptionDumpVersions = (1ull << 10),
		EOptionSpv = (1ull << 11),
		EOptionHumanReadableSpv = (1ull << 12),
		EOptionVulkanRules = (1ull << 13),
		EOptionDefaultDesktop = (1ull << 14),
		EOptionOutputPreprocessed = (1ull << 15),
		EOptionOutputHexadecimal = (1ull << 16),
		EOptionReadHlsl = (1ull << 17),
		EOptionCascadingErrors = (1ull << 18),
		EOptionAutoMapBindings = (1ull << 19),
		EOptionFlattenUniformArrays = (1ull << 20),
		EOptionNoStorageFormat = (1ull << 21),
		EOptionKeepUncalled = (1ull << 22),
		EOptionHlslOffsets = (1ull << 23),
		EOptionHlslIoMapping = (1ull << 24),
		EOptionAutoMapLocations = (1ull << 25),
		EOptionDebug = (1ull << 26),
		EOptionStdin = (1ull << 27),
		EOptionOptimizeDisable = (1ull << 28),
		EOptionOptimizeSize = (1ull << 29),
		EOptionInvertY = (1ull << 30),
		EOptionDumpBareVersion = (1ull << 31),
		EOptionCompileOnly = (1ull << 32),
	};

	class DireStackFileIncluder : public glslang::TShader::Includer
	{
	public:
		DireStackFileIncluder() : externalLocalDirectoryCount(0) { }

		virtual IncludeResult* includeLocal(const char* headerName,
			const char* includerName,
			size_t inclusionDepth) override
		{
			return readLocalPath(headerName, includerName, (int)inclusionDepth);
		}

		virtual IncludeResult* includeSystem(const char* headerName,
			const char* /*includerName*/,
			size_t /*inclusionDepth*/) override
		{
			return readSystemPath(headerName);
		}

		// Externally set directories. E.g., from a command-line -I<dir>.
		//  - Most-recently pushed are checked first.
		//  - All these are checked after the parse-time stack of local directories
		//    is checked.
		//  - This only applies to the "local" form of #include.
		//  - Makes its own copy of the path.
		virtual void pushExternalLocalDirectory(const std::string& dir)
		{
			directoryStack.push_back(dir);
			externalLocalDirectoryCount = (int)directoryStack.size();
		}

		virtual void releaseInclude(IncludeResult* result) override
		{
			if (result != nullptr) {
				delete[] static_cast<tUserDataElement*>(result->userData);
				delete result;
			}
		}

		virtual std::set<std::string> getIncludedFiles()
		{
			return includedFiles;
		}

		virtual ~DireStackFileIncluder() override { }

	protected:
		typedef char tUserDataElement;
		std::vector<std::string> directoryStack;
		int externalLocalDirectoryCount;
		std::set<std::string> includedFiles;

		// Search for a valid "local" path based on combining the stack of include
		// directories and the nominal name of the header.
		virtual IncludeResult* readLocalPath(const char* headerName, const char* includerName, int depth)
		{
			// Discard popped include directories, and
			// initialize when at parse-time first level.
			directoryStack.resize(depth + externalLocalDirectoryCount);
			if (depth == 1)
				directoryStack.back() = getDirectory(includerName);

			// Find a directory that works, using a reverse search of the include stack.
			for (auto it = directoryStack.rbegin(); it != directoryStack.rend(); ++it) {
				std::string path = *it + '/' + headerName;
				std::replace(path.begin(), path.end(), '\\', '/');
				std::ifstream file(path, std::ios_base::binary | std::ios_base::ate);
				if (file) {
					directoryStack.push_back(getDirectory(path));
					includedFiles.insert(path);
					return newIncludeResult(path, file, (int)file.tellg());
				}
			}

			return nullptr;
		}

		// Search for a valid <system> path.
		// Not implemented yet; returning nullptr signals failure to find.
		virtual IncludeResult* readSystemPath(const char* /*headerName*/) const
		{
			return nullptr;
		}

		// Do actual reading of the file, filling in a new include result.
		virtual IncludeResult* newIncludeResult(const std::string& path, std::ifstream& file, int length) const
		{
			char* content = new tUserDataElement[length];
			file.seekg(0, file.beg);
			file.read(content, length);
			return new IncludeResult(path, content, length, content);
		}

		// If no path markers, return current working directory.
		// Otherwise, strip file name and return path leading up to it.
		virtual std::string getDirectory(const std::string path) const
		{
			size_t last = path.find_last_of("/\\");
			return last == std::string::npos ? "." : path.substr(0, last);
		}
	};

}