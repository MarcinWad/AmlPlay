!!! Based on [CrashOverride original c2play](https://github.com/OtherCrashOverride) for Ordoid platform. All credits belongs to him.

Install dependencies for ALSA:
	`sudo apt install libasound2-dev`

Compile and configure latest FFMPEG from trunk:
`wget https://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2`
`./configure`
`make & make install`

Compile AmlPlay:
`make -j4`

AmlPlay needs kernel compiled with GE2D and Framebuffer support and loaded TEE modules for video decoders.

Khadas VIM1S should work out of the box. Other devices (Android Players) needs DTB to be coverted to mainline Linux before usage.

Devices tested: Khadas VIM1S, Android TV player OEM with S905W2 and HAKOPro S905Y4.

Changes from original CrashOverride c2play:
	- added AV1 (4K)
	- added VP9 (4K)
	- added AVS2 (4K)
	- moved from ffmpeg 4 to latest branch (6.x)
	- added  timeout option
	- added ALSA device selection
	- removed Subititle support 
	- reconstructed video_buffer for end of stream instead of wrong PTS calculations in small files or video-only files
	- added support for SC2 and S4 platform support
	- updated Amlogic Headers and structures to talk with decoders
	- added support for Amlogic kernel 5.4 

Warning!
AVS2 support is broken in 5.4.180 media_modules Amlogic kernel modules! Need the 5.4.120 or kernel 5.4.120.




`Command line options:`
`	--loop			Loop continuously.`
`	--audio hw:0,2	Set ALSA device output (hw:0,2 is default)`
`	--timeout X		Timeout of playing source in seconds (useful for RTSP stream)`

Supported codecs:

	Video:
	AV1 (4K60), VP9 (4K60), Mpeg2, Mpeg4v3 (Divx/Xvid), Mpeg4 (MP4), H264 (AVC), H265 (HEVC), AVS2, VC1 support is experimental

	Audio (down-mixed only):
		Mpeg Layer II (MP2), Mpeg Layer III (MP3), AAC, AC3, EAC3, DTS,
		TrueHD, Opus, Vorbis
		WmaPro support is experimental
			
	Supported Input: File, RTSP, RTMP.
			
	Supported containers:
		MKV, AVI, MP4, WEBM have been tested.

