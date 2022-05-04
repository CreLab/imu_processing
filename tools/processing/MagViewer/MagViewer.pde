/*  Processing Code to draw circles from MPU-9250 data
* draws three circles that should overlap perfectly if data is
* properly offset and scaled.
* Use the Modern Device CalibrateMPU9250() sketch in your Arduino,
* Teensy, or clone.
* Notes:
* Serial at 57600 - set your Arduino sketch appropriately
* A float of value 1.11 is used as a header
* Arduino sketch needs to send this header
*/

import processing.serial.*;

float[] M = new float[3];
float[] q = new float[4];
float pitch = 0.00;
float roll = 0.00;
float yaw = 0.00;
float[] A = new float[3];
float[] G = new float[3];
float heading = 0.00;

int maxPoints = 20000;
int i;

float cVal;
Serial myPort;

int numValues = 3; // number of input values or sensors
// * change this to match how many values your Arduino is sending *

float[] values = new float[numValues];
int[] min = new int[numValues];
int[] max = new int[numValues];
color[] valColor = new color[numValues];

void setup()
{
    size(800, 800);
    background(0);
    noFill();
    stroke(255);
    strokeWeight(2);

    myPort = new Serial(this, "COM4", 115200);
    myPort.bufferUntil('\n');
    
    background(20);
    colorMode(HSB, 300);
    setupGrid();
    
    // initialize:
    // *edit these* to match how many values you are reading, and what colors you like 
    values[0] = 0;
    min[0] = 0;
    max[0] = 1023; // full range example, e.g. any analogRead
    valColor[0] = color(255, 0, 0); // red
  
    values[1] = 0; 
    min[1] = 0;
    max[1] = 700;  // partial range example, e.g. IR distance sensor
    valColor[1] = color(0, 255, 0); // green
  
    values[2] = 0;
    min[2] = 0;
    max[2] = 1;    // digital input example, e.g. a button
    valColor[2] = color(0, 0, 255); // blue
}

void draw()
{
    stroke(180);
    strokeWeight(2);

    float Xval = M[0];        
    float Yval = M[1];      
    float Zval = M[2];

    stroke(55, 250, 300);
    point(Xval + width/2, Yval + height/2);
    stroke(130, 250, 300);
    point(Yval + width/2, Zval + height/2);
    stroke(280, 250, 300);
    point(Xval + width/2, Zval + height/2);
}

void serialEvent(Serial myPort) { 
  try {
    // get the ASCII string:
    String inString = myPort.readStringUntil('\n');
    //println("raw: \t" + inString); // <- uncomment this to debug serial input from Arduino

    if (inString != null) {
      // trim off any whitespace:
      //println(inString);
      inString = trim(inString);

      // split the string on the delimiters and convert the resulting substrings into an float array:
      values = float(splitTokens(inString, ",")); // delimiter can be comma space or tab

      // if the array has at least the # of elements as your # of sensors, you know
      //   you got the whole data packet.
      if (values.length >= numValues)
      {
        if ((int)values[0] == 65535)
        {
          M[0] = values[1];
          M[1] = values[2];
          M[2] = values[3];
          
          q[0] = values[4];
          q[1] = values[5];
          q[2] = values[6];
          q[3] = values[7];
          
          pitch = values[8];
          roll = values[9];
          yaw = values[10];
          
          A[0] = values[11];
          A[1] = values[12];
          A[2] = values[13];
          
          G[0] = values[14];
          G[1] = values[15];
          G[2] = values[16];
          
          heading = values[17];
        }
      }
    }
  }
  catch(RuntimeException e) {
    // only if there is an error:
    e.printStackTrace();
  }
}

void keyPressed()
{  // spacebar erases drawing
    if (key == ' ')
    {
        background(20);
        setupGrid();
    }
}

void setupGrid()
{
    fill(55, 300, 300);
    textSize(18);
    text("XY", width - 40, height - 100);
    fill(130, 300, 300);
    text("YZ", width - 40, height - 70);
    fill(280, 100, 300);
    text("XZ", width - 40, height - 40);
    stroke(0, 50, 50);
    
    for (int i =0; i < width; i+=(width/8))
    {
        if (i == width/2)
        {
            stroke(0, 50, 100);
        }
        else
        {
            stroke(0, 50, 50);
        }
        line(i, 0, i, height);
        fill(0, 0, 250);
        text(i, i, 20);
    }
    
    for (int i =0; i < height; i+=(height/8))
    {
        if (i == height/2)
        {
            stroke(0, 50, 100);
        }
        else
        {
            stroke(0, 50, 50);
        }
        
        line(0, i, width, i);
        fill(0, 0, 250);
        text(i, 20, i);
    }
}
