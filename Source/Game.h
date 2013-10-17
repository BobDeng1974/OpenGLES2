#pragma once

#include "ObjParser.h"
#include "ResourceManager.h"
#include "RenderingEngine.h"
#include "Node.h"
#include "Node2d.h"
#include "GameConstants.h"
#include "GameCamera.h"
#include "LensFlare.h"
#include "TouchInput.h"
#include "Text.h"
#include "Controls.h"
#include "Unit.h"
#include "UniformGrid.h"

#include "cinder/Ray.h"

#include <list>

class Game {
public:
	static Game*		get();
	
    void				setup( int width, int height );
    void				update( const float deltaTime = 0.0f );
	void				debugDraw();
	
	Node2d*				getRootGui() const { return mRootGui; }
	GameCamera*				getCamera() const { return mCamera; }
	ResourceManager*	getResourceManager() const { return mResourceManager; }
	
	void				remove( Node* node );
	void				remove( Node2d* Node2d );
	
	const std::vector<Unit*>& getUnits() const { return mUnits; }
	std::vector<Unit*>& getUnits() { return mUnits; }
	
	const UniformGrid&	getUniformGrid() const { return mUniformGrid; }
	
private:
    Game();
    ~Game();
	
	static Game* sInstance;
	
	GameCamera* mCamera;
	Controls mControls;
	
	Node2d* mRootGui;
	
    RenderingEngine* mRenderingEngine;
    ResourceManager* mResourceManager;
		
	void deselectAllControllers();
	
	std::vector<Unit*> mUnits;
	std::list<Unit*> mDeletionQueue;
	
	Node* lineTest;
	
	UniformGrid mUniformGrid;
};