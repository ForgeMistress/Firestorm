#include "stdafx.h"
#include "FirestormApp.h"

#include <libCore/Logger.h>

#include <LLGL/LLGL.h>
#include <LLGL/VertexAttribute.h>
#include <LLGL/VertexFormat.h>

#include <libIO/libIO.h>
#include <libIO/ResourceMgr.h>

#include <libScene/ShaderResource.h>
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

	auto& resourceMgr = GetSystems().GetResourceMgr();

	ResourceReference shaderRef("/Shaders/Triangle.shader");
	ResourceReference meshRef("/Models/base-female.gltf");

	_shaderResource = resourceMgr.Load<ShaderResource>(shaderRef);
	_meshResource = resourceMgr.Load<SceneGraphResource>(meshRef);

	while(true)
	{
		if(_shaderResource.valid() && _meshResource.valid())
		{
			auto shaderStatus = _shaderResource.wait_for(std::chrono::milliseconds(0));
			bool shaderFinished = shaderStatus == std::future_status::ready;
			auto meshStatus = _meshResource.wait_for(std::chrono::milliseconds(0));
			bool meshFinished = meshStatus == std::future_status::ready;
			if(shaderFinished && meshFinished)
				break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	auto meshResult = _meshResource.get();
	auto shaderResult = _shaderResource.get();

	if(shaderResult.HasError())
	{
		FIRE_LOG_ERROR("Error Loading Shader: %s", (String)shaderResult.GetError());
		FIRE_ASSERT(false);
	}

	if(meshResult.HasError())
	{
		FIRE_LOG_ERROR("Error Loading Mesh: %s", (String)meshResult.GetError());
		FIRE_ASSERT(false);
	}

	//auto res = _shaderResource->Get<ShaderResource>();
	auto resource = shaderResult.GetResource<ShaderResource>();
	bool result = resource->Compile();
	FIRE_ASSERT_MSG(result, "shader failed to compile");

	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.shaderProgram = resource->GetProgram();//_shaderResource->Get<ShaderResource>()->GetProgram();
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
		if(_commandBuffer == nullptr)
			return;

		auto& renderMgr = GetSystems().GetRenderMgr();

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

void FirestormApp::RegisterResourceTypes()
{
	// Register the resource types.
	auto& systems = GetSystems();
	auto& renderMgr = systems.GetRenderMgr();
	auto& resourceMgr = systems.GetResourceMgr();

	resourceMgr.InstallLoader<ShaderResource>(renderMgr);
	resourceMgr.InstallLoader<MeshResource>(renderMgr);
	resourceMgr.InstallLoader<SceneGraphResource>(renderMgr);
}