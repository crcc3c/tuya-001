/*
2021.06.24更新记录
删除了多余的文件，只保留这一个文件，调用<TuyaWifi.h>、<SoftwareSerial.h>和<Adafruit_NeoPixel.h>头文件即可满足通讯和控制
硬件需要将Serial3改回Serial，暂时用封线沟通回路
计划下一版硬件采用5V2.5A的AC-DC模块给板卡和彩灯供电，优化电路
*/

#include <TuyaWifi.h>
#include <SoftwareSerial.h>


// Simple demonstration on using an input device to trigger changes on your
// NeoPixels. Wire a momentary push button to connect from ground to a
// digital IO pin. When the button is pressed it will change to a new pixel
// animation. Initial state has all pixels off -- press the button once to
// start the first animation. As written, the button does not interrupt an
// animation in-progress, it works only when idle.

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Digital IO pin connected to the button. This will be driven with a
// pull-up resistor so the switch pulls the pin to ground momentarily.
// On a high -> low transition the button press logic will execute.
////数字IO引脚连接到按钮。这将由一个上拉电阻器驱动，以便开关瞬间将引脚拉到地上。在高->低转换时，按钮按下逻辑将执行。
#define BUTTON_PIN   8//MEGA2560控制按键引出IO//给这个IO取个别名为BUTTON_PIN//宏定义
                      //int key_pin = 7;

#define PIXEL_PIN    9  // MEGA2560 OUT_PIN ——Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 28  // MEGA2560 控制灯带的Number of NeoPixels

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)Arduino管脚号
// Argument 3 = Pixel type flags, add together as needed:像素类型标志
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

boolean oldState = HIGH;
int     mode     = 0;    // Currently-active animation mode, 0-9


TuyaWifi my_device;

unsigned char led_state = 0;

int key_pin = 7;//连接网络按钮pin/* Connect network button pin */
//******************************************************************************
//20210626
//#define MODE_MIC   0x02
//int MIC_PIN = A0;//将A0引脚取名为MIC_PIN
//int MIC_Date = 0;
//int MIC_C1 = 0;
//int MIC_C2 = 0;
//******************************************************************************
//数据点值定义

#define DPID_SWITCH_LED 20//开关(可下发可上报)
#define DPID_WORK_MODE 21//模式(可下发可上报)
#define DPID_BRIGHT_VALUE 22//亮度值(可下发可上报)
#define DPID_TEMP_VALUE 23//冷暖值(可下发可上报)
#define DPID_COLOUR_DATA 24//彩光(可下发可上报)//备注:类型：字符；//Value: 000011112222；//0000：H（色度：0-360，0X0000-0X0168）；//1111：S (饱和：0-1000, 0X0000-0X03E8）；//2222：V (明度：0-1000，0X0000-0X03E8)
#define DPID_SCENE_DATA 25//场景(可下发可上报)//备注:类型：字符;//Value: 0011223344445555666677778888;//00：情景号;//11：单元切换间隔时间（0-100）;//22：单元变化时间（0-100）;//33：单元变化模式（0静态1跳变2渐变）;//4444：H（色度：0-360，0X0000-0X0168）;//5555：S (饱和：0-1000, 0X0000-0X03E8);//6666：V (明度：0-1000，0X0000-0X03E8);//7777：白光亮度（0-1000）;//8888：色温值（0-1000）;//注：数字1-8的标号对应有多少单元就有多少组
#define DPID_COUNTDOWN 26//倒计时剩余时间(可下发可上报)
#define DPID_MUSIC_DATA 27//音乐灯(只下发)//备注:类型：字符串；//Value: 011112222333344445555；//0：   变化方式，0表示直接输出，1表示渐变；//1111：H（色度：0-360，0X0000-0X0168）；//2222：S (饱和：0-1000, 0X0000-0X03E8）；//3333：V (明度：0-1000，0X0000-0X03E8）；//4444：白光亮度（0-1000）；//5555：色温值（0-1000）
#define DPID_CONTROL_DATA 28//调节(只下发)//备注:类型：字符串 ;//Value: 011112222333344445555  ;//0：   变化方式，0表示直接输出，1表示渐变;//1111：H（色度：0-360，0X0000-0X0168）;//2222：S (饱和：0-1000, 0X0000-0X03E8);//3333：V (明度：0-1000，0X0000-0X03E8);//4444：白光亮度（0-1000）;//5555：色温值（0-1000）
#define DPID_SLEEP_MODE 31//入睡(可下发可上报)//备注:灯光按设定的时间淡出直至熄灭
#define DPID_WAKEUP_MODE 32//唤醒(可下发可上报)//备注:灯光按设定的时间逐渐淡入直至设定的亮度
#define DPID_POWER_MEMORY 33//断电记忆(可下发可上报)//备注:通电后，灯亮起的状态
#define DPID_DO_NOT_DISTURB 34//勿扰模式(可下发可上报)//备注:适用经常停电区域，开启通电勿扰，通过APP关灯需连续两次上电才会亮灯//Value：ABCCDDEEFFGG//A：版本，初始版本0x00；//B：模式，0x00初始默认值、0x01恢复记忆值、0x02用户定制；//CC：色相 H，0~360；//DD：饱和度 S，0~1000；//EE：明度 V，0~1000；//FF：亮度，0~1000；//GG：色温，0~1000；
#define DPID_MIC_MUSIC_DATA 42//麦克风音乐律动(可下发可上报)//备注:类型：  字符串//Value：  AABBCCDDEEFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNN//AA  版本//BB  0-关闭，1-打开//CC  模式编号，自定义从201开始//DD  变换方式：0 - 呼吸模式，1 -跳变模式 ， 2 - 经典模式//EE  变化速度//FF  灵敏度//GGGG  颜色1-色相饱和度//HHHH  颜色2-色相饱和度//......//NNNN  颜色8-色相饱和度
#define DPID_DREAMLIGHT_SCENE_MODE 51//炫彩情景(可下发可上报)//备注:专门用于幻彩灯带场景//Value：ABCDEFGHIJJKLLM...//A：版本号；//B：情景模式编号；//C：变化方式（0-静态、1-渐变、2跳变、3呼吸、4-闪烁、10-流水、11-彩虹）//D：单元切换间隔时间（0-100）;//E：单元变化时间（0-100）；//FGH：设置项；//I：亮度（亮度V：0~100）；//JJ：颜色1（色度H：0-360）；//K：饱和度1 (饱和度S：0-100)；//LL：颜色2（色度H：0-360）；//M：饱和度2（饱和度S：0~100）；//注：有多少个颜色单元就有多少组，最多支持20组；//每个字母代表一个字节
#define DPID_DREAMLIGHTMIC_MUSIC_DATA 52//炫彩本地音乐律动(可下发可上报)//用于幻彩灯带本地音乐//Value：ABCDEFGHIJKKLMMN...//A：版本号；//B：本地麦克风开关（0-关、1-开）；//C：音乐模式编号；//D：变化方式；//E：变化速度（1-100）;//F：灵敏度(1-100)；//GHI：设置项；//J：亮度（亮度V：0~100）；//KK：颜色1（色度H：0-360）；//L：饱和度1 (饱和度S：0-100)；//MM：颜色2（色度H：0-360）；//N：饱和度2（饱和度S：0~100）；//注：有多少个颜色单元就有多少组，最多支持8组；//每个字母代表一个字节
#define DPID_LIGHTPIXEL_NUMBER_SET 53//点数/长度设置(可下发可上报)//备注:幻彩灯带裁剪之后重新设置长度

///* Current device DP values 当前设备DP值*/
unsigned char dp_bool_value = 0;
long dp_value_value = 0;
unsigned char dp_enum_value = 0;
unsigned char dp_string_value[21] = {"0"};
uint16_t Hue=0; //HSV
uint8_t Sat=0;
uint8_t Val=0;
uint8_t scene_mode=0;
unsigned char hex[10] = {"0"};
//unsigned char dp_raw_value[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
//int dp_fault_value = 0x01;

//存储所有dp及其类型
/* Stores all DPs and their types. PS: array[][0]:dpid, array[][1]:dp type.
 *                                     dp type(TuyaDefs.h) : DP_TYPE_RAW, DP_TYPE_BOOL, DP_TYPE_VALUE, DP_TYPE_STRING, DP_TYPE_ENUM, DP_TYPE_BITMAP
*/
unsigned char dp_array[][2] = {
  {DPID_SWITCH_LED, DP_TYPE_BOOL},
  {DPID_WORK_MODE, DP_TYPE_ENUM},
  {DPID_BRIGHT_VALUE, DP_TYPE_VALUE},
  {DPID_TEMP_VALUE, DP_TYPE_VALUE},
  {DPID_COLOUR_DATA, DP_TYPE_STRING},
  {DPID_SCENE_DATA, DP_TYPE_STRING},
  {DPID_COUNTDOWN, DP_TYPE_VALUE},
  {DPID_MUSIC_DATA, DP_TYPE_STRING},
  {DPID_CONTROL_DATA, DP_TYPE_STRING},
  {DPID_SLEEP_MODE, DP_TYPE_RAW},
  {DPID_WAKEUP_MODE, DP_TYPE_RAW},
  {DPID_POWER_MEMORY, DP_TYPE_RAW},
  {DPID_DO_NOT_DISTURB, DP_TYPE_BOOL},
  {DPID_MIC_MUSIC_DATA, DP_TYPE_STRING},
  {DPID_DREAMLIGHT_SCENE_MODE, DP_TYPE_RAW},
  {DPID_DREAMLIGHTMIC_MUSIC_DATA, DP_TYPE_RAW},
  {DPID_LIGHTPIXEL_NUMBER_SET, DP_TYPE_VALUE},
};

unsigned char pid[] = {"vzevif4o45rhjknd"};// 处替换成涂鸦IoT平台自己创建的产品的PID
unsigned char mcu_ver[] = {"2.1.6"};

unsigned long last_time = 0;
SoftwareSerial DebugSerial(15,14);


//setup()：用于初始化，只执行一次

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);// Wi-Fi状 态 指 示 灯 初 始 化
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)初始化NeoPixel条带对象（必需）
  strip.show();  // Initialize all pixels to 'off'将所有像素初始化为“关闭”


DebugSerial.begin(9600);// 软 件 串 口 初 始 化
  Serial.begin(9600);  // PA3 RX PA2 TX
   pinMode(LED_BUILTIN, OUTPUT);//Initialize led port, turn off led.//初始化led端口，关闭led。
   digitalWrite(LED_BUILTIN, LOW);//digitalWrite(LED_BUILTIN, HIGH);//内置LED灯，高电平
   pinMode(key_pin, INPUT_PULLUP);///Initialize networking keys.//初始化网络密钥。

  //pinMode(A0,INPUT); 
   strip.setBrightness(10);//初始化灯带亮度

 //incoming all DPs and their types array, DP numbers
   //Enter the PID and MCU software version
    my_device.init(pid, mcu_ver);//输入PID和MCU软件版本
    my_device.set_dp_cmd_total(dp_array, 17);//传入所有DP及其类型数组，DP编号
    //register DP download processing callback function
    my_device.dp_process_func_register(dp_process);//寄存器DP下载处理回调函数
    //register upload all DP callback function
    my_device.dp_update_all_func_register(dp_update_all);//注册上传所有DP回调函数

    last_time = millis();

}

//loop()：循环执行函数
void loop() {
  my_device.uart_service();// 串 口 接 收 处 理
/*
  if(digitalRead(BUTTON_PIN)==LOW){
 unsigned int MIC_DateMax = 0;
   unsigned int MIC_DateMin = 1024;
  MIC_Date = digitalRead(A0);
    if (MIC_Date != 0)  // 
      {
         if (MIC_Date > MIC_DateMax)
         {
            MIC_DateMax = MIC_Date;  // 只保存最大级别
         }
         else if (MIC_Date < MIC_DateMin)
         {
            MIC_DateMin = MIC_Date;  // 仅保存最低级别
         } 
      }         
   

   Serial.println(MIC_Date);
  }
  */
  //Enter the connection network mode when Pin7 is pressed.按Pin7键时进入连接网络模式
  if (digitalRead(key_pin) == LOW) {
    delay(80);
    if (digitalRead(key_pin) == LOW) {   
      my_device.mcu_set_wifi_mode(SMART_CONFIG);
    }
  }

  /* LED blinks when network is being connected 连接网络时LED闪烁*/
  if ((my_device.mcu_get_wifi_work_state() != WIFI_LOW_POWER) && (my_device.mcu_get_wifi_work_state() != WIFI_CONN_CLOUD) && (my_device.mcu_get_wifi_work_state() != WIFI_SATE_UNKNOW)) {
    if (millis()- last_time >= 500) {
      last_time = millis();
      if (led_state == LOW) {
        led_state = HIGH;
      } else {
        led_state = LOW;
      }
      digitalWrite(LED_BUILTIN, led_state);
    }
  }
  
  boolean newState = digitalRead(BUTTON_PIN);  // Get current button state.获取当前按钮状态

  if((newState == LOW) && (oldState == HIGH)) {//检查状态是否从高变为低（按按钮）。// Check if state changed from high to low (button press).
    delay(20); //短延时去抖动按钮。 // Short delay to debounce button.  
    newState = digitalRead(BUTTON_PIN); //检查按钮在去抖动后是否仍然处于低位。 // Check if button is still low after debounce.
    if(newState == LOW) {      // Yes, still low是的，仍然很低
      if(++mode > 8) mode = 0; // Advance to next mode, wrap around after #8//前进到下一个模式，#8后环绕
      switch(mode) {           // Start the new animation...开始新动画。。。
        case 0:
          colorWipe(strip.Color(  0,   0,   0), 30);    // Black/off
          break;
        case 1:
          colorWipe(strip.Color(255,   0,   0), 30);    // Red
          break;
        case 2:

          colorWipe(strip.Color(  0, 255,   0), 30);    // Green
          break;
        case 3:
          colorWipe(strip.Color( 255,   0, 255), 30);    // Blue
          break;
        case 4:
          theaterChase(strip.Color(127, 127, 127), 30); // White
          break;
        case 5:
          theaterChase(strip.Color(255,   0,   0), 30); // Red
          break;
        case 6:
          theaterChase(strip.Color(  0,   255, 255), 30); // Blue
          break;
        case 7:
          rainbow(10);
          break;
          case 8:
          theaterChaseRainbow(50);
          break;
          case 9:
          theaterChaseRainbow(100);
          break;
      }
    }
  } 

  
  oldState = newState; // Set the last-read button state to the old state.将上次读取按钮状态设置为旧状态。
}


// Fill strip pixels one after another with a color. Strip is NOT cleared//用颜色一个接一个地填充条形像素。条带未清除
// first; anything there will be covered pixel by pixel. Pass in color//第一；那里的任何东西都会被逐像素覆盖。传递颜色
// (as a single 'packed' 32-bit value, which you can get by calling//（作为单个“压缩”32位值，您可以通过调用
// strip.Color(red, green, blue) as shown in the loop() function above),//strip.Color（红、绿、蓝），如上面的loop（）函数所示，
// and a delay time (in milliseconds) between pixels.//像素之间的延迟时间（毫秒）。
void colorWipe(uint32_t color, int wait) {//颜色擦除
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,剧院天棚式追逐灯。传入颜色（32位值，
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms) between frames.条带颜色（r、g、b）和延迟时间（ms）帧之间
void theaterChase(uint32_t color, int wait) {//剧场追逐
  for(int a=0; a<30; a++) {  // 重复次数Repeat 20 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // 刷新灯带Update strip with new contents
      delay(wait);  // 停一下Pause for a moment
    }
  }
}

void rainbow(int wait) {//彩虹沿着整个地带循环。帧之间的传递延迟时间（毫秒）。// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
  // Hue of first pixel runs 3 complete loops through the color wheel.//第一个像素的色调在颜色轮上运行3个完整的循环。
  // Color wheel has a range of 65536 but it's OK if we roll over, so//彩色车轮的范围是65536，但如果我们翻身就可以了，所以
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time//只需从0到3*65536。每次向firstPixelHue添加256
  // means we'll make 3*65536/256 = 768 passes through this outer loop://意味着我们将使3*65536/256=768通过这个外循环：
  for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the将像素色调偏移一个量，使颜色旋转一整圈
      // color wheel (range of 65536) along the length of the strip色轮（范围65536）沿带的长度
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
//彩虹增强剧院天棚。帧之间的传递延迟时间（毫秒）。
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // 重复次数Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}


/**
 * @description: DP download callback function.DP下载回调函数。
 * @param {unsigned char} dpid
 * @param {const unsigned char} value
 * @param {unsigned short} length
 * @return {unsigned char}
 */
unsigned char dp_process(unsigned char dpid,const unsigned char value[], unsigned short length)
{
  switch(dpid) {
    case DPID_SWITCH_LED:
      dp_bool_value = my_device.mcu_get_dp_download_data(DPID_SWITCH_LED, value, length); /* Get the value of the down DP command 获取down DP命令的值*/
      if (dp_bool_value) {
       colorfill (strip.Color(  0, 25,  96));  //Turn on//上一次状态
      } else {       
        colorfill (strip.Color(  0, 0,   0));  //Turn off
      }    
      my_device.mcu_dp_update(DPID_SWITCH_LED, value, length); //Status changes should be reported.应报告状态更改
    break;

    case DPID_WORK_MODE:
    colorfill (strip.Color( 66, 66,  99));
    dp_enum_value  = my_device.mcu_get_dp_download_data(DPID_WORK_MODE, value, length); /* Get the value of the down DP command */
      switch(dp_enum_value){
        case 0: // white mode
          colorfill (strip.Color( 255, 255, 255));
        break;
        case 1: // colour mode
        colorfill (strip.Color( 255, 25, 25));
        break;
        case 2: // scene mode场景
        colorfill (strip.Color( 155, 225, 225));
        break;
       case 3: // music mode
        colorfill (strip.Color( 255, 25, 75));
        break;
       case 4: // breath mode
        colorfill (strip.Color( 15, 225, 175));
        break;
        
       case 5: // ciecle mode
        colorfill (strip.Color( 255, 215, 75));
        break;
       case 6: // pride mode
        colorfill (strip.Color( 155, 25,35));
        break;
       case 7: // rainbow mode
        rainbow(10); 
        break;
       case 8: // moonlight mode
        theaterChaseRainbow(50); 
        break;
       case 9:        //流水
        theaterChaseRainbow(300);                 
       break; 
      case 200:
       colour_data_control( value,length);     
      break; 

      }
      //Status changes should be reported.应报告状态更改。
      my_device.mcu_dp_update(dpid, value, length);
    break;

    case DPID_COUNTDOWN:  //倒计时
     colorfill (strip.Color( 33, 66,  99));
      my_device.mcu_dp_update(dpid, value, length);
    break;

    
     case DPID_MIC_MUSIC_DATA: //麦克风音乐律动DPID_42    
        colour_data_control(value, length);
        my_device.mcu_dp_update(DPID_MIC_MUSIC_DATA, value, length);  
    break;
    /*
    case DPID_DREAMLIGHTMIC_MUSIC_DATA: //炫彩本地音乐律动DPID_52
        colour_data_control(value, length);
        my_device.mcu_dp_update(dpid, value, length);
        
    break;   
    */

    case DPID_DREAMLIGHT_SCENE_MODE: //炫彩情景
     my_device.mcu_dp_update(DPID_DREAMLIGHT_SCENE_MODE, value, length);

     scene_mode=value[1];

     switch(scene_mode){
       case 0:
          colorWipe(strip.Color(  0,   0,   0), 0);    // Black/off电影
          break;
        case 1:
          colorWipe(strip.Color(255,  192,   203), 10);    // Red约会
          break;
        case 2:

          colorWipe(strip.Color(  255, 132,   9), 10);    // Green晚霞
          break;
        case 3:
          colorWipe(strip.Color( 104, 224, 160), 20);    // Blue圣诞夜
          break;
        case 4:
          theaterChase(strip.Color(76, 10, 162), 15); // White浪漫
          break;
        case 5:
          theaterChase(strip.Color(166, 130, 170), 10); // Red缤纷
          break;
        case 6:
          theaterChase(strip.Color(0, 0, 227), 20); // Blue动感
          break;
        case 7:
          rainbow(10);                                    //梦幻
          break;
          case 8:
          theaterChaseRainbow(50);                        //彩虹
          break;
            
          case 9:
          theaterChaseRainbow(300);                         //流水
          break; 

     }

      break;


      case DPID_LIGHTPIXEL_NUMBER_SET: //长度设置
      my_device.mcu_dp_update(DPID_LIGHTPIXEL_NUMBER_SET, value, 28);
      break;
    default:break;
  }
  return SUCCESS;
}

/**
 * @description: Upload all DP status of the current device.
 * @param {*}
 * @return {*}
 */
void dp_update_all(void)
{
  my_device.mcu_dp_update(DPID_SWITCH_LED, led_state, 1);
   my_device.mcu_dp_update(DPID_DO_NOT_DISTURB, dp_bool_value, 1);
    my_device.mcu_dp_update(DPID_BRIGHT_VALUE, dp_value_value, 1);
    my_device.mcu_dp_update(DPID_WORK_MODE, scene_mode, 1);
   //my_device.mcu_dp_update(DPID_MUSIC_DATA, dp_string_value, (sizeof(dp_string_value) / sizeof(dp_string_value[0])));
   // my_device.mcu_dp_update(DPID_DREAMLIGHTMIC_MUSIC_DATA, dp_raw_value, (sizeof(dp_raw_value) / sizeof(dp_raw_value[0])));
   // my_device.mcu_dp_update(DPID_LIGHTPIXEL_NUMBER_SET, dp_fault_value, 1); /* 仅故障类型报告Fault type Only report */
}

//拓展
void colorfill(uint32_t color) {//彩色填充
 strip.fill(color,0,PIXEL_COUNT);
    strip.show();                          //  Update strip to match

}

 void colour_data_control( const unsigned char value[], u16 length)
 {
   u8 string_data[13];
    u16 h, s, v;
    u8 r, g, b;
    u16 hue;
    u8 sat,val;

    u32 c=0;

    string_data[0] = value[0]; //渐变、直接输出
    string_data[1] = value[1];
    string_data[2] = value[2];
    string_data[3] = value[3];
    string_data[4] = value[4];
    string_data[5] = value[5];
    string_data[6] = value[6];
    string_data[7] = value[7];
    string_data[8] = value[8];
    string_data[9] = value[9];
    string_data[10] = value[10];
    string_data[11] = value[11];
    string_data[12] = value[12];

    h = __str2short(__asc2hex(string_data[1]), __asc2hex(string_data[2]), __asc2hex(string_data[3]), __asc2hex(string_data[4]));
    s = __str2short(__asc2hex(string_data[5]), __asc2hex(string_data[6]), __asc2hex(string_data[7]), __asc2hex(string_data[8]));
    v = __str2short(__asc2hex(string_data[9]), __asc2hex(string_data[10]), __asc2hex(string_data[11]), __asc2hex(string_data[12]));

    // if (v <= 10) {
    //     v = 0;
    // } else {
    //     v = color_val_lmt_get(v);
    // }

    //hsv2rgb((float)h, (float)s / 1000.0, (float)v / 1000.0, &r , &g, &b);

    // c= r<<16|g<<8|b;
  hue=h*182;
  sat=s/4;
  val=v/4;
    c = strip.gamma32(strip.ColorHSV(hue,sat,val)); // hue -> RGB
    DebugSerial.println(hue);
    DebugSerial.println(sat);
    DebugSerial.println(val);


    strip.fill(c,0,PIXEL_COUNT);

    strip.show(); // Update strip with new contents用新内容更新灯带

    //tuya_light_gamma_adjust(r, g, b, &mcu_default_color.red, &mcu_default_color.green, &mcu_default_color.blue);

    //printf("r=%d,g=%d,b=%d\r\n", mcu_default_color.red, mcu_default_color.green, mcu_default_color.blue);
    //rgb_init(mcu_default_color.red, mcu_default_color.green, mcu_default_color.blue);
 }

/**
 * @brief  str to short
 * @param[in] {a} Single Point
 * @param[in] {b} Single Point
 * @param[in] {c} Single Point
 * @param[in] {d} Single Point
 * @return Integrated value
 * @note   Null
 */
u32 __str2short(u32 a, u32 b, u32 c, u32 d)
{
    return (a << 12) | (b << 8) | (c << 4) | (d & 0xf);
}

/**
  * @brief ASCALL to Hex
  * @param[in] {asccode} 当前ASCALL值
  * @return Corresponding value
  * @retval None
  */
u8 __asc2hex(u8 asccode)
{
    u8 ret;

    if ('0' <= asccode && asccode <= '9')
        ret = asccode - '0';
    else if ('a' <= asccode && asccode <= 'f')
        ret = asccode - 'a' + 10;
    else if ('A' <= asccode && asccode <= 'F')
        ret = asccode - 'A' + 10;
    else
        ret = 0;

    return ret;
}

/**
  * @brief Normalized
  * @param[in] {dp_val} dp value
  * @return result
  * @retval None
  */
u16 color_val_lmt_get(u16 dp_val)
{
    u16 max = 255 * 100 / 100;
    u16 min = 255 * 1 / 100;

    return ((dp_val - 10) * (max - min) / (1000 - 10) + min);
}

/**
  * @brief hsv to rgb
  * @param[in] {h} tone
  * @param[in] {s} saturation
  * @param[in] {v} Lightness
  * @param[out] {color_r} red
  * @param[out] {color_g} green
  * @param[out] {color_b} blue
  * @retval None
  */
void hsv2rgb(float h, float s, float v, u8 *color_r, u8 *color_g, u8 *color_b)
{
    float h60, f;
    u32 h60f, hi;

    h60 = h / 60.0;
    h60f = h / 60;

    hi = ( signed int)h60f % 6;
    f = h60 - h60f;

    float p, q, t;

    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - (1 - f) * s);

    float r, g, b;

    r = g = b = 0;
    if (hi == 0) {
        r = v;          g = t;        b = p;
    } else if (hi == 1) {
        r = q;          g = v;        b = p;
    } else if (hi == 2) {
        r = p;          g = v;        b = t;
    } else if (hi == 3) {
        r = p;          g = q;        b = v;
    } else if (hi == 4) {
        r = t;          g = p;        b = v;
    } else if (hi == 5) {
        r = v;          g = p;        b = q;
    }

    DebugSerial.println(r);
    DebugSerial.println(g);
    DebugSerial.println(b);
    r = (r * (float)255);
    g = (g * (float)255);
    b = (b * (float)255);

    *color_r = r;
    *color_g = g;
    *color_b = b;

    // r *= 100;
    // g *= 100;
    // b *= 100;

    // *color_r = (r + 50) / 100;
    // *color_g = (g + 50) / 100;
    // *color_b = (b + 50) / 100;
}
