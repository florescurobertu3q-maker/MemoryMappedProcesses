

\# Memory Mapped Processes



\## Descriere

Proiect realizat în C++ pentru comunicarea între două procese independente folosind memorie partajată și semafoare.



Cele două procese accesează aceeași zonă de memorie și contribuie la numărarea de la 0 la 1000.



Semaforul asigură accesul exclusiv la memoria partajată, astfel încât cele două procese să nu citească sau să modifice simultan datele.



\## Funcționare

1\. Primul proces creează și inițializează memoria partajată.

2\. Al doilea proces deschide aceeași zonă de memorie.

3\. Procesul rezervă semaforul înainte de accesarea memoriei.

4\. Citește valoarea curentă.

5\. Generează aleator un număr între 1 și 2.

6\. Cât timp rezultatul este 2, incrementează valoarea și o scrie în memorie.

7\. Când rezultatul este 1, eliberează semaforul.

8\. Procesele repetă operația până când valoarea ajunge la 1000.



\## Implementare Windows

\- Memorie partajată: CreateFileMapping și MapViewOfFile.

\- Sincronizare: CreateSemaphore, WaitForSingleObject și ReleaseSemaphore.



\### Compilare

```bash

g++ -std=c++17 windows/main\_windows.cpp -o windows/mapped\_windows.exe

```



\### Rulare

Deschide două terminale Git Bash și execută în fiecare:

```bash

./windows/mapped\_windows.exe

```



\## Implementare Linux

\- Memorie partajată: shm\_open și mmap.

\- Sincronizare: sem\_open, sem\_wait și sem\_post.



\### Compilare

```bash

g++ -std=c++17 linux/main\_linux.cpp -o linux/mapped\_linux -pthread

```



\### Rulare

Înainte de prima rulare, curăță obiectele rămase de la o execuție anterioară:

```bash

rm -f /dev/shm/MemoryMappedProcesses

rm -f /dev/shm/sem.MemoryMappedProcessesSemaphore

```



Deschide două terminale și execută în fiecare:

```bash

./linux/mapped\_linux

```



\## Rezultate

Ambele implementări au fost testate cu două procese independente. Procesele au utilizat aceeași memorie partajată și același semafor, iar valoarea finală a ajuns la 1000.



\## Structura proiectului

```text

MemoryMappedProcesses/

├── windows/

│   └── main\_windows.cpp

├── linux/

│   └── main\_linux.cpp

├── README.md

└── DOCUMENTATIE.pdf

```



