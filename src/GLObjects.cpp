#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "GLObjects.hpp"

using namespace OpenGL;

IGLObject::~IGLObject() {
	destroy();
}

void IGLBinding::bind() const {
	bind0(id);
}

void IGLBinding::unbind() const {
	bind0(0);
}


void IGLTargetBinding::bind(GLenum unit) const {
	bind0(unit, id);
}

void IGLTargetBinding::unbind(GLenum target) const {
	bind0(target, 0);
}


BufferObject::BufferObject() : size(0) {
	glCreateBuffers(1, &id);
}


void BufferObject::destroy() const {
	glDeleteBuffers(1, &id);
}

void BufferObject::bind0(GLenum target, GLuint id) const {
	glBindBuffer(target, id);
}

void BufferObject::allocate(GLsizeiptr size, unsigned int flags) {
	this->size = size;
}

void BufferObject::allocate(const void* data, GLsizeiptr size, unsigned int flags) {
	this->size = size;
}

void BufferObject::invalidate() const {
	glInvalidateBufferData(IGLObject::id);
}


void BufferObject::Mutable::allocate(GLsizeiptr size, unsigned int flags) {
	BufferObject::allocate(size, flags);
	glNamedBufferData(id, size, NULL, flags);
}

void BufferObject::Mutable::allocate(const void* data, GLsizeiptr size, unsigned int flags) {
	BufferObject::allocate(data, size, flags);
	glNamedBufferData(id, size, data, flags);
}


void BufferObject::Immutable::allocate(GLsizeiptr size, unsigned int flags) {
	BufferObject::allocate(size, flags);
	glNamedBufferStorage(id, size, NULL, flags);
}

void BufferObject::Immutable::allocate(const void* data, GLsizeiptr size, unsigned int flags) {
	BufferObject::allocate(data, size, flags);
	glNamedBufferStorage(id, size, data, flags);
};


VertexArrayObject::VertexArrayObject() : elementBuffer(NULL) {
	glCreateVertexArrays(1, &id);
}

VertexArrayObject::~VertexArrayObject() {
	destroyAll();
}

void VertexArrayObject::destroy() const {
	glDeleteVertexArrays(1, &id);
}

void VertexArrayObject::bind0(GLuint id) const {
	glBindVertexArray(id);
}

void VertexArrayObject::attachElementBuffer(BufferObject& buffer) {
	glVertexArrayElementBuffer(id, buffer.id);
	elementBuffer = &buffer;
}

void VertexArrayObject::attachVertexBuffer(BufferObject& buffer, GLuint offset, VertexAttribute& vertexAttribute) {
	GLuint binding = (GLuint) vertexBuffers.size();
	glVertexArrayVertexBuffer(id, binding, buffer.id, offset, vertexAttribute.stride);
	vertexBuffers.push_back(&buffer);
	vertexAttribute.apply(*this, binding);
}

void VertexArrayObject::attachVertexBuffer(BufferObject& buffer, VertexAttribute& vertexAttribute) {
	attachVertexBuffer(buffer, 0, vertexAttribute);
}

void VertexArrayObject::clearAttachments() {
	if (elementBuffer != NULL) {
		glVertexArrayElementBuffer(id, 0);
		elementBuffer = NULL;
	}
	GLuint length = (GLuint) vertexBuffers.size();
	for (GLuint i = 0; i < length; i++) {
		glVertexArrayVertexBuffer(id, i, 0, 0, 0);
	}
	vertexBuffers.clear();
}

void VertexArrayObject::destroyAll() {
	destroy();
	if (elementBuffer != NULL) {
		elementBuffer->destroy();
	}
	size_t length = vertexBuffers.size();
	for (size_t i = 0; i < length; i++) {
		vertexBuffers[i]->destroy();
	}
	vertexBuffers.clear();
}


VertexAttribute::VertexAttribute(std::vector<VertexAttribute::Entry*> entries, GLsizei stride, GLuint divisor) : entries(entries), stride(stride), divisor(divisor) {};

VertexAttribute::~VertexAttribute() {
	size_t length = entries.size();
	for (size_t i = 0; i < length; i++) {
		delete entries[i];
	}
	entries.clear();
}

void VertexAttribute::apply(VertexArrayObject& vao, int binding) {
	size_t length = entries.size();
	for (size_t i = 0; i < length; i++) {
		entries[i]->apply(vao, binding);
	}
	glVertexArrayBindingDivisor(vao.id, binding, divisor);
}

class VertexAttribute::Entry::Float final : VertexAttribute::Entry {
private:
	GLboolean normalized;
public:
	Float(GLuint index, GLint size, GLenum type, GLuint offset, GLboolean normalized) : normalized(normalized) {
		VertexAttribute::Entry::index = index;
		VertexAttribute::Entry::size = size;
		VertexAttribute::Entry::type = type;
		VertexAttribute::Entry::offset = offset;
	}

	void apply(VertexArrayObject& vao, GLuint binding) const override {
		glEnableVertexArrayAttrib(vao.id, index);
		glVertexArrayAttribFormat(vao.id, index, size, type, normalized, offset);
		glVertexArrayAttribBinding(vao.id, index, binding);
	};
};

class VertexAttribute::Entry::Int final : VertexAttribute::Entry {
public:
	Int(GLuint index, GLint size, GLenum type, GLuint offset) {
		VertexAttribute::Entry::index = index;
		VertexAttribute::Entry::size = size;
		VertexAttribute::Entry::type = type;
		VertexAttribute::Entry::offset = offset;
	}

	void apply(VertexArrayObject& vao, GLuint binding) const override {
		glEnableVertexArrayAttrib(vao.id, index);
		glVertexArrayAttribIFormat(vao.id, index, size, type, offset);
		glVertexArrayAttribBinding(vao.id, index, binding);
	};
};

VertexAttribute::Builder::Builder(GLsizei stride, GLuint divisor) : stride(stride), divisor(divisor), offset(0) {}

unsigned int getSize(GLenum type) {
	int result = 0;
	switch (type) {
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			result = 1;
			break;
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
			result = 2;
			break;
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
			result = 4;
			break;
		default:
			throw 0;
			break;
	}
	return result;
}

VertexAttribute::Builder& VertexAttribute::Builder::addInt(GLuint index, GLint size, GLenum type) {
	entries.push_back((VertexAttribute::Entry*) new VertexAttribute::Entry::Int(index, size, type, this->offset));
	offset += size * getSize(type);
	return *this;
}

VertexAttribute::Builder& VertexAttribute::Builder::addFloat(GLuint index, GLint size, GLenum type, GLboolean normalized) {
	entries.push_back((VertexAttribute::Entry*) new VertexAttribute::Entry::Float(index, size, type, this->offset, normalized));
	offset += size * getSize(type);
	return *this;
}

VertexAttribute::Builder& VertexAttribute::Builder::addPadding(GLuint size) {
	offset += size;
	return *this;
}

VertexAttribute& VertexAttribute::Builder::build() {
	return *new VertexAttribute(entries, stride, divisor);
}

GLint createShader(const std::string path, GLenum shaderType) {
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();

	std::string codeSrc = buffer.str();
	const GLchar* codeSrcCStr = codeSrc.c_str();

	GLint id = glCreateShader(shaderType);
	glShaderSource(id, 1, &codeSrcCStr, NULL);
	glCompileShader(id);

	GLint compiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint logLength;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0) {
			std::vector<GLchar> infoLog(logLength);
			glGetShaderInfoLog(id, logLength, &logLength, &infoLog[0]);
			std::cout << &infoLog[0] << std::endl;
		}
	}

	return id;
}

OpenGL::ShaderProgram::ShaderProgram(const std::string vertex, const std::string fragment) {
	this->id = glCreateProgram();
	int vert = createShader(vertex, GL_VERTEX_SHADER);
	int frag = createShader(fragment, GL_FRAGMENT_SHADER);

	glAttachShader(this->id, vert);
	glAttachShader(this->id, frag);

	glLinkProgram(this->id);

	GLint linked = 0;
	glGetProgramiv(this->id, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE) {
		GLint logLength;
		glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0) {
			std::vector<GLchar> infoLog(logLength);
			glGetProgramInfoLog(this->id, logLength, &logLength, &infoLog[0]);
			std::cout << &infoLog[0] << std::endl;
		}
	}

	glDetachShader(this->id, vert);
	glDetachShader(this->id, frag);

	glDeleteShader(vert);
	glDeleteShader(frag);
}

void OpenGL::ShaderProgram::destroy() const {
	glDeleteProgram(this->id);
}


OpenGL::ShaderProgram::~ShaderProgram() {
	destroy();
}

void OpenGL::ShaderProgram::bind0(const GLuint id) const {
	glUseProgram(id);
}

void OpenGL::ShaderProgram::bindBuffer(const GLuint target, const BufferObject buffer, const std::string& blockName, const GLintptr offset, const GLsizeiptr size) {
	if (this->bindings.find(blockName) == this->bindings.end()) {
		this->bindings[blockName] = this->bindings.size();
	}
	GLuint bindingIndex = this->bindings[blockName];
	if (offset == -1 || size == -1) {
		glBindBufferBase(target, bindingIndex, buffer.id);
	}
	else {
		glBindBufferRange(target, bindingIndex, buffer.id, offset, size);
	}
}
