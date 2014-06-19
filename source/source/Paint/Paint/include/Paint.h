#pragma once

#include "cinder/gl/gl.h"

#include "Leap.h"

using namespace ci;

class Paint
{
public:

  Paint()
    : mIs3D( false )
  {
  }

  void clear()
  {
    // 軌跡をクリアする
    mCompleteStrokes.clear();
    mCurrentStrokes.clear();
  }

  void update()
  {
    mCurrentFrame = mLeap.frame();

    mPointablePoints.clear();

    // ポインタブル・オブジェクトの座標を取得する
    auto pointables = mCurrentFrame.pointables();
    for ( auto pointable : pointables ) {
      auto point = pointable.tipPosition();
      mPointablePoints.push_back( point );

      // 有効なポインタブル・オブジェクトであれば、書き途中の軌跡に追加(IDと合わせて)
      if ( isValid( pointable ) ) {
        mCurrentStrokes[pointable.id()].push_back( point );
      }
    }

    // 書き途中の軌跡を調べて、今回のフレームでのポインタブル・オブジェクトがなかった場合、
    // 書くのが終わったと判断し、完了した軌跡とする
    for ( auto it = mCurrentStrokes.begin(); it != mCurrentStrokes.end();  ) {
      auto pointable = mCurrentFrame.pointable( it->first );
      if ( isValid( pointable ) ) {
        ++it;
      }
      else {
        mCompleteStrokes.push_back( it->second );
        it = mCurrentStrokes.erase( it );
      }
    }
  }

  void draw()
  {
    gl::clear( Color( 0, 0, 0 ) ); 

    // 現在のポインタブル・オブジェクトの位置を表示する
    drawPoints( mPointablePoints, 15, Color( 1, 0, 0 ) );

    // 完了した軌跡を表示する
    for ( auto strokes : mCompleteStrokes ) {
      drawLineStrip( strokes );
    }

    // 書き途中の軌跡を表示する
    for ( auto strokes : mCurrentStrokes ) {
      drawLineStrip( strokes.second );
    }
  }

  void set3DMode( bool is3d )
  {
    mIs3D = is3d;
  }

  bool get3DMode() const
  {
    return mIs3D;
  }

  static const int Width = 1280;
  static const int Height = 700;

private:

  bool isValid( Leap::Pointable pointable )
  {
    // 有効なポインタ(オブジェクトが有効で、伸びている)
    return pointable.isValid() &&
           pointable.isExtended();
  }

  // 線を書く
  void drawLineStrip(const std::vector<Leap::Vector>& strokes)
  {
    gl::lineWidth( 10 );
    setDiffuseColor( Color::white() );
    gl::begin( GL_LINE_STRIP );

    for ( auto position : strokes ) {
      drawVertex( position );
    }

    gl::end();

    // 線と線の補完
    drawPoints( strokes, 5, Color::white() );
  }

  // 点を書く
  void drawPoints( const std::vector<Leap::Vector>& points, int size, Color color )
  {
    glPointSize( size );
    setDiffuseColor( color );
    gl::begin( GL_POINTS );

    for ( auto point : points ) {
      drawVertex( point );
    }
    gl::end();
  }

  // 座標を設定する
  void drawVertex( Leap::Vector pointable )
  {
    // 3次元データで書く
    if ( mIs3D ) {
      // 3次元
      gl::vertex( toVec3f( pointable ) );
    }
    // 2次元データで書く
    else {
      // 座標変換する(0-1の値)
      auto box = mCurrentFrame.interactionBox();
      auto point = box.normalizePoint( pointable );

      // 縦(面がLeap Motionに対して垂直)
      gl::vertex( point.x * Width, Height  - (point.y * Height) );

      // 横(面がLeap Motionに対して水平)
      //gl::vertex( point.x * Width, point.z * Height );
    }
  }

  // GL_LIGHT0の色を変える
  void setDiffuseColor( ci::ColorA diffuseColor )
  {
    gl::color( diffuseColor );
    glLightfv( GL_LIGHT0 , GL_DIFFUSE, diffuseColor );
  }

  // Leap SDKのVectorをCinderのVec3fに変換する
  Vec3f toVec3f( Leap::Vector vec )
  {
    return Vec3f( vec.x, vec.y, vec.z );
  }

private:

  bool mIs3D;

  // Leap Motion
  Leap::Controller mLeap;
  Leap::Frame mCurrentFrame;

  // 描画用の座標
  std::vector<std::vector<Leap::Vector>> mCompleteStrokes;
  std::map<int, std::vector<Leap::Vector>> mCurrentStrokes;
  std::vector<Leap::Vector> mPointablePoints;
};

