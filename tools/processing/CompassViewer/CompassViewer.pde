/*   Arduino Compass 
 *      
 *  by Dejan Nedelkovski, 
 *  www.HowToMechatronics.com
 *  
 */
 
import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;
import controlP5.*;

int numValues = 18; // number of sensor data
int textSize = 15;

float[] values = new float[numValues];

float[] M = new float[3];
float[] q = new float[4];
float pitch = 0.00;
float roll = 0.00;
float yaw = 0.00;
float[] A = new float[3];
float[] G = new float[3];
float heading = 0.00;

ControlP5 cp5; // you always need the main class
int dataSelect = 0;
int numGraphs = 3; // number of graphs
int[] min = new int[numGraphs];
int[] max = new int[numGraphs];
color[] valColor = new color[numGraphs];
int xPos = 0; // horizontal position of the graph

float partW; // partial screen width
float partH; // partial screen height

Serial myPort;
PImage imgCompass;
PImage imgCompassArrow;

String data="";

void setup()
{
    size(800, 1200);
    background(0);
    
    imgCompass = loadImage("Compass.png");   
    imgCompassArrow = loadImage("CompassArrow.png");

    myPort = new Serial(this, "COM4", 115200); // starts the serial communication
    myPort.bufferUntil('\n');
    
    textSize(textSize);
    
    // initialize:
    // *edit these* to match how many values you are reading, and what colors you like 
    valColor[0] = color(255, 0, 0); // red
    valColor[1] = color(0, 255, 0); // green
    valColor[2] = color(0, 0, 255); // blue
    
    cp5 = new ControlP5(this);
   
    // add a dropdownlist at position (100,100)
    DropdownList droplist = cp5.addDropdownList("mySuperList").setPosition(700, 800);
   
    // add items to the dropdownlist
    droplist.addItem("Magnetometer", 0);
    droplist.addItem("Accelerometer", 1);
    droplist.addItem("Gyroscope", 2);
    
    droplist.setDefaultValue(0.0f);
    droplist.close();
    customize(droplist);
}

void draw()
{  
    drawCompass();
    drawPlot();
}

void drawCompass()
{  
    fill(150);
    rect(0, 0, width, height-400);
    
    noStroke();
    fill(255);
    text("Heading: " + heading, 650, 40); // Prints the value of the heading on the screen
    
    int x = width/2;
    int y = (height-400)/2;
  
    imageMode(CENTER);
    image(imgCompass, x, y); // Loads the Compass image and as the coordinate system is relocated we need need to set the image at -960x, -540y (half the screen size)

    imageMode(CENTER);

    pushMatrix();
    translate(x+8, y-2);
    rotate(radians(heading)); // Rotates the Compass around Z - Axis 
    image(imgCompassArrow, 0, 0); // Loads the CompassArrow image which is not affected by the rotateZ() function because of the popMatrix() function
    popMatrix();
    
    imageMode(CORNER);
}

void drawPlot()
{
  int posY = 800;
  
  partW = width;
  partH = 400 / numGraphs;
   
  // in the Arduino website example, everything is done in serialEvent
  // here, data is handled in serialEvent, and drawing is handled in draw()
  // when drawing every loop in draw(), you can see gaps when not updating as fast as data comes in
  // when drawing in serialEvent(), you can see frequency of data updates reflected in how fast graph moves
  // (either method can work)

  for (int i=0; i<numGraphs; i++)
  {
    float curData = M[i];
        
    if (dataSelect == 1)
    {
      curData = A[i];
      
      min[0] = -17000;
      max[0] = 17000; // full range example, e.g. any analogRead
      min[1] = -17000;
      max[1] = 17000; // full range example, e.g. any analogRead
      min[2] = -17000;
      max[2] = 17000; // full range example, e.g. any analogRead
    }
    else if (dataSelect == 2)
    {
      curData = G[i];
      
      min[0] = -250;
      max[0] = 250; // full range example, e.g. any analogRead
      min[1] = -250;
      max[1] = 250; // full range example, e.g. any analogRead
      min[2] = -250;
      max[2] = 250; // full range example, e.g. any analogRead
    }
    else
    {
      curData = M[i];
      
      min[0] = -150;
      max[0] = 150; // full range example, e.g. any analogRead
      min[1] = -150;
      max[1] = 150; // full range example, e.g. any analogRead
      min[2] = -150;
      max[2] = 150; // full range example, e.g. any analogRead
    }
    
    // map to the range of partial screen height:
    float mappedVal = map(curData, min[i], max[i], 0, partH);
   
    // remove lines:
    stroke(0);
    line(xPos+5, partH*(i+1)+posY, xPos+5, partH*(i+1)-partH+posY);
    
    // draw moving line:
    stroke(255);
    line(xPos+1, partH*(i+1)+posY, xPos+1, partH*(i+1)-partH+posY);

    // draw lines:
    stroke(0);
    line(xPos, partH*(i+1)+posY, xPos, partH*(i+1)-partH+posY);
    stroke(valColor[i]);
    line(xPos, partH*(i+1)-mappedVal+2+posY, xPos, partH*(i+1)-mappedVal+posY);
    
    // draw dividing line:
    stroke(255);
    line(0, partH*(i+1)+posY, partW, partH*(i+1)+posY);
    
    // draw ending line:
    stroke(255);
    line(partW, partH*(i+1)+posY, partW, partH*(i+1)-partH+posY);

    // display values on screen:
    fill(50);
    noStroke();
    rect(0, partH*(i+0)+1+posY, 125, textSize);
    fill(255);
    text(round(curData), 2, partH*(i+0)+textSize+posY);
    fill(125);
    text(max[i], 75, partH*(i+0)+textSize+posY);
  }

  // increment the graph's horizontal position:
  xPos++; 
  // if at the edge of the screen, go back to the beginning:
  if (xPos > partW)
  {
    xPos = 0;
  }  
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

void controlEvent(ControlEvent theEvent) {
  if(theEvent.isController())
  {
      if (theEvent.getController().getName().equals("mySuperList"))
      {
        dataSelect = (int)theEvent.getController().getValue();
        xPos = 0;
        background(0);
      }
    }
    else if(theEvent.isGroup())
    {
        System.out.println("event from group : "+theEvent.getGroup().getValue()+" from "+theEvent.getGroup());
    }
}

void customize(DropdownList ddl) {
    // a convenience function to customize a DropdownList
    ddl.setBackgroundColor(color(190));
    ddl.setItemHeight(20);
    ddl.setBarHeight(15);
    ddl.getCaptionLabel().getStyle().marginTop = 3;
    ddl.getCaptionLabel().getStyle().marginLeft = 3;
    ddl.getValueLabel().getStyle().marginTop = 3;
    ddl.setColorBackground(color(60));
    ddl.setColorActive(color(255, 128));
}
