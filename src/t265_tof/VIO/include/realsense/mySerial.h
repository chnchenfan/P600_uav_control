#include <ros/ros.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string>
#include <chrono>

//数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)		) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

class TicToc
{
  public:
    TicToc()//构造函数
    {
        tic();
    }

    void tic()//成员函数
    {
        start = std::chrono::system_clock::now();
    }

    double toc()//成员函数
    {
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        return elapsed_seconds.count();
    }

  private:
    std::chrono::time_point<std::chrono::system_clock> start, end;
};

namespace mySerial
{
  class mySerial
  {
    public:
      mySerial(std::string _name, int _baudRate);
      ~mySerial(){}//析构函数
      bool readData(unsigned char *_buf, int _nRead);
      int writeData(unsigned char *_buf, int _nWrite);
      std::string getCMD();
      void closePort();
      int getSerialPort();
      bool toAnother(int anotherPort);
      bool sendUserData(int _serial_port, int sensor_data);
      uint8_t checksum(uint8_t data_buf[], int dataLenth);
      float getDist();

      unsigned char dataBuf[7];

    private:
      std::string portName_;
      int baudRate_;
      int serial_;

      TicToc serialTimeCnt;
  };
}
