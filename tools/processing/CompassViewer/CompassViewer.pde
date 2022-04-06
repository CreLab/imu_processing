/*   Arduino Compass 
 *      
 *  by Dejan Nedelkovski, 
 *  www.HowToMechatronics.com
 *  
 */
 
import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;
PImage imgCompass;
PImage imgCompassArrow;

String data="";
float heading;

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
    translate(width/2, height/2, 0); // Translates the coordinate system into the center of the screen, so that the rotation happen right in the center
    
    image(imgCompass, -40, -40); // Loads the Compass image and as the coordinate system is relocated we need need to set the image at -960x, -540y (half the screen size)
    
    pushMatrix();
    rotateZ(radians(-heading)); // Rotates the Compass around Z - Axis 
    image(imgCompassArrow, 114, 104); // Loads the CompassArrow image which is not affected by the rotateZ() function because of the popMatrix() function
    popMatrix(); // Brings coordinate system is back to the original position 0,0,0
    
    textSize(30);
    text("Heading: " + heading, 40, 40); // Prints the value of the heading on the screen
    
    delay(40);  
}

// starts reading data from the Serial Port
void serialEvent (Serial myPort)
{ 
    data = myPort.readStringUntil('\n');// reads the data from the Serial Port and puts it into the String variable "data".
    heading = float(data); // Convering the the String value into Float value
}
