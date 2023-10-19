!!! Based on CrashOverride original c2play for Ordoid platform !!!

Install dependencies:
	sudo apt install libasound2-dev libavformat-dev


AmlPlay needs kernel compiled with GE2D and Framebuffer support and loaded TEE modules for video decoders.

Khadas VIM1S should work out of the box. Other devices (Android Players) needs DTB to be coverted to mainline Linux before usage.

Devices tested: Khadas VIM1S, Android TV player OEM with S905W2 and HAKOPro S905Y4.

Compile:
	make -j4


Command line options:
	--loop			Loop continuously.
	--audio hw:0,2	Set ALSA device output (hw:0,2 is default)

Supported codecs:
	Video:
		AV1 (4K60), VP9 (4K60), Mpeg2, Mpeg4v3 (Divx/Xvid), Mpeg4 (MP4), H264 (AVC), H265 (HEVC)
		VC1 support is experimental, AV1, VP9

	Audio (down-mixed only):
		Mpeg Layer II (MP2), Mpeg Layer III (MP3), AAC, AC3, EAC3, DTS,
		TrueHD, Opus, Vorbis
		WmaPro support is experimental

Supported Input: RTSP, RTMP.

Supported containers:
	MKV, AVI, MP4, WEBM have been tested.
