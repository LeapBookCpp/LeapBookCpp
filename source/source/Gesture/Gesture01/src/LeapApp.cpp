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

    // 以前のフレームが有効であれば、今回のフレームまでの間に検出したジェスチャーの一覧を取得する
    // 以前のフレームが有効でなければ、今回のフレームで検出し他ジェスチャーの一覧を取得する
    auto gestures = mLastFrame.isValid() ? mCurrentFrame.gestures( mLastFrame ) :
                                           mCurrentFrame.gestures();

    // 検出したジェスチャーの履歴を保存する
    for ( auto gesture : gestures ) {
      // IDによってジェスチャーを登録する
      auto it = std::find_if( mGestureList.begin(), mGestureList.end(), 
                     [gesture]( Leap::Gesture g ){ return g.id() == gesture.id(); } );
      if ( it != mGestureList.end() ) {
        *it = gesture;
      }
      else {
        mGestureList.push_back( gesture );
      }

      // 各ジェスチャー固有のパラメーターを取得する
      if ( gesture.type() == Leap::Gesture::Type::TYPE_SWIPE ){
        Leap::SwipeGesture swpie( gesture );
      }
      else if ( gesture.type() == Leap::Gesture::Type::TYPE_CIRCLE ){
        Leap::CircleGesture circle( gesture );
      }
      else if ( gesture.type() == Leap::Gesture::Type::TYPE_KEY_TAP ){
        Leap::KeyTapGesture keytap( gesture );
      }
      else if ( gesture.type() == Leap::Gesture::Type::TYPE_SCREEN_TAP ){
        Leap::ScreenTapGesture screentap( gesture );
      }
    }

    // 最後の更新から1秒たったジェスチャーを削除する(タイムスタンプはマイクロ秒単位)
    mGestureList.remove_if( [&]( Leap::Gesture g ){
      return (mCurrentFrame.timestamp() - g.frame().timestamp()) >= (1 * 1000 * 1000); } );

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
    // ジェスチャーを有効にする
    mLeap.enableGesture( Leap::Gesture::Type::TYPE_SWIPE );
    mLeap.enableGesture( Leap::Gesture::Type::TYPE_CIRCLE );
    mLeap.enableGesture( Leap::Gesture::Type::TYPE_SCREEN_TAP );
    mLeap.enableGesture( Leap::Gesture::Type::TYPE_KEY_TAP );
  }

  // フレーム情報の描画
  void renderFrameParameter()
  {
    stringstream ss;

    // 検出したジェスチャーの履歴を表示する
    for ( auto gesture : mGestureList ) {
      ss << GestureTypeToString( gesture.type() ) << " " <<
            GestureStateToString( gesture.state() ) << " " <<
            gesture.id() <<  "\n";
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

    // 表示処理

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

  // GL_LIGHT0の色を変える
  void setDiffuseColor( ci::ColorA diffuseColor )
  {
    gl::color( diffuseColor );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseColor );
  }

  // Leap SDKのVectorをCinderのVec3fに変換する
  Vec3f toVec3f( Leap::Vector vec )
  {
    return Vec3f( vec.x, vec.y, vec.z );
  }

  // ジェスチャー種別を文字列にする
  std::string GestureTypeToString( Leap::Gesture::Type type )
  {
    if ( type == Leap::Gesture::Type::TYPE_SWIPE ) {
      return "swipe";
    }
    else if ( type == Leap::Gesture::Type::TYPE_CIRCLE ) {
      return "circle";
    }
    else if ( type == Leap::Gesture::Type::TYPE_SCREEN_TAP ) {
      return "screen_tap";
    }
    else if ( type == Leap::Gesture::Type::TYPE_KEY_TAP ) {
      return "key_tap";
    }

    return "invalid";
  }

  // ジェスチャーの状態を文字列にする
  std::string GestureStateToString( Leap::Gesture::State state )
  {
    if ( state == Leap::Gesture::State::STATE_START ) {
      return "start";
    }
    else if ( state == Leap::Gesture::State::STATE_UPDATE ) {
      return "update";
    }
    else if ( state == Leap::Gesture::State::STATE_STOP ) {
      return "stop";
    }

    return "invalid";
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

  std::list<Leap::Gesture> mGestureList;
};

CINDER_APP_NATIVE( LeapApp, RendererGl )
