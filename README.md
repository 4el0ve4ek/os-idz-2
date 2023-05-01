## Аксенов Даниил Алексеевич | БПИ218 | ИДЗ-2 | Вариант 15

### Условие задачи
Задача о клумбе – 1. На клумбе растет 40 цветов, за ними непрерывно следят два процесса–садовника и поливают увядшие цветы, при этом оба садовника очень боятся полить один и тот же цветок, который еще не начал вянуть. Создать приложение, моделирующее состояния цветков на клумбе и действия садовников. Для изменения состояния цветов создать отдельный процесс (а не 40 процессов для каждого цветка), который может задавать одновременное начало увядания для нескольких цветков

### Решение

В программе фигурируют три процесса - один процесс отвечает за увядание цветов, два других отвечают за роль садовника.
Эти процессы общаются между собой с помощью выделенной памяти, в которой указано состояние каждого из цветков, а так же с помощью 41 семафора.
40 семафоров нужны для того чтобы ровно один процесс обращался к цветку. А еще один семафор работает как счетчик увядших цветов -- процесс цветов увеличивает его на каждое уведание, а процесс садовников уменьшает его, прежде чем пойти искать эти увядающие цветы.


Логика "увядания": каждые 2-5 секунд выбирается от 0 до 3 цветов которые могут завянуть, если еще не завяли. После этого увеличивается счетчик увядших цветов, если их число изменилось.

Логика "садовника": ждет пока счетчик увядших цветов станет ну нулевым, потом идет по всем клумбам от 0 до 40 и проверяет состояние цветов в разделяемой памяти. В конце он отдыхает 2 секунды и его действия повторяются)

### Запуск
```
gcc main.c -o main -lrt -lpthread && ./main
```


### Пример работы программы

```text
Flower number 21 is dried up 
Flower number 18 is dried up 
Gardener number 1 watered flower number 18 
Gardener number 2 watered flower number 21 
Flower number 8 is dried up 
Flower number 10 is dried up 
Flower number 13 is dried up 
Gardener number 2 watered flower number 8 
Gardener number 1 watered flower number 10 
Gardener number 2 watered flower number 13 
Flower number 25 is dried up 
Gardener number 1 watered flower number 25 
Flower number 11 is dried up 
Gardener number 2 watered flower number 11 
Flower number 8 is dried up 
Gardener number 1 watered flower number 8 
Flower number 32 is dried up 
Flower number 16 is dried up 
Flower number 36 is dried up 
Gardener number 2 watered flower number 16 
Gardener number 1 watered flower number 32 
Flower number 33 is dried up 
Gardener number 2 watered flower number 33 
Gardener number 1 watered flower number 36 
Flower number 10 is dried up 
Flower number 38 is dried up 
Flower number 9 is dried up 
Gardener number 2 watered flower number 9 
Gardener number 1 watered flower number 10 
Flower number 25 is dried up 
Gardener number 2 watered flower number 25 
Gardener number 1 watered flower number 38 
^C


exited and done cleanup
exited and done cleanup
exited and done cleanup
```