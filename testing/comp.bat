@echo off
call copy testing.ino main.c
call gcc main.c
call rm main.c

