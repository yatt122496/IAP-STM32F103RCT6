::�ļ��滻������
::�õ�ǰĿ¼�µ�xx.c��xx.h�滻��ǰĿ¼���������ļ����ڵ�xx.c��xx.h
::�����xx������Ҫ�滻���ļ����֣���Ҫ�Լ�������Ҫ�滻�������޸�
::�������滻����ILI93xx.c��LCD.h
@echo off&&setlocal enabledelayedexpansion 
for /f "delims=" %%a in ('dir/b/s/ad') do (
 
 
for /f "delims=" %%i in ('dir/b "%%a\*.c"') do (::���ҵ�ǰĿ¼�°���.c�ļ�����Ŀ¼
set var=%%i
if "!var!"=="exti.c" copy /y exti.c "%%a\%%i"
)

for /f "delims=" %%i in ('dir/b "%%a\*.h"') do (::���ҵ�ǰĿ¼�°���.c�ļ�����Ŀ¼
set var=%%i
if "!var!"=="exti.h" copy /y exti.h "%%a\%%i"
)
 

) 
pause.
