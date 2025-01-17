// mikrofonlar için analog giriş pinleri
#define MICROPHONE1 A0
#define MICROPHONE2 A1
#define MICROPHONE3 A2
#define MICROPHONE4 A3
#define MICROPHONE5 A4
#define MICROPHONE6 A5
#define SCALEFACTOR 1000


// ----------------1 Tüm dizileri ve değişkenleri başlat-----------------
// tüm mikrofonların genlik değerleri
// NOT: Bu dizilerde değişiklik yapılırsa, aşağıda "calculateAverage()" VE "shiftDataArray()" fonksiyonlarına verilen eleman sayıları da değiştirilmelidir!
int amplitudes1[] = {0,0,0,0,0,0,0,0,0,0};
int amplitudes2[] = {0,0,0,0,0,0,0,0,0,0};
int amplitudes3[] = {0,0,0,0,0,0,0,0,0,0};
int amplitudes4[] = {0,0,0,0,0,0,0,0,0,0};
int amplitudes5[] = {0,0,0,0,0,0,0,0,0,0};
int amplitudes6[] = {0,0,0,0,0,0,0,0,0,0};
int ledPin = 0;
float average1 = 0; // mikrofon 1'in genlik dizisinin ortalaması
float average2 = 0; // mikrofon 2'nin genlik dizisinin ortalaması
float average3 = 0; // mikrofon 3'ün genlik dizisinin ortalaması
float average4 = 0; // mikrofon 4'ün genlik dizisinin ortalaması
float average5 = 0; // mikrofon 5'in genlik dizisinin ortalaması
float average6 = 0; // mikrofon 6'nın genlik dizisinin ortalaması
// tüm mikrofonların maksimum genlik değerleri, her bir ortalama için normalize edilir
float maximum[] = {0,0,0,0,0,0};
// mikrofonların genliklerinin örnekleneceği zaman dilimi
int samplingTime = 500; // örnekleme süresi [ms]
// her döngüde yön tespiti için maksimum indeksin kaydedildiği dizi
int maximumIndexCounter = 0;
int maximumIndex[750];  // <----- bu dizi boyutu, örnekleme süresi değiştiğinde ayarlanmalıdır
// ayarlama için genel kural: güvenlik için "örnekleme süresi * 1.5" olarak belirlenir, çünkü bir döngü yaklaşık 1ms sürer


// bir dizinin tüm değerlerini 1 indeks sağa kaydıran metot; arr[len-1] (son değer) üzerine yazılır
void shiftDataArray(int arr[],int len) {
  int temp = arr[len-1];
  for(int i=len-1;i>0;i--) {
    arr[i] = arr[i-1];
  }
  arr[0] = temp;
}

// bir dizideki en yüksek değeri bulur ve dizinin indkseini döner
int findMaximumIndex(float arr[],int arrSize) {
  int index = 0;
  float currentMax = 0;
  for(int i=0;i<arrSize;i++){
    if(arr[i]>currentMax){
      currentMax = arr[i];
      index = i;
    }
  }
  return index;
}

// bir dizinin sıfırdan büyük değerlerinin ortalamasını hesaplar
float calculateAverage(int arr[],int arrLen){
  float count = 0;  // sıfırdan büyük değerlerin sayısı
  float sum = 0;    // sıfırdan büyük tüm sayıların toplamı
  for(int i=0;i<arrLen;i++){
    if(arr[i]>0){
      sum+=float(arr[i]);
      count++;
    }
  }
  return sum/count;
}

// her mikrofon için sayaçlara dayalı olarak led Pinin belirler
int calculateLedPin(int counter1,int counter2, int counter3, int counter4, int counter5,int counter6) {
  float counts[] = {counter1,counter2,counter3,counter4,counter5,counter6};
  int finalLed = 0;
  int maxIndex = findMaximumIndex(counts,6);
  finalLed = maxIndex+2; // Ledlere ait pin Numaralarımız 2 den başlar 
  return finalLed;
}

// ses kaynağının yönünü belirlemek için kullanılan metot
void determineLedPin() {
  // ---------------- geçerli zaman damgasını kaydet-----------------
  int beginTime = millis();
  maximumIndexCounter = 0;  // maximumIndexCounter'ı sıfırla
  do{
    // --------------------- analog değerleri oku-----------------------
    int microphone1 = analogRead(MICROPHONE1);
    int microphone2 = analogRead(MICROPHONE2);
    int microphone3 = analogRead(MICROPHONE3);
    int microphone4 = analogRead(MICROPHONE4);
    int microphone5 = analogRead(MICROPHONE5);
    int microphone6 = analogRead(MICROPHONE6);
    // ---------- ortalamadan maksimum sapmayı hesapla------------
    if(average1==0 && average2==0 && average3==0 && average4==0 && average5==0 && average6==0) {
      average1 = microphone1;
      average2 = microphone2;
      average3 = microphone3;
      average4 = microphone4;
      average5 = microphone5;
      average6 = microphone6;
    }
    maximum[0] = abs((microphone1-average1)/average1);
    maximum[1] = abs((microphone2-average2)/average2);
    maximum[2] = abs((microphone3-average3)/average3);
    maximum[3] = abs((microphone4-average4)/average4);
    maximum[4] = abs((microphone5-average5)/average5);
    maximum[5] = abs((microphone6-average6)/average6);
    // ---------- genlik dizilerini kaydır ve yeni genlikleri sakla------
    shiftDataArray(amplitudes1,10);
    shiftDataArray(amplitudes2,10);
    shiftDataArray(amplitudes3,10);
    shiftDataArray(amplitudes4,10);
    shiftDataArray(amplitudes5,10);
    shiftDataArray(amplitudes6,10);
    amplitudes1[0]=microphone1;
    amplitudes2[0]=microphone2;
    amplitudes3[0]=microphone3;
    amplitudes4[0]=microphone4;
    amplitudes5[0]=microphone5;
    amplitudes6[0]=microphone6;
    // --------------------4.5 yeni ortalamaları hesapla--------------------
    average1 = calculateAverage(amplitudes1,10);
    average2 = calculateAverage(amplitudes2,10);
    average3 = calculateAverage(amplitudes3,10);
    average4 = calculateAverage(amplitudes4,10);
    average5 = calculateAverage(amplitudes5,10);
    average6 = calculateAverage(amplitudes6,10);
    // ----------------4.6 en yüksek sapmayı belirle-------------------
    maximumIndex[maximumIndexCounter]=findMaximumIndex(maximum,6);
    maximumIndexCounter++;  // maximumIndexCounter'ı artır
    
  }while((millis()-beginTime)<samplingTime);
  // -----------------------4.8 ledi belirle--------------------------
  int counter1=0, counter2=0, counter3=0, counter4=0, counter5=0, counter6=0;
  for(int i=0;i<maximumIndexCounter;i++){
    int currentNumber = maximumIndex[i];
    if(currentNumber==0) counter1++;
    else if(currentNumber==1) counter2++;
    else if(currentNumber==2) counter3++;
    else if(currentNumber==3) counter4++;
    else if(currentNumber==4) counter5++;
    else if(currentNumber==5) counter6++;
  }
  ledPin = calculateLedPin(counter1,counter2,counter3,counter4,counter5,counter6);
  plotNormal();
}

void plotNormal() {
  Serial.print(amplitudes1[0]);
  Serial.print(",");
  Serial.print(amplitudes2[0]);
  Serial.print(",");
  Serial.print(amplitudes3[0]);
  Serial.print(",");
  Serial.print(amplitudes4[0]);
  Serial.print(",");
  Serial.print(amplitudes5[0]);
  Serial.print(",");
  Serial.print(amplitudes6[0]);
  Serial.print(",");
  Serial.println(ledPin);
}

void setup() {
  // -----------------------Kurulum IO-pin---------------------
  Serial.begin(9600);
  pinMode(MICROPHONE1,INPUT);
  pinMode(MICROPHONE2,INPUT);
  pinMode(MICROPHONE3,INPUT);
  pinMode(MICROPHONE4,INPUT);
  pinMode(MICROPHONE5,INPUT);
  pinMode(MICROPHONE6,INPUT);
  pinMode(9,INPUT); // Button
  pinMode(12,OUTPUT); //Led
  for(int i=2;i<8;i++){
    pinMode(i,OUTPUT);
  }
}
bool state=false;
int button;
void loop() {
  button = digitalRead(9);
  Serial.println(button);
      for(int i=2;i<8;i++){
      digitalWrite(i,0);
    }
  if(!state){
    digitalWrite(12,0);
    state = button==1  ? true:false;
    
  }else{
    determineLedPin();
    digitalWrite(ledPin,1);
    digitalWrite(12,1);
    state = button==1  ? false:true;
  }
  delay(500);
}