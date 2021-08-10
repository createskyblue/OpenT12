#include "OpenT12.h"
//BEEP
uint8_t BEEP_Channel = 2;    // 通道
// uint8_t BEEP_Channel2 = 1;    // 通道

void BeepInit(void) {
    pinMode(BEEP_PIN, OUTPUT);
    ledcAttachPin(BEEP_PIN, BEEP_Channel);  //将通道与对应的引脚连接
    ledcSetup(BEEP_Channel, 0, 8); // 设置通道

    // pinMode(BEEP_PIN2, OUTPUT);
    // ledcAttachPin(BEEP_PIN2, BEEP_Channel2);  //将通道与对应的引脚连接
    // ledcSetup(BEEP_Channel2, 0, 8); // 设置通道
}

void SetTone(double freq) {
    ledcWriteTone(BEEP_Channel, freq);
}

// void SetTone2(double freq) {
//     ledcWriteTone(BEEP_Channel2, freq);
// }

void SetNote(note_t note, uint8_t rp) {
    SetTone(GetNote(note, rp));
}

// void SetNote2(note_t note, uint8_t rp) {
//     SetTone2(GetNote(note, rp));
// }

/*** 
 * @description: 发出指定音符的音调
 * @param {note_t} note 绝对音高
 * @param {uint8_t} rp 相对音高（全音）
 * @return {*}
 * https://www.zhihu.com/question/27166538/answer/35507740
 * 王赟 Maigo ：
 * 音名是用字母CDEFGAB表示的，它表示的是绝对音高。一个八度中有12个音，分别是C, #C, D, #D, E, F, #F, G, #G, A, #A, B，相邻两个音之间的距离叫一个半音，两个半音叫一个全音。在最常见的定律法——十二平均律中，中央C（钢琴最中间的C音）右边的第一个A音被定义为440 Hz，然后其它音的频率用等比数列推出，相隔半音的两个音的频率之比为pow(2,1/12.0)。由此可以推断出中央C的频率为261 Hz，它右边的#C的频率为277 Hz，D的频率为293 Hz，等等等等。
唱名指的是do, re, mi这些，可以用数字1234567表示。一个八度中也有12个音，分别是1, #1, 2, #2, 3, 4, #4, 5, #5, 6, #6, 7，相邻两个音之间的距离也是半音。一首曲子是什么调，就是说哪个音名对应着唱名1。所以，唱名表示的是相对音高。比如C调就是说C音是1，这样1的频率就是261 Hz，#1的频率就是277 Hz，2的频率就是293 Hz，等等。而在#C调中，就变成了1是277 Hz，#1是293 Hz，等等。调式总共有12种，你可以按上述方法自己推算。
 */

double GetNote(note_t note, uint8_t rp) {
    const uint16_t noteFrequencyBase[12] = {
        //   C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
            4186,    4435,    4699,    4978,    5274,    5588,    5920,    6272,    6645,    7040,    7459,    7902
    };
    const uint8_t octave          = 4;
    const double  FreqRatio       = 1.059463094; // pow(2,1/12.0);
    const double  FreqRatioDouble = 1.122462048; // pow(2,1/6.0);
    double noteFreq = ((double)noteFrequencyBase[note] / (double)(1 << (8 - octave))) * pow(FreqRatioDouble,rp);
    //SetTone(noteFreq);
    return noteFreq;
}

