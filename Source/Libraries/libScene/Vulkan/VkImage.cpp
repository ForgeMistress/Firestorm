#include "stdafx.h"
#include "VkImage.h"

OPEN_NAMESPACE(Firestorm);

Vk_Image::Vk_Image(class RenderSystem& renderSystem)
: _renderSystem(renderSystem)
{
}

Vk_Image::~Vk_Image()
{
}

CLOSE_NAMESPACE(Firestorm);