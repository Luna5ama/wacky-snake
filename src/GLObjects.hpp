#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include <unordered_map>

namespace OpenGL {
	class IGLObject {
	public:
		~IGLObject();
		GLuint id;

		virtual void destroy() const {};
	};

	class IGLBinding : public IGLObject {
	protected:
		virtual void bind0(GLuint id) const {};

	public:
		void bind() const;
		void unbind() const;
	};

	class IGLTargetBinding : public IGLObject {
	protected:
		virtual void bind0(GLenum target, GLuint id) const {};

	public:
		void bind(GLenum target) const;
		void unbind(GLenum target) const;
	};

	class BufferObject : public IGLTargetBinding {
	public:
		BufferObject();

		GLsizeiptr size;

		void destroy() const override;
		void bind0(GLenum target, GLuint id) const override;

		virtual void allocate(GLsizeiptr size, unsigned int flags);
		virtual void allocate(const void* data, GLsizeiptr size, unsigned int flags);
		void invalidate() const;

		class Mutable;
		class Immutable;
	};

	class BufferObject::Mutable final : public BufferObject {
	public:
		void allocate(GLsizeiptr size, unsigned int flags) override;
		void allocate(const void* data, GLsizeiptr size, unsigned int flags) override;
	};

	class BufferObject::Immutable final : public BufferObject {
	public:
		void allocate(GLsizeiptr size, unsigned int flags) override;
		void allocate(const void* data, GLsizeiptr size, unsigned int flags) override;
	};

	class VertexArrayObject;

	class VertexAttribute {
	private:
		class Entry;
		std::vector<VertexAttribute::Entry*> entries;
		GLuint divisor;

	public:
		VertexAttribute(std::vector<VertexAttribute::Entry*> entries, GLsizei stride, GLuint divisor);
		~VertexAttribute();

		GLsizei stride;

		void apply(VertexArrayObject& vao, int binding);

		class Builder {
		private:
			GLsizei stride;
			GLuint divisor;
			GLuint offset;
			std::vector<VertexAttribute::Entry*> entries;
		public:
			Builder(GLsizei stride, GLuint divisor = 0);

			VertexAttribute::Builder& addInt(GLuint index, GLint size, GLenum type);
			VertexAttribute::Builder& addFloat(GLuint index, GLint size, GLenum type, GLboolean normalized);
			VertexAttribute::Builder& addPadding(GLuint size);
			VertexAttribute& build();
		};
	};

	class VertexArrayObject final : public IGLBinding {
	private:
		BufferObject* elementBuffer;
		std::vector<BufferObject*> vertexBuffers;

	public:
		VertexArrayObject();
		~VertexArrayObject();

		void destroy() const override;
		void bind0(GLuint id) const override;

		void attachElementBuffer(BufferObject& buffer);
		void attachVertexBuffer(BufferObject& buffer, GLuint offset, VertexAttribute& vertexAttribute);
		void attachVertexBuffer(BufferObject& buffer, VertexAttribute& vertexAttribute);

		void clearAttachments();

		void destroyAll();
	};

	class VertexAttribute::Entry {
	protected:
		GLuint index;
		GLint size;
		GLenum type;
		GLuint offset;
	public:
		virtual void apply(VertexArrayObject& vao, GLuint binding) const {};

		class Float;
		class Int;
	};

	class ShaderProgram : public IGLBinding {
	private:
		std::unordered_map<std::string, GLuint> bindings;
	public:
		ShaderProgram(const std::string vertex, const std::string fragment);
		~ShaderProgram();

		void destroy() const override;
		void bind0(const GLuint id) const override;

		void bindBuffer(const GLuint target, const BufferObject buffer, const std::string& blockName, const GLintptr offset = -1, const GLsizeiptr size = -1);
	};
}