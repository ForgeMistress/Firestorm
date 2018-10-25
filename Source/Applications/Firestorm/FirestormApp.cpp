#include "stdafx.h"
#include "FirestormApp.h"

#include <libCore/Logger.h>

#include <libIO/libIO.h>
#include <libIO/ResourceMgr.h>

#include <libScene/ShaderProgram.h>
#include <libScene/MeshResource.h>
#include <libScene/SceneGraphResource.h>

#include <string>
#include <variant>

FirestormApp::FirestormApp(std::thread::id mainThreadID)
: Application(mainThreadID)
{
}

FirestormApp::~FirestormApp()
{
}

void FirestormApp::OnInitialize(int ac, char** av)
{
	RegisterResourceTypes();

	FIRE_ASSERT(libIO::FileExists("/Shaders/Triangle.shader"));
	FIRE_ASSERT(libIO::FileExists("/Models/base-female.gltf"));

	RenderMgr& renderMgr = GetSystems().GetRenderMgr();
	ResourceMgr& resourceMgr = GetSystems().GetResourceMgr();

	Dispatcher.Register<ApplicationWantsToCloseEvent>([this](const ApplicationWantsToCloseEvent& e) {
		HandleApplicationWantsToClose(e);
	});

	_shaderResource = resourceMgr.Load<Vk_ShaderProgram>("/Shaders/Triangle.shader");
	_shaderResource2 = resourceMgr.Load<Vk_ShaderProgram>("/Shaders/Triangle.shader");

	IPipelineLayout::CreateInfo layoutInfo{};
	IPipelineLayout* layout = renderMgr.GetSystem().Make(layoutInfo);

	IRenderPass::CreateInfo renderPassInfo(renderMgr.GetSystem());

	IRenderPass::CreateInfo::AttachmentDescription attachDesc{};
	attachDesc.Flags = static_cast<AttachmentDescriptionFlags>(0);
	attachDesc.Format = renderMgr.GetSystem().GetSwapchainImageFormat();
	attachDesc.FinalLayout = ImageLayout::kPRESENT_SRC;
	renderPassInfo.AddAttachmentDesc(attachDesc);

	IRenderPass::CreateInfo::AttachmentReference colorRef;
	colorRef.Attachment = 0;
	colorRef.Layout = ImageLayout::kCOLOR_ATTACHMENT_OPTIMAL;

	IRenderPass::CreateInfo::SubpassDescription subpass;
	subpass.ColorAttachments.push_back(colorRef);

	renderPassInfo.AddSubpass(subpass);

	IRenderPass* renderPass = renderMgr.GetSystem().Make(renderPassInfo);

	IPipeline::CreateInfo pipelineInfo{ renderMgr.GetSystem() };
	pipelineInfo.ShaderStage = _shaderResource;
	pipelineInfo.Layout = layout;
	pipelineInfo.RenderPass = renderPass;
	IPipeline* pipeline = renderMgr.GetSystem().Make(pipelineInfo);

	FIRE_LOG_DEBUG("+++++ SIZEOF tf::Node: %d", sizeof(tf::Node));

	using StaticWork   = std::function<void()>;
	using DynamicWork  = std::function<void(tf::SubflowBuilder&)>;
	using Graph = std::forward_list<tf::Node>;
	using tfNodeVariant = std::variant<StaticWork, DynamicWork>;
	union WorkUnion
	{
		StaticWork _static;
		DynamicWork _dynamic;
	};

#define printsize(tag, type) FIRE_LOG_DEBUG("+++++    SIZEOF "#tag" ("#type"): %d", sizeof(type));

	printsize(_name, std::string);
	printsize(_work, tfNodeVariant);
	printsize(_workUnion, WorkUnion);
	printsize(_successors, std::vector<tf::Node*>);
	printsize(_dependents, std::atomic<int>);
	printsize(_dependents_SMALLER, std::atomic<uint8_t>);
	printsize(_subgraph, std::optional<Graph>);
	printsize(_subgraphRaw, Graph);
	printsize(_topology, tf::Topology*);

	//for(size_t i=0; i<1000; ++i)
	//{
	//	resourceMgr.QueueLoad<ShaderProgram>("/Shaders/Triangle.shader");
	//}
	// while(true)
	// {
	// 	FIRE_LOG_DEBUG("?? Waiting for shader resource to finish on main thread...");
	// 	if(_shaderResource->IsFinished())
	// 		break;
	// }

	/*struct Vertex
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
	ResourceReference sceneRef("/Models/base-female.gltf");

	_shaderResource = resourceMgr.Load<ShaderProgramResource>(shaderRef);
	_sceneGraphResource = resourceMgr.Load<SceneGraphResource>(sceneRef);

	while(true)
	{
		if(_shaderResource.IsFinished() && _sceneGraphResource.IsFinished())
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	FIRE_ASSERT_MSG(!_shaderResource.HasError(), 
		Format("Error Loading Shader: %s", _shaderResource.GetError().Format()));
	FIRE_ASSERT_MSG(!_sceneGraphResource.HasError(), 
		Format("Error Loading Mesh: %s", _sceneGraphResource.GetError().Format()));

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
	FIRE_ASSERT(_commandBuffer != nullptr);*/
}

void FirestormApp::OnUpdate(double deltaT)
{
	static bool message = false;
	if(message == false)
	{
		if(_shaderResource.IsFinished())
		{
			message = true;
			FIRE_LOG_DEBUG("!! Finished loading shader: '/Shaders/Triangle.shader'");
		}
	}

	static bool message2 = false;
	if(message2 == false)
	{
		if(_shaderResource2.IsFinished())
		{
			message2 = true;
			FIRE_LOG_DEBUG("!! Finished loading cached shader: '/Shaders/Triangle.shader'");
		}
	}
	if(_shaderResource.IsFinished() && _shaderResource2.IsFinished())
	{
		FIRE_ASSERT_MSG(_shaderResource.get() == _shaderResource2.get(), "the two resources were not the same pointer");
		//_shaderResource->PrintNumShaders();
	}
}

void FirestormApp::OnRender()
{
	/*try
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
	}*/
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
	// auto& systems = GetSystems();
	// auto& renderMgr = systems.GetRenderMgr();
	// auto& resourceMgr = systems.GetResourceMgr();

	// resourceMgr.InstallLoader<ShaderProgramResource>(renderMgr);
	// resourceMgr.InstallLoader<MeshResource>(renderMgr);
	// resourceMgr.InstallLoader<SceneGraphResource>(renderMgr);
}