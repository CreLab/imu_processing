import processing.serial.*;
import processing.opengl.*;
import toxi.geom.*;
import toxi.processing.*;

Serial SerialPort;
float pitch, roll, yaw;
float position;

float Xval = 0;        
float Yval = 0;      
float Zval = 0;    

import processing.serial.*;
import processing.opengl.*;
//import toxi.geom.*;
//import toxi.processing.*;

// NOTE: requires ToxicLibs to be installed in order to run properly.
// 1. Download from http://toxiclibs.org/downloads
// 2. Extract into [userdir]/Processing/libraries
//    (location may be different on Mac/Linux)
// 3. Run and bask in awesomeness

//ToxiclibsSupport gfx;

Serial port;                         // The serial port
char[] teapotPacket = new char[14];  // InvenSense Teapot packet
int serialCount = 0;                 // current packet byte position
int aligned = 0;
int interval = 0;

float[] q = new float[4];
Quaternion quat = new Quaternion(1, 0, 0, 0);

float[] gravity = new float[3];
float[] euler = new float[3];
float[] ypr = new float[3];

void setup() {
    // 300px square viewport using OpenGL rendering
    size(300, 300, OPENGL);
    //gfx = new ToxiclibsSupport(this);

    // setup lights and antialiasing
    lights();
    smooth();
    
    // open the serial port
    port = new Serial(this, "COM4", 115200);
    port.bufferUntil('\n');
    
    // send single character to trigger DMP init/start
    // (expected by MPU6050_DMP6 example Arduino sketch)
    port.write('r');
}

void draw() {
    if (millis() - interval > 1000) {
        // resend single character to trigger DMP init/start
        // in case the MPU is halted/reset while applet is running
        port.write('r');
        interval = millis();
    }
    
    // black background
    background(0);
    
    // translate everything to the middle of the viewport
    pushMatrix();
    translate(width / 2, height / 2);

    // 3-step rotation from yaw/pitch/roll angles (gimbal lock!)
    // ...and other weirdness I haven't figured out yet
    //rotateY(-ypr[0]);
    //rotateZ(-ypr[1]);
    //rotateX(-ypr[2]);

    // toxiclibs direct angle/axis rotation from quaternion (NO gimbal lock!)
    // (axis order [1, 3, 2] and inversion [-1, +1, +1] is a consequence of
    // different coordinate system orientation assumptions between Processing
    // and InvenSense DMP)
    float[] axis = quat.toAxisAngle();
    rotate(axis[0], -axis[1], axis[3], axis[2]);

    // draw main body in red
    fill(255, 0, 0, 200);
    box(10, 10, 200);
    
    // draw front-facing tip in blue
    fill(0, 0, 255, 200);
    pushMatrix();
    translate(0, 0, -120);
    rotateX(PI/2);
    drawCylinder(0, 20, 20, 8);
    popMatrix();
    
    // draw wings and tail fin in green
    fill(0, 255, 0, 200);
    //beginShape(TRIANGLES);
    //vertex(-100,  2, 30); vertex(0,  2, -80); vertex(100,  2, 30);  // wing top layer
    //vertex(-100, -2, 30); vertex(0, -2, -80); vertex(100, -2, 30);  // wing bottom layer
    //vertex(-2, 0, 98); vertex(-2, -30, 98); vertex(-2, 0, 70);  // tail left layer
    //vertex( 2, 0, 98); vertex( 2, -30, 98); vertex( 2, 0, 70);  // tail right layer
    //endShape();
    
    //beginShape(QUADS);
    //vertex(-100, 2, 30); vertex(-100, -2, 30); vertex(  0, -2, -80); vertex(  0, 2, -80);
    //vertex( 100, 2, 30); vertex( 100, -2, 30); vertex(  0, -2, -80); vertex(  0, 2, -80);
    //vertex(-100, 2, 30); vertex(-100, -2, 30); vertex(100, -2,  30); vertex(100, 2,  30);
    //vertex(-2,   0, 98); vertex(2,   0, 98); vertex(2, -30, 98); vertex(-2, -30, 98);
    //vertex(-2,   0, 98); vertex(2,   0, 98); vertex(2,   0, 70); vertex(-2,   0, 70);
    //vertex(-2, -30, 98); vertex(2, -30, 98); vertex(2,   0, 70); vertex(-2,   0, 70);
    //endShape();
    
    popMatrix();
}

void serialEvent(Serial myPort)
{
    // get the ASCII string:
    String inString = myPort.readStringUntil('\n');
    println(inString);
    
    if (inString != null)
    {
        // trim off any whitespace:
        // inString = trim(inString);
        println(inString);
        // split the string on the commas and convert the
        // resulting substrings into an integer array:
        float[] magData = float(split(inString, ","));
        // if the array has at least four elements, you know
        // you got the values plus the header.  Put the numbers 
        // in the magData array
        if (magData.length >=4)
        {
            if (magData[0] == 1.11)
            {  // test header
                Xval = (magData[1]/2);  // scale data to window
                Yval = (magData[2]/2);
                Zval = (magData[3]/2);
                
                q[0] = (magData[4]);  // scale data to window
                q[1] = (magData[5]);
                q[2] = (magData[6]);
                q[3] = (magData[7]);
                
                roll = (magData[8]);  // scale data to window
                pitch = (magData[9]);
                yaw = (magData[10]);
                
                // set our toxilibs quaternion to new data
                quat.set(q[0], q[1], q[2], q[3]);
            
            }
            
            println((int)Xval + "\t" + (int)Yval + "\t" + (int)Zval );  

            //Xval = map(Xval, -1.0, 1.0, -200.0, 200.0);
            //Yval = map(Yval, -1.0, 1.0, -200.0, 200.0);
            //Zval = map(Zval, -1.0, 1.0, 200.0, -200.0);

            //println((int)Xval + "\t" + (int)Yval + "\t" + (int)Zval );  
            //// put points into the array

            //i =  ++i % (maxPoints - 2);  
            //x[i] = round(Xval);
            //z[i] = round(Yval);  //switch channels x<-> y why?
            //y[i] = round(Zval);  //switch channels x<-> y
        }
    }
}

void drawCylinder(float topRadius, float bottomRadius, float tall, int sides) {
    float angle = 0;
    float angleIncrement = TWO_PI / sides;
    beginShape(QUAD_STRIP);
    for (int i = 0; i < sides + 1; ++i) {
        vertex(topRadius*cos(angle), 0, topRadius*sin(angle));
        vertex(bottomRadius*cos(angle), tall, bottomRadius*sin(angle));
        angle += angleIncrement;
    }
    endShape();
    
    // If it is not a cone, draw the circular top cap
    if (topRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);
        
        // Center point
        vertex(0, 0, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(topRadius * cos(angle), 0, topRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }
  
    // If it is not a cone, draw the circular bottom cap
    if (bottomRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);
    
        // Center point
        vertex(0, tall, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(bottomRadius * cos(angle), tall, bottomRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }
}
