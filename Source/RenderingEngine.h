#pragma once

#include "ShaderProgram.h"
#include "ObjParser.h"
#include "ResourceLoader.h"
#include "Node.h"
#include "Node2d.h"
#include "Text.h"
#include "GameCamera.h"
#include "LensFlare.h"
#include "ParticleSystem.h"
#include "Timer.h"

#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>

enum VertexFlags {
    VertexFlagsNormals = 1 << 0,
    VertexFlagsTexCoords = 1 << 1,
};

class Light {
public:
	Light() : mAmbientColor( ci::ColorA::black() ), mColor( ci::ColorA::white() ) {}
	void update( const float deltaTime );
	ci::ColorA mColor;
	ci::ColorA mAmbientColor;
	Node mNode;
	LensFlare mLensFlare;
};

class RenderingEngine {
public:
	static RenderingEngine* get();
    void					setup( int width, int height, float contentScaleFactor );
	void					update( const float deltaTime = 0.0f );
    void					draw();
	void					createVbo( Mesh* vboMesh, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords );
	void					createFbo( FramebufferObject* framebufferObject );
	bool					createTexture( Texture* texture );
	void					deleteTexture( Texture* texture );
	void					setRootGui( Node2d* Node2d );
	void					setBackgroundTexture( Texture* texture );
	void					setSkyboxNode( Node* node );
	void					addNode( Node* node );
	void					removeNode( Node* node, bool andCleanUp = true );
	void					addNode( Node2d* node );
	void					addBackgroundNode( Node* node );
	void					removeNode( Node2d* node );
	
	void					debugDrawCube( ci::Vec3f center, ci::Vec3f size, ci::ColorA color );
	void					debugDrawLine( ci::Vec3f from, ci::Vec3f to, ci::ColorA color );
	void					debugDrawStrokedRect( const ci::Rectf& rect, const ci::ColorA color, ci::Matrix44f = ci::Matrix44f::identity() );
	void					debugScreenDrawStrokedRect( const ci::Rectf& rect, const ci::ColorA color );
	
	
	float					getContentScaleFactor() const { return mContentScaleFactor; }
	void					bindWindowContext();
	void					bindFrameBufferObject( FramebufferObject* fbo );
	void					drawTexture( Texture* texture, const ci::Matrix44f& transform = ci::Matrix44f::identity(), bool flipY = false );
	void					drawText( Text* text );
	void					addSpriteNode( Node* node );
	
	void					addLight( Light* light );
	
	const std::list<Node*>& getObjectNodes() const { return mObjectNodes; }
	
	std::list<CustomDrawing*> mCustomDrawings;
	
private:
    RenderingEngine();
	static RenderingEngine*			sInstance;
	
	void					sortSprites( const float deltaTime );
	
	void					drawGui( Node2d* Node2d );
	inline void				drawNode( const Node* node );
	inline void				drawMesh( const Mesh* mesh, const ShaderProgram* shader, const bool wireframe = false );
	inline void				setBlendMode( Node::Layer layerType );
	inline bool				setUniforms( const Material& material, ShaderProgram* shader );
	inline void				unbindTextures( Material& material );
	
	float					mContentScaleFactor;
	ci::Matrix44f			mScreenTransform;
	ci::Matrix44f			mScreenTransformFlippedY;
	ci::Vec2i				mScreenSize;
	std::list<Node*>		mObjectNodes;
	std::list<Node*>		mBackgroundNodes;
	std::list<Node2d*>		mScreenNodes;
	std::vector<Node*>		mSpriteNodes;
	GLuint					mContextFramebuffer;
    GLuint					mContextColorRenderbuffer;
    GLuint					mContextDepthRenderbuffer;
	GameCamera*					mCamera;
	Node*					mSkyboxNode;
	
	Texture*				mBackgroundTexture;
	Texture*				mDefaultWhite;
	Texture*				mDefaultBlack;
	FramebufferObject*		mMainFbo;
	FramebufferObject*		mDepthMapFbo;
	
	std::list<Light*>		mLights;
	
	ly::Timer				mSortTimer;
};