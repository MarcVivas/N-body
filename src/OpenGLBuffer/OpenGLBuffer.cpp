#include "OpenGLBuffer.h"
#include <stdexcept> // Potentially for errors, although returning bool/nullptr now
#include <iostream>  // For error messages (optional)
#include <cstring>   // For memcpy

OpenGLBuffer::OpenGLBuffer() : id_(0), size_(0), binding_point_(0) {}

OpenGLBuffer::~OpenGLBuffer() {
	destroy();
}

bool OpenGLBuffer::createBufferStorage(GLsizeiptr size, const void* data, GLuint binding_point, GLbitfield flags) {
    if (id_ != 0) {
        // Buffer already created, destroy first or handle error
        std::cerr << "OpenGLBuffer::create error: Buffer already has ID " << id_ << ". Destroy first." << std::endl;
        return false;
    }
    if (size <= 0) {
        std::cerr << "OpenGLBuffer::create error: Size must be positive." << std::endl;
        return false;
    }


    glGenBuffers(1, &id_);
    if (id_ == 0) {
        // Handle generation error
        std::cerr << "OpenGLBuffer::create error: glGenBuffers failed." << std::endl;
        return false;
    }

    // Assume target is SSBO
    GLenum target = GL_SHADER_STORAGE_BUFFER;
    glBindBuffer(target, id_);

    // Use glBufferStorage for immutable storage / persistent mapping capabilities
    glBufferStorage(target, size, data, flags);
    


    glBindBufferBase(target, binding_point, id_);
  

    glBindBuffer(target, 0); // Unbind the target

    this->size_ = size;
    this->binding_point_ = binding_point;

    // std::cout << "OpenGLBuffer created: ID=" << id_ << ", Size=" << size_ << ", Binding=" << binding_point_ << std::endl;
    return true;
}

bool OpenGLBuffer::createBufferData(GLsizeiptr size, const void* data, GLuint binding_point, GLbitfield flags) {
    if (id_ != 0) {
        // Buffer already created, destroy first or handle error
        std::cerr << "OpenGLBuffer::create error: Buffer already has ID " << id_ << ". Destroy first." << std::endl;
        return false;
    }
    if (size <= 0) {
        std::cerr << "OpenGLBuffer::create error: Size must be positive." << std::endl;
        return false;
    }


    glGenBuffers(1, &id_);
    if (id_ == 0) {
        // Handle generation error
        std::cerr << "OpenGLBuffer::create error: glGenBuffers failed." << std::endl;
        return false;
    }

    // Assume target is SSBO
    GLenum target = GL_SHADER_STORAGE_BUFFER;
    glBindBuffer(target, id_);

    glBufferData(target, size, data, flags);

    glBindBufferBase(target, binding_point, id_);


    glBindBuffer(target, 0); // Unbind the target

    this->size_ = size;
    this->binding_point_ = binding_point;

    // std::cout << "OpenGLBuffer created: ID=" << id_ << ", Size=" << size_ << ", Binding=" << binding_point_ << std::endl;
    return true;
}

void OpenGLBuffer::destroy() {
    if (id_ != 0) {
        glDeleteBuffers(1, &id_);
        // std::cout << "OpenGLBuffer destroyed: ID=" << id_ << std::endl;
        id_ = 0;
        size_ = 0;
        binding_point_ = 0;
    }
}

void OpenGLBuffer::updateData(GLintptr offset, GLsizeiptr size, const void* data) {
    if (!isValid()) {
        std::cerr << "OpenGLBuffer::updateData error: Buffer not valid." << std::endl;
        return;
    }
    if (offset < 0 || size <= 0 || offset + size > size_) {
        std::cerr << "OpenGLBuffer::updateData error: Invalid offset/size." << std::endl;
        return;
    }

    GLenum target = GL_SHADER_STORAGE_BUFFER;
    glBindBuffer(target, id_);
    glBufferSubData(target, offset, size, data);
    glBindBuffer(target, 0);
}

void OpenGLBuffer::bindBufferBase(GLuint binding_point) {
	if (!isValid()) {
		std::cerr << "OpenGLBuffer::bindBufferBase error: Buffer not valid." << std::endl;
		return;
	}
	GLenum target = GL_SHADER_STORAGE_BUFFER;
	glBindBufferBase(target, binding_point, id_);
	binding_point_ = binding_point;
}

void* OpenGLBuffer::mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access) {
    if (!isValid()) {
        std::cerr << "OpenGLBuffer::mapBufferRange error: Buffer not valid." << std::endl;
        return nullptr;
    }
    if (offset < 0 || length <= 0 || offset + length > size_) {
        std::cerr << "OpenGLBuffer::mapBufferRange error: Invalid offset/length." << std::endl;
        return nullptr;
    }

    GLenum target = GL_SHADER_STORAGE_BUFFER;
    glBindBuffer(target, id_);
    void* ptr = glMapBufferRange(target, offset, length, access);
    glBindBuffer(target, 0); // Okay to unbind after map command issued

    
    return ptr;
}

void* OpenGLBuffer::mapBuffer(GLbitfield access) {
    return mapBufferRange(0, size_, access); // Map the entire buffer
}

bool OpenGLBuffer::unmapBuffer() {
    if (!isValid()) {
        // Cannot unmap an invalid buffer, but maybe not an error condition
        return true; // Or false? Let's say true as there's nothing *to* unmap.
    }

    GLenum target = GL_SHADER_STORAGE_BUFFER;
    glBindBuffer(target, id_);
    GLboolean result = glUnmapBuffer(target);
    glBindBuffer(target, 0);

    if (result == GL_FALSE) {
        GLenum err = glGetError(); // Check error if unmap fails
        std::cerr << "OpenGLBuffer::unmapBuffer warning: glUnmapBuffer failed for buffer " << id_ << ". GL Error: " << err << std::endl;
        return false;
    }
    return true;
}
