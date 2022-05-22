import processing.serial.*;
Serial myPort;

String data = "";
String inString = "";
PrintWriter output;

int numValues = 6; // number of input values or sensors
// * change this to match how many values your Arduino is sending *

float[] values = new float[numValues];
float[] A = new float[3];
float[] M = new float[3];
int[] min = new int[3];
int[] max = new int[3];

void setup()
{
    size(800, 800);
    colorMode(RGB);
    background(0);
    noFill();
    stroke(255);
    strokeWeight(2);
    textSize(18);
    
    background(20);
    setupGrid();
    
    // initialize:
    // *edit these* to match how many values you are reading, and what colors you like 
    min[0] = -20;
    max[0] = 20; // full range example, e.g. any analogRead
  
    min[1] = -20;
    max[1] = 20;  // partial range example, e.g. IR distance sensor
  
    min[2] = -20;
    max[2] = 20;    // digital input example, e.g. a button
    
    output = createWriter("imuOutput.csv");
    myPort = new Serial(this, "COM4", 115200);
    myPort.bufferUntil('\n');
}

void keyPressed()
{
    output.flush();
    output.close();
    exit();
}

void draw()
{   
    stroke(180);
    strokeWeight(2);

    float factor = 2.5;

    float XvalW = ((M[0] * factor) + width/2);        
    float YvalW = ((M[1] * factor) + width/2);      
       
    float YvalH = ((M[1] * factor) + height/2);      
    float ZvalH = ((M[2] * factor) + height/2);

    stroke(0, 0, 255);
    point(XvalW, YvalH);
    print(XvalW + " " + YvalH + " ");
    
    stroke(255, 179, 0);
    point(XvalW, ZvalH);
    print(XvalW + " " + ZvalH + " ");
    
    stroke(0, 255, 0);
    point(YvalW, ZvalH);
    print(YvalW + " " + ZvalH + " ");
       
    print(data);
}

void serialEvent(Serial myPort)
{ 
    data = myPort.readStringUntil('\n');
    if (data != null)
    {
      output.print(data);
    }
    
    if (data != null)
    {
      // trim off any whitespace:
      //println(inString);
      inString = trim(data);

      // split the string on the delimiters and convert the resulting substrings into an float array:
      values = float(splitTokens(inString, ";")); // delimiter can be comma space or tab

      // if the array has at least the # of elements as your # of sensors, you know
      //   you got the whole data packet.
      if (values.length >= numValues)
      {
          A[0] = values[0];
          A[1] = values[1];
          A[2] = values[2];
          
          M[0] = values[3];
          M[1] = values[4];
          M[2] = values[5];
      }
    }
}

void setupGrid()
{
    fill(0, 0, 255);
    text("XY", width - 40, height - 100);
    
    fill(255, 179, 0);
    text("XZ", width - 40, height - 70);
    
    fill(0, 255, 0);
    text("YZ", width - 40, height - 40);
    
    stroke(50, 50, 50);
    
    for (int i =0; i < width; i+=(width/8))
    {
        if (i == width/2)
        {
            stroke(100, 100, 100);
        }
        else
        {
            stroke(50, 50, 50);
        }
        line(i, 0, i, height);
        fill(250, 250, 250);
        text(i, i, 20);
    }
    
    for (int i =0; i < height; i+=(height/8))
    {
        if (i == height/2)
        {
            stroke(100, 100, 100);
        }
        else
        {
            stroke(50, 50, 50);
        }
        
        line(0, i, width, i);
        fill(250, 250, 250);
        text(i, 20, i);
    }
}
