# Сервер для игры в шашки

## Установка и запуск

1. Клонировать репозиторий:
```bash
git clone https://github.com/piriverzin/project-Victor-Piriverzin-shashki.git
```

2. Перейти в папку с проектом:
```bash
cd project-Victor-Piriverzin-shashki/project
```

3. Скомпилировать сервер:
```bash
g++ -std=c++11 -pthread src/*.cpp -o checkers_server
```

4. Запустить сервер:
```bash
./checkers_server
```

После запуска сервер будет доступен по адресу: `http://localhost:8080`
