#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "Resources.h"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class HW1App : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings(Settings* settings);
  private:
	Surface* mySurface_;
	uint8_t* data;
	Color8u blue;
	Color8u red;
	Color8u white;

	static const int kAppWidth=800;
	static const int kAppHeight=600;
	static const int kTextureSize=1024;
	
	int frameNumber;
	boost::posix_time::ptime app_start_time_;

	void makeRectangle(uint8_t* pixels, int posX, int posY, int endX, int endY, Color8u c)
	{
		for(int x = posX; x < posX + endX; x++){
			for(int y = posY; y < posY + endY; y++){
				pixels[3*(x + y*kTextureSize)] = c.r;
				pixels[3*(x + y*kTextureSize)+1] = c.g;
				pixels[3*(x + y*kTextureSize)+2] = c.b;
			}
		}
	}

	void makeTriangle(uint8_t* pixels){
		int endRow = 76;
		for(int y = 0; y < 50; y++){
			for(int x = 75; x < endRow; x++){
				if(x <= endRow){
					pixels[3*(x + y*kTextureSize)] = 0;
					pixels[3*(x + y*kTextureSize)+1] = 255;
					pixels[3*(x + y*kTextureSize)+2] = 255;
				}
			}
			endRow++;
			if(endRow > 100) continue;
		}
	}

	void makeGradient(uint8_t* pixels, Color8u startColor, Color8u endColor)
	{
		Color8u tempColor = startColor;

		for(int x = 0; x < 50; x++){
			if(tempColor.r != endColor.r){
				tempColor.r += (tempColor.r < endColor.r) ? 5 : -5;
			}
			if(tempColor.g != endColor.g){
				tempColor.g += (tempColor.g < endColor.g) ? 5 : -5;
			}
			if(tempColor.b != endColor.b){
				tempColor.b += (tempColor.b < endColor.b) ? 5 : -5;
			}

			for(int y = 100; y < 150; y++){

				pixels[3*(x + y*kTextureSize)] = tempColor.r;
				pixels[3*(x + y*kTextureSize)+1] = tempColor.g;
				pixels[3*(x + y*kTextureSize)+2] = tempColor.b;
			}
		}
	}

	void makeCopy(uint8_t* pixels, int copyX, int copyY, int copyX2, int copyY2, int posX, int posY, int endX, int endY){
		
		for(int x = posX; x < posX + endX; x++){
			for(int y = posY; y < posY + endY; y++){
				pixels[3*(x + y*kTextureSize)] = pixels[3*(copyX + copyY*kTextureSize)];
				pixels[3*(x + y*kTextureSize)+1] = pixels[3*(copyX + copyY*kTextureSize)+1];
				pixels[3*(x + y*kTextureSize)+2] = pixels[3*(copyX + copyY*kTextureSize)+2];
			}
		}
	}

	void makeLineSegment(uint8_t* pixels, int startX, int startY, int endX, int endY, float xSlope, float ySlope, Color8u color)
	{
		int y = startY;
		int tempY = startY;
		int tempX = startX;
		for(int x = startX; x < endX; (x = (int)tempX)){
			pixels[3*(x + y*kTextureSize)] = color.r;
			pixels[3*(x + y*kTextureSize)+1] = color.g;
			pixels[3*(x + y*kTextureSize)+2] = color.b;
			tempX += xSlope;
			tempY += ySlope;
			y = (int)tempY;
		}
	}

	void makeCircle(uint8_t* pixels, int endX, int endY, int radius, Color8u color)
	{
		bool mid = false;	
		int rChange = 0;
		for(int y = endY - (2*radius); y < endY; y++){
			for(int x = endX - (2*radius); x < endX; x-1){
				if(x + radius == endX - radius) mid = true;
				if(!mid){
					pixels[3*((x + (radius-rChange)) + y*kTextureSize)] = color.r;
					pixels[3*((x + (radius-rChange)) + y*kTextureSize)+1] = color.g;
					pixels[3*((x + (radius-rChange)) + y*kTextureSize)+2] = color.b;
					rChange++;
				}
				else{
					pixels[3*((x + (radius + rChange)) + y*kTextureSize)] = color.r;
					pixels[3*((x + (radius + rChange)) + y*kTextureSize)+1] = color.g;
					pixels[3*((x + (radius + rChange)) + y*kTextureSize)+2] = color.b;
					rChange--;
					x += 2;
				}
			}
		}
	}

	void addTint(uint8_t* pixels, int startX, int startY, int endX, int endY, Color8u color)
	{
		for(int x = startX; x < startX + endX; x++){
			for(int y = startY; y < startY + endY; y++){
				if((pixels[3*(x + y*kTextureSize)] += color.r) > 255){
					pixels[3*(x + y*kTextureSize)] = 255;
				}
				else{
					pixels[3*(x + y*kTextureSize)] += color.r;
				}
				if((pixels[3*(x + y*kTextureSize)+1] += color.g) > 255){
					pixels[3*(x + y*kTextureSize)+1] = 255;
				}
				else{
					pixels[3*(x + y*kTextureSize)+1] += color.g;
				}
				if((pixels[3*(x + y*kTextureSize)+2] + color.b) > 255){
					pixels[3*(x + y*kTextureSize)+2] = 255;
				}
				else{
					pixels[3*(x + y*kTextureSize)+2] += color.b;
				}
			}
		}	
	}

	void blur(uint8_t* pixels)
	{
		int sumR = 0;
		int sumG = 0;
		int sumB = 0;

		for(int x = 1; x < 799; x++){
			for(int y = 1; y < 599; y++){
				sumR = (pixels[3*((x-1) + (y-1)*kTextureSize)] + pixels[3*(x + (y-1)*kTextureSize)] + pixels[3*((x+1) + (y-1)*kTextureSize)] +
					pixels[3*((x-1) + y*kTextureSize)] + pixels[3*(x + y*kTextureSize)] + pixels[3*((x+1) + y*kTextureSize)] + 
					pixels[3*((x-1) + (y+1)*kTextureSize)] + pixels[3*(x + (y+1)*kTextureSize)] + pixels[3*((x+1) + (y+1)*kTextureSize)]);
				sumG = (pixels[3*((x-1) + (y-1)*kTextureSize)+1] + pixels[3*(x + (y-1)*kTextureSize)+1] + pixels[3*((x+1) + (y-1)*kTextureSize)+1] +
					pixels[3*((x-1) + y*kTextureSize)+1] + pixels[3*(x + y*kTextureSize)+1] + pixels[3*((x+1) + y*kTextureSize)+1] + 
					pixels[3*((x-1) + (y+1)*kTextureSize)+1] + pixels[3*(x + (y+1)*kTextureSize)+1] + pixels[3*((x+1) + (y+1)*kTextureSize)+1]);
				sumB = (pixels[3*((x-1) + (y-1)*kTextureSize)+2] + pixels[3*(x + (y-1)*kTextureSize)+2] + pixels[3*((x+1) + (y-1)*kTextureSize)+2] +
					pixels[3*((x-1) + y*kTextureSize)+2] + pixels[3*(x + y*kTextureSize)+2] + pixels[3*((x+1) + y*kTextureSize)+2] + 
					pixels[3*((x-1) + (y+1)*kTextureSize)+2] + pixels[3*(x + (y+1)*kTextureSize)+2] + pixels[3*((x+1) + (y+1)*kTextureSize)+2]);
				pixels[3*(x + y*kTextureSize)] = sumR/9;
				pixels[3*(x + y*kTextureSize)+1] = sumG/9;
				pixels[3*(x + y*kTextureSize)+2] = sumB/9;
			}
		}
	}
};

void HW1App::prepareSettings(Settings* settings){
	(*settings).setWindowSize(kAppWidth,kAppHeight);
	(*settings).setResizable(false);
}


void HW1App::setup()
{
	mySurface_ = new Surface(kTextureSize,kTextureSize,false);
	data = (*mySurface_).getData();
	white = Color8u(0,0,0);
	blue = Color8u(0,0,255);
	red = Color8u(100,0,0);
	//makeRectangle(data, 0, 0, 800, 600, white);
	frameNumber = 0;


	//makeRectangle(data, 750, 550, 50, 50, red);
	//makeTriangle(data);
	//makeGradient(data, blue, red);
	//makeCopy(data,0,100,50,150,0,0,50,50);
	//makeLineSegment(data, 0, 0, 50, 50, 1, 1, blue);
	makeCircle(data,50,50,50,red);
	//addTint(data,0,0,800,600,red);
	//blur(data);
}

void HW1App::mouseDown( MouseEvent event )
{
}

void HW1App::update()
{

}

void HW1App::draw()
{
	gl::draw(*mySurface_); 
}

CINDER_APP_BASIC( HW1App, RendererGl )
