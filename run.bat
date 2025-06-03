@echo off

REM Кодировка терминала для сообщений с utf-8:
chcp 65001 > nul

REM Имя проекта:
set VAR_PROJECT_NAME=ParticlesGame
set UPD_CMAKE_CONFIG=ON


REM Удаляем старую сборку:
if exist build (

  cd build
  del %file_name%.exe
  cd ..

  if "%UPD_CMAKE_CONFIG%"=="ON" (
    
    echo Предыдущая конфигурация Cmake удалена
    rmdir build /s /q
  REM           ^  ^
  REM           |  без запроса подтверждения
  REM           удаляются все подкаталоги
  )
)

if not exist build (

  mkdir build
  echo Папка 'build' создана
)

cd build


REM Конфигурируем проект CMake:
cmake -G "MinGW Makefiles" -B .   -S ..                 -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ -D VAR_PROJECT_NAME=%VAR_PROJECT_NAME%
REM    ^                    ^     ^                      ^
REM    сборщик | папка сборки  |  папка с исходниками  | переменные, устанавливаемые в CMake сборку


REM Выполняем сборку:
mingw32-make


REM Проверяем успешное завершение сборки:
if not exist %VAR_PROJECT_NAME%.exe (
    echo Компиляция завершилась ошибкой
)

REM В bat файлах нет `else`,
REM так что проверяем обратное условие:

if exist %VAR_PROJECT_NAME%.exe (

  echo Компиляция завершилась успешно
  
  REM Запускаем сборку:
  %VAR_PROJECT_NAME%
)

