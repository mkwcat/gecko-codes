#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <windows.h>

#include <processthreadsapi.h>
#include <winuser.h>

static HANDLE g_process = nullptr;

DWORD readU32(size_t address)
{
    DWORD value = 0;
    if (!ReadProcessMemory(
            g_process, (LPCVOID) address, &value, sizeof(DWORD), nullptr
        )) {
        std::printf("Failed to read memory at 0x%08X\n", address);
        std::exit(1);
    }
    return __builtin_bswap32(value);
}

DWORD writeU8(size_t address, BYTE value)
{
    if (!WriteProcessMemory(
            g_process, (LPVOID) address, &value, sizeof(BYTE), nullptr
        )) {
        std::printf("Failed to write memory at 0x%08X\n", address);
        std::exit(1);
    }
    return value;
}

DWORD writeU32(size_t address, DWORD value)
{
    value = __builtin_bswap32(value);

    if (!WriteProcessMemory(
            g_process, (LPVOID) address, &value, sizeof(DWORD), nullptr
        )) {
        std::printf("Failed to write memory at 0x%08X\n", address);
        std::exit(1);
    }
    return value;
}

size_t SearchForStartGameInfo()
{
    // Search and return first match of 09 86 54 45 24 2F B6 7F 21 31 DA 7C, 4
    // byte aligned

    MEMORY_BASIC_INFORMATION mbi;
    size_t address = 0;
    while (VirtualQueryEx(g_process, (LPCVOID) address, &mbi, sizeof(mbi)) ==
           sizeof(mbi)) {
        if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE) {
            BYTE* buffer = new BYTE[mbi.RegionSize];
            SIZE_T bytesRead;
            if (ReadProcessMemory(
                    g_process, mbi.BaseAddress, buffer, mbi.RegionSize,
                    &bytesRead
                )) {
                for (size_t i = 0; i < mbi.RegionSize - 12; i += 4) {
                    if (buffer[i + 0] == 0x09 && buffer[i + 1] == 0x86 &&
                        buffer[i + 2] == 0x54 && buffer[i + 3] == 0x45 &&
                        buffer[i + 4] == 0x24 && buffer[i + 5] == 0x2F &&
                        buffer[i + 6] == 0xB6 && buffer[i + 7] == 0x7F &&
                        buffer[i + 8] == 0x21 && buffer[i + 9] == 0x31 &&
                        buffer[i + 10] == 0xDA && buffer[i + 11] == 0x7C) {
                        delete[] buffer;
                        return (size_t) mbi.BaseAddress + i + 0xC;
                    }
                }
            }
            delete[] buffer;
        }
        address += mbi.RegionSize;
    }

    return 0;
}

int main(int argc, char** argv)
{
    const char* programName = "nsmbu-load";
    if (argc >= 1 && argv != nullptr && argv[0] != nullptr) {
        programName = argv[0];
    }

    if (argc < 2) {
        printf("Usage: %s <course e.g. 1-2> [area] [entrance]\n", programName);
        return EXIT_FAILURE;
    }

    const char* course = argv[1];
    unsigned int world = 0;
    unsigned int level = 0;
    if (std::sscanf(course, "%u-%u", &world, &level) != 2) {
        std::printf("Invalid course: %s\n", course);
        return EXIT_FAILURE;
    }

    std::printf("Loading course %u-%u\n", world, level);
    world -= 1;
    level -= 1;

    unsigned int area = 0;
    if (argc >= 3) {
        if (std::sscanf(argv[2], "%u", &area) != 1) {
            std::printf("Invalid area: %s\n", argv[2]);
            return EXIT_FAILURE;
        }

        if (area == 0 || area > 4) {
            std::printf("Invalid area: %u\n", area);
            return EXIT_FAILURE;
        }

        area -= 1;
    }

    unsigned int entrance = 255;
    if (argc >= 4) {
        if (std::sscanf(argv[3], "%u", &entrance) != 1) {
            std::printf("Invalid entrance: %s\n", argv[3]);
            return EXIT_FAILURE;
        }

        if (entrance > 255) {
            std::printf("Invalid entrance: %u\n", entrance);
            return EXIT_FAILURE;
        }
    }

    // Search for all windows starting with "Cemu"
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExA(nullptr, hwnd, nullptr, nullptr)) != nullptr) {
        char windowTitle[256];
        GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
        if (std::strncmp(windowTitle, "Cemu", 4) == 0) {
            break;
        }
    }

    if (hwnd == nullptr) {
        printf("Could not find Cemu window!\n");
        return EXIT_FAILURE;
    }

    // Get the process ID
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == 0) {
        std::printf("Failed to get the process ID!\n");
        return 1;
    }

    // Open the process
    g_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (g_process == nullptr) {
        std::printf("Failed to open the process!\n");
        return 1;
    }

    std::printf("Opened process with PID: %u\n", pid);

    size_t startGameInfo = SearchForStartGameInfo();
    if (startGameInfo == 0) {
        std::printf("Failed to find StartGameInfo!\n");
        return 1;
    }

    std::printf("Found StartGameInfo at 0x%08X\n", startGameInfo);

    // Write the course data
    writeU32(startGameInfo + 0x4, world);
    writeU32(startGameInfo + 0x8, level);
    writeU8(startGameInfo + 0x2, area);
    writeU8(startGameInfo + 0x3, entrance);
    writeU8(startGameInfo, 1); // Trigger load

    // Close the process
    CloseHandle(g_process);

    return 0;
}