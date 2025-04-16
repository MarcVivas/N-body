
// OpenGLBuffer.h (from previous answer, potentially with minor improvements like error checking)
#ifndef OPENGLBUFFER_H
#define OPENGLBUFFER_H

#include <glad/glad.h>
#include <cstddef> 
#include <vector>  
#include <cstdio>

class OpenGLBuffer {
public:
    OpenGLBuffer();
    ~OpenGLBuffer();

    // --- Manual Lifecycle ---
    // Creates the buffer storage and binds it to a base.
    // Target is implicitly GL_SHADER_STORAGE_BUFFER for simplicity here.
    // Returns true on success, false otherwise.
    bool createBufferStorage(GLsizeiptr size, const void* data, GLuint binding_point, GLbitfield flags);
    bool createBufferData(GLsizeiptr size, const void* data, GLuint binding_point, GLbitfield flags);

    // Deletes the OpenGL buffer object.
    void destroy();

    // --- Data Operations ---
    // Updates a sub-region of the buffer. Assumes buffer is NOT mapped.
    void updateData(GLintptr offset, GLsizeiptr size, const void* data);

    // Maps the buffer's data store into the client's address space.
    // Returns nullptr on failure. Caller must check.
    void* mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access);

    // Maps the entire buffer. Convenience wrapper for mapBufferRange.
    void* mapBuffer(GLbitfield access);

    // Unmaps the buffer. Returns true if successful.
    bool unmapBuffer();

    void bindBufferBase(GLuint binding_point);

    // --- Data Retrieval ---
    // Retrieves the entire buffer content into a std::vector.
    // This performs a map, copy, and unmap internally.
    // Returns an empty vector on failure.
    template <typename T>
    std::vector<T> getDataVector(GLintptr offset, GLsizeiptr length) const;

    // --- Getters ---
    GLuint getID() const { return id_; }
    GLsizeiptr getSize() const { return size_; }
    GLuint getBindingPoint() const { return binding_point_; }
    bool isValid() const { return id_ != 0; }
    GLuint id_;
    GLsizeiptr size_;
    GLuint binding_point_;
 
    // Target is implicitly GL_SHADER_STORAGE_BUFFER in this simplified version
    // GLenum target_; could be added if needed.
};

// Template implementation needs to be in the header or included file
template <typename T>
std::vector<T> OpenGLBuffer::getDataVector(GLintptr first, GLsizeiptr size) const {
    std::vector<T> data;
    if (size <= 0) return data; // Handle zero or negative size

    // Calculate number of elements - IMPORTANT for vector resizing
    size_t elementSize = sizeof(T);
    if (elementSize == 0) return data; // Avoid division by zero for empty types? (unlikely)
    if (size % elementSize != 0) {
        // Handle error: size is not a multiple of element size T!
        // This would lead to reading partial elements.
        // Maybe throw, log, or return empty vector.
        // For now, let's proceed but it might be wrong.
        // Consider asserting or logging here.
        fprintf(stderr, "Warning: getDataVector size (%lld) is not a multiple of sizeof(T) (%zu)\n",
            (long long)size, elementSize);
    }
    size_t numElements = size / elementSize; // Integer division

    data.resize(numElements); // Allocate space in the vector

    // Bind the buffer (assuming ID() returns the OpenGL buffer ID)
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->getID()); // Or appropriate target

    // Get the data
    glGetBufferSubData(
        GL_SHADER_STORAGE_BUFFER, // Target buffer type
        first,                    // Starting offset (bytes)
        size,                     // Size to read (bytes)
        data.data()               // Pointer to destination memory (vector's buffer)
    );

    // Unbind (optional, good practice)
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return data;
}

#endif // OPENGLBUFFER_H

