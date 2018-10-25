#ifndef FIRESTORM_FIRESTORMAPP_H_
#define FIRESTORM_FIRESTORMAPP_H_
#pragma once

#include <libIO/ResourceHandle.h>
#include <libCore/SOA.h>

#include <libScene/ShaderProgram.h>

using namespace Firestorm;

class FirestormApp : public Application
{
public:
	FirestormApp(std::thread::id mainThreadID);
	virtual ~FirestormApp();

	virtual void OnInitialize(int ac, char** av) override;
	virtual void OnUpdate(double deltaT) override;
	virtual void OnRender() override;
	virtual void OnClose() override;
	virtual int  OnShutdown() override;

	void HandleApplicationWantsToClose(const ApplicationWantsToCloseEvent& event);

private:
	void RegisterResourceTypes();

	Resource<Vk_ShaderProgram> _shaderResource;
	Resource<Vk_ShaderProgram> _shaderResource2;

};

#endif