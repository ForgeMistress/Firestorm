#ifndef FIRESTORM_FIRESTORMAPP_H_
#define FIRESTORM_FIRESTORMAPP_H_
#pragma once

#include <libIO/ResourceHandle.h>
#include <libCore/SOA.h>

#include <libScene/IShader.h>

using namespace Firestorm;

class FirestormApp : public Application
{
public:
	FirestormApp(std::thread::id mainThreadID);
	virtual ~FirestormApp();

	virtual void OnInitialize(int ac, char** av) override;
	virtual void OnProcess(double deltaT) override;
	virtual void OnClose() override;
	virtual int  OnShutdown() override;

	void HandleApplicationWantsToClose(const ApplicationWantsToCloseEvent& event);

private:
	void RegisterResourceTypes();

	shared_ptr<IPipelineLayout> _pipelineLayout{ nullptr };
	shared_ptr<IRenderPass>     _renderPass{ nullptr };
	shared_ptr<IPipeline>       _pipeline{ nullptr };

	Resource<IShaderProgram> _shaderResource;
};

#endif