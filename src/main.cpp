/*
* Forked by Speed666 to add support for S905W2 and S905Y4 SC2 and S4gen CPUs.
* 
* 
* Copyright (C) 2016 OtherCrashOverride@users.noreply.github.com.
* All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2, as 
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
*/

#include <stdio.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <alsa/asoundlib.h>
#include <string> 
#include <queue>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include "InputDevice.h"
#include "MediaPlayer.h"

#include "FbdevAmlWindow.h"

#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <linux/input.h>
#include <linux/uinput.h>

#include <linux/kd.h>
#include <linux/fb.h>

#include "Osd.h"
#include "Codec.h"
#include "Compositor.h"


bool isRunning = true;

std::vector<InputDevicePtr> inputDevices;
std::vector<std::string> devices;


/*
# Disable alpha layer (hide video)
echo "d01068b4 0x7fc0" | sudo tee /sys/kernel/debug/aml_reg/paddr

# Enable alpha layer (show video)
echo "d01068b4 0x3fc0" | sudo tee /sys/kernel/debug/aml_reg/paddr
*/


// Signal handler for Ctrl-C
void SignalHandler(int s)
{
	isRunning = false;
}




void DisplayHelp()
{
		printf("Usage: amlplay [OPTIONS] [FILE|URL]\n");
		printf("Play video using hardware acceleration\n\n");

		printf("      --help\t\tDisplay this help information\n");
		printf("      --timeout\t\tTimeout for playing in sec\n");
		printf("      --audio hw:0,2\t\tAlsa Device to play audio on\n");
		printf("      --avdict 'opts'\tOptions to pass to libav\n");
		printf("  -l, --loop\t\tLoop continuously\n");
}

struct option longopts[] = {
	{ "help",			no_argument,        NULL,          'h' },
	{ "time",			required_argument,  NULL,          't' },
	{ "chapter",		required_argument,  NULL,          'c' },
	{ "video",			required_argument,  NULL,          'v' },
	{ "audio",			required_argument,  NULL,          'a' },
	{ "timeout",		required_argument,  NULL,          't' },
	{ "avdict",			required_argument,  NULL,          'A' },
	{ "loop",			no_argument,        NULL,          'l' },
	{ 0, 0, 0, 0 }
};




int main(int argc, char** argv)
{
	if (argc < 2)
	{
		DisplayHelp();
		exit(EXIT_SUCCESS);
	}


	// Trap signal to clean up
	signal(SIGINT, SignalHandler);


	// options
	int c;
	double optionStartPosition = 0;
	int optionChapter = -1;
	int timeout = -1;
	int optionVideoIndex = 0;
	int optionAudioIndex = 0;
	int optionSubtitleIndex = -1;	//disabled by default
	std::string avOptions;
	std::string alsaDevice = "hw:0,2";
	bool optionLoop = false;

	while ((c = getopt_long(argc, argv, "t:c:l", longopts, NULL)) != -1)
	{
		switch (c)
		{
			
			case 'h':
				DisplayHelp();
				exit(EXIT_SUCCESS);

			case 'A':
				avOptions = optarg;
				break;

			case 't':
				timeout = atoi(optarg);
				printf("timeout=%d\n", timeout);
			
			break;

			case 'c':
				optionChapter = atoi(optarg);
				printf("optionChapter=%d\n", optionChapter);
				break;

			case 'v':
				optionVideoIndex = atoi(optarg);
				printf("optionVideoIndex=%d\n", optionVideoIndex);
				break;

			case 'a':
				alsaDevice = optarg;
				printf("optionAudioIndex=%d\n", optionAudioIndex);
				break;

			case 's':
				optionSubtitleIndex = atoi(optarg);
				printf("optionSubtitleIndex=%d\n", optionSubtitleIndex);
				break;

			case 'l':
				optionLoop = true;
				printf("optionLoop=true\n");
				break;

			default:
				DisplayHelp();
				exit(EXIT_FAILURE);

				//printf("?? getopt returned character code 0%o ??\n", c);
				//break;
		}
	}


	const char* url = nullptr;
	if (optind < argc)
	{
		//printf("non-option ARGV-elements: ");
		while (optind < argc)
		{
			url = argv[optind++];
			//printf("%s\n", url);
			break;
		}
	}


	if (url == nullptr)
	{
		DisplayHelp();
		exit(EXIT_FAILURE);
	}
	
	//Enable Video Layer in GE2D in SC2 and S4 SoC
	int f = open("/sys/module/decoder_common/parameters/debugflags",O_WRONLY);
	if (f != -1)
	{
		write(f, "4", 1);
		close(f);
	}
	int f2 = open("/sys/class/graphics/fb0/blank",O_WRONLY);
	if (f2 != -1)
	{
		write(f2, "1", 1);
		close(f2);
	}
	int f3 = open("/sys/class/graphics/fb1/blank",O_WRONLY);
	if (f3 != -1)
	{
		write(f3, "1", 1);
		close(f3);
	}
	
	f3 = open("/sys/class/video/disable_video",O_WRONLY);
	if (f != -1)
	{
		write(f3, "2", 1);
		close(f3);
	}
	
	// Initialize libav
	av_log_set_level(AV_LOG_VERBOSE);
	
	avformat_network_init();





	
	WindowSPTR window;
	OsdSPTR osd;
	bool isFbdev = false;


	window = std::make_shared<FbdevAmlWindow>();
	isFbdev = true;

	window->ProcessMessages();


	CompositorSPTR compositor = std::make_shared<Compositor>(window->Width(), window->Height());
	osd = std::make_shared<Osd>(compositor);
	
	//printf("Alsa device %s",alsaDevice.c_str());

	MediaPlayerSPTR mediaPlayer = std::make_shared<MediaPlayer>(url,
		avOptions,
		compositor,
		optionVideoIndex,
		optionAudioIndex,
		optionSubtitleIndex,
		alsaDevice);


	mediaPlayer->Seek(optionStartPosition);
	mediaPlayer->SetState(MediaState::Play);

	
	isRunning = true;
	bool isPaused = false;
	
	long timeout_counter = 0;
	
	while (isRunning)
	{
		isRunning = window->ProcessMessages();

		if (mediaPlayer->IsEndOfStream())
		{
			if (optionLoop)
			{
				mediaPlayer->Seek(0);
				mediaPlayer->SetState(MediaState::Play);
			}
			else
			{
				isRunning = false;
			}
		}
		else if (timeout != 1 && timeout_counter/1000000 > timeout)
		{
			printf("Timeout after %ld secs, exiting.",timeout_counter/1000000);
			isRunning = false;
		}
		else
		{
			usleep(100);
			timeout_counter += 100;
		}
	}


	printf("MAIN: Playback finished.\n");

	return 0;
}
