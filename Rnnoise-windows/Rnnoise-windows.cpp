// Rnnoise-windows.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include "fstream"
#include <iostream>
//
#include <string>
#include <stdio.h>

#include "rnnoise.h"

#include <malloc.h>
#include "denoise.c"

//#include "_kiss_fft_guts.h"
#include "kiss_fft.c"
#include "pitch.c"

#include "rnn.c"
//#include "rnn.h"
#include "rnn_data.c"
#include "AudioFile.cpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

#ifndef FRAME_SIZE
#define FRAME_SIZE 480
#endif // !FRAME_SIZE

struct HEADER
{
	char chunk_ID[4];                           // RIFF string
	std::uint32_t  chunk_size;                  // overall size of
	char format[4];                             // WAVE string
	char fmt_chunk_marker[4];                   // fmt string with trailing null char
	std::uint32_t length_of_fmt;                // length of the format data
	std::uint16_t  format_type;                 // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	std::uint16_t  channels;                    // no.of channels
	std::uint32_t  sample_rate;                 // sampling rate (blocks per second)
	std::uint32_t  byte_rate;                   // SampleRate * NumChannels * BitsPerSample/8
	std::uint16_t  block_align;                 // NumChannels * BitsPerSample/8
	std::uint16_t  bits_per_sample;             // bits per sample, 8- 8bits, 16- 16 bits etc
	char data_chunk_header[4];                 // DATA string or FLLR string
	std::uint32_t  data_size;                   // NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read

	std::uint32_t file_size;	// 文件大小
	unsigned char *data;		// 存放数据的指针
};


using namespace std;
int main(int argc, char **argv) {
	// -task 1: read the wav file's PCM data
	// -task 2: apply the PCM data to the rnnoise and denoise the audio data
	// -task 3: output the wav file 

	string directory = ".\\assets\\";
	ifstream fi;
	ofstream fo;
	float x[FRAME_SIZE];
	DenoiseState *st;
	st = rnnoise_create();
	fi.open(directory + "audio_in_with_noise.wav" , ios::binary);
	fo.open(directory + "denoise_3.wav" , ios::binary);

	string temp;
	char header[44];
	fi.read( (char*)&header , sizeof(char) * 44);		// read the header data of the input .wav file
	fo.write(header, 44);								// write the header for the output .wav file
	vector<char> tt;
	cout << "ready to process\n";
	while (fi.good()) {
		short tmp[FRAME_SIZE];
		fi.read( (char*)&tmp , sizeof(short) * FRAME_SIZE);
		for (size_t i = 0; i<FRAME_SIZE; i++) x[i] = tmp[i];
		rnnoise_process_frame(st, x, x);
		for (size_t i = 0; i<FRAME_SIZE; i++) tmp[i] = x[i];
		fo.write( (char*)&tmp , sizeof(short)*FRAME_SIZE);
	}

	rnnoise_destroy(st);
	fi.close();
	fo.close();
	cout << "\n ok\n";

	return 0;
}
/*
//====================
// Can not use this library to read the PCM data
AudioFile<double> af;
af.load(directory + file_name);
vector<float> mono_data(af.samples.at(0).begin(), af.samples.at(0).end()); // 读取单声道数据

af.printSummary();


DenoiseState *st;
st = rnnoise_create();
cout << "\n ready to denoise...\n";
size_t frame_num = mono_data.size() / FRAME_SIZE;
vector<float> output;
float frame_output_temp[FRAME_SIZE];

for (size_t n = 0; n != frame_num - 1; ++n) {
vector<float> temp(mono_data.begin()+n*FRAME_SIZE, mono_data.begin() + (n+1)* FRAME_SIZE);
float frame_input[FRAME_SIZE];
for (int up = 0; up != FRAME_SIZE;++up) {
frame_input[up] = temp[up];
}
////rnnoise_process_frame(st, &temp[0], frame_output_temp);// convert vector to array
rnnoise_process_frame(st, frame_input, frame_output_temp);// convert vector to array
//output.emplace_back(temp.begin(), temp.end());
for (int up = 0; up != FRAME_SIZE; ++up) {
output.push_back( frame_output_temp[n] );
}
}

vector<vector<double>> output_mono(2);	//  需要指定大小
output_mono.at(0).assign(output.begin(), output.end());
output_mono.at(1).assign(output.begin(), output.end());
af.setAudioBuffer(output_mono);
cout << "\n ready to output\n";
af.save(directory+"after_rnnoise_2.wav");

rnnoise_destroy(st);
//=====================
*/

/*

//int i;
//int first = 1;
//float x[FRAME_SIZE];
//FILE *f1, *fout;
DenoiseState *st;
st = rnnoise_create();

//if (argc != 3) {
//	fprintf(stderr, "usage: %s <noisy speech> <output denoised>\n", argv[0]);
//	return 1;
//}
//f1 = fopen(argv[1], "r");
//fout = fopen(argv[2], "w");
//while (1) {
//	short tmp[FRAME_SIZE];
//	fread(tmp, sizeof(short), FRAME_SIZE, f1);
//	if (feof(f1)) break;
//	for (i = 0; i<FRAME_SIZE; i++) x[i] = tmp[i];
//	rnnoise_process_frame(st, x, x);
//	for (i = 0; i<FRAME_SIZE; i++) tmp[i] = x[i];
//	if (!first) fwrite(tmp, sizeof(short), FRAME_SIZE, fout);
//	first = 0;
//}

rnnoise_destroy(st);
//fclose(f1);
//fclose(fout);

*/

/*
//   以下自己编写的读取 wav 文件方式可以留作参考
HEADER wav_header;
ifstream fi;												// 输入时 ifstream
fi.open(file_name, ios::in || ios::binary);					// 读取文件
if (fi.is_open())
cout << "Open it successful." << endl;
else
cout << "Can not open it." << endl;

// 用c++常用方法获得文件大小
fi.seekg(0, ios::end);
wav_header.file_size = fi.tellg();

// 读取 PCM 数据大小
fi.seekg(0x28);
fi.read((char*)&wav_header.data_size, sizeof(wav_header.data_size));

// 将读取指针移动到 data 开端读取 data 数据
fi.seekg(0x2c);
wav_header.data = new unsigned char [wav_header.data_size];
fi.read( (char*)&wav_header.data, sizeof(char) * (wav_header.data_size));
*/