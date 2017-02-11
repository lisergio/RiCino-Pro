import processing.serial.*;
import ddf.minim.*;

Serial myPort;      
int fila=15;
AudioPlayer player;
Minim minim;
int ancho = 800;
int alto = 300;
PFont f; 
PFont f2;
String letras = "";
int puerto;
String[] port_list;
int num_port;
char num;

void setup()
{
  f = createFont("Arial",16,true);
  f2 = createFont("Arial Black",16,true);
  num_port = Serial.list().length;
  background(0,0,0);
  size(ancho,alto);
  int y=80;
  fill(255,255,255);
  textFont(f2,30);
  text("Selecciona un puerto", 50, 50);
  textFont(f,15);
  for (int i=0;i<num_port;i++)
  {   
    text(i,50,y);
    text("-",70,y);
    text(Serial.list()[i],90,y);
    y=y+20;
  }
}


void setup2 () 
{
background(0,0,0);
size(ancho, alto);   
f = createFont("Arial",16,true);
f2 = createFont("Arial Black",16,true);
minim = new Minim(this);
myPort = new Serial(this, Serial.list()[puerto], 9600);
myPort.bufferUntil('\n');
fill(255,0,0);
rect(10,77,163,150);
fill(255,0,0);
rect(183,77,607,150);
fill(255,255,255);
textFont(f2,30);
text("VUELTAS",13,60);
text("TIEMPO ULTIMA VUELTA",270,60);
fill(183,183,183);
textFont(f2,45);
text("RiCino Pro 1.1 Live View",100,285);
fill(255,255,255);
textFont(f,110);
text("0    00:00:000",30,190);
}

void draw () 
{

}

void serialEvent (Serial myPort)
{
String inString = myPort.readStringUntil('\n');
player = minim.loadFile("beep.mp3");
player.play();
fill(0,255,0);
rect(10,77,163,150);
fill(0,255,0);
rect(183,77,607,150);  
fill(0,0,0);//negro
textFont(f,110); 
text(inString,30,190);
delay(100);
}

void keyPressed() 
{
 
if (key == '0')
  {
    puerto=0;
    setup2();
  }
if (key == '1')
  {
    puerto=1;
    setup2();
  }
if (key == '2')
  {
    puerto=2;
    setup2();
  }
if (key == '3')
  {
    puerto=3;
    setup2();
  }
if (key == '4')
  {
    puerto=4;
    setup2();
  }
if (key == '5')
  {
    puerto=5;
    setup2();
  } 
if (key == '6')
  {
    puerto=6;
    setup2();
  }
if (key == '7')
  {
    puerto=7;
    setup2();
  } 
if (key == '8')
  {
    puerto=8;
    setup2();
  }
if (key == '9')
  {
    puerto=9;
    setup2();
  }  
}



