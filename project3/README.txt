Запуск:
server.exe
client.exe
дождаться в сервере 'new connected client!'
затем client.exe
работа с первым клиентом, ввод 0 0 0
работы со вторым клиентом

g++ server.cpp -o server.exe -lwsock32
- аналогично для клиента
pragma не пашет для mingw

ДЛЯ НОВОГО КОДА:
-lws2_32