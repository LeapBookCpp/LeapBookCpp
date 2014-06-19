#include "Leap.h"

class SampleListener : public Leap::Listener
{
public:


    void onInit(const Leap::Controller& controller)  
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onConnect(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onDisconnect(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onExit(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onFrame(const Leap::Controller&) 
    {
      //std::cout << __FUNCTION__ << std::endl;
    }

    void onFocusGained(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onFocusLost(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onServiceConnect(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onServiceDisconnect(const Leap::Controller&) 
    {
      std::cout << __FUNCTION__ << std::endl;
    }

    void onDeviceChange(const Leap::Controller& controller) 
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

  std::cout << "終了するには何かキーを押してください" << std::endl;
  std::cin.get();

  leap.removeListener( listener );
}
