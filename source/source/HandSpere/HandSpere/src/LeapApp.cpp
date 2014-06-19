#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/MayaCamUI.h"

#include "Leap.h"

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

    // 光源を追加する
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    // 表示フォントと領域の設定
    mFont = Font( "メイリオ", 20 );

    // カメラ(視点)の設定
    float y = 250;
    mCam.setEyePoint( Vec3f( 0.0f, y, 500.0f ) );
    mCam.lookAt( Vec3f( 0.0f, y, 0.0f ) );
    mCam.setPerspective( 45.0f, getWindowAspectRatio(), 5.0f, 3000.0f );

    mMayaCam.setCurrentCam(mCam);

    // 描画時に奥行きの考慮を有効にする
    gl::enableDepthRead();

    // Leap Motion関連のセットアップ
    setupLeapObject();
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

  // 更新処理
  void update()
  {
    // フレームの更新
    mLastFrame = mCurrentFrame;
    mCurrentFrame = mLeap.frame();

    renderFrameParameter();
  }

  // 描画処理
  void draw()
  {
    gl::clear( Color( 0, 0, 0 ) ); 

    drawLeapObject();
    drawTexture();
  }

  // Leap Motion関連のセットアップ
  void setupLeapObject()
  {
  }

  // フレーム情報の描画
  void renderFrameParameter()
  {
    stringstream ss;

    // 検出した手の数
    ss << "Hand Count : "<< mCurrentFrame.hands().count() << "\n";

    // 手に接する球の座標を取得する
    for ( auto hand : mCurrentFrame.hands() ) {
      if ( hand.isRight() ) {
        ss << "Right Hand" << "\n";
      }

      if ( hand.isLeft() ) {
        ss << "Left Hand" << "\n";
      }

      ss << "Hand Sphere Center : " << hand.sphereCenter().x << ", " 
                                    << hand.sphereCenter().y << ", "
                                    << hand.sphereCenter().z << "\n";
      ss << "Hand Sphere Radius : " << hand.sphereRadius() << "\n";

      ss << "Hand Grab Strength  : " << hand.grabStrength() << "\n";
      ss << "Hand Pinch Strength : " << hand.pinchStrength() << "\n";

      ss << "\n";
    }

    // テキストボックスを作成する
    auto tbox = TextBox()
      .alignment( TextBox::LEFT )
      .font( mFont )
      .text ( ss.str() )
      .color(Color( 1.0f, 1.0f, 1.0f ))
      .backgroundColor( ColorA( 0, 0, 0, 0.5f ) );

    mTextTexture = gl::Texture( tbox.render() );
  }

  // Leap Motion関連の描画
  void drawLeapObject()
  {
    // 表示座標系の保持
    gl::pushMatrices();

    // カメラ位置を設定する
    gl::setMatrices( mMayaCam.getCamera() );

    // 手に接する球の位置を表示する
    for ( auto hand : mCurrentFrame.hands() ) {
      double red = hand.isRight() ? 1.0 : 0.0;

      setDiffuseColor( ci::ColorA( red, 1.0, 0.5 ) );
      gl::drawSphere( toVec3f( hand.sphereCenter() ), hand.sphereRadius() );

            
      // 開閉度合いを表示する位置の座標(原点から左右対称におく)
      int sign = hand.isRight() ? 1 : -1;

      // 手の開閉度合い
      auto grabStrength = 1.0f - hand.grabStrength();
      setDiffuseColor( ci::ColorA( red, 1.0, 0.5 ) );
      gl::lineWidth( 10 );
      gl::begin( GL_LINES );
      gl::vertex( Vec3f( 300 * sign, 50, 0) );
      gl::vertex( Vec3f( 300 * sign, 50, 0) + (Vec3f(0, 100, 0) * (grabStrength)) );
      gl::end();

      // ピンチの開閉度合い
      auto pinchStrength = 1.0f - hand.pinchStrength();
      setDiffuseColor( ci::ColorA( red, 0.0, 1.5 ) );
      gl::lineWidth( 10 );
      gl::begin( GL_LINES );
      gl::vertex( Vec3f( 310 * sign, 50, 0) );
      gl::vertex( Vec3f( 310 * sign, 50, 0) + (Vec3f(0, 100, 0) * (pinchStrength)) );
      gl::end();
    }

    // デフォルトに戻す
    setDiffuseColor( ci::ColorA( 0.8, 0.8, 0.8 ) );

    // 表示座標系を戻す
    gl::popMatrices();
  }

  // テクスチャの描画
  void drawTexture()
  {
    if( mTextTexture ) {
      gl::draw( mTextTexture );
    }
  }

  // Leap SDKのVectorをCinderのVec3fに変換する
  Vec3f toVec3f( Leap::Vector vec )
  {
    return Vec3f( vec.x, vec.y, vec.z );
  }
  
  void setDiffuseColor( ci::ColorA diffuseColor )
  {
    gl::color( diffuseColor );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseColor );
  }

  // カメラ
  CameraPersp  mCam;
  MayaCamUI    mMayaCam;

  // パラメータ表示用のテクスチャ
  gl::Texture mTextTexture;
  Font mFont;

  // Leap Motion
  Leap::Controller mLeap;
  Leap::Frame mCurrentFrame;
  Leap::Frame mLastFrame;
};

CINDER_APP_NATIVE( LeapApp, RendererGl )
