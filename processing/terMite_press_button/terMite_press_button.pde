import mqtt.*;
import java.awt.AWTException;
import java.awt.Robot;
import java.awt.event.KeyEvent;

MQTTClient client;

void setup() {
  size(200, 200);
  client = new MQTTClient(this);
  client.connect("mqtt://replace.media.mit.edu");
  client.subscribe("sandbox/keytouch/sensor");
  // client.unsubscribe("/example");   
  client.publish("sandbox/2349444/settings", "th=10");  
}                                                  

void draw() {
}

void keyPressed() {
  //client.publish("sandbox/2349444/settings", "th=0");  
  //;println("SENDING");
}

void messageReceived(String topic, byte[] payload) {
  String received = new String(payload);
  println("new message: " + topic + " - " + received );
  int[] nums = int(split(received, '&'));           
  if (nums.length>2)
    if (nums[2]>0) {
      PressKeyA();
    }                
}                   

void PressKeyA(){
  // Simulate a key pr  ess  
  try {
    Robot robot = new Robot();
    //robot.keyPress(KeyEvent.VK_A);
    //robot.keyRelease(KeyEvent.VK_A);
    robot.keyPress(KeyEvent.VK_SPACE);
    robot.keyRelease(KeyEvent.VK_SPACE);
  } 
  catch(AWTException e) {
    e.printStackTrace();
  }
}