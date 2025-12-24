#ifndef DENOISER_H
#define DENOISER_H

#include "OpenImageDenoise/oidn.h"

// OIDN denoising process
void denoise(std::vector<float>& color_buffer, std::vector<float>& albedo_buffer, std::vector<float>& normal_buffer, int image_width, int image_height) {
	OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
	oidnCommitDevice(device);

	OIDNFilter filter = oidnNewFilter(device, "RT");

	oidnSetSharedFilterImage(filter, "color", color_buffer.data(), OIDN_FORMAT_FLOAT3, image_width, image_height, 0, 0, 0);
	oidnSetSharedFilterImage(filter, "output", color_buffer.data(), OIDN_FORMAT_FLOAT3, image_width, image_height, 0, 0, 0);
	oidnSetSharedFilterImage(filter, "albedo", albedo_buffer.data(), OIDN_FORMAT_FLOAT3, image_width, image_height, 0, 0, 0);
	oidnSetSharedFilterImage(filter, "normal", normal_buffer.data(), OIDN_FORMAT_FLOAT3, image_width, image_height, 0, 0, 0);

	oidnCommitFilter(filter);
	oidnExecuteFilter(filter);

	const char* error;
	if (oidnGetDeviceError(device, &error) != OIDN_ERROR_NONE)
		std::cerr << "Denoiser Error: " << error << std::endl;
	else
		std::clog << "image.png denoised" << std::endl;

	oidnReleaseFilter(filter);
	oidnReleaseDevice(device);
}

#endif