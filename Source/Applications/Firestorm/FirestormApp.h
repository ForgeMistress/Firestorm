#ifndef FIRESTORM_FIRESTORMAPP_H_
#define FIRESTORM_FIRESTORMAPP_H_
#pragma once

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
	LLGL::VertexFormat _vertFormat;

	RefPtr<LLGL::Buffer> _vertexBuffer{ nullptr };

	RefPtr<LLGL::Shader> _vertexShader{ nullptr };
	RefPtr<LLGL::Shader> _fragmentShader{ nullptr };
	RefPtr<LLGL::ShaderProgram> _shader{ nullptr };
	RefPtr<LLGL::GraphicsPipeline> _pipeline{ nullptr };
	RefPtr<LLGL::CommandQueue> _commandQueue{ nullptr };
	RefPtr<LLGL::CommandBuffer> _commandBuffer{ nullptr };
};

#endif