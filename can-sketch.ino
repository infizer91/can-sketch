#include <can.h>
#include <mcp2515.h>
#include <SPI.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);


void setup() {
  Serial.begin(115200);
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}

void loop() {
  
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      
    Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
        
      Serial.print(canMsg.data[i],HEX);
      Serial.print(" ");

    }

    Serial.println();   

  if(canMsg.can_id == 0x21F)
    {
      Serial.println("Get command from steering buttons.");
      Serial.print("\t Command is");
        
      if(canMsg.data[0]==0x08)
        {           
          Serial.println("vol++");
          Serial.print("\t");

          struct can_frame canMsg1;
          canMsg1.can_id  = 0x122;
          canMsg1.can_dlc = 8;
          canMsg1.data[0] = 0x00;
          canMsg1.data[1] = 0x00;
          canMsg1.data[2] = 0x00;
          canMsg1.data[3] = 0x00;
          canMsg1.data[4] = 0x00;
          canMsg1.data[5] = 0x02;
          canMsg1.data[6] = 0x01;  
          canMsg1.data[7] = 0x00;
        
          mcp2515.sendMessage(&canMsg1);#include <can.h>              //подключаем библиотеку CAN
#include <mcp2515.h>          //подключаем библиотеку mcp2515
#include <SPI.h>              //подключаем библиотеку SPI

struct can_frame canMsg;      //создаем структуру принимаемого пакета can
struct can_frame new_canMsg;  //создаем структуру отправляемого пакета can

MCP2515 mcp2515(10);          //  

void setup()                  //задаем настройки серийного интерфейса, интерфейса работы с can-шиной
{
  Serial.begin(115200);
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
}

int front_panel_command;      //объявляем переменную команды от панели FMUX (5 байт - нумерация с 0)
int steer_key_1;              //объявляем переменную команды нажатия кнопки на руле 
int steer_key_2;              //объявляем переменную команды нажатия кнопки на руле
int volume;                   //объявляем переменную текущего значения громкости

void loop()                   //запускаем цикл чтения данных данных в can-шине
{
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)          //если нет ошибок, работаем дальше
  {
    if (canMsg.can_id == 0x122)                                 //если пакет поступил от панели FMUX
    { 
      //далее определяется способ увеличения громкости. От него зависит, какую команду от имени панели FMUX необходимо отправить, 
      //чтобы кнопка регулировки громкости на руле корректно сработала.  
      if(canMsg.data[5] == 0)                                 //данное значение правдиво при выходе автомобиля из сна, если не 
      //использована регулировка громкости на панели FMUX либо на руле (через Arduino)
      {
        steer_key_1 = 0x08;                                 //переменной присваиваем значение кнопки увеличения громкости
        steer_key_2 = 0x04;                                 //переменной присваиваем значение кнопки уменьшения громкости
        front_panel_command = 0x02;                         //переменной присваиваем знаение 0 (5 байт)
      }
      
      else if(canMsg.data[5] == 2)                            //данное сравнение правдиво, если уже использована регулировка громкости на панели FMUX 
      //либо на руле (через Arduino)
      {
        steer_key_1 = 0x04;                                 //переменной присваиваем значение кнопки уменьшения громкости
        steer_key_2 = 0x08;                                 //переменной присваиваем значение кнопки увеличения громкости     
        front_panel_command = 0x00;                         //переменной присваиваем знаение 0 (5 байт)        
      }
      
      else
      {
        //ничего не делаем, иных данных нет
      }
      
      volume = canMsg.data[6];                                //присваиваем значение текущего положения регулятора громкости панели FMUX    
    }
    
    if (canMsg.can_id == 0x21F)                                 //если пакет поступил от кнопок руля
    {     
      if (canMsg.data[0] == steer_key_1)                      //определяем, какая кнопка нажата  
      {                
        //значения регулятора громкости на панели FMUX меняются от 0 до 255 в цикле. При достижении 255 - следующее значение 0. В обратную сторону также.
        if (volume == 255)
        {
          volume = 0;
        }
        
        else
        {
          volume++;   
        }
                
        new_canMsg.can_id  = 0x122;                  //присваеваем id панели FMUX
        new_canMsg.can_dlc = 8;                      //указываем длину пакета
        new_canMsg.data[0] = 0x00;                   //пустые данные
        new_canMsg.data[1] = 0x00;                   //пустые данные 
        new_canMsg.data[2] = 0x00;                   //пустые данные
        new_canMsg.data[3] = 0x00;                   //пустые данные 
        new_canMsg.data[4] = 0x00;                   //пустые данные
        new_canMsg.data[5] = front_panel_command;    //команда панели
        new_canMsg.data[6] = volume;                 //присваеваем новый уровень громкости
        new_canMsg.data[7] = 0x00;                   //пустые данные
        
        mcp2515.sendMessage(&new_canMsg);            //отправляем новую команду изменения уровня громкости
        
        delay(150);                                  //задержка 0,15 сек.
      }
      
      if (canMsg.data[0] == steer_key_2)                      //определяем, какая кнопка нажата    
      {       
        if (volume == 0)
        {
          volume = 255;
        }
        
        else
        {
          volume--;
        }
        
        new_canMsg.can_id  = 0x122;                  //присваеваем id панели FMUX
        new_canMsg.can_dlc = 8;                      //указываем длину пакета
        new_canMsg.data[0] = 0x00;                   //пустые данные
        new_canMsg.data[1] = 0x00;                   //пустые данные 
        new_canMsg.data[2] = 0x00;                   //пустые данные
        new_canMsg.data[3] = 0x00;                   //пустые данные 
        new_canMsg.data[4] = 0x00;                   //пустые данные
        new_canMsg.data[5] = front_panel_command;    //команда панели
        new_canMsg.data[6] = volume;                 //присваеваем новый уровень громкости
        new_canMsg.data[7] = 0x00;                   //пустые данные
        
        mcp2515.sendMessage(&new_canMsg);            //отправляем новую команду изменения уровня громкости
        
        delay(150);                                  //задержка 0,15 сек.
      }
    }
  }
}
        }
        
     if(canMsg.data[0]==0x04)
        {
          Serial.println("vol--");
          Serial.print("\t");


          struct can_frame canMsg1;
          canMsg1.can_id  = 0x122;
          canMsg1.can_dlc = 8;
          canMsg1.data[0] = 0x00;
          canMsg1.data[1] = 0x00;
          canMsg1.data[2] = 0x00;
          canMsg1.data[3] = 0x00;
          canMsg1.data[4] = 0x00;
          canMsg1.data[5] = 0x02;
          canMsg1.data[6] = 0xFF;  
          canMsg1.data[7] = 0x00;
    
          mcp2515.sendMessage(&canMsg1);    
        }   
      
    }
  }
}
