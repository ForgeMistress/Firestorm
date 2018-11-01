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
	virtual void OnUpdate(double deltaT) override;
	virtual void OnRender() override;
	virtual void OnClose() override;
	virtual int  OnShutdown() override;

	void HandleApplicationWantsToClose(const ApplicationWantsToCloseEvent& event);

private:
	void RegisterResourceTypes();

	IPipelineLayout* _pipelineLayout{ nullptr };
	IRenderPass* _renderPass{ nullptr };
	IPipeline* _pipeline{ nullptr };

	Resource<IShaderProgram> _shaderResource;
	Resource<IShaderProgram> _shaderResource2;

};

#endif