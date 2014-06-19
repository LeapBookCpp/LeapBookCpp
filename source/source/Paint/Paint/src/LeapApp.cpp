#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/MayaCamUI.h"

#include "Paint.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LeapApp : public AppNative {
public:


  void setup()
  {
    // ウィンドウの位置とサイズを設定
    setWindowPos( 50, 50 );
    setWindowSize( Paint::Width, Paint::Height );

    // 光源を追加する
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glLightfv( GL_LIGHT0 , GL_AMBIENT, ci::ColorA( 0.5f, 0.5f, 0.5f, 1 ) );
    glMaterialfv( GL_FRONT, GL_AMBIENT, ci::ColorA( 1.0f, 1.0f, 1.0f, 1 ) );

    // カメラ(視点)の設定
    float y = 250;
    mCam.setEyePoint( Vec3f( 0.0f, y, 500.0f ) );
    mCam.setCenterOfInterestPoint( Vec3f( 0.0f, y, 0.0f ) );
    mCam.setPerspective( 45.0f, getWindowAspectRatio(), 5.0f, 3000.0f );

    mMayaCam.setCurrentCam(mCam);

    // 描画時に奥行きの考慮を有効にする
    gl::enableDepthRead();
  }

  // マウスダウン
  void mouseDown( MouseEvent event )
  {
    mMayaCam.mouseDown( event.getPos() );
  }

  // マウスのドラッグ
  void mouseDrag( MouseEvent event )
  {
    mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(),
      event.isMiddleDown(), event.isRightDown() );
  }

  // キーダウン
  void keyDown( KeyEvent event ) 
  {
    // Cを押したら軌跡をクリアする
    if ( event.getChar() == event.KEY_c ) {
      mPaint.clear();
    }
    // Mを押したらモードを変える
    else if ( event.getChar() == event.KEY_m ) {
      mPaint.set3DMode( !mPaint.get3DMode() );
    }
  }

  // 更新処理
  void update()
  {
    mPaint.update();
  }

  // 描画処理
  void draw()
  {
    // 3次元のお絵かきの時は、カメラ座標を考慮する
    if ( mPaint.get3DMode() ) {
      // 表示座標系の保持
      gl::pushMatrices();

      // カメラ位置を設定する
      gl::setMatrices( mMayaCam.getCamera() );
    }

    // 描画
    mPaint.draw();

    // 3次元のお絵かきの時は、カメラ座標を考慮する
    if ( mPaint.get3DMode() ) {
      // 表示座標系を戻す
      gl::popMatrices();
    }
  }

  // カメラ
  CameraPersp  mCam;
  MayaCamUI    mMayaCam;

  Paint mPaint;
};

CINDER_APP_NATIVE( LeapApp, RendererGl )
