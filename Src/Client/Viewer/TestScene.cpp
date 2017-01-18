
#include "stdafx.h"
#include "TestScene.h"



cTestScene::cTestScene(LPD3DXSPRITE sprite) :
	framework::cWindow(sprite, 0, "testScene")
{
	SetTexture("The-Waters-Edge.jpg");

	framework::cButton *btn1 = new framework::cButton(sprite, 1);
	btn1->SetTexture("button1.png");
	InsertChild(btn1);

	framework::cButton *btn2 = new framework::cButton(sprite, 2);
	btn2->SetTexture("button2.png");
	btn2->SetScale(Vector3(2,2,0));
	btn2->SetPos(Vector3(200,0,0));
	InsertChild(btn2);

	framework::cButton *btn3 = new framework::cButton(sprite, 3);
	btn3->SetTexture("button1.png");
	btn3->SetPos(Vector3(200,0,0));
	InsertChild(btn3);


	EventConnect(this, framework::EVENT::BUTTON_CLICK, 1, (framework::EventFunction)&cTestScene::Button1Click);
}

cTestScene::~cTestScene()
{

}


void cTestScene::Button1Click(framework::cEvent &event)
{

}
