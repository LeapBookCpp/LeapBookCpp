#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"

#include "Leap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InteractionBoxApp : public AppNative {
public:

  void setup()
  {
    setWindowPos( 50, 50 );
    setWindowSize( WindowWidth, WindowHeight );
    gl::enableAlphaBlending();
  }

  void draw()
  {
    gl::clear( Color( 1, 1, 1 ) );

    gl::pushMatrices();

    // 人差し指を取得する
    Leap::Finger finger = mLeap.frame()
                               .fingers()
                               .fingerType( Leap::Finger::Type::TYPE_INDEX )[0];
    if ( !finger.isValid() ) {
      return;
    }

    // InteractionBoxの座標に変換する
    Leap::InteractionBox iBox = mLeap.frame().interactionBox();
    Leap::Vector normalizedPosition =
                    iBox.normalizePoint( finger.stabilizedTipPosition() );

    // ウィンドウの座標に変換する
    float x = normalizedPosition.x * WindowWidth;
    float y = WindowHeight - (normalizedPosition.y * WindowHeight);

    // ホバー状態
    if ( finger.touchZone() == Leap::Pointable::Zone::ZONE_HOVERING ) {
      gl::color(0, 1, 0, 1 - finger.touchDistance());
    }
    // タッチ状態
    else if( finger.touchZone() == Leap::Pointable::Zone::ZONE_TOUCHING ) {
      gl::color(1, 0, 0, -finger.touchDistance());
    }
    // タッチ対象外
    else {
      gl::color(0, 0, 1, .05);
    }

    gl::drawSolidCircle( Vec2f( x, y ), 20 );

    // 指の座標を表示する
    stringstream ss;
    ss << normalizedPosition.x << ", " << normalizedPosition.y;

    auto tbox = TextBox()
      .font( Font( "游ゴシック体", 20 ) )
      .text ( ss.str() );
    auto texture = gl::Texture( tbox.render() );

    auto textX = (normalizedPosition.x < 0.5) ?
                  x : x - texture.getBounds().getWidth();
    auto textY = (normalizedPosition.y > 0.5) ?
                  y : y - texture.getBounds().getHeight();

    gl::color( 0, 0, 0, 1 );
    gl::translate( textX, textY );
    gl::draw( texture );

    gl::popMatrices();
  }

  static const int WindowWidth = 640;
  static const int WindowHeight = 480;

  Leap::Controller mLeap;
};

CINDER_APP_NATIVE( InteractionBoxApp, RendererGl )
