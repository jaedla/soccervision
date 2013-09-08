#ifndef GUI_H
#define GUI_H

#define _WINSOCKAPI_
#include <windows.h>

#include "DisplayWindow.h"
#include "ImageBuffer.h"
#include "MouseListener.h"
#include <vector>
#include <string>

class Command;
class Vision;
class Blobber;
class ParticleFilterLocalizer;

class Gui : public MouseListener {

public:
	class Element : public MouseListener {
		public:
			virtual void draw(unsigned char* image, int width, int height) = 0;
			ImageBuffer img;
	};

	class Button : public Element {
		public:
			Button(std::string text, int x, int y, int width = 0, int type = 0, void* data = NULL);
			void draw(unsigned char* image, int width, int height);

			std::string text;
			int x;
			int y;
			int width;
			int type;
			void* data;
	};

    Gui(HINSTANCE instance, Blobber* blobberFront, Blobber* blobberRear, int width, int height);
    ~Gui();

	DisplayWindow* createWindow(int width, int height, std::string name);
	Button* createButton(std::string text, int x, int y, int width = 0, int type = 0, void* data = NULL);
	void drawElements(unsigned char* image, int width, int height);
    bool update();
	void addMouseListener(MouseListener* listener);
	void setFPS(int fps) { this->fps = fps; };
	void setFrontImages(unsigned char* rgb, unsigned char* yuyv, unsigned char* dataY, unsigned char* dataU, unsigned char* dataV, unsigned char* classification);
	void setRearImages(unsigned char* rgb, unsigned char* yuyv, unsigned char* dataY, unsigned char* dataU, unsigned char* dataV, unsigned char* classification);
	void onMouseMove(int x, int y, DisplayWindow* win);
	void onMouseDown(int x, int y, MouseListener::MouseBtn btn, DisplayWindow* win);
	void onMouseUp(int x, int y, MouseListener::MouseBtn btn, DisplayWindow* win);
	void onMouseWheel(int delta, DisplayWindow* win);
	void emitMouseDown(int x, int y, MouseListener::MouseBtn btn, DisplayWindow* win);
	void emitMouseUp(int x, int y, MouseListener::MouseBtn btn, DisplayWindow* win);
	void emitMouseMove(int x, int y, DisplayWindow* win);
	void emitMouseWheel(int delta, DisplayWindow* win);

private:
	void handleColorThresholding(unsigned char* dataY, unsigned char* dataU, unsigned char* dataV, unsigned char* rgb, unsigned char* classification);

	HINSTANCE instance;
	MSG msg;
	std::vector<MouseListener*> mouseListeners;
	std::vector<DisplayWindow*> windows;
	DisplayWindow* frontRGB;
	DisplayWindow* rearRGB;
	DisplayWindow* frontClassification;
	DisplayWindow* rearClassification;
	Blobber* blobberFront;
	Blobber* blobberRear;
	std::vector<Element*> elements;
	int width;
	int height;
	int fps;
	int mouseX;
	int mouseY;
	bool mouseDown;
	MouseListener::MouseBtn mouseBtn;
	int brushRadius;
};

#endif // GUI_H
