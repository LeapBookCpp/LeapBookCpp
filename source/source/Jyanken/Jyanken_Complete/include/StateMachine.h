#pragma once 

#include "GameAssets.h"

// 状態の基底クラス
class State
{
protected:

  GameAssets* mAssets;
  int64_t mEnterTime;

public:

  State( GameAssets* assets )
    : mAssets( assets )
  {
    mEnterTime = mAssets->getFrame().timestamp();
  }

  virtual State* update()
  {
    return this;
  }

protected:

  // 指の数から、ポーズを決める
  JyankenPose fingerCountToPose( Leap::Hand hand )
  {
    // 伸びている指の数を数える
    auto count = std::count_if( hand.fingers().begin(), hand.fingers().end(),
      [](const Leap::Finger& finger) { return finger.isExtended(); } );
    if ( count <= 1 ) {
      return JyankenPose::Gu;
    }
    else if ( count <= 3 ) {
      return JyankenPose::Choki;
    }
      
    return JyankenPose::Pa;
  }
};

// アイドル(待機)状態
class IdleState : public State
{
public:

  IdleState( GameAssets* assets )
    : State( assets )
  {
  }

  State* update();
};

// 「じゃんけん」の掛け声を行う状態
class JyankenState : public State
{
public:

  JyankenState( GameAssets* assets )
    : State( assets )
  {
  }

  State* update();
};

// 「ぽん」の掛け声とポーズの決定行う状態
class PonState : public State
{
  JyankenPose mPlayer;
  JyankenPose mComputer;
  std::list<float> mTransrates;

public:

  PonState( GameAssets* assets )
    : State( assets )
  {
    // コンピューターのポーズを決める
    mComputer = (JyankenPose)(Rand::randUint() % 3);
  }

  State* update();
};

// 結果の表示を行う状態
class ResultState : public State
{
  JyankenPose mPlayer;
  JyankenPose mComputer;

public:

  ResultState( GameAssets* assets, JyankenPose player, JyankenPose computer )
    : State( assets )
    , mPlayer( player )
    , mComputer( computer )
  {
  }

  State* update();
};

State* IdleState::update()
{
  mAssets->setComputerPose( JyankenPose::Gu );
  mAssets->setText( T( "手を出してね" ) );

  // 手を検出したら、次の状態へ遷移する
  auto frame = mAssets->getFrame();
  if ( frame.hands().count() == 0 ) {
    return this;
  }
  else {
    return new JyankenState( mAssets );
  }
}

State* JyankenState::update()
{
  auto hand = mAssets->getFrame().hands()[0];
  if ( hand.isValid() ) {
    mAssets->setPlayerPose( fingerCountToPose( hand ) );
  }

  mAssets->setComputerPose( JyankenPose::Gu );
  mAssets->setText( T( "じゃんけん" ) );

  // 一定時間で次の状態へ遷移する
  if ( (mAssets->getFrame().timestamp() - mEnterTime) < (2 * 1000 * 1000) ) {
    return this;
  }
  else {
    return new PonState( mAssets );
  }
}

State* PonState::update()
{
  mAssets->setComputerPose( mComputer );
  mAssets->setText( T( "ぽん" ) );

  auto hand = mAssets->getFrame().hands()[0];
  if ( hand.isValid() ) {
    mPlayer = fingerCountToPose( hand );
    mAssets->setPlayerPose( mPlayer );

    // 手の移動量を保存する
    auto transrate = hand.translation( mAssets->getFrame( 1 ) );
    mTransrates.push_back( transrate.x + transrate.y + transrate.z );
    if ( mTransrates.size() >= 10 ) {
      mTransrates.pop_front();
    }
  }

  if ( mTransrates.size() >= 5 ) {
    // 手の移動量が一定以下なら、プレイヤーのポーズを決定する
    auto count = std::count_if(mTransrates.begin(), mTransrates.end(),
      []( float move ) { return std::abs( move ) < 10; } );
    if ( count == mTransrates.size() ) {
      return new ResultState( mAssets, mPlayer ,mComputer );
    }
  }

  // 一定時間ポーズが決まらなかったら、アイドル状態にもどる
  if ( (mAssets->getFrame().timestamp() - mEnterTime) > (5 * 1000 * 1000) ) {
    return new IdleState( mAssets );
  }

  return this;
}

State* ResultState::update()
{
  // 勝ち負けを判定する
  mAssets->setComputerPose( mComputer );
  mAssets->setPlayerPose( mPlayer );

  if ( mComputer == mPlayer ) {
    mAssets->setText( T( "あいこ" ) );
  }
  else if ( (mComputer == JyankenPose::Gu) && (mPlayer == JyankenPose::Pa) ) {
    mAssets->setText( T( "あなたの勝ち" ) );
  }
  else if ( (mComputer == JyankenPose::Pa) && (mPlayer == JyankenPose::Choki) ) {
    mAssets->setText( T( "あなたの勝ち" ) );
  }
  else if ( (mComputer == JyankenPose::Choki) && (mPlayer == JyankenPose::Gu) ) {
    mAssets->setText( T( "あなたの勝ち" ) );
  }
  else {
    mAssets->setText( T( "あなたの負け" ) );
  }

  // 一定時間でアイドル状態に戻る
  if ( (mAssets->getFrame().timestamp() - mEnterTime) < (2 * 1000 * 1000) ) {
    return this;
  }
  else {
    return new IdleState( mAssets );
  }
}

// 状態遷移の処理
class StateMachine
{
  State* state;

public:

  StateMachine()
    : state( 0 )
  {
  }

  void setup( GameAssets* assets )
  {
    state = new IdleState( assets );
  }

  void update()
  {
    if ( state != 0 ) {
      auto nextState = state->update();
      if ( state != nextState ) {
        delete state;

        state = nextState;
      }
    }
  }
};
