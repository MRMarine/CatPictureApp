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
	// nice sorting of variables.  They are neat and easy to read!
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

	//int colorMod;

	//int colorChange;

	/**
	* Creates a rectangle on the surface
	* Satisfies A.1
	* @param pixels The array of pixels to edit
	* @param posX The X-coordinate of the top left corner of the rectangle
	* @param posY The Y-coordinate of the top left corner of the rectangle
	* @param endX The X-coordinate of the bottom right corner of the rectangle
	* @param endY The Y-coordinate of the bottom right corner of the rectangle
	* @param color The color to fill the rectangle
	**/
	void makeRectangle(uint8_t* pixels, int posX, int posY, int endX, int endY, Color8u color)
	{
		for(int x = posX; x < endX; x++){
			for(int y = posY; y < endY; y++){
				int offset = 3*(x + y*kTextureSize);
				pixels[offset] = color.r;
				pixels[offset + 1] = color.g;
				pixels[offset + 2] = color.b;
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
				int offset = 3*(x + y*kTextureSize);
				pixels[offset] = tempColor.r;
				pixels[offset + 1] = tempColor.g;
				pixels[offset + 2] = tempColor.b;
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
				int offset = 3*(posX + posY*kTextureSize);

				pixels[offset] = pixels[offset];
				pixels[offset+1] = pixels[offset + 1];
				pixels[offset+2] = pixels[offset + 2];

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
				int offset = 3*(x + y*kTextureSize);

				if((pixels[offset] + color.r) > 255){
					pixels[offset] = 255;
				}
				else{
					pixels[offset] += color.r;
				}
				if((pixels[offset+1] + color.g) > 255){
					pixels[offset+1] = 255;
				}
				else{
					pixels[offset+1] += color.g;
				}
				if((pixels[offset+2] + color.b) > 255){
					pixels[offset+2] = 255;
				}
				else{
					pixels[offset+2] += color.b;
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

		// You shouldn't use arbitrary numbers in for loop.
		// If the values make sense for what you're doing, it's fine, but 799 and 599 only make sense if you know kAppWidth and kAppHeight
		// If kAppWidth and kAppHeight are ever changed, using 799 and 599 in a for loop is a liability, especially if the surface size is reduced as well.
		// If it is related to a constant, use that constant and add, subtract, etc. as you would to get the number you would otherwise need. It is safer and makes more sense to readers
		for(int x = 1; x < kAppWidth - 1; x++){
			for(int y = 1; y < kAppHeight - 1; y++){
				// I recommend nesting these in two additional for loops (like the two just below this) and use variables to hold everything in square brackets.
				// It took me a while to see what you were doing here, as well as a few seconds to come to terms with seeing a huge block of mostly unorganized text.
				// A plus of restructuring this is that if you were to use a kernel that wasn't identical across all values, it could do other convolution-based filter effects.

				// I'm actually afraid to restructure this for you for fear of breaking something I can't see running in your app as it is.

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
						int offset = 3*((x + j) + (y + i)*kTextureSize);
						int kOffset = (i+1) + (j+1);

						pixels[offset] = sumR * kernal[kOffset];
						pixels[offset + 1] = sumG * kernal[kOffset];
						pixels[offset + 2] = sumB * kernal[kOffset];
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

/**
	* Performs the initializations needed to get the app ready to run.
	**/
void HW1App::setup()
{
	mySurface_ = new Surface(kTextureSize,kTextureSize,false);
	data = (*mySurface_).getData();
	black = Color8u(0,0,0);
	blue = Color8u(0,0,255);
	red = Color8u(100,0,0);
	green = Color8u(0,255,0);
	mouseMod = false;
	
	/* These two variables are commented out because they don't do anything
		but might be useful later on or for an unimplemented method */
	//colorChange = 0;
	//colorMod = false;

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
	// is there a way to make mouseX2, mouse Y2 values changed to mouseX1, mouseY1 if they do not draw anything in makeRectangle()?
	// I had to read through the source code to find out why things were only happening some of the time even when I was clicking below and to the right of my previous click

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
/**
* Updates the app before the draw method is called.
* It currently does nothing
**/
void HW1App::update()
{
	// colorChange has been commented out because it was never used and there was no sense allocating memory that would never be used
	//colorChange++;
	//Giving both colors colorChange as a parameter creates moving effect
	//Leave one color parameter with non changing parameters to remove motion effect
	//Counts for E.5 Animation
	//Note blur() slows down animation extremely
	//makeGradient(data, 0, 0, 800, 600, Color8u(0,255-colorChange, 0), Color8u(0,0,colorChange));
}

/**
* Draws the surface to the screen
**/
void HW1App::draw()
{
	gl::draw(*mySurface_); 

}

CINDER_APP_BASIC( HW1App, RendererGl )
