#include "head/chatWindow.h"
#include "head/netWorkManager.h"
#include <iostream>
#include <windows.h>

int main() {
    SetConsoleOutputCP(65001);
    unsigned short port = 53000;
    NetWorkManager netWorkManager(port);
    if (!netWorkManager.beginListen()) {
        std::cout << "监听端口:" << port << " 失败，请检查端口是否被占用！" << std::endl;
        return -1;
    }
    ChatWindow window(800, 600, L"服务器", 28, 40);
    window.serverInterface(netWorkManager);

    return 0;
}
