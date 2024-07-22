#pragma once
#include <unordered_map>
#include <string>

#include <rsl/primitives>
#include <rsl/utilities>
#include <rsl/math>

#include "core/components/transform.hpp"
#include "graphics/interface/definitions/definitions.hpp"
#include "graphics/components/camera.hpp"

namespace rythe::rendering
{
	class PipelineBase
	{
	protected:
		bool m_abort = false;
	public:
		RenderInterface RI;//This should be the only real version
		std::unordered_map<rsl::id_type, std::unique_ptr<framebuffer>> m_framebuffers;
		std::unordered_map<rsl::id_type, std::string> m_names;

		virtual void init() =0;
		virtual void render(core::transform camTransf, camera& cam) =0;
		virtual void shutdown() {}

		void abort()
		{
			m_abort = true;
		}

		void releaseFramebuffers()
		{
			for (auto& [id, fbo] : m_framebuffers)
			{
				//log::debug("Releasing framebuffer \"{}\"", m_names[id].c_str());
				fbo->release();
			}
		}

		bool hasFramebuffer(const std::string& name) const { return hasFramebuffer(rsl::nameHash(name)); }
		bool hasFramebuffer(rsl::id_type nameHash) const { return m_framebuffers.count(nameHash); }

		framebuffer* addFramebuffer(const std::string& name)
		{
			rsl::id_type id = rsl::nameHash(name);
			if (hasFramebuffer(id))
			{
				log::warn("Framebuffer \"{}\" already exists, returning existing framebuffer", name);
				return m_framebuffers[id].get();
			}

			auto fbo = m_framebuffers.emplace(id,std::make_unique<framebuffer>()).first->second.get();
			m_names[id] = name;
			fbo->initialize();
			//log::debug("Framebuffer created \"{}\"", name);
			return fbo;
		}		
		framebuffer* getFramebuffer(rsl::id_type nameHash) { if (hasFramebuffer(nameHash)) return m_framebuffers[nameHash].get(); return nullptr; }
		framebuffer* getFramebuffer(const std::string& name) { return getFramebuffer(rsl::nameHash(name)); }

	};
}
