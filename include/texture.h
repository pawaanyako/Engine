#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <config.h>

class Texture {
public:
    GLuint id;
    std::string type;

    Texture();
    ~Texture();
};

#endif
