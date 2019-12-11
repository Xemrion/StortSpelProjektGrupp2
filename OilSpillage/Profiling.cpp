#include <d3d11_4.h>
#include <dxgi1_6.h>

#pragma comment(lib, "dxgi.lib")

#include <psapi.h>
#include "Profiling.h"

std::string vramUsage(bool messageBox)
{
	IDXGIFactory* dxgifactory = nullptr;
	HRESULT ret_code = ::CreateDXGIFactory(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(&dxgifactory));

	char msg[100] = { 0 };
	if (SUCCEEDED(ret_code))
	{
		IDXGIAdapter* dxgiAdapter = nullptr;

		if (SUCCEEDED(dxgifactory->EnumAdapters(0, &dxgiAdapter)))
		{
			IDXGIAdapter4* dxgiAdapter4 = NULL;
			if (SUCCEEDED(dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter4), (void**)& dxgiAdapter4)))
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO info;

				if (SUCCEEDED(dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
				{
					float memoryUsage = float(info.CurrentUsage / 1024.0 / 1024.0); //MiB

					sprintf_s(msg, "%.2f MiB VRAM used", memoryUsage);
					if (messageBox)
						MessageBoxA(0, msg, "VRAM", 0);
				};

				dxgiAdapter4->Release();
			}
			dxgiAdapter->Release();
		}
		dxgifactory->Release();
	}

	return std::string(msg);
}

std::string ramUsage(bool messageBox)
{
	//src: https://docs.microsoft.com/en-us/windows/desktop/api/psapi/ns-psapi-_process_memory_counters

	DWORD currentProcessID = GetCurrentProcessId();

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, currentProcessID);
	char msg[100] = { 0 };

	PROCESS_MEMORY_COUNTERS pmc{};
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		//PagefileUsage is the:
			//The Commit Charge value in bytes for this process.
			//Commit Charge is the total amount of memory that the memory manager has committed for a running process.

		float memoryUsage = float(pmc.PagefileUsage / 1024.0 / 1024.0); //MiB

		sprintf_s(msg, "%.2f MiB committed RAM", memoryUsage);
		if (messageBox)
			MessageBoxA(0, msg, "RAM", 0);
	}

	CloseHandle(hProcess);
	return std::string(msg);
}