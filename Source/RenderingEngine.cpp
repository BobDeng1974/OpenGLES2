#include "RenderingEngine.h"
#include "ResourceManager.h"

const static int kNormalAttribLoc		= 0;
const static int kPositionAttribLoc		= 1;
const static int kTexCoordAttribLoc		= 2;
const static int kColorAttribLoc		= 3;

using namespace ci;

RenderingEngine::RenderingEngine() : mContentScaleFactor(1.0f), mSkyboxNode(NULL), mRootGui(NULL)
{
    glGenRenderbuffers( 1, &m_colorRenderbuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, m_colorRenderbuffer );
}

void RenderingEngine::addNode( Node* node )
{
	auto match = std::find( mObjectNodes.begin(), mObjectNodes.end(), node );
	if ( match == mObjectNodes.end() ) {
		mObjectNodes.push_back( node );
	}
}

void RenderingEngine::removeNode( Node* node )
{
	auto match = std::find( mObjectNodes.begin(), mObjectNodes.end(), node );
	if ( match != mObjectNodes.end() ) {
		mObjectNodes.erase( match );
	}
}
void RenderingEngine::addNode( Node2d* node )
{
	auto match = std::find( mScreenNodes.begin(), mScreenNodes.end(), node );
	if ( match == mScreenNodes.end() ) {
		mScreenNodes.push_back( node );
	}
}

void RenderingEngine::removeNode( Node2d* node )
{
	auto match = std::find( mScreenNodes.begin(), mScreenNodes.end(), node );
	if ( match != mScreenNodes.end() ) {
		mScreenNodes.erase( match );
	}
}

void RenderingEngine::setSkyboxNode( Node* node )
{
	mSkyboxNode = node;
	mSkyboxNode->update();
}

void RenderingEngine::createVbo( VboMesh* vboMesh, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords )
{
	// Create the VBO for the vertices.
	glGenBuffers(1, &vboMesh->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vboMesh->vertexBuffer );
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
	vboMesh->vertexCount = vertices.size() / 3;
	
	glGenBuffers(1, &vboMesh->texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vboMesh->texCoordBuffer );
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);
	vboMesh->texCoordCount = texCoords.size() / 2;
	
	glGenBuffers(1, &vboMesh->normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vboMesh->normalBuffer ) ;
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW );
	vboMesh->normalCount = normals.size() / 3;
}

bool RenderingEngine::createTexture( Texture* texture )
{
	if ( !texture ) return false;
	
	glGenTextures( 1, &texture->mHandle );
	glBindTexture( GL_TEXTURE_2D, texture->mHandle );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, texture->mWidth, texture->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->mImageData );
	glGenerateMipmap( GL_TEXTURE_2D );
	
	// Unbind texture
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	// When the time comes:
	//glDeleteTextures( 1, &texture->mHandle );
	
	// TODO: Check errors more ways than this:
	return texture->mHandle != 0;
}

void RenderingEngine::createFbo()
{
	// Create renderbuffer object
	GLuint renderbuffer;
	glGenRenderbuffers( 1, &renderbuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, renderbuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA4, mScreenSize.x, mScreenSize.y );
	
	// Create framebuffer object
	GLuint colorFramebuffer;
	glGenFramebuffers( 1, &colorFramebuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, colorFramebuffer );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorFramebuffer );
}

void RenderingEngine::addShader( std::string key, const char* vShader, const char* fShader )
{
	mShaders[ key ] = ShaderProgram( vShader, fShader );
}

void RenderingEngine::setup( int width, int height, float contentScaleFactor )
{
	mScreenSize.x = width * contentScaleFactor;
	mScreenSize.y = height * contentScaleFactor;
	mContentScaleFactor = contentScaleFactor;
	
	mCamera = Camera::get();
	mCamera->setScreenSize( width, height, contentScaleFactor );
	
	mRootGui = new Node2d();
	
	mScreenTransform = Matrix44f::identity();
	mScreenTransform.translate( Vec3f( -1.0f, 1.0f, 0.0f ) );
	mScreenTransform.scale( Vec3f( 1 / (float) width * 2.0f,
								   1 / (float) height * 2.0f * -1.0f,
								   1.0f ) );
    
    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mScreenSize.x, mScreenSize.y );
    
    // Create the framebuffer object.
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
	
}

void RenderingEngine::update( const float deltaTime )
{
	if ( mSkyboxNode ) {
		mSkyboxNode->position = mCamera->getGlobalPosition();
	}
	
	if ( mRootGui ) {
		mRootGui->update( deltaTime );
	}
}

void RenderingEngine::draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Set the viewport transform.
	glViewport( 0, 0, mScreenSize.x, mScreenSize.y );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	
	if ( mSkyboxNode ) {
		Node* node = mSkyboxNode;
		
		ShaderProgram& shader = mShaders[ node->mShader ];
		
		glUseProgram( shader.getHandle() );
		shader.uniform( "DiffuseMaterial",		node->mColor );
		shader.uniform( "Modelview",			mCamera->getModelViewMatrix() );
		shader.uniform( "Projection",			mCamera->getProjectionMatrix() );
		shader.uniform( "Transform",			node->getTransform() );
		
		if ( node->mTexture != NULL ) {
			glActiveTexture( GL_TEXTURE0 );
			shader.uniform( "DiffuseTexture", 0 );
			glBindTexture( GL_TEXTURE_2D, node->mTexture->mHandle );
		}
		
		drawMesh( node->mMesh );
		
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	
	glEnable(GL_DEPTH_TEST);
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
	for( auto iter = mObjectNodes.begin(); iter != mObjectNodes.end(); iter++ ) {
		Node* node = *iter;
		
		ShaderProgram& shader = mShaders[ node->mShader ];
		
		glUseProgram( shader.getHandle() );
		shader.uniform( "AmbientMaterial",		Vec4f( 0.1f, 0.1f, 0.1f, 1.0f ) );
		shader.uniform( "LightPosition",		Vec3f( 100.0, 50.0, 50.0 ) );
		shader.uniform( "EyePosition",			mCamera->getGlobalPosition() );
		shader.uniform( "SpecularMaterial",		node->mColorSpecular );
		shader.uniform( "DiffuseMaterial",		node->mColor );
		shader.uniform( "RimMaterial",			node->mColorRim );
		shader.uniform( "Shininess",			node->mShininess );
		shader.uniform( "Glossiness",			node->mGlossiness );
		shader.uniform( "RimPower",				node->mRimPower );
		shader.uniform( "Modelview",			mCamera->getModelViewMatrix() );
		shader.uniform( "Projection",			mCamera->getProjectionMatrix() );
		shader.uniform( "Transform",			node->getTransform() );
		
		if ( node->mTexture != NULL ) {
			glActiveTexture( GL_TEXTURE0 );
			shader.uniform( "DiffuseTexture", 0 );
			glBindTexture( GL_TEXTURE_2D, node->mTexture->mHandle );
		}
		
		if ( node->mTextureNormal != NULL ) {
			glActiveTexture( GL_TEXTURE1 );
			shader.uniform( "NormalTexture", 1 );
			glBindTexture( GL_TEXTURE_2D, node->mTextureNormal->mHandle );
		}
		
		drawMesh( node->mMesh );
		
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	
	// Additive blending for lighting effects, such as lens flare
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	for( auto iter = mScreenNodes.begin(); iter != mScreenNodes.end(); iter++ ) {
		drawGui( *iter );
	}
}

void RenderingEngine::drawMesh( VboMesh* mesh )
{
	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glEnableVertexAttribArray( kPositionAttribLoc );
	glVertexAttribPointer( kPositionAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0 );
	
	if ( mesh->normalBuffer > 0 ) {
		glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
		glEnableVertexAttribArray( kNormalAttribLoc );
		glVertexAttribPointer( kNormalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0 );
	}
	
	if ( mesh->texCoordBuffer > 0 ) {
		glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
		glEnableVertexAttribArray( kTexCoordAttribLoc );
		glVertexAttribPointer( kTexCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), 0 );
	}
	
	glDrawArrays( GL_TRIANGLES, 0, mesh->vertexCount );
}

void RenderingEngine::drawGui( Node2d* gui )
{
	if ( !gui->getIsVisible() ) return;
	
	for( auto iter = gui->getChildren().begin(); iter != gui->getChildren().end(); iter++ ) {
		drawGui( *iter );
	}
	
	Node* node = gui->getNode();
	
	ShaderProgram& shader = mShaders[ node->mShader ];
	
	glUseProgram( shader.getHandle() );
	shader.uniform( "DiffuseMaterial",		node->mColor );
	shader.uniform( "Transform",			node->getTransform() );
	shader.uniform( "ScreenTransform",		mScreenTransform );
	shader.uniform( "ScreenSize",		mScreenSize );
	
	if ( node->mTexture != NULL ) {
		glActiveTexture( GL_TEXTURE0 );
		shader.uniform( "DiffuseTexture", 0 );
		glBindTexture( GL_TEXTURE_2D, node->mTexture->mHandle );
	}
	
	drawMesh( node->mMesh );
	
	if ( node->mTexture != NULL ) {
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
}




