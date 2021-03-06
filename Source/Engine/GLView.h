#pragma once

#include "Game.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreMotion/CoreMotion.h>

@interface GLView : UIView {
@private
    Game* mGame;
    EAGLContext* mContext;
    float mTimestamp;
	std::vector<ci::Vec2i> mTouches;
	bool hasUpdatedOnce;
	CMMotionManager* mMotionManager;
}

- (void) update: (CADisplayLink*) displayLink;

@end
