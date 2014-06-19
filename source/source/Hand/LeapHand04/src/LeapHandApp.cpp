// 手の回転を反映する
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Camera.h"

#include <Leap.h>

using namespace ci;
using namespace ci::app;
using namespace std;


class LeapHandApp : public AppNative {
public:
  void setup()
  {
    // ウィンドウの位置とサイズを設定
    setWindowPos(50, 50);
    setWindowSize(1280, 700);

    // 表示フォントと領域の設定
    mFont = Font( "メイリオ", 20 );

    // カメラ(視点)の設定
    float y = 250;
    mCam.setPerspective( 60.0f, getWindowAspectRatio(), 5.0f, 3000.0f );
    mCam.lookAt( Vec3f( 0.0f, y, 500.0f ), Vec3f( 0.0f, y, 0.0f ), Vec3f( 0.0f, 1.0f, 0.0f ) );

    // 描画時に奥行きの考慮を有効にする
    gl::enableDepthRead();

    // 光源を追加する
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
  }

  void update()
  {
    auto frame = mLeap.frame();

    renderFrameParameter( frame );
  }

  void draw()
  {
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) ); 

    gl::setMatrices( mCam );

    // 手の位置を表示する
    auto frame = mLeap.frame();
    for ( auto hand : frame.hands() ) {
      //double red = hand.isRight() ? 1.0 : 0.0;
      double red = hand.isLeft() ? 1.0 : 0.0;

      setDiffuseColor( ci::ColorA( red, 1.0, 0.5 ) );
      gl::drawSphere( toVec3f( hand.palmPosition() ), 10 );

      // 移動して、回転して、描画
      {
        gl::pushModelView();
        gl::translate( toVec3f( hand.palmPosition() ) );
        gl::rotate( Quatf(hand.direction().pitch(), hand.direction().yaw(), hand.palmNormal().roll()) );
        gl::drawColorCube(Vec3f::zero(), Vec3f(50, 10, 50));
        gl::popModelView();
      }

      setDiffuseColor( ci::ColorA( red, 0.5, 1.0 ) );
      for ( auto finger : hand.fingers() ){
        const Leap::Finger::Joint jointType[] = {
          Leap::Finger::Joint::JOINT_MCP,
          Leap::Finger::Joint::JOINT_PIP,
          Leap::Finger::Joint::JOINT_DIP,
          Leap::Finger::Joint::JOINT_TIP,
        };

        for ( auto type : jointType ) {
          gl::drawSphere( toVec3f( finger.jointPosition( type ) ), 10 );
        }
      }
    }

    // デフォルトに戻す
    setDiffuseColor( ci::ColorA( 0.8, 0.8, 0.8 ) );

    if( mTextTexture ) {
      gl::pushModelView();
      gl::translate( Vec3f( -500, 0, 0 ) );
      mTextTexture.setFlipped();
      gl::draw( mTextTexture );
      gl::popModelView();
    }
  }

  void renderFrameParameter(const Leap::Frame& frame)
  {
    stringstream ss;

    ss << "Hand Count : "<< frame.hands().count() << "\n";

    // 指の座標を取得する
    for ( auto hand : frame.hands() ) {
      if ( hand.isRight() ) {
        ss << "Right Hand" << "\n";
      }

      if ( hand.isLeft() ) {
        ss << "Left Hand" << "\n";
      }


      ss << "Hand Palm Position: " << hand.palmPosition().x << ", " 
        << hand.palmPosition().y << ", "
        << hand.palmPosition().z << "\n";

      ss << "Hand Palm Normal  : " << hand.palmNormal().x << ", " 
        << hand.palmNormal().y << ", "
        << hand.palmNormal().z << "\n";

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

  Vec3f toVec3f( Leap::Vector vec )
  {
    return Vec3f( vec.x, vec.y, vec.z );
  }

  void setDiffuseColor( ci::ColorA diffuseColor )
  {
    gl::color( diffuseColor );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseColor );
  }

  gl::Texture	mTextTexture;
  Font mFont;

  CameraPersp mCam;

  Leap::Controller mLeap;
};

CINDER_APP_NATIVE( LeapHandApp, RendererGl )
