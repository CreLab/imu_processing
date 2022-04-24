/*   Arduino Compass 
 *      
 *  by Dejan Nedelkovski, 
 *  www.HowToMechatronics.com
 *  
 */
 
import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

int numValues = 18; // number of sensor data

float[] values = new float[numValues];

float[] M = new float[3];
float[] q = new float[4];
float pitch = 0.00;
float roll = 0.00;
float yaw = 0.00;
float[] A = new float[3];
float[] G = new float[3];
float heading = 0.00;

Serial myPort;
PImage imgCompass;
PImage imgCompassArrow;

String data="";

void setup()
{
    size(700, 700);
    background(255);
    noFill();
    stroke(255);
    strokeWeight(2);
    
    imgCompass = loadImage("Compass.png");
    imgCompassArrow = loadImage("CompassArrow.png");
    
    myPort = new Serial(this, "COM4", 115200); // starts the serial communication
    myPort.bufferUntil('\n');
    
    colorMode(HSB, 300);
}

void draw()
{  
    pushMatrix();
    translate(width/2, height/2, 0); // Translates the coordinate system into the center of the screen, so that the rotation happen right in the center
    rotateZ(radians(-heading)); // Rotates the Compass around Z - Axis 
    image(imgCompass, -40, -40); // Loads the Compass image and as the coordinate system is relocated we need need to set the image at -960x, -540y (half the screen size)
    
    popMatrix(); // Brings coordinate system is back to the original position 0,0,0
    
    image(imgCompassArrow, 114, 104); // Loads the CompassArrow image which is not affected by the rotateZ() function because of the popMatrix() function
    textSize(30);
    text("Heading: " + heading, 40, 40); // Prints the value of the heading on the screen
    
    println("Heading: " + heading);
    
    delay(40);  
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
