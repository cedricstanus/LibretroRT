﻿#include "pch.h"
#include "BeetlePCFXCoreInternal.h"

#include "../LibretroRT/libretro.h"
#include "../LibretroRT/libretro_extra.h"
#include "../LibretroRT_Tools/Converter.h"

using namespace BeetlePCFXRT;
using namespace LibretroRT_Tools;

BeetlePCFXCoreInternal^ coreInstance = nullptr;

BeetlePCFXCoreInternal^ BeetlePCFXCoreInternal::Instance::get()
{
	if (coreInstance == nullptr)
	{
		coreInstance = ref new BeetlePCFXCoreInternal();

		retro_set_environment([](unsigned cmd, void* data) { return coreInstance->EnvironmentHandler(cmd, data); });
		retro_set_input_poll([]() { coreInstance->RaisePollInput(); });
		retro_set_input_state([](unsigned port, unsigned device, unsigned index, unsigned keyId) { return coreInstance->RaiseGetInputState(port, device, index, keyId); });
		retro_set_audio_sample([](int16_t left, int16_t right) { coreInstance->SingleAudioFrameHandler(left, right); });
		retro_set_audio_sample_batch([](const int16_t* data, size_t numFrames) { return coreInstance->RaiseRenderAudioFrames(data, numFrames); });
		retro_set_video_refresh([](const void *data, unsigned width, unsigned height, size_t pitch) { coreInstance->RaiseRenderVideoFrame(data, width, height, pitch); });
		retro_extra_set_open_file([](String^ filePath, FileAccessMode accessMode) { return coreInstance->OpenFileStream(filePath, accessMode); });
		retro_extra_set_close_file([](IRandomAccessStream^ stream) { coreInstance->CloseFileStream(stream); });
	}

	return coreInstance;
}

BeetlePCFXCoreInternal::BeetlePCFXCoreInternal() : LibretroRT_Tools::CoreBase(retro_init, retro_deinit,
	retro_get_system_info, retro_get_system_av_info, retro_set_controller_port_device,
	retro_load_game, retro_unload_game, retro_run, retro_reset, retro_serialize_size, retro_serialize, retro_unserialize,
	true, true, false)
{
	fileDependencies->Append(ref new FileDependency(L"pcfx.rom", L"PC-FX BIOS", L"08e36edbea28a017f79f8d4f7ff9b6d7"));
}

BeetlePCFXCoreInternal::~BeetlePCFXCoreInternal()
{
	coreInstance = nullptr;
}