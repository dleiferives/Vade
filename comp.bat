@echo off
call copy vade.ino main.c
call gcc main.c
call rm main.c
call git add .
call git commit -"automated commit with comp"

