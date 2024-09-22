#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <require_cpp11.h>

#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <require_cpp11.h>

#include <TFT.h>

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SDA_PIN 5  // Pin 5 for SDA (SS)
#define RST_PIN 9  // Pin 9 for RST (Reset)
#define BUZZER_PIN 8
#define SERVO_PIN 9

MFRC522 rfid(SDA_PIN, RST_PIN);  // Initialize the MFRC522 instance
Servo myServo;

// This is the authorized card with UID "43 21 87 fd"
String authorizedCard = "43 21 87 fd";

void setup() {
  Serial.begin(9600);  
  SPI.begin();  // Initialize SPI bus
  rfid.PCD_Init();  // Initialize the RFID reader

  pinMode(BUZZER_PIN, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);  // Set servo to locked position

  Serial.println("Place your card near the reader...");
}

void loop() {
  // Look for new RFID cards
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String cardID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    cardID += String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    cardID += String(rfid.uid.uidByte[i], HEX);
  }
  cardID.trim();  // Remove leading/trailing spaces
  
  Serial.println("Card ID: " + cardID);  // Print the card ID to serial monitor

  if (cardID == authorizedCard) {
    // Correct card
    Serial.println("Authorized Access");
    tone(BUZZER_PIN, 1000, 100);        // Make a short peep
    myServo.write(90);                  // Unlock the door
    delay(2000);                        // Keep door unlocked for 2 seconds
    myServo.write(0);                   // Lock the door again
  } else {
    // Incorrect card
    Serial.println("Unauthorized Access");
    tone(BUZZER_PIN, 1000);             // Buzz for incorrect card
    delay(3000);                        // Wait for 3 seconds
    noTone(BUZZER_PIN);                 // Stop the buzzer
  }

  // Halt PICC
  rfid.PICC_HaltA();
}
