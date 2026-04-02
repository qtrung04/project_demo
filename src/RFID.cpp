#include "RFID.h"

#include "Display.h"

void RFIDinit() {
  SPI.begin();
  mfrc522.PCD_Init();
  prefs.begin(RFID_NAMESPACE, false);
}

// Scan RFID card and get UID
bool scanRFID(char* uid) {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // Convert UID to HEX string
  uid[0] = '\0';
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    char tmp[4];
    sprintf(tmp, "%02X", mfrc522.uid.uidByte[i]);
    strcat(uid, tmp);
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  return true;
}

// Check if RFID card is valid
bool isValidRFIDCard(char* uid) {
  int count = getRFIDCardCount();
  char stored_uid[30];

  for (int i = 0; i < count; i++) {
    if (getRFIDCard(i, stored_uid)) {
      if (strcmp(uid, stored_uid) == 0) {
        return true;
      }
    }
  }
  return false;
}

// Add RFID card to storage
bool addRFIDCard(char* uid) {
  int count = getRFIDCardCount();

  if (count >= MAX_RFID_CARDS) {
    return false;  // Storage full
  }

  // Check if card already exists
  if (isValidRFIDCard(uid)) {
    return false;  // Card already exists
  }

  // Store card
  char key[10];
  sprintf(key, "card_%d", count);
  prefs.putString(key, String(uid));

  // Update count
  prefs.putInt("count", count + 1);

  return true;
}

// Delete RFID card from storage
bool deleteRFIDCard(char* uid) {
  int count = getRFIDCardCount();
  char stored_uid[30];
  int delete_index = -1;

  // Find card to delete
  for (int i = 0; i < count; i++) {
    if (getRFIDCard(i, stored_uid)) {
      if (strcmp(uid, stored_uid) == 0) {
        delete_index = i;
        break;
      }
    }
  }

  if (delete_index == -1) {
    return false;  // Card not found
  }

  // Shift remaining cards
  for (int i = delete_index; i < count - 1; i++) {
    char key_src[10], key_dst[10];
    sprintf(key_src, "card_%d", i + 1);
    sprintf(key_dst, "card_%d", i);

    String uid_str = prefs.getString(key_src, "");
    prefs.putString(key_dst, uid_str);
  }

  // Remove last card
  char key[10];
  sprintf(key, "card_%d", count - 1);
  prefs.remove(key);

  // Update count
  prefs.putInt("count", count - 1);

  return true;
}

// Get number of stored RFID cards
int getRFIDCardCount() { return prefs.getInt("count", 0); }

// Get RFID card at index
bool getRFIDCard(int index, char* uid) {
  int count = getRFIDCardCount();

  if (index < 0 || index >= count) {
    return false;
  }

  char key[10];
  sprintf(key, "card_%d", index);
  String uid_str = prefs.getString(key, "");

  if (uid_str.length() == 0) {
    return false;
  }

  strcpy(uid, uid_str.c_str());
  return true;
}

// Clear all stored RFID cards
void clearAllRFIDCards() {
  int count = getRFIDCardCount();

  for (int i = 0; i < count; i++) {
    char key[10];
    sprintf(key, "card_%d", i);
    prefs.remove(key);
  }

  prefs.putInt("count", 0);
}
