#pragma once

#include "cinder/Vector.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <string>

class ResourceLoader {
public:
	void			loadImage( const std::string& path, void** pixelDataDestination, int* width, int* height );
    std::string		getResourcePath() const;
	char*			loadShader( const std::string& path );
};