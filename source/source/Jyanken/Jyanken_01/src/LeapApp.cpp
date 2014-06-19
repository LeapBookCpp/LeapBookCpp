#include <numeric>

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/MayaCamUI.h"
#include "cinder/ImageIo.h"
#include "cinder/ip/Blend.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"

#include "Leap.h"
#include "GameAssets.h"
#include "StateMachine.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LeapApp : public AppNative {
public:

  void setup()
  {
    // ウィンドウの位置とサイズを設定
    setWindowPos( 50, 50 );
    setWindowSize( 1280, 700 );

    // アプリケーションのセットアップ
    mAssets.setup();
    mState.setup( &mAssets );
  }
  // 描画処理
  void draw()
  {
    mState.update();
    gl::draw( mAssets.getSurface() );
  }


  GameAssets mAssets;
  StateMachine mState;
};

CINDER_APP_NATIVE( LeapApp, RendererGl )
