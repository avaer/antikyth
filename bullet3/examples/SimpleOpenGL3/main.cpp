


#include "OpenGLWindow/SimpleOpenGL3App.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3CommandLineArgs.h"
#include "assert.h"
#include <stdio.h>

char* gVideoFileName = 0;
char* gPngFileName = 0;

static b3WheelCallback sOldWheelCB = 0;
static b3ResizeCallback sOldResizeCB = 0;
static b3MouseMoveCallback sOldMouseMoveCB = 0;
static b3MouseButtonCallback sOldMouseButtonCB = 0;
static b3KeyboardCallback sOldKeyboardCB = 0;
//static b3RenderCallback sOldRenderCB = 0;

float gWidth = 1024;
float gHeight = 768;

void MyWheelCallback(float deltax, float deltay)
{
	if (sOldWheelCB)
		sOldWheelCB(deltax,deltay);
}
void MyResizeCallback( float width, float height)
{
    gWidth = width;
    gHeight = height;
    
	if (sOldResizeCB)
		sOldResizeCB(width,height);
}
void MyMouseMoveCallback( float x, float y)
{
	printf("Mouse Move: %f, %f\n", x,y);

	if (sOldMouseMoveCB)
		sOldMouseMoveCB(x,y);
}
void MyMouseButtonCallback(int button, int state, float x, float y)
{
	if (sOldMouseButtonCB)
		sOldMouseButtonCB(button,state,x,y);
}


void MyKeyboardCallback(int keycode, int state)
{
	//keycodes are in examples/CommonInterfaces/CommonWindowInterface.h
	//for example B3G_ESCAPE for escape key
	//state == 1 for pressed, state == 0 for released.
	// use app->m_window->isModifiedPressed(...) to check for shift, escape and alt keys
	printf("MyKeyboardCallback received key:%c in state %d\n",keycode,state);
	if (sOldKeyboardCB)
		sOldKeyboardCB(keycode,state);
}


int main(int argc, char* argv[])
{
	{
		b3CommandLineArgs myArgs(argc, argv);


		SimpleOpenGL3App* app = new SimpleOpenGL3App("SimpleOpenGL3App", gWidth, gHeight, true);

		app->m_instancingRenderer->getActiveCamera()->setCameraDistance(13);
		app->m_instancingRenderer->getActiveCamera()->setCameraPitch(0);
		app->m_instancingRenderer->getActiveCamera()->setCameraTargetPosition(0, 0, 0);
		sOldKeyboardCB = app->m_window->getKeyboardCallback();
		app->m_window->setKeyboardCallback(MyKeyboardCallback);
		sOldMouseMoveCB = app->m_window->getMouseMoveCallback();
		app->m_window->setMouseMoveCallback(MyMouseMoveCallback);
		sOldMouseButtonCB = app->m_window->getMouseButtonCallback();
		app->m_window->setMouseButtonCallback(MyMouseButtonCallback);
		sOldWheelCB = app->m_window->getWheelCallback();
		app->m_window->setWheelCallback(MyWheelCallback);
		sOldResizeCB = app->m_window->getResizeCallback();
		app->m_window->setResizeCallback(MyResizeCallback);


		myArgs.GetCmdLineArgument("mp4_file", gVideoFileName);
		if (gVideoFileName)
			app->dumpFramesToVideo(gVideoFileName);

		myArgs.GetCmdLineArgument("png_file", gPngFileName);
		char fileName[1024];

		int textureWidth = 128;
		int textureHeight = 128;

		unsigned char*	image = new unsigned char[textureWidth*textureHeight * 4];


		int textureHandle = app->m_renderer->registerTexture(image, textureWidth, textureHeight);

		int cubeIndex = app->registerCubeShape(1, 1, 1);

		b3Vector3 pos = b3MakeVector3(0, 0, 0);
		b3Quaternion orn(0, 0, 0, 1);
		b3Vector3 color = b3MakeVector3(1, 0, 0);
		b3Vector3 scaling = b3MakeVector3 (1, 1, 1);
		app->m_renderer->registerGraphicsInstance(cubeIndex, pos, orn, color, scaling);
		app->m_renderer->writeTransforms();

		do
		{
			static int frameCount = 0;
			frameCount++;
			if (gPngFileName)
			{
				printf("gPngFileName=%s\n", gPngFileName);

				sprintf(fileName, "%s%d.png", gPngFileName, frameCount++);
				app->dumpNextFrameToPng(fileName);
			}



			//update the texels of the texture using a simple pattern, animated using frame index
			for (int y = 0; y < textureHeight; ++y)
			{
				const int	t = (y + frameCount) >> 4;
				unsigned char*	pi = image + y*textureWidth * 3;
				for (int x = 0; x < textureWidth; ++x)
				{
					const int		s = x >> 4;
					const unsigned char	b = 180;
					unsigned char			c = b + ((s + (t & 1)) & 1)*(255 - b);
					pi[0] = pi[1] = pi[2] = pi[3] = c; pi += 3;
				}
			}

			app->m_renderer->activateTexture(textureHandle);
			app->m_renderer->updateTexture(textureHandle, image);

			float color[4] = { 255, 1, 1, 1 };
			app->m_primRenderer->drawTexturedRect(100, 200, gWidth / 2 - 50, gHeight / 2 - 50, color, 0, 0, 1, 1, true);


			app->m_instancingRenderer->init();
			app->m_instancingRenderer->updateCamera();

			app->m_renderer->renderScene();
			app->drawGrid();
			char bla[1024];
			sprintf(bla, "Simple test frame %d", frameCount);

			app->drawText(bla, 10, 10);
			app->swapBuffer();
		} while (!app->m_window->requestedExit());



		delete app;

		delete[] image;
	}
	return 0;
}
