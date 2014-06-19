#include "Leap.h"

class SampleListener : public Leap::Listener
{
public:

    void onFrame(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onFocusGained(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onFocusLost(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }
};

void main()
{
  // リスナーを登録する
  // リスナーとのやり取りは別スレッドにて行われる
  SampleListener listener;
  Leap::Controller leap;
  leap.addListener( listener );

  leap.setPolicyFlags( Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES );

  std::cout << "終了するには何かキーを押してください" << std::endl;
  std::cin.get();

  leap.removeListener( listener );
}
