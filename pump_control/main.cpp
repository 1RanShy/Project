#include "pump_control.h"

int main()
{
    PumpControl pump(17, 5000000); // 创建一个PumpControl对象，使用GPIO17和延迟时间5000微秒
    while (true)
    {
        pump.start(); // 启动水泵
        pump.stop(); // 停止水泵
    }
    return 0;
}