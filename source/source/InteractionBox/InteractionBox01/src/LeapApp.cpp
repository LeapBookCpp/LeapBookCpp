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
    // Mac OSX(Mavericks)とWindows 8.1に標準搭載されているフォント
    mFont = Font( "游ゴシック体", 20 );

    // カメラ(視点)の設定
    float y = 250;
    mCam.setEyePoint( Vec3f( 0.0f, y, 500.0f ) );
    mCam.setCenterOfInterestPoint( Vec3f( 0.0f, y, 0.0f ) );
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

    iBox = mCurrentFrame.interactionBox();

    mLeft = iBox.center().x - (iBox.width() / 2);
    mRight = iBox.center().x + (iBox.width() / 2);
    mTop = iBox.center().y + (iBox.height() / 2);
    mBaottom = iBox.center().y - (iBox.height() / 2);
    mBackSide = iBox.center().z - (iBox.depth() / 2);
    mFrontSide = iBox.center().z + (iBox.depth() / 2);

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

    // 現在のフレームレート
    ss << "FPS : "<< mCurrentFrame.currentFramesPerSecond() << "\n";

    ss << "Width :" << iBox.width() << "mm" << "\n";
    ss << "Height:" << iBox.height() << "mm" << "\n";
    ss << "Depth :" << iBox.depth() << "mm" << "\n";
    ss << "Center:" << iBox.center() << "\n";

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
    drawInteractionBoxFrame();
    drawFingerPoint();

    // 表示座標系を戻す
    gl::popMatrices();
  }

  // InteractionBoxの枠を描画する
  void drawInteractionBoxFrame()
  {
    // 中心点
    //gl::drawSphere( toVec3f( iBox.center() ), 5 );

    // 上面
    gl::drawLine( Vec3f( mLeft, mTop, mBackSide ),
                  Vec3f( mRight, mTop, mBackSide ) );

    gl::drawLine( Vec3f( mRight, mTop, mBackSide ),
              Vec3f( mRight, mTop, mFrontSide ) );

    gl::drawLine( Vec3f( mRight, mTop, mFrontSide ),
                  Vec3f( mLeft, mTop, mFrontSide ) );

    gl::drawLine( Vec3f( mLeft, mTop, mFrontSide ),
                  Vec3f( mLeft, mTop, mBackSide ) );

    // 下面
    gl::drawLine( Vec3f( mLeft, mBaottom, mBackSide ),
                  Vec3f( mRight, mBaottom, mBackSide ) );

    gl::drawLine( Vec3f( mRight, mBaottom, mBackSide ),
              Vec3f( mRight, mBaottom, mFrontSide ) );

    gl::drawLine( Vec3f( mRight, mBaottom, mFrontSide ),
                  Vec3f( mLeft, mBaottom, mFrontSide ) );

    gl::drawLine( Vec3f( mLeft, mBaottom, mFrontSide ),
                  Vec3f( mLeft, mBaottom, mBackSide ) );

    // 側面
    gl::drawLine( Vec3f( mLeft, mTop, mFrontSide ),
                  Vec3f( mLeft, mBaottom, mFrontSide ) );

    gl::drawLine( Vec3f( mLeft, mTop, mBackSide ),
                  Vec3f( mLeft, mBaottom, mBackSide ) );

    gl::drawLine( Vec3f( mRight, mTop, mFrontSide ),
                  Vec3f( mRight, mBaottom, mFrontSide ) );

    gl::drawLine( Vec3f( mRight, mTop, mBackSide ),
                  Vec3f( mRight, mBaottom, mBackSide ) );
  }

  // 指の位置を描画する
  void drawFingerPoint()
  {
    // 人差し指を取得する
    Leap::Finger finger = mLeap.frame()
                               .fingers()
                               .fingerType( Leap::Finger::Type::TYPE_INDEX )[0];
    if ( !finger.isValid() ) {
      return;
    }

    Leap::Vector normalizedPosition =
                iBox.normalizePoint( finger.tipPosition() );

    // 位置の割合から実際の座標を計算
    // 原点を左下奥にする
    auto x = (normalizedPosition.x * iBox.width()) + mLeft;
    auto y = (normalizedPosition.y * iBox.height()) + mBaottom;
    auto z = (normalizedPosition.z * iBox.depth()) + mBackSide;
    gl::drawSphere( Vec3f( x, y, z ), 5 );
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

  Leap::InteractionBox iBox;

  float mLeft;
  float mRight;
  float mTop;
  float mBaottom;
  float mBackSide;
  float mFrontSide;
};

CINDER_APP_NATIVE( LeapApp, RendererGl )
