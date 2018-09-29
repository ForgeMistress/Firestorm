#ifndef FIRESTORM_FIRESTORMAPP_H_
#define FIRESTORM_FIRESTORMAPP_H_
#pragma once

#include <libScene/ShaderResource.h>

using namespace Firestorm;

class FirestormApp : public Application
{
public:
	FirestormApp(std::thread::id mainThreadID);
	virtual ~FirestormApp();

	virtual void OnInitialize(int ac, char** av);
	virtual void OnUpdate(double deltaT);
	virtual void OnClose();
	virtual int  OnShutdown();

	void HandleApplicationWantsToClose(const ApplicationWantsToCloseEvent& event);

private:
	void RegisterResourceTypes();

	LLGL::VertexFormat _vertFormat;

	LLGL::Buffer*                      _vertexBuffer{ nullptr };
	Future<ResourceLoader::LoadResult> _shaderResource;
	Future<ResourceLoader::LoadResult> _meshResource;
	LLGL::GraphicsPipeline*            _pipeline{ nullptr };
	LLGL::CommandQueue*                _commandQueue{ nullptr };
	LLGL::CommandBuffer*               _commandBuffer{ nullptr };
};

#endif