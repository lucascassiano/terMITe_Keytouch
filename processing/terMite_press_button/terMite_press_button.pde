import mqtt.*;
import java.awt.AWTException;
import java.awt.Robot;
import java.awt.event.KeyEvent;

MQTTClient client;
//Buttons
boolean pressSpace = false;
boolean pressA = false;
float _threshold = 0;
int threshold = 1000;

void setup() {
  size(200, 300);
  client = new MQTTClient(this);
  client.connect("mqtt://replace.media.mit.edu");
  client.subscribe("sandbox/keytouch/sensor");
  // client.unsubscribe("/example");   
  client.publish("sandbox/2349444/settings", "th=10");  
  smooth();
}                                                  

void draw() {
  background( c_very_dark );
  //All texts
  fill(255);
  textAlign(LEFT, CENTER);
  text("terMITe keyTouch", 10, 10);
  textSize(14);

  if (pressSpace)
    text("Press Space [ON]", 60, 40);
  else {
    text("Press Space [OFF]", 60, 40);
  }

  if (pressA)
    text("Press A [ON]", 60, 70);
  else {
    text("Press A [OFF]", 60, 70);
  }

  pressSpace = Toggle(pressSpace, 10, 32, 40, 20);
  pressA = Toggle(pressA, 10, 62, 40, 20);

  beginCard( "Received Data", 0, height-100, width, 100);
  textAlign(LEFT, CENTER);
  textSize(10);
  fill(255);
  text(received, card_x+10, card_y+10);
  //Card content here
  endCard();

  _threshold = Slider(_threshold, 10, 100, 180, 20);
  threshold =(int)( _threshold * 1000);
  textAlign(LEFT, CENTER);
  textSize(20);
  fill(255);
  text("Threshold:\n"+threshold, 10, 150);
  if (Button("SEND", 90, 150)) {

    client.publish("sandbox/2349444/settings", "th="+threshold);
    println("SENT NEW THRESHOLD");
  }
}
/*
void keyPressed() {
 //client.publish("sandbox/2349444/settings", "th=0");  
 //;println("SENDING");
 }*/
String received = "waiting device...";
void messageReceived(String topic, byte[] payload) {
  received = new String(payload);
  println("new message: " + topic + " - " + received );
  int[] nums = int(split(received, '&'));           
  if (nums.length>2)
    if (nums[2]>0) {
      if (pressA) {
        PressKeyA();
      }
      if (pressSpace) {
        PressKeySpace();
      }
    }
}                   

void PressKeyA() {
  // Simulate a key pr  ess  
  try {
    Robot robot = new Robot();
    robot.keyPress(KeyEvent.VK_A);
    robot.keyRelease(KeyEvent.VK_A);
  } 
  catch(AWTException e) {
    e.printStackTrace();
  }
}

void PressKeySpace() {
  // Simulate a key pr  ess  
  try {
    Robot robot = new Robot();
    robot.keyPress(KeyEvent.VK_SPACE);
    robot.keyRelease(KeyEvent.VK_SPACE);
  } 
  catch(AWTException e) {
    e.printStackTrace();
  }
}