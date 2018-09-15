#include "stdafx.h"
#include "FirestormApp.h"

#include <LLGL/LLGL.h>
#include <LLGL/VertexAttribute.h>
#include <LLGL/VertexFormat.h>

#include <libIO/libIO.h>
#include <libIO/Logger.h>

FirestormApp::~FirestormApp()
{
	RenderMgr& renderMgr = GetRenderMgr();
	renderMgr.System->Release(*_commandBuffer);
	renderMgr.System->Release(*_vertexShader);
	renderMgr.System->Release(*_fragmentShader);
	renderMgr.System->Release(*_shader);
	renderMgr.System->Release(*_pipeline);
	renderMgr.System->Release(*_vertexBuffer);
}

void FirestormApp::OnInitialize(int ac, char** av)
{
	RenderMgr& renderMgr = GetRenderMgr();
	Dispatcher.Register(&FirestormApp::HandleApplicationWantsToClose, this);

	struct Vertex
	{
		Vector2 position;
		Vector3 color;
	};

	Vertex vertices[] = {
		{ {  0,  1 }, { 1, 0, 0 } },
		{ {  1, -1 }, { 0, 1, 0 } },
		{ { -1, -1 }, { 0, 0, 1 } }
	};

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({ "position", LLGL::Format::RG32Float  });
	vertexFormat.AppendAttribute({ "color",    LLGL::Format::RGB32Float });
	vertexFormat.stride = sizeof(Vertex);

	LLGL::BufferDescriptor vertexBufferDesc;
	{
		vertexBufferDesc.type = LLGL::BufferType::Vertex;
		vertexBufferDesc.size = sizeof(vertices);
		vertexBufferDesc.vertexBuffer.format = vertexFormat;
	}
	LLGL::RenderSystem* renderer = renderMgr.System.get();
	_vertexBuffer = renderer->CreateBuffer(vertexBufferDesc, vertices);

	String vertShaderSource = libIO::LoadFileString("/Shaders/Triangle.vert");
	FIRE_ASSERT(!vertShaderSource.empty());

	String fragShaderSource = libIO::LoadFileString("/Shaders/Triangle.frag");
	FIRE_ASSERT(!fragShaderSource.empty());

	LLGL::ShaderDescriptor vertShaderDesc;
	{
		vertShaderDesc.source = vertShaderSource.c_str();
		vertShaderDesc.sourceType = LLGL::ShaderSourceType::CodeString;
		vertShaderDesc.type = LLGL::ShaderType::Vertex;
	}
	_vertexShader = renderer->CreateShader(vertShaderDesc);

	LLGL::ShaderDescriptor fragShaderDesc;
	{
		fragShaderDesc.source = fragShaderSource.c_str();
		fragShaderDesc.sourceType = LLGL::ShaderSourceType::CodeString;
		fragShaderDesc.type = LLGL::ShaderType::Fragment;
	}
	_fragmentShader = renderer->CreateShader(fragShaderDesc);

	for (auto shader : { _vertexShader, _fragmentShader })
	{
		if (shader != nullptr)
		{
			std::string log = shader->QueryInfoLog();
			if(!log.empty())
			{
				//FIRE_LOG_ERROR(log);
			}
		}
	}

	LLGL::ShaderProgramDescriptor shaderProgramDesc;
	{
		shaderProgramDesc.vertexFormats = { vertexFormat };
		shaderProgramDesc.vertexShader = _vertexShader;
		shaderProgramDesc.fragmentShader = _fragmentShader;
	}
	_shader = renderer->CreateShaderProgram(shaderProgramDesc);

	if(_shader->HasErrors())
	{
		throw std::runtime_error(_shader->QueryInfoLog());
	}

	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.shaderProgram = _shader;
		pipelineDesc.renderPass = renderMgr.Context->GetRenderPass();
	}
	_pipeline = renderer->CreateGraphicsPipeline(pipelineDesc);

	_commandQueue = renderer->GetCommandQueue();
	FIRE_ASSERT(_commandQueue != nullptr);
	_commandBuffer = renderer->CreateCommandBuffer();
	FIRE_ASSERT(_commandBuffer != nullptr);
}

void FirestormApp::OnUpdate(double deltaT)
{
	try
	{
		RenderMgr& renderMgr = GetRenderMgr();

		_commandBuffer->Begin();
		{
			Vector2 res(GetResolution());
			_commandBuffer->SetViewport(LLGL::Viewport{
				LLGL::Offset2D{0,0},
				LLGL::Extent2D{800,600 }
			});

			_commandBuffer->SetGraphicsPipeline(*_pipeline);

			_commandBuffer->SetVertexBuffer(*_vertexBuffer);

			_commandBuffer->BeginRenderPass(*renderMgr.Context);
			{
				_commandBuffer->Clear(LLGL::ClearFlags::Color);
				_commandBuffer->Draw(3, 0);
			}
			_commandBuffer->EndRenderPass();
		}
		_commandBuffer->End();
		_commandQueue->Submit(*_commandBuffer);
	}
	catch(std::exception& e)
	{
		FIRE_LOG_ERROR(e.what());
		Close();
	}
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