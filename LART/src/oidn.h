// 假设你已经将图像渲染到了 std::vector<float> color, albedo, normal 中
// 每个 vector 的大小是 width * height * 3 (RGB)

OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
oidnCommitDevice(device);

OIDNFilter filter = oidnNewFilter(device, “RT”); // 使用 RT 过滤器

// 设置颜色缓冲区（输入和输出可以是同一个）
oidnSetSharedFilterImage(filter, “color”, color.data(), OIDN_FORMAT_FLOAT3, width, height, 0, 0, 0);
oidnSetSharedFilterImage(filter, “output”, color.data(), OIDN_FORMAT_FLOAT3, width, height, 0, 0, 0);
// 设置辅助缓冲区
oidnSetSharedFilterImage(filter, “albedo”, albedo.data(), OIDN_FORMAT_FLOAT3, width, height, 0, 0, 0);
oidnSetSharedFilterImage(filter, “normal”, normal.data(), OIDN_FORMAT_FLOAT3, width, height, 0, 0, 0);

oidnCommitFilter(filter);
oidnExecuteFilter(filter);

// 检查错误
const char* error;
if (oidnGetDeviceError(device, &error) != OIDN_ERROR_NONE)
std::cerr << “Denoiser Error : “ << error << std::endl;

// 清理
oidnReleaseFilter(filter);
oidnReleaseDevice(device);

// 此时 color 中的数据已经被降噪后的结果覆盖
给出完整头文件