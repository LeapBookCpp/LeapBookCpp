#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "Leap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PollingApp : public AppNative, public Leap::Listener {
public:

	void setup()
  {
    mLeap.addListener( *this );
  }

  void draw()
  {
	  gl::clear( Color( 0, 0, 0 ) ); 

    Leap::Frame frame = mLeap.frame();
    console() << frame.id() << std::endl;
  }

  void onConnect(const Leap::Controller&)
  {
    console() << "Connect!!" << std::endl;
  }

  void onDisconnect(const Leap::Controller&)
  {
    console() << "Disconnect!!" << std::endl;
  }

  Leap::Controller mLeap;
};

CINDER_APP_NATIVE( PollingApp, RendererGl )
