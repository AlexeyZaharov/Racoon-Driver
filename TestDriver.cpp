#include "windows.h"
#include <iostream>

#include "TestDriver.h"

void CommunicateWithDriver()
{
    HANDLE hFile;
    DWORD dwReturn;

    hFile = CreateFile(L"\\\\.\\Example",
        GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL);

    if (hFile)
    {
        bool stop = false;
        while (!stop)
        {
            char choice;
            std::cout << "1 - Write; 2 - Read; 3 - ioctl write; 4 - ioctl read; 5 - Naruto paschal; something else to quit";
            std::cin >> choice;

            switch(choice)
            {
            case '1':
            {
                WriteFile(hFile, "Hello from user mode!", sizeof("Hello from user mode!"), &dwReturn, NULL);
                break;
            }
            case '2':
            {
                char buffer[1024];
                ZeroMemory(buffer, sizeof(buffer));
                ReadFile(hFile, buffer, sizeof(buffer), &dwReturn, NULL);
                std::cout << buffer << std::endl;
                break;
            }
            case '3':
            {
                char buffer[1024];
                ZeroMemory(buffer, sizeof(buffer));
                DeviceIoControl(hFile,
                    IOCTL_EXAMPLE_SAMPLE_DIRECT_IN_IO,
                    PVOID("** Hello from User Mode Direct IN I/O"),
                    sizeof("** Hello from User Mode Direct IN I/O"),
                    NULL,
                    0,
                    &dwReturn,
                    NULL);
                break;
            }
            case '4':
            {
                char buffer[1024];
                ZeroMemory(buffer, sizeof(buffer));
                DeviceIoControl(hFile,
                    IOCTL_EXAMPLE_SAMPLE_DIRECT_OUT_IO,
                    NULL,
                    0,
                    buffer,
                    sizeof(buffer),
                    &dwReturn,
                    NULL);

                std::cout << buffer << std::endl;
                break;
            }
            case '5':
            {
                char buffer[1024];
                ZeroMemory(buffer, sizeof(buffer));
                DeviceIoControl(hFile,
                    IOCTL_EXAMPLE_SAMPLE_DIRECT_OUT_IO,
                    NULL,
                    0,
                    buffer,
                    sizeof(buffer),
                    &dwReturn,
                    NULL);

                std::cout << buffer << std::endl;
                break;
            }
            default:
            {
                stop = true;
            }
            }
        }
    }
    CloseHandle(hFile);
}

int _cdecl main(void)
{
    std::cout << "Load Driver" << std::endl;

    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (hSCManager)
    {
        std::cout << "Create Service" << std::endl;

        SERVICE_STATUS ss;
        SC_HANDLE hService = CreateService(hSCManager, L"Example",
            L"Example Driver",
            SERVICE_START | DELETE | SERVICE_STOP,
            SERVICE_KERNEL_DRIVER,
            SERVICE_DEMAND_START,
            SERVICE_ERROR_IGNORE,
            TEST_DRIVER_PATH,
            NULL, NULL, NULL, NULL, NULL);

        if (!hService)
            hService = OpenService(hSCManager, L"Example", SERVICE_START | DELETE | SERVICE_STOP);

        if (hService)
        {
            std::cout << "Start Service" << std::endl;

            StartService(hService, 0, NULL);

            CommunicateWithDriver();

            ControlService(hService, SERVICE_CONTROL_STOP, &ss);
            DeleteService(hService);
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCManager);
    }

    return 0;
}