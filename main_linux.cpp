
#include <iostream>
#include <random>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

const char* MEMORY_NAME = "/MemoryMappedProcesses";
const char* SEMAPHORE_NAME = "/MemoryMappedProcessesSemaphore";

int main()
{
    // Deschidem sau cream memoria partajata
    int shm_fd = shm_open(
        MEMORY_NAME,
        O_CREAT | O_EXCL | O_RDWR,
        0666
    );

    bool firstProcess = (shm_fd != -1);

    if (firstProcess)
    {
        if (ftruncate(shm_fd, sizeof(int)) == -1)
        {
            std::cerr << "Eroare la dimensionarea memoriei.\n";
            close(shm_fd);
            return 1;
        }
    }
    else
    {
        shm_fd = shm_open(MEMORY_NAME, O_RDWR, 0666);

        if (shm_fd == -1)
        {
            std::cerr << "Eroare la deschiderea memoriei.\n";
            return 1;
        }
    }

    // Mapam memoria in spatiul procesului
    int* sharedNumber = (int*)mmap(
        nullptr,
        sizeof(int),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_fd,
        0
    );

    if (sharedNumber == MAP_FAILED)
    {
        std::cerr << "Eroare la maparea memoriei.\n";
        close(shm_fd);
        return 1;
    }

    // Primul proces initializeaza memoria
    if (firstProcess)
    {
        *sharedNumber = 0;
    }

    // Deschidem semaforul comun
    sem_t* semaphore = sem_open(
        SEMAPHORE_NAME,
        O_CREAT,
        0666,
        1
    );

    if (semaphore == SEM_FAILED)
    {
        std::cerr << "Eroare la deschiderea semaforului.\n";
        munmap(sharedNumber, sizeof(int));
        close(shm_fd);
        return 1;
    }

    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> coin(1, 2);

    pid_t processId = getpid();

    std::cout << "Procesul " << processId << " a pornit.\n";

    while (true)
    {
        // Rezervam semaforul
        while (sem_wait(semaphore) == -1)
        {
            if (errno != EINTR)
            {
                std::cerr << "Eroare la rezervarea semaforului.\n";
                return 1;
            }
        }

        // Citim numarul din memoria partajata
        int number = *sharedNumber;

        std::cout << "Procesul " << processId
                  << " citeste: " << number << "\n";

        if (number >= 1000)
        {
            sem_post(semaphore);
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
        sem_post(semaphore);

        // Oferim si celuilalt proces sansa sa execute
        usleep(50000);
    }

    std::cout << "Procesul " << processId
              << " a terminat. Valoare finala: "
              << *sharedNumber << "\n";

    munmap(sharedNumber, sizeof(int));
    close(shm_fd);
    sem_close(semaphore);

    return 0;
}
