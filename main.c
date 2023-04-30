#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE (40 * sizeof(char))
#define FLOWER_COUNT 40
#define SHARED_MEMORY_OBJECT_NAME "/flowers"
void handle_end(int signum) {
    key_t key = ftok(SHARED_MEMORY_OBJECT_NAME, 'R'); // получаем ключ разделяемой памяти
    int shmid = shmget(key, 40, 0666 | IPC_CREAT);
    if (shmdt(shmat(shmid, NULL, 0)) == -1) { // отключаем разделяемую память
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 40; ++i) { // отключаем семафоры
        char semaphore_name[20];
        sprintf(semaphore_name, "/semaphore_%d", i);
        sem_unlink(semaphore_name);
    }

    printf("\nexited and done cleanup\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    int fd;
    char *ptr;
    pid_t pid;

    signal(SIGTERM, handle_end);
    signal(SIGINT, handle_end);

    sem_t *semaphores[FLOWER_COUNT + 1]; // 40 семафоров для поливания цветов и последний семафор-счетчик увядших

    // создаем семафоры
    for (int i = 0; i < FLOWER_COUNT + 1; ++i) {
        char semaphore_name[20];
        sprintf(semaphore_name, "/semaphore_%d", i);

        semaphores[i] = sem_open(semaphore_name, O_CREAT, 0666, 1);
    }


    // Создание объекта разделяемой памяти
    fd = shm_open(SHARED_MEMORY_OBJECT_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Установка размера объекта разделяемой памяти
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Отображение объекта разделяемой памяти в адресное пространство процесса
    ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Изначально все цветы свежие
    for(int i = 0; i < FLOWER_COUNT; ++i) {
        ptr[i] = '1';
    }

    // Создание дочернего процесса увядания цветков
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        srand(time(NULL));
        while(1) {
            sleep(rand() % 4 + 2); // Дочерний процесс с каким-то промежутком делает некоторые цветы увядшимы
            int count = rand() % 4; // от 0 до 3 цветов могут увянуть
            int post_val = 0; // на сколько изменить счетчик увядший цветов
            while(count--) {
                int flower = rand() % FLOWER_COUNT; // цветок, который увянет
                sem_wait(semaphores[flower]);
                if(ptr[flower] != '0') {
                    ptr[flower] = '0';
                    ++post_val;
                    printf("Flower number %d is dried up \n", flower + 1);
                }
                sem_post(semaphores[flower]);
            }
            // уведомляем садовников об увядших цветах
            for (int i = 0; i < post_val; ++i) {
                sem_post(semaphores[FLOWER_COUNT]);
            }
        }
    }

    // Создание дочерних процессов садовников
    for (int i = 0; i < 2; ++i) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            while (1) {
                // ждем пока цветы увянут
                sem_wait(semaphores[FLOWER_COUNT]);
                int stop = 0;
                // как только кто-то завял, то пробегаемся по всем цветам и ищем увядший
                for (int j = 0; j < FLOWER_COUNT && !stop; ++j) {
                    sem_wait(semaphores[j]); // захватываем семафор чтобы дважды не поливать цветок
                    if (ptr[j] == '0') {
                        ptr[j] = '1';
                        stop = 1;
                        printf("Gardener number %d watered flower number %d \n", i + 1, j + 1);
                    }
                    sem_post(semaphores[j]);
                }
                sleep(4);
            }
        }
    }

    while(1) {sleep(1000);}
}
