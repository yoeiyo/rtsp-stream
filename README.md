# rtsp-stream

## Проблемы

1. При подключении стрима возможны зависания
2. медленная прорисовка кадров
   

## Запуск стрима

# Через vlc: 

vlc C:\Users\Lenovo\Documents\sample.mp4 --sout="#rtp{sdp=rtsp://:8554/stream}"
Затем для подключения используем адрес 'rtsp://localhost:8554/stream'

# vlc gui

1. Media -> Stream (Ctrl + S)
2. Add the video file
   
![image](https://github.com/yoeiyo/rtsp-stream/assets/106560105/89f679c9-7509-407f-8330-e18586f00635)

4. Stream -> Next
5. New destination: rtsp -> add

![image](https://github.com/yoeiyo/rtsp-stream/assets/106560105/23c48ab7-a783-4dfa-8015-b26ae56d3cf0)

![image](https://github.com/yoeiyo/rtsp-stream/assets/106560105/74eb0fe8-c2b1-45a6-9fdd-b86243c5fed9)


   После этого просто продолжать дальше, отмеченные/неотмеченные чекбоксы не влияют на запуск или подключение
Для подключения используем адрес 'rtsp://localhost:8554/'

***

https://www.gyan.dev/ffmpeg/builds/ 

http://trac.ffmpeg.org/wiki/StreamingGuide#:~:text=FFmpeg%20can%20stream%20a%20single,a%20live%20streaming%20%E2%80%8Bsource

## конвертирование библиотек (bcc)

implib.exe -a /s lib_name.lib dll_name.dll

(replaces lib_name.lib)


## Текущее

Текущая версия - rstp2
Добавлены классы formatCtx и codecCtx

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
