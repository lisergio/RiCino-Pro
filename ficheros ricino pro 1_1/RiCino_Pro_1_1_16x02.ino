
/* PROYECTO RiCino Pro 1.1

Cuenta Vuletas personal

Creado por Lisergio

12/2012
*/

//*********************************************************************************

#include <IRremote.h>
#include <LiquidCrystal.h>
#include <EEPROM.h> 
#include <SD.h>
#include <Wire.h>
#include <VirtualWire.h>
#define DS1307_I2C_ADDRESS 0x68


byte decToBcd(byte val)
{
return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)
{
return ( (val/16*10) + (val%16) );
}

int m,s,l;                                
int mu=0,md=0,su=0,sd=0,lu=0,ld=0,lc=0;
int mu2=0,md2=0,su2=0,sd2=0,lu2=0,ld2=0,lc2=0;
int pulsador,antes,despues;
long int tiempo,inicio,tiempo_parcial,tiempo_vuelta,tiempo_minimo_vuelta,tiempo_entrenos,tiempo_inicio,vuelta_rapida,control_vuelta_rapida;
LiquidCrystal lcd(32, 30, 28, 26, 24, 22);

int empezar_crono = LOW;
int numero_vuelta_rapida;
int numero_vueltas;
int config_numero_transponder=1;
int config_beep = 1;
int config_sd = 1;
int ee=0;

File myFile;

int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;

int message;
int conex=0;
int entrada;
int tx=0;
int tx2=0;
int cronoiniciado=0;
char mensaje[3];

long int guarda_vueltas[200];
int menu=1;

int Tx=0;

int button1=6;
int button2=7;
int button3=8;
int button4=9;



int buttonstate1=0;
int buttonstate2=0;
int buttonstate3=0;
int buttonstate4=0;

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;


//*********************************************************************************


void(* Resetea) (void) = 0;

//*********************************************************************************

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  lcd.begin(20, 4);
  lcd.setCursor(1,0);
  lcd.print("RiCino Pro 1.1");
  lcd.setCursor(3,1);
  lcd.print("By Lisergio");
  
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);  
  
  antes=HIGH;                            
  despues=HIGH;                           
  
  tiempo_parcial=0;
  tiempo_vuelta=0;
  numero_vueltas=0;
  vuelta_rapida=0;
  control_vuelta_rapida=0;
  numero_vuelta_rapida=0;
  
  ee = EEPROM.read(0);
  
  vw_set_ptt_inverted(true); 
  vw_setup(2000);	 
  
  if (ee==0)
  {
    tiempo_minimo_vuelta= 4000;
    tiempo_entrenos = 0;
    config_beep = 1;
    config_numero_transponder=1;
    config_sd=1;    
  }
  else
  {
    tiempo_minimo_vuelta= EEPROM.read(1)*1000;
    tiempo_entrenos=EEPROM.read(2)*60000;
    config_numero_transponder=EEPROM.read(4);
    config_beep=EEPROM.read(5);
    config_sd=EEPROM.read(6);
  }
  
  pinMode(53, OUTPUT);
  
  delay(1000);
  
  buttonstate3=digitalRead(button3);
  buttonstate2=digitalRead(button2);
  buttonstate1=digitalRead(button1);
  buttonstate4=digitalRead(button4);
  
  
  if (buttonstate2 == LOW)
  {
    menu=100;
    leer_codigo();
  }
  
  else if (buttonstate1 == LOW)
  {
   menu=11;
   menu_1_1();
  }
  else if (buttonstate3 == LOW)
  {
    menu = 17;
    menu_1_7();
  }
  


  else 
  {
  menu_1(); 
  }
}

//*********************************************************************************

void loop()
{

 
}

//*********************************************************************************

void menu_1()
{
  lcd.clear();
  borrar_array();
  vuelta_rapida=0;
  numero_vuelta_rapida=0;
  while (menu==1)
  {
    lcd.setCursor(1,0);
    lcd.print("RiCino Pro 1.1"); 
    lcd.setCursor(0,1);
    lcd.print("     Inicio");
   
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    buttonstate1=digitalRead(button1);
    if (buttonstate3 == LOW )
    {
      pitido_menu();
      menu = 2;
      delay(250);
      menu_2();
    }
    if (buttonstate4 == LOW )
    {
      pitido_menu();
      menu = 30;
      delay(250);
      mostrar_hora();
    }
  }
}

//*********************************************************************************

void menu_1_1()
{
  lcd.clear();
  while (menu == 11)
  {   
    lcd.setCursor(2,0);
    lcd.print("RiCino Setup");
    lcd.setCursor(0,1);
    lcd.print("T.Min.Vuelta: ");
    lcd.print(tiempo_minimo_vuelta/1000);
    

    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);

    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu=12;
      delay(250); 
      menu_1_2();     
    }
    
    if (buttonstate3 == LOW)
    {
      pitido_menu();
      tiempo_minimo_vuelta=tiempo_minimo_vuelta+1000;
      delay(250);      
    }
    
    if (buttonstate4 == LOW)
    {
      if (tiempo_minimo_vuelta > 1000)
     {
       pitido_menu();
       tiempo_minimo_vuelta=tiempo_minimo_vuelta - 1000;
       delay(250);  
       lcd.clear();     
     } 
    }    
  }
}

//*********************************************************************************

void menu_1_2()
{
  lcd.clear();
  while (menu == 12)
  {   
    lcd.setCursor(2,0);
    lcd.print("RiCino Setup");
    lcd.setCursor(0,1);
    lcd.print("T.Entrenos: ");
    lcd.print(tiempo_entrenos/60000);
    
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    
    if (buttonstate1 == LOW)
    {
      pitido_menu();
      menu=11;
      delay(250); 
      menu_1_1();     
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu=14;
      delay(250);
      menu_1_4();      
    }
    
    if (buttonstate3 == LOW)
    {
      pitido_menu();
      tiempo_entrenos=tiempo_entrenos+60000;
      delay(250);
    }
    
    if (buttonstate4 == LOW)
    {
      if (tiempo_entrenos >= 60000)
     {
       pitido_menu();
       tiempo_entrenos=tiempo_entrenos - 60000;
       delay(250);
       lcd.clear();
     } 
    }    
  }  
}

//*********************************************************************************

void menu_1_4()
{
  lcd.clear();
  while (menu == 14)
  {
    
    lcd.setCursor(2,0);
    lcd.print("RiCino Setup");
    lcd.setCursor(0,1);
    lcd.print("Transponder: ");
    lcd.setCursor(13,1);
    lcd.print("   ");
    lcd.setCursor(13,1);    
    lcd.print(config_numero_transponder);
    delay(250);
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);

    if (buttonstate1 == LOW)
    {
      pitido_menu();
      menu=12;
      delay(250);
      menu_1_2();
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu=15;
      delay(250);
      menu_1_5();
    }
    
    if (buttonstate3 == LOW)
    {
      if (config_numero_transponder < 20)
      {
      pitido_menu();
      config_numero_transponder=config_numero_transponder+1;
      delay(250);
      }
    }
    
    if (buttonstate4 == LOW)
    {
      if (config_numero_transponder > 1)
     {
       pitido_menu();
       config_numero_transponder=config_numero_transponder - 1;
       delay(250);
     } 
    } 
  }    
}

//*********************************************************************************

void menu_1_5()
{
  lcd.clear();
  while (menu == 15)
  {
    
    lcd.setCursor(2,0);
    lcd.print("RiCino Setup");
    lcd.setCursor(0,1);
    lcd.print("Beep");
    if (config_beep==1)
    {
      lcd.setCursor(13,1);
      lcd.print(" On");
    }
    if (config_beep==0)
    {
      lcd.setCursor(13,1);
      lcd.print("Off");
    }
    
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    
    if (buttonstate1 == LOW)
    {
      pitido_menu();
      menu=14;
      delay(250);
      menu_1_4();
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu=16;
      delay(250);
      menu_1_6();
    }    

    if (buttonstate3 == LOW)
    {
      if (config_beep==1)
      {
        pitido_menu();
        config_beep=0;
        delay(250);
      }
      else
      {
        pitido_menu();
        config_beep=1;
        delay(250);
      }
    }

    if (buttonstate4 == LOW)
    {
      if (config_beep==1)
      {
        pitido_menu();
        config_beep=0;
        delay(250);
      }
      else
      {
        pitido_menu();
        config_beep=1;
        delay(250);
      }
    }
  } 
}


//*********************************************************************************

void menu_1_6()
{
  lcd.clear();
  while (menu == 16)
  {
    
    lcd.setCursor(2,0);
    lcd.print("RiCino Setup");
    lcd.setCursor(0,1);
    lcd.print("Datos SD");
    
     if (config_sd==1)
    {
      lcd.setCursor(13,1);
      lcd.print(" On");
    }
    if (config_sd==0)
    {
      lcd.setCursor(13,1);
      lcd.print("Off");
    }

    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    
    if (buttonstate1 == LOW)
    {
      pitido_menu();
      menu=15;
      delay(250);
      menu_1_5();
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      //menu=18;
      menu=31;
      delay(250);
      menu_3_1();
      //menu_1_8();
      
    }    

    if (buttonstate3 == LOW)
    {
      if (config_sd==1)
      {
        pitido_menu();
        config_sd=0;
        delay(250);
      }
      else
      {
        pitido_menu();
        config_sd=1;
        delay(250);
      }
    }

    if (buttonstate4 == LOW)
    {
      if (config_sd==1)
      {
        pitido_menu();
        config_sd=0;
        delay(250);
      }
      else
      {
        pitido_menu();
        config_sd=1;
        delay(250);
      }   
        
    }
  }
}



//*********************************************************************************


void menu_1_7()
{
  irrecv.enableIRIn();
  lcd.clear();
  while (menu == 17)
  {
    lcd.setCursor(0,0);
    lcd.print("Transponder Test");
    lcd.setCursor(3,1);
    lcd.print("NO SIGNAL!");
    
    if (irrecv.decode(&results))       
    {  
    
        if(results.value==0x10)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 1  OK!");    
         if (config_beep==1)
         {
          pitido_deteccion();
         }      
       } 
        if(results.value==0x810)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 2  OK!"); 
         if (config_beep==1)
         {
          pitido_deteccion();
         }       
       } 
        if(results.value==0x410)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 3  OK!"); 
         if (config_beep==1)
         {
          pitido_deteccion();
         }       
       } 
        if(results.value==0xC10)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 4  OK!"); 
         if (config_beep==1)
         {
          pitido_deteccion();
         }        
       } 
        if(results.value==0x210)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 5  OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }
        
       } 
        if(results.value==0xA10)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 6  OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }         
       } 
        if(results.value==0x610)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 7  OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }          
       } 
        if(results.value==0xE10)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 8  OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }          
       } 
        if(results.value==0x110)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 9  OK!");
         pitido_deteccion();         
       } 
        if(results.value==0x910)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 10 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }          
       } 
        if(results.value==0x510)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 11 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }         
       } 
        if(results.value==0xD10)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 12 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }         
       } 
        if(results.value==0x310)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 13 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }          
       } 
        if(results.value==0xB10)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 14 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }          
       } 
        if(results.value==0x710)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 15 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }          
       } 
        if(results.value==0xF10)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 16 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }          
       } 
        if(results.value==0x90)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 17 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }         
       } 
        if(results.value==0x890)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 18 OK!"); 
         if (config_beep==1)
         {
          pitido_deteccion();
         }         
       } 
        if(results.value==0x490)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 19 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }          
       } 
        if(results.value==0xc90)
       {
         lcd.setCursor(3,1);
         lcd.print("TX: 20 OK!");
         if (config_beep==1)
         {
          pitido_deteccion();
         }          
       } 
      
       else
       {
         delay(500);
         lcd.setCursor(3,1);
         lcd.print("NO SIGNAL");
       }
        irrecv.resume();
    }

  }
}

//*********************************************************************************

void menu_1_8()
{
  lcd.clear();
  while (menu==18)
  {
    lcd.setCursor(0,0);
    lcd.print("    MEMORIA");
    lcd.setCursor(0,1);
    lcd.print("GUARDAR   BORRAR");
    
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    
    if (buttonstate1 == LOW)
    {
      pitido_menu();
      menu=181;
      delay(250);
      menu_1_8_1();
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu=182;
      delay(250);
      menu_1_8_2();
    }
    
    if (buttonstate3 == LOW)
    {
      pitido_menu();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Reiniciando...");
      delay(1000);
      Resetea(); 
    }    
  }
}

//*********************************************************************************

void menu_1_8_1()
{
  lcd.clear();
  while (menu==181)
  {
    lcd.setCursor(0,0);
    lcd.print("GUARDAR MEMORIA?");
    lcd.setCursor(0,1);
    lcd.print("SI            NO");    
  
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    
    if (buttonstate1 == LOW)
    {
      EEPROM.write(0,1);
      EEPROM.write(1,tiempo_minimo_vuelta/1000);      
      EEPROM.write(2,tiempo_entrenos/60000);           
      EEPROM.write(4,config_numero_transponder);      
      EEPROM.write(5,config_beep);
      EEPROM.write(6,config_sd);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("   COMPLETADO");
      pitido_borrar_memoria();
      delay(500); 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Reiniciando...");
      delay(1000);
      Resetea();                
    }    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Reiniciando...");
      delay(1000);
      Resetea(); 
    }
  }
}

//*********************************************************************************

void menu_1_8_2()
{
  lcd.clear();
  while (menu == 182)
  {
    lcd.setCursor(0,0);
    lcd.print("BORRAR MEMORIA??");
    lcd.setCursor(0,1);
    lcd.print("SI            NO");

    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    
    if ( buttonstate1 == LOW)
    { 
      for (int i = 0; i < 512; i++)
      {
      EEPROM.write(i, 0);
      }
      EEPROM.write(0,1);
      EEPROM.write(1,4);      
      EEPROM.write(2,0);      
      EEPROM.write(3,0);      
      EEPROM.write(4,1);      
      EEPROM.write(5,1);
      EEPROM.write(6,1);
      EEPROM.write(7,0);
      tiempo_minimo_vuelta= EEPROM.read(1)*1000;
      tiempo_entrenos=EEPROM.read(2)*60000;
      config_numero_transponder=EEPROM.read(4);
      config_beep=EEPROM.read(5);
      config_sd=EEPROM.read(6);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("   COMPLETADO");
      pitido_borrar_memoria();
      delay(500);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Reiniciando...");
      delay(1000);
      Resetea();   
    }
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Reiniciando...");
      delay(1000);
      Resetea(); 
    }
  }  
}

//*********************************************************************************

void menu_2()
{
  lcd.clear();
  while (menu == 2)
  {
    lcd.setCursor(0,0);
    lcd.print("INICIAR ENTRENO?"); 
    lcd.setCursor(0,1);
    lcd.print("SI            NO");
  
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
  
    if ( buttonstate1 == LOW)
    {
      pitido_menu();   
      menu = 21;
      delay(250);
      menu_2_1();      
    }
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu = 1;
      delay(250);
      menu_1();
    }
  }
}

//*********************************************************************************

void menu_2_1()
{
  comprobar_transponder();
  irrecv.enableIRIn();
 
  if (empezar_crono==LOW)
  {

    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("PREPARADO...");
  
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
  }
  
  while (menu==21)
  {     
    buttonstate2=digitalRead(button2);
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu=22;
      delay(250);
      menu_2_2();      
    }
    
    if (empezar_crono == HIGH)
    { 
      tiempo=millis()-tiempo_inicio;
      m=tiempo/60000;                     
      mu=m%10;                           
      md=(m-mu)/10;                       
      s=(tiempo/1000)-(m*60);            
      su=s%10;                            
      sd=(s-su)/10;                       
      l=tiempo-(s*1000)-(m*60000);        
      lu=l%10;                           
      ld=((l-lu)/10)%10;                  
      lc=(l-(ld*10)-lu)/100;              
      lcd.setCursor(7, 0);                
      lcd.print(md);                      
      lcd.print(mu);               
      lcd.print(":");
      lcd.print(sd);
      lcd.print(su);
      lcd.print(":");
      lcd.print(lc);
      lcd.print(ld);
      lcd.print(lu);

      if (irrecv.decode(&results))       
      {      
        if(results.value==Tx)contar_vuelta(); 
        irrecv.resume();
      }
       
      buttonstate2=digitalRead(button2);
      
      if (buttonstate2 == LOW)
      {
        pitido_menu();
        menu=22;
        delay(250);
        menu_2_2();      
      }      
    }
    else
    {
      
      if (irrecv.decode(&results))       
      {
        if(results.value==Tx)contar_vuelta();      
        irrecv.resume();
      }    
    }  
  }
}
//*********************************************************************************

void menu_2_2()
{
  lcd.clear();
  menu = 22;
  while (menu == 22)
  {
    lcd.setCursor(3,0);
    lcd.print("TERMINAR??");
    lcd.setCursor(0,1);
    lcd.print("SI            NO");
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    
    if (buttonstate1==LOW)
    {
      pitido_menu();    
      menu=3;
      delay(250);
      final_entreno();
      irrecv.enableIRIn();
    }
    
    if (buttonstate2==LOW)
    {
      pitido_menu();
      menu = 21;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ttotal ");
      lcd.setCursor(7, 0);                    
      lcd.print("00:00:000");                 
      lcd.setCursor(0, 1);
      lcd.print("Uv ");
      lcd.setCursor(3,1);
      m=tiempo_vuelta/60000;                     
      mu=m%10;                            
      md=(m-mu)/10;                       
      s=(tiempo_vuelta/1000)-(m*60);             
      su=s%10;                           
      sd=(s-su)/10;                       
      l=tiempo_vuelta-(s*1000)-(m*60000);        
      lu=l%10;                           
      ld=((l-lu)/10)%10;                  
      lc=(l-(ld*10)-lu)/100;                                                         
      lcd.print(md);                      
      lcd.print(mu);               
      lcd.print(":");
      lcd.print(sd);
      lcd.print(su);
      lcd.print(":");
      lcd.print(lc);
      lcd.print(ld);
      lcd.print(lu);  
      lcd.setCursor(13,1);
      lcd.print("V");
      lcd.setCursor(14,1);
      lcd.print(numero_vueltas);      
      delay(250);
      menu_2_1();
    }   
  }
}

//*********************************************************************************

void menu_2_3()
{
  lcd.clear();
  while (menu==23)
  {
    lcd.setCursor(0,0);
    lcd.print("    RESUMEN??");
    lcd.setCursor(0,1);
    lcd.print("SI            NO");
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    
    if (buttonstate1==LOW)
    {
      pitido_menu();
      lcd.clear();
      lcd.setCursor(0,0);
      if (config_sd==1)
      {
      lcd.print("Guardando datos.");
      lcd.setCursor(0,1);
      lcd.print("   Espere...");
      guardar_datos_sd_entreno(); 
      }     
      menu=24;
      delay(250);
      menu_2_4();
    }
    
    if (buttonstate2==LOW)
    {  
      pitido_menu();   
      lcd.clear();
      lcd.setCursor(0,0);
      if(config_sd==1)
      {
      lcd.print("Guardando datos.");
      lcd.setCursor(0,1);
      lcd.print("   Espere...");
      guardar_datos_sd_entreno();
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Reiniciando...");
      delay(1000);
      Resetea();          
    }
  }  
}

//*********************************************************************************

void guardar_datos_sd_entreno()
{

if (!SD.begin(53)) 
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" NO HAY TARJETA");
  lcd.setCursor(0,1);
  lcd.print("       SD");
  delay(1000);
  return;
}  
lcd.clear();

char nombrefichero[40];
char carpeta[20];
char mes[10];
  int i = 1;
  int exist = LOW ;
  
//  while (exist == LOW)
//  {
//    sprintf(nombrefichero, "ENTRENO/%d.txt",i);
//    if (SD.exists(nombrefichero))
//    {
//      i = i+1;
//    }
//    else
//    {
//      exist = HIGH;
//    }    
//  }

//SD.mkdir("ENTRENO");

getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

if (month==1)
{
sprintf(carpeta,"ENTRENO/20%d/enero/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/enero/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==2)
{
sprintf(carpeta,"ENTRENO/20%d/febrero/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/febrero/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==3)
{
sprintf(carpeta,"ENTRENO/20%d/marzo/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/marzo/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==4)
{
sprintf(carpeta,"ENTRENO/20%d/abril/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/abril/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==5)
{
sprintf(carpeta,"ENTRENO/20%d/mayo/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/mayo/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==6)
{
sprintf(carpeta,"ENTRENO/20%d/junio/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/junio/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==7)
{
sprintf(carpeta,"ENTRENO/20%d/julio/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/julio/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==8)
{
sprintf(carpeta,"ENTRENO/20%d/agosto/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/agosto/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==9)
{
sprintf(carpeta,"ENTRENO/20%d/septiemb/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/septiemb/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==10)
{
sprintf(carpeta,"ENTRENO/20%d/octubre/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/octubre/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==11)
{
sprintf(carpeta,"ENTRENO/20%d/noviemb/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/noviemb/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}
if (month==12)
{
sprintf(carpeta,"ENTRENO/20%d/diciemb/dia_%d",year,dayOfMonth);
sprintf(nombrefichero,"ENTRENO/20%d/diciemb/dia_%d/%d_%d.txt",year,dayOfMonth,hour,minute);
}

SD.mkdir(carpeta);

myFile = SD.open(nombrefichero, FILE_WRITE);
if (myFile) 
  {
   myFile.println("******************** INICIO DEL ENTRENO ********************");
   myFile.println("");
   myFile.println("");
   myFile.print("Tiempo total del entreno: ");
   m=tiempo/60000;                     
   mu=m%10;                            
   md=(m-mu)/10;                       
   s=(tiempo/1000)-(m*60);             
   su=s%10;                           
   sd=(s-su)/10;                       
   l=tiempo-(s*1000)-(m*60000);        
   lu=l%10;                           
   ld=((l-lu)/10)%10;                  
   lc=(l-(ld*10)-lu)/100;       
   myFile.print(md);                      
   myFile.print(mu);               
   myFile.print(":");
   myFile.print(sd);
   myFile.print(su);
   myFile.print(":");
   myFile.print(lc);
   myFile.print(ld);
   myFile.println(lu); 
   myFile.println("");
   myFile.print("Total de vueltas:         ");   
   myFile.println(numero_vueltas);
   myFile.println("");
   myFile.print("Vuelta rapida - Vuelta ");
   if (numero_vuelta_rapida==0)
    {
      numero_vuelta_rapida=1;
    }
   myFile.print(numero_vuelta_rapida);
   myFile.print(" en: ");
   m=vuelta_rapida/60000;                     
   mu=m%10;                            
   md=(m-mu)/10;                       
   s=(vuelta_rapida/1000)-(m*60);             
   su=s%10;                           
   sd=(s-su)/10;                       
   l=vuelta_rapida-(s*1000)-(m*60000);        
   lu=l%10;                           
   ld=((l-lu)/10)%10;                  
   lc=(l-(ld*10)-lu)/100;                                                    
   myFile.print(md);                      
   myFile.print(mu);               
   myFile.print(":");
   myFile.print(sd);
   myFile.print(su);
   myFile.print(":");
   myFile.print(lc);
   myFile.print(ld);
   myFile.println(lu);
   myFile.println("");
   myFile.println("RESUMEN DE VUELTAS:");
   myFile.println("");
   int posicion_array = 0;
   
   while (posicion_array<numero_vueltas)
    {
     myFile.print(posicion_array+1);
     myFile.print(" -- ");
     tiempo_vuelta=guarda_vueltas[posicion_array];
     m=tiempo_vuelta/60000;                     
     mu=m%10;                            
     md=(m-mu)/10;                       
     s=(tiempo_vuelta/1000)-(m*60);             
     su=s%10;                           
     sd=(s-su)/10;                       
     l=tiempo_vuelta-(s*1000)-(m*60000);        
     lu=l%10;                           
     ld=((l-lu)/10)%10;                  
     lc=(l-(ld*10)-lu)/100;                                                    
     myFile.print(md);                      
     myFile.print(mu);               
     myFile.print(":");
     myFile.print(sd);
     myFile.print(su);
     myFile.print(":");
     myFile.print(lc);
     myFile.print(ld);
     myFile.println(lu); 
     posicion_array=posicion_array+1;
  }
   myFile.println("");
   myFile.println("********************* FIN DEL ENTRENO **********************");
   myFile.println("");
   myFile.println("");
   myFile.println("");
   myFile.close();   
  }
}

//*********************************************************************************

void menu_2_4()
{ 
  lcd.clear();
  int posicion_array = 0;
  while (menu == 24)
  {
    lcd.setCursor(0,0);
    lcd.print("Vuelta a Vuelta");
    lcd.setCursor(0,1);
    lcd.print("V:");
    lcd.print(posicion_array+1);
    lcd.setCursor(5,1);
    lcd.print("T:");    
    tiempo_vuelta=guarda_vueltas[posicion_array];
    m=tiempo_vuelta/60000;                     
    mu=m%10;                            
    md=(m-mu)/10;                       
    s=(tiempo_vuelta/1000)-(m*60);             
    su=s%10;                           
    sd=(s-su)/10;                       
    l=tiempo_vuelta-(s*1000)-(m*60000);        
    lu=l%10;                           
    ld=((l-lu)/10)%10;                  
    lc=(l-(ld*10)-lu)/100;                                    
    lcd.setCursor(7, 1);                
    lcd.print(md);                      
    lcd.print(mu);               
    lcd.print(":");
    lcd.print(sd);
    lcd.print(su);
    lcd.print(":");
    lcd.print(lc);
    lcd.print(ld);
    lcd.print(lu);
  
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
  
    if (buttonstate2==LOW)
    {
      pitido_menu();
      menu=242;
      delay(250);
      menu_2_4_2();
    }
  
    if (buttonstate3 == LOW)
    {
      if (posicion_array<numero_vueltas-1)
      {
        
       pitido_menu();
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Vuelta a Vuelta");
       lcd.setCursor(0,1);
       lcd.print("V:");
       lcd.print(posicion_array+1);
       lcd.setCursor(5,1);
       lcd.print("T:");
       posicion_array++;
       tiempo_vuelta=guarda_vueltas[posicion_array];
        m=tiempo_vuelta/60000;                     
        mu=m%10;                            
        md=(m-mu)/10;                       
        s=(tiempo_vuelta/1000)-(m*60);             
        su=s%10;                           
        sd=(s-su)/10;                       
        l=tiempo_vuelta-(s*1000)-(m*60000);        
        lu=l%10;                           
        ld=((l-lu)/10)%10;                  
        lc=(l-(ld*10)-lu)/100;                                    
        lcd.setCursor(7, 1);                
        lcd.print(md);                      
        lcd.print(mu);               
        lcd.print(":");
        lcd.print(sd);
        lcd.print(su);
        lcd.print(":");
        lcd.print(lc);
        lcd.print(ld);
        lcd.print(lu);   
        delay(150);
       }
     }
     
   if (buttonstate4 == LOW)
   {
     if (posicion_array>0)
     {
       pitido_menu();       
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Vuelta a Vuelta");
       lcd.setCursor(0,1);
       lcd.print("V:");
       lcd.print(posicion_array+1);
       lcd.setCursor(5,1);
       lcd.print("T:");
       posicion_array=posicion_array-1;
       tiempo_vuelta=guarda_vueltas[posicion_array];
        m=tiempo_vuelta/60000;                     
        mu=m%10;                            
        md=(m-mu)/10;                       
        s=(tiempo_vuelta/1000)-(m*60);             
        su=s%10;                           
        sd=(s-su)/10;                       
        l=tiempo_vuelta-(s*1000)-(m*60000);        
        lu=l%10;                           
        ld=((l-lu)/10)%10;                  
        lc=(l-(ld*10)-lu)/100;                                    
        lcd.setCursor(7, 1);                
        lcd.print(md);                      
        lcd.print(mu);               
        lcd.print(":");
        lcd.print(sd);
        lcd.print(su);
        lcd.print(":");
        lcd.print(lc);
        lcd.print(ld);
        lcd.print(lu);       
        delay(150);
       }
     }
  } 
}

//*********************************************************************************

void menu_2_4_2()
{
  lcd.clear();
  while (menu==242)
  {
    lcd.setCursor(0,0);  
    lcd.print(" DESEAS SALIR??");
    lcd.setCursor(0,1);
    lcd.print("SI            NO");
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
 
    if (buttonstate1==LOW)
   {     
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Reiniciando...");
      delay(1000);
      Resetea();           
    }   
    if (buttonstate2==LOW)
    {
     pitido_menu();
     menu = 24;
     delay(250);
     menu_2_4();
    }   
  }
  irrecv.enableIRIn();
}

//*********************************************************************************

void contar_vuelta()
{
  if (empezar_crono == LOW)
  {
    empezar_crono = HIGH;
    tiempo_inicio = millis();
    pitido_inicio_entreno();
    lcd.setCursor(0, 0);
    lcd.print("Ttotal ");
    lcd.setCursor(7, 0);                    
    lcd.print("00:00:000");                 
    lcd.setCursor(0, 1);
    lcd.print("Uv ");
    lcd.setCursor(3,1);
    lcd.print("00:00:000");
    lcd.setCursor(13,1);
    lcd.print("V");
    lcd.setCursor(14,1);
    lcd.print("0 ");
    
    const char *msg = "Inicio de entreno";
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone 
    
    Serial.println("0    00:00:000");   
  }
  else
  {
   tiempo_vuelta =(tiempo)-(tiempo_parcial);
   
   if (tiempo_vuelta<tiempo_minimo_vuelta)
   {          
    pitido_error_vuelta();
   } 
   else
   {       
     m=tiempo_vuelta/60000;                     
     mu=m%10;                            
     md=(m-mu)/10;                       
     s=(tiempo_vuelta/1000)-(m*60);             
     su=s%10;                           
     sd=(s-su)/10;                       
     l=tiempo_vuelta-(s*1000)-(m*60000);        
     lu=l%10;                           
     ld=((l-lu)/10)%10;                  
     lc=(l-(ld*10)-lu)/100;                                    
     lcd.setCursor(3, 1);                
     lcd.print(md);                      
     lcd.print(mu);               
     lcd.print(":");
     lcd.print(sd);
     lcd.print(su);
     lcd.print(":");
     lcd.print(lc);
     lcd.print(ld);
     lcd.print(lu);
     tiempo_parcial = tiempo;
     int v; 
     numero_vueltas++;
     v = numero_vueltas;
     lcd.setCursor(14,1);
     lcd.print(numero_vueltas);
     pitido_vuelta();
     control_vuelta_rapida=tiempo_vuelta;
     
    char msg[16];
    char msg2[16];
    sprintf (msg,"VUv-%i%i:%i%i:%i%i%i V%i",md,mu,sd,su,lc,ld,lu,numero_vueltas); 
    //sprintf (msg2,"%i V-%i",msg,numero_vueltas);  
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    if (numero_vueltas<10)
    {
    sprintf (msg2,"%i    %i%i:%i%i:%i%i%i",numero_vueltas,md,mu,sd,su,lc,ld,lu);
    Serial.println(msg2);
    }
    else
    {
    sprintf (msg2,"%i   %i%i:%i%i:%i%i%i",numero_vueltas,md,mu,sd,su,lc,ld,lu);
    Serial.println(msg2);      
    }
     if(numero_vueltas<128)
     {
       int posicion_array;
       posicion_array=numero_vueltas-1;
       guarda_vueltas[posicion_array]=tiempo_vuelta;  
       if (vuelta_rapida==0)
       {
         vuelta_rapida=control_vuelta_rapida;
         char msg[16];
         char msg2[16];
         sprintf (msg,"RUv-%i%i:%i%i:%i%i%i V%i",md,mu,sd,su,lc,ld,lu,numero_vueltas);  
         vw_send((uint8_t *)msg, strlen(msg));
         vw_wait_tx(); // Wait until the whole message is gone 
         pitido_vuelta_rapida();

       }
       else
       {
         if (control_vuelta_rapida<vuelta_rapida)
         {
           vuelta_rapida=control_vuelta_rapida;
           numero_vuelta_rapida=numero_vueltas;
         char msg[16];
         sprintf (msg,"RUv-%i%i:%i%i:%i%i%i V%i",md,mu,sd,su,lc,ld,lu,numero_vueltas);  
         vw_send((uint8_t *)msg, strlen(msg));
         vw_wait_tx(); // Wait until the whole message is gone 

           pitido_vuelta_rapida();
           
         }
       }      
     }  
     if (tiempo_entrenos>0)
     {
       if ( tiempo >= tiempo_entrenos)
        {
          menu=3;
          
         if (numero_vuelta_rapida==0)
         {
          numero_vuelta_rapida=1;
         }
         m=vuelta_rapida/60000;                     
         mu=m%10;                            
         md=(m-mu)/10;                       
         s=(vuelta_rapida/1000)-(m*60);             
         su=s%10;                           
         sd=(s-su)/10;                       
         l=vuelta_rapida-(s*1000)-(m*60000);        
         lu=l%10;                           
         ld=((l-lu)/10)%10;                  
         lc=(l-(ld*10)-lu)/100;                 
          
         char msg[16]; 
         sprintf (msg,"FVR-%i%i:%i%i:%i%i%i V%i",md,mu,sd,su,lc,ld,lu,numero_vuelta_rapida);
         vw_send((uint8_t *)msg, strlen(msg));
         vw_wait_tx(); // Wait until the whole message is gone 
         Serial.println(msg);        
         final_entreno();
        }
     }
        
     delay(100);
    }  
  }
  irrecv.enableIRIn();
}

//*********************************************************************************

void pitido_menu()
{
  if(config_beep==1)
  {
   tone(34,1000,150);
   delay(150);
   noTone(34); 
  }
 irrecv.enableIRIn();
  
}

//*********************************************************************************

void pitido_deteccion()
{
  if(config_beep==1)
  {
   tone(34,1500,150);
   delay(150);
   noTone(34); 
  }
 irrecv.enableIRIn();
  
}

//*********************************************************************************

void pitido_vuelta()
{
  if (config_beep==1)
  {
     tone(34,750,250);
     delay(250);
     noTone(34);
  }
   irrecv.enableIRIn();
}

//*********************************************************************************

void pitido_error_vuelta()
{
  if (config_beep==1)
  {
     tone(34,700,150);
     delay(150);
     noTone(34);
     tone(34,700,150);
     delay(150);
     noTone(34);
     tone(34,700,150);
     delay(150);
     noTone(34);     
  }
   irrecv.enableIRIn(); 
}

//*********************************************************************************

void pitido_inicio_entreno()
{
  if (config_beep==1)
  {
   tone(34,250,1000);
   delay(1000);
   noTone(34); 
   tone(34,500,500);
   delay(500);
   noTone(34); 
   tone(34,750,300);
   delay(300);
   noTone(34); 
  }
 irrecv.enableIRIn();
}

//*********************************************************************************

void pitido_error()
{
  if(config_beep==1)
  {
    tone(34,600,1000);
    delay(1200);
    noTone(34);
  }
}

//*********************************************************************************

void pitido_vuelta_rapida()
{
  if (config_beep==1)
  {
   tone(34,500,250);
   delay(300);
   noTone(34); 
   tone(34,500,350);
   delay(400);
   noTone(34); 
   tone(34,500,450);
   delay(500);
   noTone(34); 
  }
  irrecv.enableIRIn();
}

//*********************************************************************************

void pitido_borrar_memoria()
{
  if (config_beep==1)
  {
   tone(34,1000,250);
   delay(300);
   noTone(34); 
   tone(34,1000,250);
   delay(300);
   noTone(34); 
   tone(34,1000,250);
   delay(300);
   noTone(34); 
  }
}

//*********************************************************************************
void borrar_array()
{  
  for (int i=0;i<200;i++)
  {
    guarda_vueltas[i]=0;
  }  
}

//*********************************************************************************

void final_entreno()
{
  int i=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    ENTRENO");
  lcd.setCursor(0,1);
  lcd.print("   TERMINADO");
  pitido_inicio_entreno();
  while (menu==3)
  {  
    i=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  TIEMPO TOTAL");
    lcd.setCursor(3,1);
    m=tiempo/60000;                     
    mu=m%10;                            
    md=(m-mu)/10;                       
    s=(tiempo/1000)-(m*60);             
    su=s%10;                           
    sd=(s-su)/10;                       
    l=tiempo-(s*1000)-(m*60000);        
    lu=l%10;                           
    ld=((l-lu)/10)%10;                  
    lc=(l-(ld*10)-lu)/100;                                                    
    lcd.print(md);                      
    lcd.print(mu);               
    lcd.print(":");
    lcd.print(sd);
    lcd.print(su);
    lcd.print(":");
    lcd.print(lc);
    lcd.print(ld);
    lcd.print(lu);
    
    while (i<=3000)
    {
    buttonstate2=digitalRead(button2);
    if(buttonstate2==LOW)
    {
        pitido_menu();
       menu=23;
       delay(250);
       menu_2_3();
    } 
    delay(50);
    i=i+50;    
    }
    i=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("TOTAL DE VUELTAS");
    lcd.setCursor(6,1);
    lcd.print(numero_vueltas);
    
    while (i<=3000)
    {
    buttonstate2=digitalRead(button2);
    if(buttonstate2==LOW)
    {
        pitido_menu();
       menu=23;
       delay(250);
       menu_2_3();
    } 
    delay(50);
    i=i+50;
    }
    i=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  VUELTA RAPIDA");
    lcd.setCursor(0,1);
    lcd.print("V-");
    if (numero_vuelta_rapida==0)
    {
      numero_vuelta_rapida=1;
    }
    lcd.print(numero_vuelta_rapida);
    lcd.setCursor(7,1);
    m=vuelta_rapida/60000;                     
    mu=m%10;                            
    md=(m-mu)/10;                       
    s=(vuelta_rapida/1000)-(m*60);             
    su=s%10;                           
    sd=(s-su)/10;                       
    l=vuelta_rapida-(s*1000)-(m*60000);        
    lu=l%10;                           
    ld=((l-lu)/10)%10;                  
    lc=(l-(ld*10)-lu)/100;                                                    
    lcd.print(md);                      
    lcd.print(mu);               
    lcd.print(":");
    lcd.print(sd);
    lcd.print(su);
    lcd.print(":");
    lcd.print(lc);
    lcd.print(ld);
    lcd.print(lu);
    
    while (i<=3000)
    {
    buttonstate2=digitalRead(button2);
    if(buttonstate2==LOW)
    {
        pitido_menu();
       menu=23;
       delay(250);
       menu_2_3();
    } 
    delay(50);
    i=i+50;    
    }     
  }
}

//*********************************************************************************

void comprobar_transponder()   // Despues de leer un codigo, podemos sutituir los codigos de esta lista por el leido. Ejp. Tx=0xE01 (codigo leido de un mando de euskaltel)
{
  if (config_numero_transponder==1)
  {
    Tx=0x10;
  }
  
  if (config_numero_transponder==2)
  {
    Tx=0x810;
  }
  if (config_numero_transponder==3)
  {
    Tx=0x410;
  }
  if (config_numero_transponder==4)
  {
    Tx=0xC10;
  }  
  if (config_numero_transponder==5)
  {
    Tx=0x210;
  }
  if (config_numero_transponder==6)
  {
    Tx=0xA10;
  }
  if (config_numero_transponder==7)
  {
    Tx=0x610;
  }
  if (config_numero_transponder==8)
  {
    Tx=0xE10;
  }
  if (config_numero_transponder==9)
  {
    Tx=0x110;
  }
  if (config_numero_transponder==10)
  {
    Tx=0x910;
  }
  if (config_numero_transponder==11)
  {
    Tx=0x510;
  }
  if (config_numero_transponder==12)
  {
    Tx=0xD10;
  }
  if (config_numero_transponder==13)
  {
    Tx=0x310;
  }
  if (config_numero_transponder==14)
  {
    Tx=0xB10;
  }
  if (config_numero_transponder==15)
  {
    Tx=0x710;
  }
  if (config_numero_transponder==16)
  {
    Tx=0xF10;
  }
  if (config_numero_transponder==17)
  {
    Tx=0x90;
  }
  if (config_numero_transponder==18)
  {
    Tx=0x890;
  }
  if (config_numero_transponder==19)
  {
    Tx=0x490;
  }
  if (config_numero_transponder==20)
  {
    Tx=0xC90;
  }
irrecv.enableIRIn();
}


//*********************************************************************************

void leer_codigo()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" LEYENDO CODIGO");
    lcd.setCursor(3,1);
    
    irrecv.enableIRIn();
 while (menu== 100)
{ 
   if (irrecv.decode(&results))       
    {  
      lcd.setCursor(3,1);
      lcd.print("               ");
      lcd.setCursor(3,1);
      lcd.print( results.value,HEX);
      delay (100);
      irrecv.resume();
    }
}

}



//*********************************************************************************

void setDateDs1307(byte second,        // 0-59
                   byte minute,        // 0-59
                   byte hour,          // 1-23
                   byte dayOfWeek,     // 1-7
                   byte dayOfMonth,    // 1-28/29/30/31
                   byte month,         // 1-12
                   byte year){          // 0-99

  Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(0);
    Wire.write(decToBcd(second));    // 0 to bit 7 starts the clock
    Wire.write(decToBcd(minute));
    Wire.write(decToBcd(hour));      // If you want 12 hour am/pm you need to set
                                    // bit 6 (also need to change readDateDs1307)
    Wire.write(decToBcd(dayOfWeek));
    Wire.write(decToBcd(dayOfMonth));
    Wire.write(decToBcd(month));
    Wire.write(decToBcd(year));
  Wire.endTransmission();
}


//*********************************************************************************


void getDateDs1307(byte *second,
                   byte *minute,
                   byte *hour,
                   byte *dayOfWeek,
                   byte *dayOfMonth,
                   byte *month,
                   byte *year){
                     
// Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

// A few of these need masks because certain bits are control bits
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3f);  // Need to change this if 
                                                      // 12 hour am/pm
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}

//*********************************************************************************

void mostrar_hora()
{

  while (menu==30)
  {
    getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  
    lcd.clear();
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    lcd.setCursor(0,0);
      int dia=(dayOfWeek);
      if (dia==1)
      {
        lcd.print("Lun");
      }
      if (dia==2)
      {
        lcd.print("Mar");
      }
      if (dia==3)
      {
        lcd.print("Mie");
      }
      if (dia==4)
      {
        lcd.print("Jue");
      }
      if (dia==5)
      {
        lcd.print("Vie");
      }
      if (dia==6)
      {
        lcd.print("Sab");
      }
      if (dia==7)
      {
        lcd.print("Dom");
      }  
         
      lcd.setCursor(4,0);
      if (dayOfMonth<10)
      {
        lcd.print("0");
      }
      lcd.print(dayOfMonth,DEC);
      lcd.setCursor(6,0);
      lcd.print("/");
      lcd.setCursor(7,0);
      
      if (month==1)
      {
       lcd.print("ENE"); 
      }
       if (month==2)
      {
       lcd.print("FEB"); 
      }
      if (month==3)
      {
       lcd.print("MAR"); 
      }
      if (month==4)
      {
       lcd.print("ABR"); 
      }
      if (month==5)
      {
       lcd.print("MAY"); 
      }
      if (month==6)
      {
       lcd.print("JUN"); 
      }
      if (month==7)
      {
       lcd.print("JUL"); 
      }
      if (month==8)
      {
       lcd.print("AGO"); 
      }
      if (month==9)
      {
       lcd.print("SEP"); 
      }
      if (month==10)
      {
       lcd.print("OCT"); 
      }
      if (month==11)
      {
       lcd.print("NOV"); 
      }
      if (month==12)
      {
       lcd.print("DIC"); 
      } 
      lcd.setCursor(10,0);
      lcd.print("/");
      lcd.setCursor(11,0);
      /*
      if (month<10)
      {
        lcd.print("0");
      }
      lcd.print(month,DEC);
      lcd.setCursor(8,0);
      lcd.print("/");
      lcd.setCursor(9,0);
      */
      lcd.print("20");
      if (year<10)
      {
        lcd.print("0");
      }
      lcd.print(year,DEC);
      
      lcd.setCursor(4,1);
      if (hour<10)
      {
        lcd.print("0");
      }
      lcd.print(hour,DEC);
      lcd.setCursor(6,1);
      lcd.print(":");
      lcd.setCursor(7,1);
      if (minute<10)
      {
        lcd.print("0");
      }
      lcd.print(minute,DEC);
      lcd.setCursor(9,1);
      lcd.print(":");
      lcd.setCursor(10,1);
      if (second<10)
      {
        lcd.print("0");
      }
      lcd.print(second,DEC);
      
    if ( buttonstate1 == LOW)
    {
      pitido_menu();   
      menu = 1;
      delay(250);
      menu_1();      
    }
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu = 1;
      delay(250);
      menu_1();
    }
      
      delay(1000);
    } 
 }

//**********************************************************************
void menu_3_1()
{
  lcd.clear();
  while (menu == 31)
  {
    lcd.setCursor(0,0);
    lcd.print(" AJUSTAR RELOJ?"); 
    lcd.setCursor(0,1);
    lcd.print("SI            NO");
  
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
  
    if ( buttonstate1 == LOW)
    {
      pitido_menu();   
      menu = 32;
      delay(250);
      menu_3_2();      
    }
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu = 181;
      delay(250);
      menu_1_8_1();
    }
  }
}
//**********************************************************************

void menu_3_2()
{
  menu =32;
  lcd.clear();
  getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  while (menu == 32)
  {
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    lcd.setCursor(0,0);
    lcd.print("AJUSTE DEL RELOJ"); 
    lcd.setCursor(0,1); 
    lcd.print("ANO:");
    lcd.setCursor(12,1);
    lcd.print("20");
    lcd.print(year);
    if (year == 0)
    {
      lcd.print("0");
    }
    if (buttonstate3 == LOW)
    {
      if (year <= 99)
      {
      pitido_menu();
      year=year+1;
      if(year == 100)
      {
        year = 0;
      }
      delay(250);
      }
    }
    
    if (buttonstate4 == LOW)
    {
       if (year>0)
       {
        pitido_menu();
        year=year-1;
        if (year < 0)
        {
          year=99;
        }
        delay(250);
       }
    }
    
    if ( buttonstate1 == LOW)
    {
      pitido_menu();   
      menu = 31;
      delay(250);
      menu_3_1();      
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu = 33;
      delay(250);
      menu_3_3();
    }
    
  }
}

//**********************************************************************

void menu_3_3()
{
  lcd.clear();
  while (menu == 33)
  {
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    lcd.setCursor(0,0);
    lcd.print("AJUSTE DEL RELOJ"); 
    lcd.setCursor(0,1); 
    lcd.print("MES:");
    lcd.setCursor(13,1);
    if (month==1)
    {
      lcd.print("ENE");
    }
    if (month==2)
    {
      lcd.print("FEB");
    }
    if (month==3)
    {
      lcd.print("MAR");
    } 
    if (month==4)
    {
      lcd.print("ABR");
    }  
    if (month==5)
    {
      lcd.print("MAY");
    }
    if (month==6)
    {
      lcd.print("JUN");
    }
    if (month==7)
    {
      lcd.print("JUL");
    }
    if (month==8)
    {
      lcd.print("AGO");
    } 
    if (month==9)
    {
      lcd.print("SEP");
    } 
    if (month==10)
    {
      lcd.print("OCT");
    } 
    if (month==11)
    {
      lcd.print("NOV");
    }
    if (month==12)
    {
      lcd.print("DIC");
    }    
    if (buttonstate3 == LOW)
    {
      if (month <= 12)
      {
      pitido_menu();
      month=month+1;
      if(month == 13)
      {
        month = 1;
      }
      delay(250);
      }
    }
    
    if (buttonstate4 == LOW)
    {
      if (month > 1)
     {
       if (month>=1)
       {
        pitido_menu();
        month=month-1;
        if (month == 0)
        {
          month=12;
        }
        delay(250);
       }
     } 
    }
    
    if ( buttonstate1 == LOW)
    {
      pitido_menu();   
      menu = 32;
      delay(250);
      menu_3_2();      
    }
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu = 34;
      delay(250);
      menu_3_4();
    }
    
  }  
}

//**********************************************************************

void menu_3_4()
{
  lcd.clear();
  while (menu==34)
  {
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    lcd.setCursor(0,0);
    lcd.print("AJUSTE DEL RELOJ");  
    lcd.setCursor(0,1); 
    lcd.print("DIA MES:");
    lcd.setCursor(14,1);
    lcd.print("  ");
    lcd.setCursor(14,1);
    lcd.print(dayOfMonth);
    
    if (buttonstate3 == LOW)
    {
      if (dayOfMonth <= 30)
      {
      pitido_menu();
      dayOfMonth=dayOfMonth+1;
      delay(250);
      }
    }
    
    if (buttonstate4 == LOW)
    {
       if (dayOfMonth>1)
       {
        pitido_menu();
        dayOfMonth=dayOfMonth-1;
        delay(250);
       }
    }
    
    if ( buttonstate1 == LOW)
    {
      pitido_menu();   
      menu = 33;
      delay(250);
      menu_3_3();      
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu = 35;
      delay(250);
      menu_3_5();
    }  
    
  }
}

//**********************************************************************

void menu_3_5()
{
    lcd.clear();
    while (menu==35)
    {
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    lcd.setCursor(0,0);
    lcd.print("AJUSTE DEL RELOJ");  
    lcd.setCursor(0,1);
    lcd.print("DIA SEMANA");
    lcd.setCursor(13,1);
    if (dayOfWeek==1)
    {
      lcd.print("LUN");
    }
    if (dayOfWeek==2)
    {
      lcd.print("MAR");
    }
    if (dayOfWeek==3)
    {
      lcd.print("MIE");
    }
    if (dayOfWeek==4)
    {
      lcd.print("JUE");
    }
    if (dayOfWeek==5)
    {
      lcd.print("VIE");
    }
    if (dayOfWeek==6)
    {
      lcd.print("SAB");
    }
    if (dayOfWeek==7)
    {
      lcd.print("DOM");
    }
   
    if (buttonstate3 == LOW)
    {
      if (dayOfWeek <= 7)
      {
      pitido_menu();
      dayOfWeek=dayOfWeek+1;
      if(dayOfWeek == 8)
      {
        dayOfWeek = 1;
      }
      delay(250);
      }
    }
    
    if (buttonstate4 == LOW)
    {
       if (dayOfWeek>0)
       {
        pitido_menu();
        dayOfWeek=dayOfWeek-1;
        if (dayOfWeek == 0)
        {
          dayOfWeek=7;
        }
        delay(250);
       }
    }
    
    if ( buttonstate1 == LOW)
    {
      pitido_menu();   
      menu = 34;
      delay(250);
      menu_3_4();      
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu = 36;
      delay(250);
      menu_3_6();
    }     
   
    }
   
}

//**********************************************************************

void menu_3_6()
{
  lcd.clear();
  while (menu==36)
  {
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    lcd.setCursor(0,0);
    lcd.print("AJUSTE DEL RELOJ");  
    lcd.setCursor(0,1);
    lcd.print("HORA");
    lcd.setCursor(14,1);
    lcd.print("  ");
    lcd.setCursor(14,1);    
    lcd.print(hour);
    
    if (buttonstate3 == LOW)
    {
      if (hour <= 22)
      {
      pitido_menu();
      hour=hour+1;
      delay(250);
      }
    }
    
    if (buttonstate4 == LOW)
    {
       if (hour>0)
       {
        pitido_menu();
        hour=hour-1;
        delay(250);
       }
    }
    
    if ( buttonstate1 == LOW)
    {
      pitido_menu();   
      menu = 35;
      delay(250);
      menu_3_5();      
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      menu = 37;
      delay(250);
      menu_3_7();
    }     
    
    
  }
}

//**********************************************************************

void menu_3_7()
{
  lcd.clear();
  while (menu==37)
  {
    buttonstate1=digitalRead(button1);
    buttonstate2=digitalRead(button2);
    buttonstate3=digitalRead(button3);
    buttonstate4=digitalRead(button4);
    lcd.setCursor(0,0);
    lcd.print("AJUSTE DEL RELOJ");  
    lcd.setCursor(0,1);
    lcd.print("MINUTOS");
    lcd.setCursor(14,1);
    lcd.print("  ");
    lcd.setCursor(14,1);
    lcd.print(minute);
    
    if (buttonstate3 == LOW)
    {
      if (minute <= 58)
      {
      pitido_menu();
      minute=minute+1;
      delay(250);
      }
    }
    
    if (buttonstate4 == LOW)
    {
       if (minute>0)
       {
        pitido_menu();
        minute=minute-1;
        delay(250);
       }
    }
    
    if ( buttonstate1 == LOW)
    {
      pitido_menu();   
      menu = 36;
      delay(250);
      menu_3_6();      
    }
    
    if (buttonstate2 == LOW)
    {
      pitido_menu();
      lcd.clear();
      lcd.print("Actualizando hora...");
      second=0;
      setDateDs1307(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
      delay(500);
      lcd.clear();
      menu = 181;
      delay(250);
      menu_1_8_1();
    }     
    
    
  }
}

//**********************************************************************

