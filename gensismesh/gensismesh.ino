#include <M5StickCPlus.h>
#include <BLEDevice.h>
#include <BLEServer.h>

// change the BLE Server name to connect to
#define bleServerName "meowmeowcluster"

/* UUID's of the service, characteristic that we want to read*/
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

//  Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 2000;

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
    // Check for exist table
    // If exist 
    deviceConnected = true;
    // Find the count
    // int countArr = 0;
    // for (int i = 0; i < MAXNODESIZE; i++) {
    //   if((mainMeshTable[i] == "\0")){
    //     break;
    //   }
    //   countArr++;
    // }

    // for(int i = 0;  i<countArr;i++ ){
    //   String s =mainMeshTable[i]+ String(countArr);
    //   Serial.println(s);
    //   char* charArray = new char[s.length() + 1];
    //   strcpy(charArray, s.c_str());
    //   catCharacteristics.setValue(charArray);
    //   catCharacteristics.notify();
    // }
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

void callNotify(){
    toggleLed();
    // Find the count
    int countArr = 0;
    for (int i = 0; i < MAXNODESIZE; i++) {
      if((mainMeshTable[i] == "\0")){
        break;
      }
      countArr++;
    }

    for(int i = 0;  i<countArr;i++ ){
      String s =mainMeshTable[i]+ String(countArr);
      Serial.println(s);
      char* charArray = new char[s.length() + 1];
      strcpy(charArray, s.c_str());
      catCharacteristics.setValue(charArray);
      catCharacteristics.notify();
    }
}
 
static void catNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                              uint8_t* pData, size_t length, bool isNotify) {
  if(String((char*)pData).substring(0,4) == "meow"){
    M5.Lcd.printf("\n Pingging this device", 0);
    toggleLed();
    catCharacteristics.setValue("meow");
    catCharacteristics.notify();
  }else{
    static int count = 0 ;
    Serial.println("cat data callback");
    
    String bleAddress = String((char*)pData).substring(0,17);  
    Serial.println(bleAddress);
    tableCount = String((char*)pData).substring(17,18).toInt();
    mainMeshTable[count] = bleAddress;
    
    if((tableCount-1) == count){
      // Reset count and add my ble to the table
      mainMeshTable[tableCount] = myBleAddress;
      tableCount += tableCount;
      Serial.println("reset");
      // toggleLed();
      // callNotify();
      count = 0;
    }else{
      count++;
    }
  }
}

//Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress) {
  BLEClient* pClient = BLEDevice::createClient();
  
  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");
  Serial.println(pAddress.toString().c_str());
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(bleServiceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(bleServiceUUID.toString().c_str());
    return (false);
  }

  catCharacteristic = pRemoteService->getCharacteristic(catCharacteristicUUID);
  if ( catCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");

  //Assign callback functions for the Characteristics
  catCharacteristic->registerForNotify(catNotifyCallback);
  return true;
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

//function that prints the latest sensor readings in the OLED display
// void printReadings() {

//   Serial.print("Print data :");
//   // Serial.print(catValue);
//   Serial.println(" cats");
// }

void setup() {
  //Start serial communication
  Serial.begin(115200);
  mainMeshTable[0] = "4c:75:25:cb:86:62"; // Set up genesis node  
  Serial.println("Starting BLE Client application...");

  // put your setup code here, to run once:
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

  // --Client start --
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  // BLEScan* pBLEScan = BLEDevice::getScan();
  // pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  // pBLEScan->setActiveScan(true);
  // pBLEScan->start(30);

  
}

void loop() {

  if (deviceConnected) {
    // Server is connected

    if(digitalRead(M5_BUTTON_HOME) == LOW ){
      // callNotify();
      catCharacteristics.setValue("meow");
      catCharacteristics.notify();
      toggleLed();
    }
    // lastTime = millis();
    
  }

  // if (doConnect == true) {
  //   // Client do da connect
  //   if (connectToServer(*pServerAddress)) {
  //     // Client do da connect
  //     Serial.println("Connected to the BLE Server.");
  //     // M5.Lcd.setCursor(0, 4, 2);
  //     M5.Lcd.printf("\n Connected to the BLE Server", 0);
  //     //Activate the Notify property of each Characteristic
  //     catCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2904))->writeValue((uint8_t*)notificationOn, 2, true);
  //     connected = true;
  //   } else {
  //     Serial.println("Failed to connect to the server; Restart device to scan for nearby BLE server again.");
  //   }
  //   doConnect = false;
  // }

  delay(1000);  // Delay one second between loops.
}


