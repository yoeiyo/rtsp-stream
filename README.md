# rtsp-stream

## Текущее
***
В файле source.cpp - первая версия, консольное приложение, адрес потока подаётся как аргумент

Несмотря на то, что были удачные тесты, иногда всё равно происходит зависание при открытии потока (строка 63). Скорее всего проблемы непосредственно с подключением.
Тесты были проведены с созданием стрима mp4 файла через vlc.

## Планы

1. Разобраться в причине зависаний при подключении потока
2. Разобраться с интерфейсом и оболочкой библиотеки
3. Посмотреть способы реализации получения звука

## подключение ffmpeg через vcpkg
Вроде подключала по этому гайду
https://trac.ffmpeg.org/wiki/CompilationGuide/vcpkg
