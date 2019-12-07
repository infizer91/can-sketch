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
        
          mcp2515.sendMessage(&canMsg1);
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
