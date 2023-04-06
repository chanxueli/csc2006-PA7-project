// The nodes have ble mesh table is setup already
// However , the table is currently not used 
// The table will be able to used to efficently set up the mesh routes and node connections
// The table can also be use to set up one static end point.
// We can use table and rssi to do tracking of a small distance 

#include <M5StickCPlus.h>
#include <BLEDevice.h>
#include <BLEServer.h>

// Declare to the mesh cluster name
#define bleServerName "meowmeowcluster"

// BLE Service for server and client
#define SERVICE_UUID "a930af71-cfd9-4e3c-9a89-50df1d60dce0"
static BLEUUID bleServiceUUID("a930af71-cfd9-4e3c-9a89-50df1d60dce0"); 

// My characterist set up
static BLEUUID catCharacteristicUUID("73cfb262-56a8-4463-beaf-115684e71070");

//Flags stating if should begin connecting and if the connection is up
static boolean doConnect = false;
static boolean connected = false;

//Address of the peripheral device. Address will be found during scanning...
static BLEAddress* pServerAddress;

//Characteristicd that we want to read
static BLERemoteCharacteristic* catCharacteristic;

//Activate notify
const uint8_t notificationOn[] = { 0x1, 0x0 };
const uint8_t notificationOff[] = { 0x0, 0x0 };

const int ledPin = 10;

// Server set up
bool deviceConnected = false;
BLECharacteristic catCharacteristics("73cfb262-56a8-4463-beaf-115684e71070", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor catsDescriptor(BLEUUID((uint16_t)0x2904));

// Mesh table setup
const int MAXNODESIZE = 9;
char* myBleAddress;
static String mainMeshTable [MAXNODESIZE] = {};
int tableCount = 1 ;

bool containsAddress( char* target) {
  // Check if address contain in ble main mesh table
  Serial.println("CHECKINGG");
  for (int i = 0; i < MAXNODESIZE ; i++) {
    if (mainMeshTable[i].compareTo(target) == 0) {
      return true;
    }
  }
  return false;
}

class MyServerCallbacks: public BLEServerCallbacks {
  // For Server
  void onConnect(BLEServer* pServer) {
    // Server is connected
    deviceConnected = true;
    M5.Lcd.printf("\n Found client!", 0);
    Serial.println("MyServerCallbacks::Connected...");
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    ESP.restart();
    Serial.println("MyServerCallbacks::Disconnected...");
  }
};

void toggleLed(){
  if(digitalRead(ledPin) == HIGH){
    digitalWrite(ledPin, LOW);
  }else{
    digitalWrite(ledPin, HIGH);
  }
}

// Set up main notify callback
static void catNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                              uint8_t* pData, size_t length, bool isNotify) {

  // Check for pre scripted data , for different actions
  if (String((char*)pData).substring(0, 4) == "ping") {
    M5.Lcd.printf("\n Pingging this device", 0);
    toggleLed();
    catCharacteristics.setValue("ping");
    catCharacteristics.notify();
  } else {
    // Set ble mesh table
    static int count = 0;
    String bleAddress = String((char*)pData).substring(0, 17);
    Serial.println(String((char*)pData).substring(0, 4));
    tableCount = String((char*)pData).substring(17, 18).toInt();
    mainMeshTable[count] = bleAddress;

    if ((tableCount - 1) == count) {
      // Reset count and add my ble to the table
      mainMeshTable[tableCount] = myBleAddress;
      tableCount += tableCount;
      Serial.println("Ble successfuly added to table, ready to notify other nodes.");
      count = 0;
    } else {
      count++;
    }
  }
}

//Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) {                 //Check if the name of the advertiser matches   
       
       //Scan can be stopped, we found what we are looking for
       advertisedDevice.getScan()->stop();
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());  //Address of advertiser is the one we need
      const char* myString2 = pServerAddress->toString().c_str();
      char* currentAddress = new char[strlen(myString2) + 1]; // Allocate memory for the pointer
      strcpy(currentAddress, myString2);
      Serial.println(currentAddress);
      if(containsAddress(currentAddress)){
        doConnect = false;  
        Serial.println("in table do not connect");
      }
      else {
         doConnect = true;                                                //Set indicator, stating that we are ready to connect
        Serial.println("Device found. Connecting!");
      }
      
    } else
      Serial.print(".");
  }
};

void setup() {
  //Start serial communication
  Serial.begin(115200);
  mainMeshTable[0] = "4c:75:25:cb:86:62"; // Set up genesis node  
  Serial.println("Starting BLE Client application...");

  //Set up led and oled
  M5.begin();
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.printf(" BLE Client and Server", 0);

  //Init BLE device
  BLEDevice::init(bleServerName);

  // Get my ble address.
  BLEAddress myAddress = BLEDevice::getAddress();
  const char* myString = myAddress.toString().c_str();
  myBleAddress = new char[strlen(myString) + 1]; // Allocate memory for the pointer
  strcpy(myBleAddress, myString); 
  
  // --Server start --
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Note: Service > charater > descriptor
  BLEService *bleService = pServer->createService(SERVICE_UUID);
  // Create BLE Characteristics and Create a BLEpServer Descriptor
  bleService->addCharacteristic(&catCharacteristics);
  catsDescriptor.setValue("Cat value");
  catCharacteristics.addDescriptor(&catsDescriptor); 
  // Start the service
  bleService->start();
  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  
}

void loop() {
  if (deviceConnected) {
    // Server is connected
    if(digitalRead(M5_BUTTON_HOME) == LOW ){
      // Ping to set a chain reaction to toggle led of all connected device.
      catCharacteristics.setValue("ping");
      catCharacteristics.notify();
      toggleLed();
    }
  }

  delay(1000);  // Delay one second between loops.
}


