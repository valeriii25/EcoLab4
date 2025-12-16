@echo off
echo Setting up EcoOS Environment...

rem 1. Путь к исходникам (у вас он уже есть)
set ECO_FRAMEWORK=C:\Eco.Framework

rem 2. Путь к папке с инструментами (создайте её, как описано выше)
set ECO_TOOLCHAIN=C:\Eco.Toolchain

rem 3. Добавляем Make и Компилятор в Path, чтобы Windows их видела
rem Укажите здесь путь к папке, где лежит make.exe
set PATH=%ECO_TOOLCHAIN%\BuildTools\bin;%PATH%

rem Проверка
make --version
if %errorlevel% neq 0 (
    echo [ERROR] MAKE not found! Check path in env.bat
) else (
    echo [OK] Make found.
)

cmd
