QT += core gui widgets
TARGET = SnakeGame
TEMPLATE = app

# Пути к файлам с учетом вашей структуры
HEADERS += appsnake/Header\ Files/snake.h
SOURCES += appsnake/Source\ Files/main.cpp \
           appsnake/Source\ Files/snake.cpp

# Дополнительные настройки
CONFIG += c++11
