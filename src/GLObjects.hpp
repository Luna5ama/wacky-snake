#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

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

    class TextureObject : public IGLTargetBinding {
    public:
        GLenum target;

        TextureObject(GLenum target);
        ~TextureObject();

        void destroy() const override;
        void bind0(GLenum unit, GLuint id) const override;

        class Texture2D;
    };

    class TextureObject::Texture2D final : public TextureObject {
    public:
        glm::i32vec2 size;

        Texture2D();

        void allocate(const GLsizei levels, const GLenum internalformat, const glm::i32vec2 size);
        void upload(const GLint level, const glm::i32vec2 offset, const glm::i32vec2 size, GLenum format, GLenum type, const void* pixels);
    };

    class RenderBuffer final : public IGLBinding {
    public:
        glm::i32vec2 size;

        RenderBuffer();
        ~RenderBuffer();

        void destroy() const override;
        void bind0(GLuint id) const override;

        void allocate(const GLenum internalformat, const glm::i32vec2 size);
    };

    class Framebuffer final : public IGLTargetBinding {
    private:
        std::vector<const IGLObject*> colorAttachments;
        const IGLObject* depthAttachment;
        const IGLObject* stencilAttachment;

    public:
        Framebuffer();
        ~Framebuffer();

        void destroy() const override;
        void bind0(const GLenum target, const GLuint id) const override;

        void attach(const TextureObject& texture, const GLenum attachment, const GLint level = 0);
        void attach(const RenderBuffer& buffer, const GLenum attachment, const GLint level = 0);
        void check() const;
        void destroyAll();
    };

    class ShaderProgram : public IGLBinding {
    public:
        ShaderProgram(const std::string vertex, const std::string fragment);
        ~ShaderProgram();

        void destroy() const override;
        void bind0(const GLuint id) const override;
    };
}