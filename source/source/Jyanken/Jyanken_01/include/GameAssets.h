#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/MayaCamUI.h"
#include "cinder/ImageIo.h"
#include "cinder/ip/Blend.h"
#include "cinder/Utilities.h"

#include "Leap.h"

using namespace ci;
using namespace ci::app;
using namespace std;


enum JyankenPose
{
  None,
  Gu,
  Pa,
  Choki,
};

class GameAssets
{
  Surface32f mPaPlayer;
  Surface32f mGuPlayer;
  Surface32f mChokiPlayer;

  Surface32f mPa;
  Surface32f mGu;
  Surface32f mChoki;
  Surface32f mJyanken;
  Surface32f mOutputSurface;

  Surface mText;
  JyankenPose mPose;
  JyankenPose mPosePlayer;

  Leap::Controller mLeap;

public:

  GameAssets()
  {
  }

  Leap::Frame getFrame( int history = 0 )
  {
    return mLeap.frame( history );
  }

  void setup()
  {
    auto app = App::get();

    mPaPlayer = loadImage( app->loadAsset( "Image/pa_player.png" ) );
    mGuPlayer = loadImage( app->loadAsset( "Image/gu_player.png" ) );
    mChokiPlayer = loadImage( app->loadAsset( "Image/choki_player.png" ) );

    mPa = loadImage( app->loadAsset( "Image/pa.png" ) );
    mGu = loadImage( app->loadAsset( "Image/gu.png" ) );
    mChoki = loadImage( app->loadAsset( "Image/choki.png" ) );
    mJyanken = loadImage( app->loadAsset( "Image/jyanken.png" ) );

    mOutputSurface = Surface32f( app->getWindowWidth(), app->getWindowHeight(), false );
  }

  void setPlayerPose( JyankenPose pose )
  {
    mPosePlayer = pose;
  }

  void setComputerPose( JyankenPose pose )
  {
    mPose = pose;
  }

  void setText( const std::wstring& text )
  {
    TextLayout renderText;
    renderText.clear( ColorA::zero() );
    renderText.setColor( Color::black() );
  	renderText.setFont( Font( "ü‡ÉSÉVÉbÉNëÃ", 128 ) );
    renderText.addLine((char*)cinder::toUtf8(text).c_str());
    mText = renderText.render( true );
  }

  Surface getSurface()
  {
    mOutputSurface = mJyanken.clone();

    if ( mPose == JyankenPose::Pa ) {
      ip::blend( &mOutputSurface, mPa );
    }
    else if ( mPose == JyankenPose::Gu ) {
      ip::blend( &mOutputSurface, mGu );
    }
    else if ( mPose == JyankenPose::Choki ) {
      ip::blend( &mOutputSurface, mChoki );
    }

    if ( mPosePlayer == JyankenPose::Pa ) {
      ip::blend( &mOutputSurface, mPaPlayer );
    }
    else if ( mPosePlayer == JyankenPose::Gu ) {
      ip::blend( &mOutputSurface, mGuPlayer );
    }
    else if ( mPosePlayer == JyankenPose::Choki ) {
      ip::blend( &mOutputSurface, mChokiPlayer );
    }

    ip::blend( (Surface*)&mOutputSurface, mText, mText.getBounds(), Vec2f( 500, 50 ) );

    return mOutputSurface;
  }
};

