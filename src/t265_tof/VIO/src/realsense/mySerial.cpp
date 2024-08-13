#include "realsense/mySerial.h"
// #define DEBUG

namespace mySerial
{
  mySerial::mySerial(std::string _name, int _baudRate) :
    portName_(_name), baudRate_(_baudRate), serialTimeCnt()
  {
    serial_ = open(_name.c_str(), O_RDWR | O_NDELAY);
    if(serial_ == -1)
    {
      ROS_ERROR_STREAM("Failed to open serial port!");
      exit(0);
    }

    struct termios options;
    if(tcgetattr(serial_, &options) != 0){
      ROS_ERROR_STREAM("Can't get serial port sets!");
      exit(0);
    }
    tcflush(serial_, TCIOFLUSH);
    switch(_baudRate)
    {
      case 921600:
        cfsetispeed(&options, B921600);
        cfsetospeed(&options, B921600);
        break;
      case 576000:
        cfsetispeed(&options, B576000);
        cfsetospeed(&options, B576000);
        break;
      case 500000:
        cfsetispeed(&options, B500000);
        cfsetospeed(&options, B500000);
        break;
      case 460800:
        cfsetispeed(&options, B460800);
        cfsetospeed(&options, B460800);
        break;
      case 230400:
        cfsetispeed(&options, B230400);
        cfsetospeed(&options, B230400);
        break;
      case 115200:
        cfsetispeed(&options, B115200);
        cfsetospeed(&options, B115200);
        break;
      case 57600:
        cfsetispeed(&options, B57600);
        cfsetospeed(&options, B57600);
        break;
      case 38400:
        cfsetispeed(&options, B38400);
        cfsetospeed(&options, B38400);
        break;
      case 19200:
        cfsetispeed(&options, B19200);
        cfsetospeed(&options, B19200);
        break;
      case 9600:
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        break;
      case 4800:
        cfsetispeed(&options, B4800);
        cfsetospeed(&options, B4800);
        break;
      default:
        ROS_ERROR_STREAM("Unsupported baud rate!");
        exit(0);
    }
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    options.c_oflag &= ~(ONLCR | OCRNL);

    if(tcsetattr(serial_, TCSANOW, &options) != 0){
      ROS_ERROR_STREAM("Can't set serial port options!");
      exit(0);
    }
  }

  int mySerial::getSerialPort()
  {
      return this->serial_;
  }

  bool mySerial::toAnother(int anotherPort)
  {
      short _channel_address = 0;           // 辐照探测器的通道地址
      int _channel_count = 0;               // 辐照探测器的通道计数值
      static int _channel_count_sum = 0;    // 辐照探测器4096个通道计数器之和
      unsigned char _buf[1000];

      int ret = read(this->serial_, _buf, sizeof(_buf));

      if(ret < 0)
        return false;
      else
      {
        int data_cnt = 0;
        while(data_cnt < ret)
        {        
          if((_buf[data_cnt] == 0xF0) && (_buf[data_cnt + 5] == checksum(&_buf[data_cnt], 5)))
          {
            data_cnt += 6;
            _channel_address = (_buf[data_cnt + 1] << 4) | (_buf[data_cnt + 2] >> 4);
            _channel_count = (int)(_buf[data_cnt + 2] & 0x0f) << 16 | (_buf[data_cnt + 3] << 8) | _buf[data_cnt + 4];

            if(_channel_count > 10)       // 偶尔有错误数据不能通过if条件过滤掉，在这里设置阈值对其过滤
              continue;
            
            if(_channel_address < 4095)   // 对4096个通道计数器求和
            {
              _channel_count_sum += _channel_count;
            }
            else                          //求完4096个通道计数器的值
            {
              double time_used = serialTimeCnt.toc();
              serialTimeCnt.tic();
              // int cps = (int)(((double)_channel_count_sum) / (time_used * 2));  // 计数率
              int cps = (int)(((double)_channel_count_sum) / (time_used));  // 计数率
              std::cout << "cps " << cps << "\ttime " << time_used << std::endl;
              sendUserData(anotherPort, cps);
              _channel_count_sum = 0;
            }
          }
          else
          {
            data_cnt += 1;
          }
        }
        return true;
      }
  }

  bool mySerial::readData(unsigned char* _buf, int _nRead)
  {
      int total = 0, ret = 0;

      while(total != _nRead)
      {
        ret = read(serial_, _buf + total, (_nRead - total));
        if(ret < 0)
        {
          return false;
        }
        total += ret;
      }
  #ifdef DEBUG
      for(int i = 0; i < _nRead; i++)
      {
        printf("%02x ", _buf[i]);
      }
      printf("\n");
  #endif // DEBUG

      return true;
    }

  int mySerial::writeData(unsigned char *_buf, int _nWrite)
  {
    return(write(serial_, _buf, _nWrite));
  }
  
  std::string mySerial::getCMD(void)
  {
    unsigned char state = 0, _data_len = 0, _data_cnt = 0, _data = 0x00;
    unsigned char _DataBuffer[5];
    bool _data_ok = false;

    for(int i = 0; i < (2 * 5 - 1); i++)        // 循环读取数据，循环次数为2倍数据个数减1
    {
      if(read(serial_, &_data, 1) != 1)         // 读取一字节数据失败
        return std::string("READ_DATA_FAIL");

      if (state == 0 && _data == 0xAA)          // 帧头0xAA
      {
        state = 1;
        _DataBuffer[0] = _data;
      }
      else if (state == 1 && _data == 0x06)     // 功能字0x06
      {
        state = 2;
        _DataBuffer[1] = _data;
      }
      else if (state == 2)                      // 数据长度
      {
        state = 3;
        _DataBuffer[2] = _data;
        _data_len = _data;
        _data_cnt = 0;
      }
      else if (state == 3 && _data_len > 0 && _data_len < 10) // 接收真正的数据
      {
        _data_len--;
        _DataBuffer[3 + _data_cnt++] = _data;
        if(_data_len == 0)
          state = 4;
      }
      else if(state == 4)                                     // 校验和
      {
        state = 0;
        _DataBuffer[3 + _data_cnt] = _data;
        _data_ok = true;
        break;                                                // 跳出循环，开始下一次数据验收
      }
      else
        state = 0;

      if(state == 0 & i == (5 - 1) )                          // 帧头或功能字错误
        return std::string("FRAME_HEADER_OR_FUNCTION_CHARACTER_ERROR");
    }

    uint8_t check_sum = 0;
    for(int i = 0; i < (3 + _data_cnt); i++)                  // 求校验和
    {
        check_sum += _DataBuffer[i];

    }

    if(check_sum == _DataBuffer[3 + _data_cnt])               // 校验和正确
    {
      switch (_DataBuffer[3])
      {
      case 0x01:
        return (std::string("TAKEOFF_MODE"));
      case 0x02:
        return (std::string("TRACKING_MODE"));
      case 0x03:
        return (std::string("LAND_MODE"));
      default:                                                // 指令错误
        return (std::string("COMMAND_ERROR"));
      }
    }
    else                                                      // 校验和错误
    {
      return (std::string("CHECK_SUM_ERROR"));
    }
  }

float mySerial::getDist(void)
{
  unsigned char state = 0, _data_len = 0, _data_cnt = 0, _data = 0x00;
  unsigned char _DataBuffer[9];
  bool _data_ok = false;

  // for(int i = 0; i < (2 * 9 - 1); i++)        // 循环读取数据，循环次数为2倍数据个数减1
  while(true)
  {
    if(read(serial_, &_data, 1) != 1)         // 读取一字节数据失败
      continue;
      // return -1;

    if (state == 0 && _data == 0x59)          // 帧头0xAA
    {
      state = 1;
      _DataBuffer[0] = _data;
    }
    else if (state == 1 && _data == 0x59)     // 功能字0x06
    {
      state = 2;
      _DataBuffer[1] = _data;
      _data_len = 6;
      _data_cnt = 0;
    }
    else if (state == 2 && _data_len > 0 && _data_len < 10) // 接收真正的数据
    {
      _data_len--;
      _DataBuffer[2 + _data_cnt++] = _data;
      if(_data_len == 0)
        state = 3;
    }
    else if(state == 3)                                     // 校验和
    {
      state = 0;
      _DataBuffer[2 + _data_cnt] = _data;
      _data_ok = true;

      break;                                                // 跳出循环，开始下一次数据验收
    }
    else
      state = 0;

    // if(state == 0 && i == (9 - 1) )                          // 帧头或功能字错误
    //   return -1;
  }

  uint8_t check_sum = 0;
  for(int i = 0; i < (2 + _data_cnt); i++)                  // 求校验和
  {
      check_sum += _DataBuffer[i];

  }

  // for(int i = 0; i < 9; i++)
  // {
  //   printf("%02x ", _DataBuffer[i]);
  // }
  // printf("\t%02x \n",check_sum);

  if(check_sum == _DataBuffer[2 + _data_cnt])               // 校验和正确
  {
    return ((float)(_DataBuffer[3] << 8 | _DataBuffer[2]) / 100.0);
  }
  else                                                      // 校验和错误
  {
    return -1;
  }
}

  void mySerial::closePort()
  {
    close(serial_);
  }

bool mySerial::sendUserData(int _serial_port, int sensor_data)
    {
      unsigned char _cnt = 0;
      unsigned char data_to_send[10];
      
      data_to_send[_cnt++]=0xAA; 
      data_to_send[_cnt++]=0x05;                // 功能字
      data_to_send[_cnt++]=0;
      
      data_to_send[_cnt++]=BYTE3(sensor_data);  // sensor_data
      data_to_send[_cnt++]=BYTE2(sensor_data);	
      data_to_send[_cnt++]=BYTE1(sensor_data);
      data_to_send[_cnt++]=BYTE0(sensor_data);	

      data_to_send[2] = _cnt-3;                 // 数据长度
      
      unsigned char sum = 0;
      for(int i = 0; i < _cnt; i++)
        sum += data_to_send[i];
      
      data_to_send[_cnt++]=sum;                 // 校验和

      write(_serial_port, data_to_send, _cnt);  // 发送数据
    }

    uint8_t mySerial::checksum(uint8_t data_buf[], int dataLenth)
    {
      int sum = 0;
      for(int i = 0; i < dataLenth; i++)
      {
        sum += data_buf[i];
      }

      while((sum >> 8) != 0)
      {
        sum = (sum & 0xff) + (sum >> 8);
      }

      return BYTE0(sum);
    }
}
