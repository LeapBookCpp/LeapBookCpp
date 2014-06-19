#pragma once 

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/MayaCamUI.h"

#include "Leap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Motion
{
public:

  // 移動量、回転量、拡大率
  Leap::Vector mTranslation;
  Leap::Matrix mRotationMatrix;
  float mScale;

  Motion()
  {
    mRotationMatrix = Leap::Matrix::identity();
    mTranslation = Leap::Vector::zero();
    mScale = 1.0f;
  }
  
  template<typename Object>
  static Motion getCurrentMotion( Object object, Leap::Frame lastFrame )
  {
    Motion motion;

    // 前のフレームからの移動量
    if ( object.translationProbability( lastFrame ) > 0.4 ) {
      // 回転を考慮して移動する
      motion.mTranslation = object.translation( lastFrame );
    }

    // 前のフレームからの回転量
    if ( object.rotationProbability( lastFrame ) > 0.4 ) {
      motion.mRotationMatrix = object.rotationMatrix( lastFrame );
    }

    // 前のフレームからの拡大縮小
    if ( object.scaleProbability( lastFrame ) > 0.4 ) {
      motion.mScale = object.scaleFactor( lastFrame );
    }

    return motion;
  }

  void update( const Motion& motion )
  {
    // 前のフレームからの移動量
    // 回転を考慮して移動する
    mTranslation += mRotationMatrix
                    .rigidInverse()
                    .transformDirection( motion.mTranslation );

    // 前のフレームからの回転量
    mRotationMatrix *= motion.mRotationMatrix;
    
    // 前のフレームからの拡大縮小
    mScale *= motion.mScale;
    if ( mScale < 0.1f ) {
      mScale = 0.1f;
    }
  }

  template<typename Object>
  void update( Object object, Leap::Frame lastFrame )
  {
    update( Motion::getCurrentMotion( object, lastFrame ) );
  }

  void draw( int offsetX )
  {
    gl::pushMatrices();

    // 表示処理
    glTranslatef( mTranslation.x,
                  mTranslation.y,
                  mTranslation.z );
    glMultMatrixf( mRotationMatrix.toArray4x4() );
    glScalef( mScale, mScale, mScale );
    gl::drawColorCube( Vec3f( offsetX, 0, 0 ),
                        Vec3f( 100, 100, 100 ) );

    gl::popMatrices();
  }
};
