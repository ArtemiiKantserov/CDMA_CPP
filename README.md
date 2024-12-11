# CDMA modulation 
## Запуск программы
Для запуска нужно:
1. Перейти в дерикторию ``fixed``:

```bash
cd fixed
```


2. 1. В случае Make нужно перейти в директорию builded_from_Make/ и запустить main.exe.

```bash
make
cd ./built_from_Makefile
./main.exe
```

2. 2. В случае запуска собранной при помощи CMake версии необходимо перейти в директорию build/ и запустить main.exe.

```bash
cd ./build
cmake ..
./main.exe
```

## Устройство программы

1. Отправляемые сообщения от пользователей берутся из директории ``messages/``

2. Для создания нового сообщения (задания нового пользователя) необходимо создать новый текстовый файл в директории ``messages/``

3. Получаемые сообщения автоматически сохраняются в папке ``received_messages/`` с названиями вида ``message[0 - n-1].txt``, что соответствует передаче сообщений из директории ``messages/``, где все файлы (пользователи) отсортированы лексикографически

4. В файле ``errors.csv`` после выполнения работы программы содержится количества паккетов к неправильно полученным битам в связи с шумом. 

5. В файле ``latest.log`` содержится вся информация о работе программы.