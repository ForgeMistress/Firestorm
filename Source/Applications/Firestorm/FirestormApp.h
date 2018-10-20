#ifndef FIRESTORM_FIRESTORMAPP_H_
#define FIRESTORM_FIRESTORMAPP_H_
#pragma once

#include <libIO/ResourceHandle.h>
#include <libCore/SOA.h>

using namespace Firestorm;

class FirestormApp : public Application
{
public:
	FirestormApp(std::thread::id mainThreadID);
	virtual ~FirestormApp();

	virtual void OnInitialize(int ac, char** av);
	virtual void OnUpdate(double deltaT);
	virtual void OnRender();
	virtual void OnClose();
	virtual int  OnShutdown();

	void HandleApplicationWantsToClose(const ApplicationWantsToCloseEvent& event);

private:
	void RegisterResourceTypes();

	Resource _shaderResource;
};

#endif