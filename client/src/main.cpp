#include "head/chatWindow.h"
#include "head/netWorkManager.h"
#include <iostream>
#include <windows.h>

int main() {
    SetConsoleOutputCP(65001);
    std::string serverIP;
    std::cout << "请输入服务器IP地址: ";
    std::cin >> serverIP;
    unsigned short port;
    std::cout << "请输入端口号：";
    std::cin >> port;
    NetWorkManager netWorkManager(serverIP, port);
    if (!netWorkManager.beginConnectServer()) {
        std::cout << "连接服务器失败，请检查服务器是否正常启动！" << std::endl;
        return -1;
    }
    ChatWindow window(800, 600, L"客户端", 28, 40);
    window.clientInterface(netWorkManager);

    return 0;
}
