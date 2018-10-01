#include "stdafx.h"
#include "FirestormApp.h"

#include <libCore/Logger.h>

#include <LLGL/LLGL.h>
#include <LLGL/VertexAttribute.h>
#include <LLGL/VertexFormat.h>

#include <libIO/libIO.h>
#include <libIO/ResourceMgr.h>

#include <libScene/ShaderProgramResource.h>
#include <libScene/MeshResource.h>
#include <libScene/SceneGraphResource.h>

FirestormApp::FirestormApp(std::thread::id mainThreadID)
: Application(mainThreadID)
{
}

FirestormApp::~FirestormApp()
{
	RenderMgr& renderMgr = GetSystems().GetRenderMgr();
	renderMgr.System->Release(*_commandBuffer);
	renderMgr.System->Release(*_pipeline);
	renderMgr.System->Release(*_vertexBuffer);
}

void FirestormApp::OnInitialize(int ac, char** av)
{
	RegisterResourceTypes();

	EnableWindowResizing(true);	

	FIRE_ASSERT(libIO::FileExists("/Shaders/Triangle.shader"));
	FIRE_ASSERT(libIO::FileExists("/Models/base-female.gltf"));

	RenderMgr& renderMgr = GetSystems().GetRenderMgr();
	Dispatcher.Register(&FirestormApp::HandleApplicationWantsToClose, this);

	struct Vertex
	{
		Vector2 position;
		Vector4 color;
	};

	const float s = 0.5f;
	Vertex vertices[] = {
		{ {  0,  s }, { 1, 0, 0, 1 } },
		{ {  s, -s }, { 0, 1, 0, 1 } },
		{ { -s, -s }, { 0, 0, 1, 1 } }
	};

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({ "position", LLGL::Format::RG32Float  });
	vertexFormat.AppendAttribute({ "color",    LLGL::Format::RGBA32Float });
	vertexFormat.stride = sizeof(Vertex);

	LLGL::BufferDescriptor vertexBufferDesc;
	{
		vertexBufferDesc.type = LLGL::BufferType::Vertex;
		vertexBufferDesc.size = sizeof(vertices);
		vertexBufferDesc.vertexBuffer.format = vertexFormat;
	}
	_vertexBuffer = renderMgr.System->CreateBuffer(vertexBufferDesc, vertices);

	auto& resourceMgr = GetSystems().GetResourceMgr();

	ResourceReference shaderRef("/Shaders/Triangle.shader");
	ResourceReference meshRef("/Models/female-base.gltf");

	_shaderResource = resourceMgr.Load<ShaderProgramResource>(shaderRef);
	_meshResource = resourceMgr.Load<MeshResource>(meshRef);

	while(true)
	{
		if(_shaderResource.IsFinished() && _meshResource.IsFinished())
		{
			/*auto shaderStatus = _shaderResource.wait_for(std::chrono::milliseconds(0));
			bool shaderFinished = shaderStatus == std::future_status::ready;
			if(shaderFinished)
				break;*/
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	/*if(_shaderResource.HasError())
	{
		FIRE_LOG_ERROR("Error Loading Shader: %s", (String)_shaderResource.GetError());
		FIRE_ASSERT(false);
	}*/

	FIRE_ASSERT_MSG(!_shaderResource.HasError(), Format("Error Loading Shader: %s", (String)_shaderResource.GetError()));
	FIRE_ASSERT_MSG(!_meshResource.HasError(), Format("Error Loading Mesh: %s", (String)_meshResource.GetError()));
	/*if(_meshResource.HasError())
	{
		FIRE_LOG_ERROR("Error Loading Shader: %s", (String)_shaderResource.GetError());
		FIRE_ASSERT(false);
	}*/

	auto resource = _shaderResource.Get<ShaderProgramResource>();
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.shaderProgram = resource->Compile({vertexFormat});
		pipelineDesc.renderPass = renderMgr.Context->GetRenderPass();
	}
	_pipeline = renderMgr.System->CreateGraphicsPipeline(pipelineDesc);

	_commandQueue = renderMgr.System->GetCommandQueue();
	FIRE_ASSERT(_commandQueue != nullptr);

	_commandBuffer = renderMgr.System->CreateCommandBuffer();
	FIRE_ASSERT(_commandBuffer != nullptr);
}

void FirestormApp::OnRender()
{
	try
	{
		if(_commandBuffer == nullptr)
			return;

		auto& renderMgr = GetSystems().GetRenderMgr();

		_commandBuffer->Begin();
		{
			Vector2 res(GetResolution());
			_commandBuffer->SetViewport(LLGL::Viewport{
				LLGL::Offset2D{0,0},
				LLGL::Extent2D{800,600}
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

void FirestormApp::RegisterResourceTypes()
{
	// Register the resource types.
	auto& systems = GetSystems();
	auto& renderMgr = systems.GetRenderMgr();
	auto& resourceMgr = systems.GetResourceMgr();

	resourceMgr.InstallLoader<ShaderProgramResource>(renderMgr);
	resourceMgr.InstallLoader<MeshResource>(renderMgr);
	resourceMgr.InstallLoader<SceneGraphResource>(renderMgr);
}