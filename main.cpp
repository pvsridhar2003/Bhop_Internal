#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <cstdint>
#include <thread>

namespace offset
{
	constexpr std::ptrdiff_t dwLocalPlayerController = 0x181DC98;
	constexpr std::ptrdiff_t dwForceJump = 0x16CE390;

	constexpr std::ptrdiff_t m_iHealth = 0x32C;
	constexpr std::ptrdiff_t m_fFlags = 0x63;
}

void bhop(const HMODULE instance) noexcept
{
	const auto client = reinterpret_cast<std::uintptr_t>(GetModuleHandle("client.dll"));

	while (!GetAsyncKeyState(VK_END))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!GetAsyncKeyState(VK_SPACE))
			continue;

		const auto localPlayer = *reinterpret_cast<std::uintptr_t*>(client + offset::dwLocalPlayerController);

		if (!localPlayer)
			continue;

		const auto health = *reinterpret_cast<std::int32_t*>(localPlayer + offset::m_iHealth);

		if (!health)
			continue;

		const auto flags = *reinterpret_cast<std::int32_t*>(localPlayer + offset::m_fFlags);

		(flags & (1 << 0)) ?
			*reinterpret_cast<std::uintptr_t*>(client + offset::dwForceJump) = 6 :
			*reinterpret_cast<std::uintptr_t*>(client + offset::dwForceJump) = 4;
	}

	FreeLibraryAndExitThread(instance, 0);
}

int __stdcall DLLMain(
	const HMODULE instance,
	const std::uintptr_t reason,
	const void* reserved
)
{
	if (reason == 1)
	{
		DisableThreadLibraryCalls(instance);

		const auto thread = CreateThread(
			nullptr,
			0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(bhop),
			instance,
			0,
			nullptr
		);

		if (thread)
			CloseHandle(thread);
	}

	return 1;
}