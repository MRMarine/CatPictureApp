#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "Resources.h"

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
	Color8u black;
	Color8u green;

	static const int kAppWidth=800; // Surface width
	static const int kAppHeight=600; // Surface height
	static const int kTextureSize=1024; 
	
	//Mouse Coordinates and modifying boolean
	int mouseX1; 
	int mouseY1;
	int mouseX2;
	int mouseY2;
	bool mouseMod;

	int colorMod;

	int colorChange;

	/**
	* Creates a rectangle on the surface
	* Satisfies A.1
	* @param pixels The array of pixels to edit
	* @param posX The X-coordinate of the top left corner of the rectangle
	* @param posY The Y-coordinate of the top left corner of the rectangle
	* @param endX The X-coordinate of the bottom right corner of the rectangle
	* @param endY The Y-coordinate of the bottom right corner of the rectangle
	* @param c The color to fill the rectangle
	**/
	void makeRectangle(uint8_t* pixels, int posX, int posY, int endX, int endY, Color8u c)
	{
		for(int x = posX; x < endX; x++){
			for(int y = posY; y < endY; y++){
				pixels[3*(x + y*kTextureSize)] = c.r;
				pixels[3*(x + y*kTextureSize)+1] = c.g;
				pixels[3*(x + y*kTextureSize)+2] = c.b;
			}
		}
	}

	/**
	* Creates a rectangular area filled with a gradient from startColor to endColor
	* Actually doesnt do a real gradient but visual effect is cool
	* Possibly Satisfies A.4 
	* @param pixels The array of pixels to edit
	* @param startX The X-coordinate of the top left corner of the rectangle
	* @param startY The Y-coordinate of the top left corner of the rectangle
	* @param endX The X-coordinate of the bottom right corner of the rectangle
	* @param endY The Y-coordinate of the bottom right corner of the rectangle
	* @param startColor The first color
	* @param endColor The color which the gradient changes to
	**/
	void makeGradient(uint8_t* pixels, int startX, int startY, int endX, int endY, Color8u startColor, Color8u endColor)
	{
		Color8u tempColor = startColor;
		float tempRed = tempColor.r;
		float tempGreen = tempColor.g;
		float tempBlue = tempColor.b;

		for(int x = startX; x < endX; x++){

			if(tempColor.r != endColor.r){
				tempColor.r += (tempColor.r < endColor.r) ? (endX-startX)/10 : -(endX-startX)/10;
			}
			if(tempColor.g != endColor.g){
				tempColor.g += (tempColor.g < endColor.g) ? (endX-startX)/10 : -(endX-startX)/10;
			}
			if(tempColor.b != endColor.b){
				tempColor.b += (tempColor.b < endColor.b) ? (endX-startX)/10 : -(endX-startX)/10;
			}

			for(int y = startY; y < endY; y++){

				pixels[3*(x + y*kTextureSize)] = tempColor.r;
				pixels[3*(x + y*kTextureSize)+1] = tempColor.g;
				pixels[3*(x + y*kTextureSize)+2] = tempColor.b;
			}
		}
	}

	/**
	* Copies a rectangular area of the surface and pastes it elsewhere on the surface
	* Satisfies A.5
	* @param pixels The pixel array to edi
	* @param copyX The X-coordinate of the top left corner of the rectangle to copy
	* @param copyY The Y-coordinate of the top left corner of the rectangle to copy
	* @param copyX2 The X-coordinate of the bottom right corner of the rectangle to copy
	* @param copyY2 the Y-coordinate of the bottom right corner of the rectangle to copy
	* @param posX The X-coordinate of the top left corner of the destination rectangle
	* @param posY The Y-coordinate of the top left corner of the destination rectangle
	* @param endX The X-coordinate of the bottom right corner of the destination rectangle
	* @param endY the Y-coordinate of the bottom right corner of the destination rectangle
	**/
	void makeCopy(uint8_t* pixels, int copyX, int copyY, int copyX2, int copyY2, int posX, int posY, int endX, int endY){
		int tempY = posY;
		for(int x = copyX; x < copyX2; x++){
			for(int y = copyY; y < copyY2; y++){
				pixels[3*(posX + posY*kTextureSize)] = pixels[3*(x + y*kTextureSize)];
				pixels[3*(posX + posY*kTextureSize)+1] = pixels[3*(x + y*kTextureSize)+1];
				pixels[3*(posX + posY*kTextureSize)+2] = pixels[3*(x + y*kTextureSize)+2];
				posY += 1;
				if(posY >= endY){
					posY = tempY;
				}
				if(posY >= endY && posX == endX) return;
			}
			posX += 1;
			if(posX >= endX) return;
		}
	}

	/**
	* Applies a tint of chosen color to a rectangle of specified size
	* Satisfies A.6
	* @param pixels The array of pixels to edit
	* @param startX The X-coordinate of the top left corner of the rectangle
	* @param startY The Y-coordinate of the top left corner of the rectangle
	* @param endX The X-coordinate of the bottom right corner of the rectangle
	* @param endY The Y-coordinate of the bottom right corner of the rectangle
	* @param color The color to tint
	**/
	void addTint(uint8_t* pixels, int startX, int startY, int endX, int endY, Color8u color)
	{
		for(int x = startX; x < startX + endX; x++){
			for(int y = startY; y < startY + endY; y++){
				if((pixels[3*(x + y*kTextureSize)] + color.r) > 255){
					pixels[3*(x + y*kTextureSize)] = 255;
				}
				else{
					pixels[3*(x + y*kTextureSize)] += color.r;
				}
				if((pixels[3*(x + y*kTextureSize)+1] + color.g) > 255){
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

	/**
	* Blurs the colors of the surace by averaging pixels with all of their surrounding pixels
	* Satisfies B.1
	* @param pixels The array of pixels to edit
	**/
	void blur(uint8_t* pixels)
	{
		int sumR = 0;
		int sumG = 0;
		int sumB = 0;
		float kernal[] = {1/9.0, 1/9.0, 1/9.0, 1/9.0, 1/9.0, 1/9.0, 1/9.0, 1/9.0, 1/9.0};

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
				
				for(int i = -1; i < 2; i++){
					for(int j = -1; j < 2; j++){
						pixels[3*((x + j) + (y + i)*kTextureSize)] = sumR * kernal[(i+1) + (j+1)];
						pixels[3*((x + j) + (y + i)*kTextureSize)+1] = sumG * kernal[(i+1) + (j+1)];
						pixels[3*((x + j) + (y + i)*kTextureSize)+2] = sumB * kernal[(i+1) + (j+1)];
					}
				}
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
	black = Color8u(0,0,0);
	blue = Color8u(0,0,255);
	red = Color8u(100,0,0);
	green = Color8u(0,255,0);
	mouseMod = false;
	colorChange = 0;
	colorMod = false;

	makeGradient(data, 0, 0, 800, 600, Color8u(0,255, 0), Color8u(0,0,255));
	makeGradient(data,0,0,200,200, Color8u(255,0,0), Color8u(0,255,0));
	makeGradient(data,600,0,800,200, Color8u(255,255,0), Color8u(0,0,255));
	makeGradient(data,200,200,600,400, Color8u(0,0,255), Color8u(255,0,0));
	makeGradient(data,0,400,200,600, Color8u(100,50,0), Color8u(55,0,0));
	makeGradient(data,600,400,800,600, Color8u(50,0,100), Color8u(0,0,50));
	makeRectangle(data,200,0,600,200,blue);
	makeRectangle(data,200,400,600,600,red);
	//makeRectangle(data,0,0,800,600,black); //Fill window with black
	//makeRectangle(data, 750, 550, 800, 600, red);
	
	//addTint(data,0,0,800,600,red); // Uncomment to add a tint, currently set to red
	//makeCopy(data,0,0,50,50,200,200,250,250);
	blur(data);
	cinder::writeImage("cseimage.png",*mySurface_);
}
/**
* Catches 2 mouse clicks and creates a rectangle between them
* only draws a rectangle if first click is the top left point 
* and second is the bottom right point of the rectangle
* @param event A mouse event to allow interaction with the program
**/
void HW1App::mouseDown( MouseEvent event )
{
	//Sets bottom right coordinate of rectangle and draws it
	//Color is currently set to blue
	if(event.isLeft() && mouseMod){
		mouseX2 = event.getX();
		mouseY2 = event.getY();
		makeRectangle(data, mouseX1, mouseY1, mouseX2, mouseY2, blue);
		mouseMod = false;
		return;
	}
	if(event.isLeft()){ //Sets top left coordinate of rectangle/prepares function for next click
		mouseX1 = event.getX();
		mouseY1 = event.getY();
		mouseMod = true;
		return;
	}
}

void HW1App::update()
{
	colorChange++;
	//Giving both colors colorChange as a parameter creates moving effect
	//Leave one color parameter with non changing parameters to remove motion effect
	//Counts for E.5 Animation
	//Note blur() slows down animation extremely
	//makeGradient(data, 0, 0, 800, 600, Color8u(0,255-colorChange, 0), Color8u(0,0,colorChange));
}

void HW1App::draw()
{
	gl::draw(*mySurface_); 

}

CINDER_APP_BASIC( HW1App, RendererGl )
