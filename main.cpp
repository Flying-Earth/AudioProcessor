#include "mainwindow.h"
#include <QApplication>

void test();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    test();

    return a.exec();
}

void test()
{
    AudioFile<double> audioFile;
    audioFile.load("F:/workspace/qt/audio/tests/AudioFileTests/test-audio/wav_stereo_8bit_44100.wav");

    int channel = 0;
    int numSamples = audioFile.getNumSamplesPerChannel();

    for (int i = 0; i < numSamples; i++)
    {
        audioFile.samples[channel][i] = 0;
    }

    audioFile.save("F:/workspace/qt/audio/tests/AudioFileTests/test-audio/1.wav");

    audioFile.load("F:/workspace/qt/audio/tests/AudioFileTests/test-audio/wav_stereo_8bit_44100.wav");

    numSamples = audioFile.getNumSamplesPerChannel();

    AudioFile<double> audio;
    audio.setAudioBufferSize (1, numSamples);

    for (int i = 0; i < numSamples; i++)
    {
        audio.samples[channel][i] = audioFile.samples[channel][i];
    }

    audio.save("F:/workspace/qt/audio/tests/AudioFileTests/test-audio/2.wav", AudioFileFormat::Wave);
    audio.save("F:/workspace/qt/audio/tests/AudioFileTests/test-audio/3.aif", AudioFileFormat::Aiff);
}
