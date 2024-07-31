// This tool is not meant to be used on its own
// But the game patch is too complicated to explain

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
    // return __builtin_bswap32(value);
    return value;
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
    // value = __builtin_bswap32(value);

    if (!WriteProcessMemory(
            g_process, (LPVOID) address, &value, sizeof(DWORD), nullptr
        )) {
        std::printf("Failed to write memory at 0x%08X\n", address);
        std::exit(1);
    }
    return value;
}

static BYTE s_originalCode[] = {0x95, 0xB5, 0xE0, 0xEB, 0xBC, 0x00, 0x9F, 0xE5};

static BYTE s_patchedCode[] = {0x95, 0xB5, 0xE0, 0xEB, 0x0F, 0x40, 0xA0,
                               0xE1, 0x10, 0x00, 0x00, 0xEA, 0x04, 0x10,
                               0x9F, 0xE5, 0x04, 0x20, 0x9F, 0xE5};

// void patchCode(size_t address)
// {
//     if (!WriteProcessMemory(
//             g_process, (LPVOID) address, s_patchedCode,
//             sizeof(s_patchedCode), nullptr
//         )) {
//         std::printf("Failed to write memory at 0x%08X\n", address);
//         std::exit(1);
//     }
// }

size_t SearchForStartGameFunction(bool ryujinx)
{
    MEMORY_BASIC_INFORMATION mbi;
    size_t address = 0;
    while (VirtualQueryEx(g_process, (LPCVOID) address, &mbi, sizeof(mbi)) ==
           sizeof(mbi)) {
        // std::printf("mem region: 0x%llX    0x%llX\n", mbi.BaseAddress,
        // mbi.RegionSize);

        if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE &&
                (ryujinx && mbi.RegionSize == 0x1700000) ||
            (!ryujinx && mbi.RegionSize == 0x100000000)) {
            // Found RAM region, search for the pattern using a sliding window
            BYTE* buffer = new BYTE[sizeof(s_originalCode)];
            SIZE_T bytesRead;

            for (size_t i = 0x654; i + sizeof(s_originalCode) < mbi.RegionSize;
                 i += 0x1000) {
                size_t baseAddr = size_t(mbi.BaseAddress) + i;

                if (!ReadProcessMemory(
                        g_process, (LPCVOID) baseAddr, buffer,
                        sizeof(s_originalCode), &bytesRead
                    )) {
                    continue;
                }

                // if (std::memcmp(buffer, s_originalCode,
                // sizeof(s_originalCode)) == 0) {
                //     delete[] buffer;
                //     patchCode(baseAddr);
                //     return baseAddr;
                // }

                if (std::memcmp(
                        buffer, s_patchedCode, sizeof(s_originalCode)
                    ) == 0) {
                    delete[] buffer;
                    return baseAddr;
                }
            }
        }
        address += mbi.RegionSize;
    }

    return 0;
}

int main(int argc, char** argv)
{
    const char* programName = "nsmbudx-load";
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

    // Search for all windows starting with "Ryujinx "
    HWND hwnd = nullptr;
    bool ryujinx = false;
    while ((hwnd = FindWindowExA(nullptr, hwnd, nullptr, nullptr)) != nullptr) {
        char windowTitle[256];
        GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
        if (std::strncmp(windowTitle, "Ryujinx ", 5) == 0) {
            // Check if the window contains "New Super Mario Bros. U Deluxe"
            if (std::strstr(windowTitle, "New Super Mario Bros. U Deluxe") !=
                nullptr) {
                ryujinx = true;
                break;
            }
        }

        if (std::strncmp(windowTitle, "yuzu ", 5) == 0) {
            // Check if the window contains "New Super Mario Bros. U Deluxe"
            if (std::strstr(windowTitle, "New Super Mario Bros. U Deluxe") !=
                nullptr) {
                break;
            }
        }
    }

    if (hwnd == nullptr) {
        printf("Could not find yuzu/ryujinx window!\n");
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

    size_t startGameInfo = SearchForStartGameFunction(ryujinx);
    if (startGameInfo == 0) {
        std::printf("Failed to find StartGameFunction!\n");
        return 1;
    }

    std::printf("Found StartGameInfo at 0x%08llX\n", startGameInfo);

    // Write the course data
    // writeU32(startGameInfo + 0x4, world);
    // writeU32(startGameInfo + 0x8, level);
    // writeU8(startGameInfo + 0x2, area);
    // writeU8(startGameInfo + 0x3, entrance);
    // writeU8(startGameInfo, 1); // Trigger load

    writeU32(startGameInfo + 0xC + 0xC, world);
    writeU32(startGameInfo + 0xC + 0x10, level);
    writeU8(startGameInfo + 0xC + 0x16, area);
    writeU8(startGameInfo + 0xC + 0x17, entrance);

    // Close the process
    CloseHandle(g_process);

    return 0;
}