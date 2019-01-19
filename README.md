# Audio Processor
通过 Qt 实现对音频文件的加载及多种处理功能，主要功能如下所示：
1. 读取和保存音频文件，并在界面日志中显示音频信息，依次点击界面中的 Browse 、Load 和 Save 选项。
2. 实现基本的音频暂停与播放功能，点击界面中的 Play 和 Stop 选项。
3. 实现音频的反转播放功能，并在界面中显示操作日志，点击界面菜单栏中的 Reverse 选项。
4. 实现音频的淡入淡出效果，以 JSON 的形式输入需要的参数：起点(start)、终点(end)，形如 { "start" : 2, "end" : 4 }，点击界面菜单栏中的 Fade 选项。
5. 实现对音频文件声音大小的更改，以 JSON 的形式输入需要的参数，形如 { "factor" : 1 }，点击界面菜单栏中的 Volume 选项，执行相应功能之后可通过Save按键保存处理后的音频文件。
6. 实现音频文件的减采样及过采样效果（加速及减速），以 JSON 的形式输入需要的参数，形如 { "factor" : 1 }，点击界面菜单栏中的 Resample 选项，之后可通过Save按键保存处理后的音频文件。
7. 实现音频文件的变速不变调效果，以 JSON 的形式输入需要的参数，形如 { "factor" : 1.5 }，点击界面菜单栏中的 OLA 选项，之后可通过Save按键保存处理后的音频文件。
8. 用DTW算法比较两段音频，在界面日志中显示较短音频在较长音频的最佳匹配起始和终止位置（比如输入音频A和B，其中B比较短；算法输出音频A和B最匹配的片段的起止点）及较短音频在较长音频的对应位置上的匹配相似度，点击界面菜单栏中的DTW选项，选取需要与当前音频进行比较的音频文件。
# AudioFile

<!-- Version and License Badges -->
![Version](https://img.shields.io/badge/version-1.0.2-green.svg?style=flat-square) 
![License](https://img.shields.io/badge/license-GPL-blue.svg?style=flat-square) 
![Language](https://img.shields.io/badge/language-C++-yellow.svg?style=flat-square) 

A simple C++ library for reading and writing audio files. 

Current supported formats:

* WAV
* AIFF

Author
------

AudioFile is written and maintained by Adam Stark.

[http://www.adamstark.co.uk](http://www.adamstark.co.uk)

Usage
-----

### Create an AudioFile object:

	#include "AudioFile.h"

	AudioFile<double> audioFile;
		
### Load an audio file:

	audioFile.load ("/path/to/my/audiofile.wav");
	
### Get some information about the loaded audio:

	int sampleRate = audioFile.getSampleRate();
	int bitDepth = audioFile.getBitDepth();
	
	int numSamples = audioFile.getNumSamplesPerChannel();
	double lengthInSeconds = audioFile.getLengthInSeconds();
	
	int numChannels = audioFile.getNumChannels();
	bool isMono = audioFile.isMono();
	bool isStereo = audioFile.isStereo();
	
	// or, just use this quick shortcut to print a summary to the console
	audioFile.printSummary();
	
### Access the samples directly:

	int channel = 0;
	int numSamples = audioFile.getNumSamplesPerChannel();

	for (int i = 0; i < numSamples; i++)
	{
		double currentSample = audioFile.samples[channel][i];
	}

### Replace the AudioFile audio buffer with another

	// 1. Create an AudioBuffer 
	// (BTW, AudioBuffer is just a vector of vectors)
	
	AudioFile<double>::AudioBuffer buffer;
	
	// 2. Set to (e.g.) two channels
	buffer.resize (2);
	
	// 3. Set number of samples per channel
	buffer[0].resize (100000);
	buffer[1].resize (100000);
	
	// 4. do something here to fill the buffer with samples
	
	// 5. Put into the AudioFile object
	bool ok = audioFile.setAudioBuffer (buffer);
	
	
### Resize the audio buffer	

	// Set both the number of channels and number of samples per channel
	audioFile.setAudioBufferSize (numChannels, numSamples);
	
	// Set the number of samples per channel
	audioFile.setNumSamplesPerChannel (numSamples);
	
	// Set the number of channels
	audioFile.setNumChannels (int numChannels);
	
### Set bit depth and sample rate
	
	audioFile.setBitDepth (24);
	audioFile.setSampleRate (44100);
	
### Save the audio file to disk
	
	// Wave file (implicit)
	audioFile.save ("path/to/desired/audioFile.wav");
	
	// Wave file (explicit)
	audioFile.save ("path/to/desired/audioFile.wav", AudioFileFormat::Wave);
	
	// Aiff file
	audioFile.save ("path/to/desired/audioFile.aif", AudioFileFormat::Aiff);


A Note On Types
-----------------

AudioFile is a template class and so it can be instantiated using floating point precision:

	AudioFile<float> audioFile;

...or double precision:

	AudioFile<double> audioFile;
	
This simply reflects the data type you would like to use to store the underlying audio samples. You can still read or write 8, 16 or 24-bit audio files, regardless of the type that you use (unless your system uses a precision for floats less than your desired bit depth).

Versions
-------

##### 1.0.2 - 6th June 2017

- Bug fixes

License
-------

Copyright (c) 2017 Adam Stark

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
