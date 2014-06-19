#include "Leap.h"

// イベントを受け取るためのクラス
class SampleListener : public Leap::Listener
{
public:

    // フレームの更新イベント
    void onFrame(const Leap::Controller& controller ) 
    {
      Leap::Frame frame = controller.frame();
      std::cout <<  frame.id() << std::endl;
    }
};

void main()
{
  // リスナーを登録する
  // リスナーとのやり取りは別スレッドにて行われる
  SampleListener listener;
  Leap::Controller leap;
  leap.addListener( listener );

  std::cout << "終了するには何かキーを押してください" << std::endl;
  std::cin.get();

  leap.removeListener( listener );
}
