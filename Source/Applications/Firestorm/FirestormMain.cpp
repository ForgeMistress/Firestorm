#include "stdafx.h"

#include <libIO/libIO.h>
#include <libIO/Logger.h>

#include <libApp/libApp.h>
#include <libApp/Application.h>

using namespace Firestorm;

class FirestormApp : public Application
{
public:
	virtual void OnInitialize(int ac, char** av);
	virtual void OnUpdate(double deltaT);
	virtual void OnClose();
	virtual int  OnShutdown();

	void HandleApplicationWantsToClose(const ApplicationWantsToCloseEvent& event);
private:
};

void FirestormApp::OnInitialize(int ac, char** av)
{
	Dispatcher.Register(&FirestormApp::HandleApplicationWantsToClose, this);
}

void FirestormApp::OnUpdate(double deltaT)
{
}

void FirestormApp::OnClose()
{
}

int FirestormApp::OnShutdown()
{
	return 0;
}

void FirestormApp::HandleApplicationWantsToClose(const ApplicationWantsToCloseEvent& event)
{
	event.App->AllowClose();
}

FIRE_RUN_APPLICATION(FirestormApp);