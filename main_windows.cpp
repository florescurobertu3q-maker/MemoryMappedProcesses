
#include <windows.h>
#include <iostream>
#include <random>
#include <string>

const char* MEMORY_NAME = "Local\\PrimeLabMemory";
const char* SEMAPHORE_NAME = "Local\\PrimeLabSemaphore";

int main()
{
    // Cream memoria partajata
    HANDLE hMap = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        nullptr,
        PAGE_READWRITE,
        0,
        sizeof(int),
        MEMORY_NAME
    );

    if (hMap == nullptr)
    {
        std::cerr << "Eroare la crearea memoriei partajate.\n";
        return 1;
    }

    // Accesam memoria partajata
    int* sharedNumber = (int*)MapViewOfFile(
        hMap,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(int)
    );

    if (sharedNumber == nullptr)
    {
        std::cerr << "Eroare la maparea memoriei.\n";
        CloseHandle(hMap);
        return 1;
    }

    // Cream semaforul. Primul proces il initializeaza.
    HANDLE hSemaphore = CreateSemaphoreA(
        nullptr,
        0,
        1,
        SEMAPHORE_NAME
    );

    if (hSemaphore == nullptr)
    {
        std::cerr << "Eroare la crearea semaforului.\n";
        UnmapViewOfFile(sharedNumber);
        CloseHandle(hMap);
        return 1;
    }

    bool firstProcess = (GetLastError() != ERROR_ALREADY_EXISTS);

    // Primul proces initializeaza memoria cu 0.
    if (firstProcess)
    {
        *sharedNumber = 0;
        ReleaseSemaphore(hSemaphore, 1, nullptr);
    }

    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> coin(1, 2);

    DWORD processId = GetCurrentProcessId();

    std::cout << "Procesul " << processId << " a pornit.\n";

    while (true)
    {
        // Rezervam semaforul
        WaitForSingleObject(hSemaphore, INFINITE);

        // Citim numarul din memoria partajata
        int number = *sharedNumber;

        std::cout << "Procesul " << processId
                  << " citeste: " << number << "\n";

        if (number >= 1000)
        {
            ReleaseSemaphore(hSemaphore, 1, nullptr);
            break;
        }

        // Aruncam moneda cat timp rezultatul este 2
        while (coin(generator) == 2 && number < 1000)
        {
            number++;
            *sharedNumber = number;

            std::cout << "Procesul " << processId
                      << " scrie: " << number << "\n";
        }

        // Eliberam semaforul
        ReleaseSemaphore(hSemaphore, 1, nullptr);

        // Oferim si celuilalt proces sansa sa execute
        Sleep(50);
    }

    std::cout << "Procesul " << processId
              << " a terminat. Valoare finala: "
              << *sharedNumber << "\n";

    UnmapViewOfFile(sharedNumber);
    CloseHandle(hSemaphore);
    CloseHandle(hMap);

    return 0;
}
