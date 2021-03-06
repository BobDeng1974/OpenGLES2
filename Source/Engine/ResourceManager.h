#pragma once

#include "ResourceLoader.h"
#include "RenderingEngine.h"
#include "ShaderProgram.h"
#include "Node.h"

#include <map>

class ResourceManager {
public:
	virtual ~ResourceManager();
	static ResourceManager* get();
	
	void					setup( RenderingEngine* renderingEngine );
	
	void					loadShader( std::string key, std::string vertexShaderPath, std::string fragmentShaderPath );
	void					loadTexture( std::string filePath );
	void					loadCubemapTexture( std::string key, const std::vector<std::string>& filePaths );
	void					loadMesh( std::string filePath );
	void					loadFont( std::string fontpath );
	void					loadSound( std::string fontpath );
	
	ShaderProgram*			getShader( std::string key );
	Texture*				getTexture( std::string key );
	Mesh*					getMesh( std::string key );
	Font*					getFont( std::string key );
	Sound*					getSound( std::string key );
	
private:
	static ResourceManager* sInstance;
	RenderingEngine*		mRenderingEngine;
	ResourceLoader			mResourceLoader;
	
	std::map<std::string, Texture*> mTextures;
	std::map<std::string, Mesh*> mMeshes;
	std::map<std::string, ShaderProgram*> mShaders;
	std::map<std::string, Font*> mFonts;
	std::map<std::string, Sound*> mSounds;
};