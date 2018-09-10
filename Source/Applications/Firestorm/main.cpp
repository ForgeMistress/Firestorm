#include "stdafx.h"

#include <libApp/libApp.h>
#include <libCore/libCore.h>
#include <libExistence/libExistence.h>
#include <libIO/libIO.h>
#include <libJson/libJson.h>
#include <libMath/libMath.h>
#include <libMirror/libMirror.h>
#include <libScene/libScene.h>
#include <libScript/libScript.h>
#include <libSerial/libSerial.h>
#include <libUI/libUI.h>

#include <libIO/Logger.h>
#include <libIO/FileIOMgr.h>

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
	FileIOMgr _fileIOMgr;
};

void FirestormApp::OnInitialize(int ac, char** av)
{
	Dispatcher.Register(&FirestormApp::HandleApplicationWantsToClose, this);
}

void FirestormApp::OnUpdate(double deltaT)
{
	_fileIOMgr.ProcessQueues();
}

void FirestormApp::OnClose()
{
	_fileIOMgr.Shutdown();
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