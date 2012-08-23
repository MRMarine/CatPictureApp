#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CatPictureAppApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
  private:
	float redScaler_;
	float greenScaler_;
	float blueScaler_;
};

void CatPictureAppApp::setup()
{
	redScaler_ = 0.0f;
	greenScaler_ = 0.5f;
	blueScaler_ = 1.0f;
}

void CatPictureAppApp::mouseDown( MouseEvent event )
{
}

void CatPictureAppApp::update()
{
	redScaler_ += 0.01f;
	if(redScaler_ >= 1.0f){
		redScaler_ = 0.0f;
	}
	
	greenScaler_ -= 0.01f;
	if(greenScaler_ <= 0.0f){
		greenScaler_ = 1.0f;
	}

	blueScaler_ += 0.01f;
	if(blueScaler_ >= 1.0f){
		blueScaler_ = 0.5f;
	}
}

void CatPictureAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( redScaler_, greenScaler_, blueScaler_ ) ); 
}

CINDER_APP_BASIC( CatPictureAppApp, RendererGl )
