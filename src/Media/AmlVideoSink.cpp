/*
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

#include "AmlVideoSink.h"
#include "obuparse.h"

extern bool needAudioSync;

void AmlVideoSinkElement::timer_Expired(void* sender, const EventArgs& args)
{
	//vdec_status vdecStatus = amlCodec.GetVdecStatus();
	//printf("AmlVideoSinkElement: timer_Expired - width=%u, height=%u, fps=%u, error_count=%u, status=0x%x\n",
	//	vdecStatus.width, vdecStatus.height, vdecStatus.fps, vdecStatus.error_count, vdecStatus.status);

	double pts = amlCodec.GetCurrentPts();
	//printf("AmlVideoSinkElement: pts=%f, \n", pts);


	timerMutex.Lock();

	if (isEndOfStream && (State() != MediaState::Pause))
	{

#if 0
		double pts = amlCodec.GetCurrentPts();
		//printf("AmlVideoSinkElement: timer_Expired - isEndOfStream=true, pts=%f, eosPts=%f\n", pts, eosPts);

		// If the pts is the same as last time (clock tick = 1/4 s),
		// then assume that playback is done.
		if (pts == eosPts)
		{
			//printf("AmlVideoSinkElement: timer_Expired (pausing) - isEndOfStream=true, pts=%f, eosPts=%f\n", pts, eosPts);
			SetState(MediaState::Pause);
			isEndOfStream = false;
			eosPts = -1;
		}
		else
		{
			eosPts = pts;
		}
#else
		//Fix this
		buf_status bufferStatus = amlCodec.GetBufferStatus();
		//int api = codec_get_vbuf_state(&codecContext, &bufferStatus);
		//if (api == 0)
		{
			//printf("AmlVideoSinkElement: codec_get_vbuf_state free_len=%d, size=%d, data_len=%d, read_pointer=%x, write_pointer=%x\n",
			//	bufferStatus.free_len, bufferStatus.size, bufferStatus.data_len, bufferStatus.read_pointer, bufferStatus.write_pointer);

			// Testing has shown this value does not reach zero
			if (bufferStatus.data_len < 50000)
			{
				//printf("AmlVideoSinkElement: timer_Expired - isEndOfStream=true, bufferStatus.data_len=%d (Pausing).\n", bufferStatus.data_len);
				SetState(MediaState::Pause);
				isEndOfStream = true;
				eosPts = -1;
				////debuging
				//printf("AmlVideoSinkElement: timer_Expired - size=%d, data_len=%d, free_len=%d, read_pointer=%u, write_pointer=%u\n",
				//	bufferStatus.size, bufferStatus.data_len, bufferStatus.free_len, bufferStatus.read_pointer, bufferStatus.write_pointer);

				//vdec_status vdecStatus = amlCodec.GetVdecStatus();
				//printf("AmlVideoSinkElement: timer_Expired - width=%u, height=%u, fps=%u, error_count=%u, status=0x%x\n",
				//	vdecStatus.width, vdecStatus.height, vdecStatus.fps, vdecStatus.error_count, vdecStatus.status);
			}
		}
#endif
	}

	timerMutex.Unlock();


	// Wake up video thread to prevent stalls if there
	// is no clock input.
	Wake();

	//printf("AmlVideoSinkElement: timer expired.\n");
}

void AmlVideoSinkElement::SetupHardware()
{
	int width = videoPin->InfoAs()->Width;
	int height = videoPin->InfoAs()->Height;
	double frameRate = videoPin->InfoAs()->FrameRate;
	bool hasAudio = needAudioSync;
	if (width == 0) width = 1920;
	if (height == 0) height = 1080;
	amlCodec.Open(videoPin->InfoAs()->Format, width, height, frameRate, hasAudio);

	//memset(&codecContext, 0, sizeof(codecContext));

	//codecContext.stream_type = STREAM_TYPE_ES_VIDEO;
	//codecContext.has_video = 1;
	//codecContext.noblock = 1;

	//// Note: Without EXTERNAL_PTS | SYNC_OUTSIDE, the codec auto adjusts
	//// frame-rate from PTS 
	//codecContext.am_sysinfo.param = (void*)(EXTERNAL_PTS | SYNC_OUTSIDE); //USE_IDR_FRAMERATE

	//// Note: Testing has shown that the ALSA clock requires the +1
	//codecContext.am_sysinfo.rate = 96000.0 / frameRate + 1;


	//switch (videoPin->InfoAs()->Format)
	//{
	//case VideoFormatEnum::Mpeg2:
	//	printf("AmlVideoSink - VIDEO/MPEG2\n");

	//	codecContext.video_type = VFORMAT_MPEG12;
	//	codecContext.am_sysinfo.format = VIDEO_DEC_FORMAT_UNKNOW;
	//	break;

	//case VideoFormatEnum::Mpeg4V3:
	//	printf("AmlVideoSink - VIDEO/MPEG4V3\n");

	//	codecContext.video_type = VFORMAT_MPEG4;
	//	codecContext.am_sysinfo.format = VIDEO_DEC_FORMAT_MPEG4_3;
	//	break;

	//case VideoFormatEnum::Mpeg4:
	//	printf("AmlVideoSink - VIDEO/MPEG4\n");

	//	codecContext.video_type = VFORMAT_MPEG4;
	//	codecContext.am_sysinfo.format = VIDEO_DEC_FORMAT_MPEG4_5;
	//	break;

	//case VideoFormatEnum::Avc:
	//{
	//	if (width > 1920 || height > 1080)
	//	{
	//		printf("AmlVideoSink - VIDEO/H264_4K2K\n");

	//		codecContext.video_type = VFORMAT_H264_4K2K;
	//		codecContext.am_sysinfo.format = VIDEO_DEC_FORMAT_H264_4K2K;
	//	}
	//	else
	//	{
	//		printf("AmlVideoSink - VIDEO/H264\n");

	//		codecContext.video_type = VFORMAT_H264;
	//		codecContext.am_sysinfo.format = VIDEO_DEC_FORMAT_H264;
	//	}
	//}
	//break;

	//case VideoFormatEnum::Hevc:
	//	printf("AmlVideoSink - VIDEO/HEVC\n");

	//	codecContext.video_type = VFORMAT_HEVC;
	//	codecContext.am_sysinfo.format = VIDEO_DEC_FORMAT_HEVC;
	//	//codecContext.am_sysinfo.param = (void*)(EXTERNAL_PTS | SYNC_OUTSIDE);
	//	break;


	//case VideoFormatEnum::VC1:
	//	printf("AmlVideoSink - VIDEO/VC1\n");
	//	codecContext.video_type = VFORMAT_VC1;
	//	codecContext.am_sysinfo.format = VIDEO_DEC_FORMAT_WVC1;
	//	break;

	//default:
	//	printf("AmlVideoSink - VIDEO/UNKNOWN(%d)\n", (int)videoFormat);
	//	throw NotSupportedException();
	//}


	//// Rotation
	////codecContext.am_sysinfo.param = (void*)((unsigned long)(codecContext.am_sysinfo.param) | 0x10000); //90
	////codecContext.am_sysinfo.param = (void*)((unsigned long)(codecContext.am_sysinfo.param) | 0x20000); //180
	////codecContext.am_sysinfo.param = (void*)((unsigned long)(codecContext.am_sysinfo.param) | 0x30000); //270


	//// Debug info
	//printf("\tw=%d h=%d ", width, height);

	//printf("fps=%f ", frameRate);

	//printf("am_sysinfo.rate=%d ",
	//	codecContext.am_sysinfo.rate);

	//printf("\n");


	//// Intialize the hardware codec
	//int api = codec_init(&codecContext);
	////int api = codec_init_no_modeset(&codecContext);
	//if (api != 0)
	//{
	//	printf("codec_init failed (%x).\n", api);
	//	throw Exception();
	//}


	//// This is needed because the codec remains paused
	//// even after closing
	//int ret = codec_resume(&codecContext);


	//WriteToFile("/sys/class/graphics/fb0/blank", "1");
	//WriteToFile("/sys/module/amvdec_h265/parameters/dynamic_buf_num_margin", "16");
}

void AmlVideoSinkElement::ProcessBuffer(AVPacketBufferSPTR buffer)
{
	//printf("AmlVideoSinkElement::ProcessBuffer\n");
	FILE *fp;
	playPauseMutex.Lock();
	 uint32_t length = 0;
	uint8_t meta_buffer[1024] = {0};
	uint32_t meta_size = 0;
	uint8_t *avdata;
	if (doResumeFlag)
	{
		//codec_resume(&codecContext);
		amlCodec.Resume();

		doResumeFlag = false;
	}


	//AVPacketBufferPTR buffer = std::static_pointer_cast<AVPacketBuffer>(buf);
	AVPacket* pkt = buffer->GetAVPacket();

	
	unsigned char* nalHeader = (unsigned char*)pkt->data;

#if 0
	printf("Header (pkt.size=%x):\n", pkt->size);
	for (int j = 0; j < 256; ++j)	//nalHeaderLength
	{
		printf("%02x ", nalHeader[j]);
	}
	printf("\n");
#endif

	if (isFirstVideoPacket)
	{
		printf("Header (pkt.size=%x):\n", pkt->size);
		for (int j = 0; j < 16; ++j)	//nalHeaderLength
		{
			printf("%02x ", nalHeader[j]);
		}
		printf("\n");

		if (nalHeader[0] == 0 && nalHeader[1] == 0 &&
			nalHeader[2] == 1)
		{
			isAnnexB = true;
			isShortStartCode = true;
		}
		else if (nalHeader[0] == 0 && nalHeader[1] == 0 &&
			nalHeader[2] == 0 && nalHeader[3] == 1)
		{
			isAnnexB = true;
			isShortStartCode = false;
		}

		//double timeStamp = av_q2d(buffer->TimeBase()) * pkt->pts;
		//unsigned long pts = (unsigned long)(timeStamp * PTS_FREQ);

		//amlCodec.SetSyncThreshold(pts);

		isFirstVideoPacket = false;

		printf("isAnnexB=%u\n", isAnnexB);
		printf("isShortStartCode=%u\n", isShortStartCode);
	}


	uint64_t pts = 0;

	if (pkt->pts != AV_NOPTS_VALUE)
	{
		double timeStamp = av_q2d(buffer->TimeBase()) * pkt->pts;
		pts = (uint64_t)(timeStamp * PTS_FREQ);

		estimatedNextPts = pkt->pts + pkt->duration;
		lastTimeStamp = timeStamp;
	}


	isExtraDataSent = false;
	
	

	switch (videoFormat)
	{
		case VideoFormatEnum::Mpeg2:
		{
			SendCodecData(pts, pkt->data, pkt->size);
			break;
		}
		
		
		
		case VideoFormatEnum::AV1:
		{
			//Outgoing data will be bigger than original because of AML headers 
			//every OBU has extra 20bytes of AML header and we don't know yet how many OBUs we have in current packet
			
			avdata = static_cast<uint8_t *>(malloc(pkt->size + 0x1000));
			//Amlogic need it's 20 bytes header to every AV1 frame, fix it...
			AV1CreateFrame(pkt->data, pkt->size, avdata, &length);
			SendCodecData(pts, avdata, length);
			free(avdata);
			break;
		}
		
		case VideoFormatEnum::VP9:
		{
			//Outgoing data will be bigger than original because of AML headers 
			//every OBU has extra 16bytes of AML header and we don't know yet how many frames we have in current packet
			
			avdata = static_cast<uint8_t *>(malloc(pkt->size + 0x1000));
			//Amlogic need it's 20 bytes header to every VP9 frames, fix it...
			VP9CreateFrame(pkt->data, pkt->size, avdata, &length);
			SendCodecData(pts, avdata, length);
			free(avdata);
			break;
		}

		case VideoFormatEnum::Mpeg4:
		{
			unsigned char* video_extra_data = &extraData[0];
			int video_extra_data_size = extraData.size();

			SendCodecData(0, video_extra_data, video_extra_data_size);


			SendCodecData(pts, pkt->data, pkt->size);

			break;
		}

		case VideoFormatEnum::Mpeg4V3:
		{
			//printf("Sending Divx3\n");
			Divx3Header(videoPin->InfoAs()->Width, videoPin->InfoAs()->Height, pkt->size);
			SendCodecData(pts, &videoExtraData[0], videoExtraData.size());
			//amlCodec.SendData(pts, &videoExtraData[0], videoExtraData.size());

			SendCodecData(0, pkt->data, pkt->size);
			//amlCodec.SendData(0, pkt->data, pkt->size);
			break;
		}
			
		case VideoFormatEnum::Avc:
		case VideoFormatEnum::Hevc:
		{
			if (!isAnnexB)
			{
				// Five least significant bits of first NAL unit byte signify nal_unit_type.
				int nal_unit_type;
				const int nalHeaderLength = 4;

				while (nalHeader < (pkt->data + pkt->size))
				{
					switch (videoFormat)
					{
						case VideoFormatEnum::Avc:
						{
							// Copy AnnexB data if NAL unit type is 5
							nal_unit_type = nalHeader[nalHeaderLength] & 0x1F;

							if (!isExtraDataSent || nal_unit_type == 5)
							{
								ConvertH264ExtraDataToAnnexB();

								SendCodecData(pts, &videoExtraData[0], videoExtraData.size());
								//amlCodec.SendData(pts, &videoExtraData[0], videoExtraData.size());
							}

							isExtraDataSent = true;
						}
						break;

						case VideoFormatEnum::Hevc:
						{
							nal_unit_type = (nalHeader[nalHeaderLength] >> 1) & 0x3f;

							/* prepend extradata to IRAP frames */
							if (!isExtraDataSent || (nal_unit_type >= 16 && nal_unit_type <= 23))
							{
								HevcExtraDataToAnnexB();

								SendCodecData(0, &videoExtraData[0], videoExtraData.size());
								//amlCodec.SendData(0, &videoExtraData[0], videoExtraData.size());
							}

							isExtraDataSent = true;
						}
						break;

						default:
							throw NotSupportedException("Unexpected video format.");
					}


					// Overwrite header NAL length with startcode '0x00000001' in *BigEndian*
					int nalLength = nalHeader[0] << 24;
					nalLength |= nalHeader[1] << 16;
					nalLength |= nalHeader[2] << 8;
					nalLength |= nalHeader[3];

					if (nalLength < 0 || nalLength > pkt->size)
					{
						printf("Invalid NAL length=%d, pkt->size=%d\n", nalLength, pkt->size);
						throw Exception();
					}

					nalHeader[0] = 0;
					nalHeader[1] = 0;
					nalHeader[2] = 0;
					nalHeader[3] = 1;

					nalHeader += nalLength + 4;
				}
			}

			if (!SendCodecData(pts, pkt->data, pkt->size))
			{
				// Resend extra data on codec reset
				isExtraDataSent = false;

				printf("AmlVideoSinkElement::ProcessBuffer - SendData Failed.\n");
			}

			break;
		}
		case VideoFormatEnum::AVS2:
		{
			SendCodecData(pts, pkt->data, pkt->size);
			break;
		}
		case VideoFormatEnum::VC1:
		{
			SendCodecData(pts, pkt->data, pkt->size);

			break;
		}

		default:
			throw NotSupportedException("none");
	}


	if (doPauseFlag)
	{
		//codec_pause(&codecContext);
		amlCodec.Pause();
		doPauseFlag = false;
	}

	playPauseMutex.Unlock();
}

bool AmlVideoSinkElement::SendCodecData(unsigned long pts, unsigned char* data, int length)
{
	//printf("AmlVideoSink: SendCodecData - pts=%lu, data=%p, length=0x%x\n", pts, data, length);
	bool result = true;

	if (pts > 0)
	{
		amlCodec.CheckinPts(pts);
	}
	//AVS2 Needs smaller parts maybe....
	
	
	
	
	int maxAttempts = 150;
	int offset = 0;
	/*
	int parts = 32768;
	
	if (length < parts)
	{
			parts = length;
	}
	
	while (offset != length)
	{
		printf("offset %d %d \n",offset,parts);
		if (!IsRunning())
		{
			result = false;
			break;
			
		}
		
		int count = amlCodec.WriteData(data + offset, parts);
		
		offset += parts;
		
		if (offset + parts > length)
		{
			//40000
			parts = length - offset;
			
		}
		else
		{
			parts = 32768;
			
		}	
		
		
	}
	return result;
	*/
	
	while (offset < length)
	{
		if (!IsRunning())
		{
			printf("is not running");
			result = false;
			break;
			
		}

		int count = amlCodec.WriteData(data + offset, length - offset);
		if (count > 0)
		{
			offset += count;
			//printf("codec_write send %x bytes of %x total.\n", count, length);
			
		}
		else
		{
			//printf("codec_write failed (%x).\n", count);
			maxAttempts -= 1;

			if (maxAttempts <= 0)
			{
				//printf("codec_write max attempts exceeded.\n");
				
				amlCodec.Reset();
				result = false;

				break;
			}

			sleep(0);
		}
	}
	
	//printf("OUT ==== AmlVideoSink: SendCodecData - pts=%lu, data=%p, length=0x%x\n", pts, data, length);

	return result;
}



double AmlVideoSinkElement::Clock()
{
	// NOTE: This value is not valid until pts check-in
	// and SetPts have converged.

	//int vpts = codec_get_vpts(&codecContext);
	//double result = vpts / (double)PTS_FREQ;

	//printf("AmlVideoSinkElement::Clock() = %f\n", result);

	//return result;

	if (amlCodec.IsOpen())
	{
		return amlCodec.GetCurrentPts();
	}
	else
	{
		return 0;
	}
}



void AmlVideoSinkElement::Initialize()
{
	ClearOutputPins();
	ClearInputPins();

	// TODO: Pin format negotiation

	{
		// Create a video pin
		VideoPinInfoSPTR info = std::make_shared<VideoPinInfo>();
		info->Format = VideoFormatEnum::Unknown;
		info->FrameRate = 0;

		ElementWPTR weakPtr = shared_from_this();
		videoPin = std::make_shared<VideoInPin>(weakPtr, info);
		AddInputPin(videoPin);
	}

	{
		// Create a clock pin
		PinInfoSPTR info = std::make_shared<PinInfo>(MediaCategoryEnum::Clock);

		ElementWPTR weakPtr = shared_from_this();
		//clockInPin = std::make_shared<AmlVideoSinkClockInPin>(weakPtr, info, &codecContext);
		clockInPin = std::make_shared<AmlVideoSinkClockInPin>(weakPtr, info, &amlCodec);
		AddInputPin(clockInPin);
	}

	//{
	//	// Create a clock out pin
	//	//
	//	PinInfoSPTR info = std::make_shared<PinInfo>(MediaCategoryEnum::Clock);

	//	ElementWPTR weakPtr = shared_from_this();
	//	clockOutPin = std::make_shared<AmlVideoSinkClockOutPin>(weakPtr, info, &codecContext);
	//	AddOutputPin(clockOutPin);
	//}


	// Event handlers
	timerExpiredListener = std::make_shared<EventListener<EventArgs>>(
		std::bind(&AmlVideoSinkElement::timer_Expired, this, std::placeholders::_1, std::placeholders::_2));

	timer.Expired.AddListener(timerExpiredListener);
	timer.SetInterval(0.5f); //0.25f
	timer.Start();
}

void AmlVideoSinkElement::DoWork()
{
	// TODO: Refactor to thread each input pin

	// TODO: Investigate merging PushProcessedBuffer and ReturnProcessedBuffer
	//	     into the latter.

	//printf("DoWork");
	BufferSPTR buffer;

	//if (videoPin->TryPeekFilledBuffer(&buffer))
	//if (State() == MediaState::Play)
	{
		//AVPacketBufferSPTR avPacketBuffer = std::static_pointer_cast<AVPacketBuffer>(buffer);

		// Video
		while (State() == MediaState::Play && 
			   videoPin->TryGetFilledBuffer(&buffer))
		{
			if (isFirstData)
			{
				OutPinSPTR otherPin = videoPin->Source();
				if (otherPin)
				{
					if (otherPin->Info()->Category() != MediaCategoryEnum::Video)
					{
						throw InvalidOperationException("AmlVideoSink: Not connected to a video pin.");
					}

					VideoPinInfoSPTR info = std::static_pointer_cast<VideoPinInfo>(otherPin->Info());
					videoFormat = info->Format;
					//frameRate = info->FrameRate;
					extraData = *(info->ExtraData);

					// TODO: This information should be copied
					//       as part of pin negotiation
					videoPin->InfoAs()->Format = info->Format;
					videoPin->InfoAs()->Width = info->Width;
					videoPin->InfoAs()->Height = info->Height;
					videoPin->InfoAs()->FrameRate = info->FrameRate;
					videoPin->InfoAs()->ExtraData = info->ExtraData;
					videoPin->InfoAs()->HasEstimatedPts = info->HasEstimatedPts;

					clockInPin->SetFrameRate(info->FrameRate);

					//printf("AmlVideoSink: ExtraData size=%ld\n", (long int)extraData.size());

					SetupHardware();

					//clockThread.Start();

					isFirstData = false;
				}
			}


			switch (buffer->Type())
			{
				case BufferTypeEnum::Marker:
				{
					MarkerBufferSPTR markerBuffer = std::static_pointer_cast<MarkerBuffer>(buffer);
					printf("AmlVideoSinkElement: got marker buffer Marker=%d\n", (int)markerBuffer->Marker());

					switch (markerBuffer->Marker())
					{
						case MarkerEnum::EndOfStream:
							isEndOfStream = true;
							//eosPts = buffer->TimeStamp();
							break;

						case MarkerEnum::Discontinue:
							//codec_reset(&codecContext);
							break;

						default:
							// ignore unknown 
							break;
					}

					break;
				}

				case BufferTypeEnum::AVPacket:
				{
					//printf("AmlVideoSink: Got a buffer.\n");
					AVPacketBufferSPTR avPacketBuffer = std::static_pointer_cast<AVPacketBuffer>(buffer);
					ProcessBuffer(avPacketBuffer);
					//eosPts = buffer->TimeStamp();
					break;
				}

				default:
					throw NotSupportedException("Unexpected buffer type.");
			}

			videoPin->PushProcessedBuffer(buffer);
			videoPin->ReturnProcessedBuffers();

			//printf("AmlVideoSink: filledBufferCount=%d\n", (int)videoPin->FilledBufferCount());
		}
	}
}

void AmlVideoSinkElement::ChangeState(MediaState oldState, MediaState newState)
{
	Element::ChangeState(oldState, newState);

//	printf("AmlVideoSinkElement::ChangeState");

	switch (newState)
	{
		case MediaState::Play:
		{
			playPauseMutex.Lock();

			//int ret = codec_resume(&codecContext);
			if (amlCodec.IsOpen())
			{
				amlCodec.Resume();
			}

			//doPauseFlag = false;
			//doResumeFlag = true;
			//isEndOfStream = false;
			//timer.Start();

			playPauseMutex.Unlock();
			break;
		}

		case MediaState::Pause:
		{
			playPauseMutex.Lock();

			//doResumeFlag = false;
			//doPauseFlag = true;
			//int ret = codec_pause(&codecContext);

			if (amlCodec.IsOpen())
			{
				amlCodec.Pause();
			}

			//timer.Stop();

			playPauseMutex.Unlock();
			break;
		}

		default:
			break;
	}
}


void AmlVideoSinkElement::Flush()
{
	timerMutex.Lock();
	playPauseMutex.Lock();

	timer.Stop();


	////int codec_flush_video(codec_para_t *pcodec)
	//if (codec_flush_video(&codecContext) < 0)
	//{
	//	printf("codec_flush_video failed.\n");
	//}

	// Set the time to max
	//codec_set_pcrscr(&codecContext, 0);



	////
	//printf("AmlVideoSinkElement: codec_set_dec_reset.\n");
	//codec_set_dec_reset(&codecContext);

	//printf("AmlVideoSinkElement: codec_reset.\n");
	//codec_reset(&codecContext);



	printf("AmlVideoSinkElement: calling base.\n");

	Element::Flush();


	if (amlCodec.IsOpen())
	{
		//codec_set_syncenable(&codecContext, 0);

		//printf("AmlVideoSinkElement: codec_resume.\n");
		////codec_resume(&codecContext);
		//amlCodec.Resume();

		printf("AmlVideoSinkElement: reset.\n");
		//codec_close(&codecContext);
		amlCodec.Reset();

		//printf("AmlVideoSinkElement: codec_init.\n");
		//codec_init(&codecContext);

		//printf("AmlVideoSinkElement: codec_pause.\n");
		////codec_pause(&codecContext);
		//amlCodec.Pause();

		//codec_set_syncenable(&codecContext, 1);
	}

	isEndOfStream = false;

	timer.Start();

	playPauseMutex.Unlock();
	timerMutex.Unlock();

	printf("AmlVideoSinkElement: Flush exited.\n");

}

void AmlVideoSinkElement::Terminating()
{
	timerMutex.Lock();
	playPauseMutex.Lock();

	timer.Stop();

	//codec_resume(&codecContext);
	amlCodec.Resume();

	//codec_close(&codecContext);
	amlCodec.Close();

	//codec_close_no_modeset(&codecContext);

	playPauseMutex.Unlock();
	timerMutex.Unlock();
}


//void AmlVideoSinkElement::WriteToFile(const char* path, const char* value)
//{
//	int fd = open(path, O_RDWR | O_TRUNC, 0644);
//	if (fd < 0)
//	{
//		printf("WriteToFile open failed: %s = %s\n", path, value);
//		throw Exception();
//	}
//
//	if (write(fd, value, strlen(value)) < 0)
//	{
//		printf("WriteToFile write failed: %s = %s\n", path, value);
//		throw Exception();
//	}
//
//	close(fd);
//}

void AmlVideoSinkElement::Divx3Header(int width, int height, int packetSize)
{
	// Bitstream info from Kodi

	videoExtraData.clear();

	videoExtraData.push_back(0x00);
	videoExtraData.push_back(0x00);
	videoExtraData.push_back(0x00);
	videoExtraData.push_back(0x01);

	unsigned i = (width << 12) | (height & 0xfff);
	videoExtraData.push_back(0x20);
	videoExtraData.push_back((i >> 16) & 0xff);
	videoExtraData.push_back((i >> 8) & 0xff);
	videoExtraData.push_back(i & 0xff);
	videoExtraData.push_back(0x00);
	videoExtraData.push_back(0x00);

	const unsigned char divx311_chunk_prefix[] =
	{
		0x00, 0x00, 0x00, 0x01,
		0xb6, 'D', 'I', 'V', 'X', '3', '.', '1', '1'
	};

	for (size_t i = 0; i < sizeof(divx311_chunk_prefix); ++i)
	{
		videoExtraData.push_back(divx311_chunk_prefix[i]);
	}

	videoExtraData.push_back((packetSize >> 24) & 0xff);
	videoExtraData.push_back((packetSize >> 16) & 0xff);
	videoExtraData.push_back((packetSize >> 8) & 0xff);
	videoExtraData.push_back(packetSize & 0xff);
}





void AmlVideoSinkElement::ConvertH264ExtraDataToAnnexB()
{
	void* video_extra_data = &extraData[0];
	int video_extra_data_size = extraData.size();

	videoExtraData.clear();

	if (video_extra_data_size > 0)
	{
		unsigned char* extraData = (unsigned char*)video_extra_data;

		// http://aviadr1.blogspot.com/2010/05/h264-extradata-partially-explained-for.html

		const int spsStart = 6;
		int spsLen = extraData[spsStart] * 256 + extraData[spsStart + 1];

		videoExtraData.push_back(0);
		videoExtraData.push_back(0);
		videoExtraData.push_back(0);
		videoExtraData.push_back(1);

		for (int i = 0; i < spsLen; ++i)
		{
			videoExtraData.push_back(extraData[spsStart + 2 + i]);
		}


		int ppsStart = spsStart + 2 + spsLen + 1; // 2byte sbs len, 1 byte pps start code
		int ppsLen = extraData[ppsStart] * 256 + extraData[ppsStart + 1];

		videoExtraData.push_back(0);
		videoExtraData.push_back(0);
		videoExtraData.push_back(0);
		videoExtraData.push_back(1);

		for (int i = 0; i < ppsLen; ++i)
		{
			videoExtraData.push_back(extraData[ppsStart + 2 + i]);
		}

	}

#if 0
	printf("EXTRA DATA = ");

	for (int i = 0; i < videoExtraData.size(); ++i)
	{
		printf("%02x ", videoExtraData[i]);

	}

	printf("\n");
#endif
}

void AmlVideoSinkElement::AmlVideoSinkElement::HevcExtraDataToAnnexB()
{
	void* video_extra_data = &extraData[0];
	int video_extra_data_size = extraData.size();


	videoExtraData.clear();

	if (video_extra_data_size > 0)
	{
		unsigned char* extraData = (unsigned char*)video_extra_data;

		// http://fossies.org/linux/ffmpeg/libavcodec/hevc_mp4toannexb_bsf.c

		int offset = 21;
		int length_size = (extraData[offset++] & 3) + 1;
		length_size = length_size;// silence compiler warning

		int num_arrays = extraData[offset++];

		//printf("HevcExtraDataToAnnexB: length_size=%d, num_arrays=%d\n", length_size, num_arrays);


		for (int i = 0; i < num_arrays; i++)
		{
			int type = extraData[offset++] & 0x3f;
			type = type; // silence compiler warning

			int cnt = extraData[offset++] << 8;
			cnt |= extraData[offset++];

			for (int j = 0; j < cnt; j++)
			{
				videoExtraData.push_back(0);
				videoExtraData.push_back(0);
				videoExtraData.push_back(0);
				videoExtraData.push_back(1);

				int nalu_len = extraData[offset++] << 8;
				nalu_len |= extraData[offset++];

				for (int k = 0; k < nalu_len; ++k)
				{
					videoExtraData.push_back(extraData[offset++]);
				}
			}
		}

#if 0
		printf("EXTRA DATA = ");

		for (int i = 0; i < videoExtraData.size(); ++i)
		{
			printf("%02x ", videoExtraData[i]);
		}

		printf("\n");
#endif
	}
	
	
	
}

int AmlVideoSinkElement::AV1CreateFrame(uint8_t *data, size_t datasize,
		uint8_t *dst_data, uint32_t *new_ffmpeg_packet_length)
{
	
	uint8_t aml_obu_header[20];
	int obu_bytes_read = 0;
	int obu_offset_start = 0;
	int obu_offset_stop = 0;
	int obu_total_len = 0;
	size_t header_size = 0;
	char err_buf[1024];
    ptrdiff_t offset;
    size_t obu_size2;
    int temporal_id, spatial_id;
    OBPOBUType obu_type;
    OBPError err = { &err_buf[0], 1024 };
	int aml_head_size = 20;
		
	while ( obu_bytes_read < datasize)
	{
		//Read obu's from ffmpeg input
		//Add special AML header to every OBU packet
		//Based on OSMC KODI disassembly for VERA3
		
		obp_get_next_obu(data + obu_bytes_read, datasize - obu_bytes_read, 
                                   &obu_type, &offset, &obu_size2, &temporal_id, &spatial_id, &err);	
                              
		obu_offset_start =  obu_bytes_read;
		obu_total_len = obu_size2 + offset;
		
		aml_obu_header[0] = ((obu_total_len + 4) >> 24) & 0xff;
		aml_obu_header[1] = ((obu_total_len + 4) >> 16) & 0xff;
		aml_obu_header[2] = ((obu_total_len + 4) >> 8) & 0xff;
		aml_obu_header[3] = ((obu_total_len + 4) >> 0) & 0xff;
		aml_obu_header[4] = aml_obu_header[0] ^ 0xff;
		aml_obu_header[5] = aml_obu_header[1] ^ 0xff;
		aml_obu_header[6] = aml_obu_header[2] ^ 0xff;
		aml_obu_header[7] = aml_obu_header[3] ^ 0xff;
		aml_obu_header[8] = 0;
		aml_obu_header[9] = 0;
		aml_obu_header[10] = 0;
		aml_obu_header[11] = 1;
		aml_obu_header[12] = 'A';
		aml_obu_header[13] = 'M';
		aml_obu_header[14] = 'L';
		aml_obu_header[15] = 'V';

		// insert data len as trailer
		aml_obu_header[16] = (obu_total_len & 0xff) | 0x80;
		aml_obu_header[17] = ((obu_total_len >> 7) & 0xff) | 0x80;
		aml_obu_header[18] = ((obu_total_len >> 14) & 0xff) | 0x80;
		aml_obu_header[19] = ((obu_total_len >> 21) & 0xff) | 0x00;

		memcpy(dst_data, aml_obu_header, aml_head_size);
		dst_data += aml_head_size;
		memcpy(dst_data, data+obu_offset_start, obu_total_len);
		dst_data += obu_total_len;
		obu_bytes_read = obu_bytes_read + obu_total_len;
		*new_ffmpeg_packet_length += (aml_head_size + obu_total_len);
		
	}
	
	return 1;
}




int AmlVideoSinkElement::VP9CreateFrame(uint8_t *data, size_t datasize,
		uint8_t *dst_data, uint32_t *new_ffmpeg_packet_length)
{

		int mag_ptr;
        int dsize;
		unsigned char *buf;
		unsigned char marker;
		int frame_number;
		int total_datasize = 0;
		unsigned char fdata[16];
		int cur_frame, cur_mag, mag, index_sz, offset[9], size[8], tframesize[9];
		*new_ffmpeg_packet_length = 0;
		dsize = datasize;
         marker = data[datasize - 1];
         if ((marker & 0xe0) == 0xc0) {
			frame_number = (marker & 0x7) + 1;
			mag = ((marker >> 3) & 0x3) + 1;
			index_sz = 2 + mag * frame_number;
			//printf(" frame_number : %d, mag : %d; index_sz : %d\n", frame_number, mag, index_sz);
			offset[0] = 0;
			mag_ptr = dsize - mag * frame_number - 2;
			
			if (data[mag_ptr] != marker) {
				printf(" Wrong marker2 : 0x%X --> 0x%X\n", marker, data[mag_ptr]);
				return -2;
			}
			 mag_ptr++;
			for (cur_frame = 0; cur_frame < frame_number; cur_frame++) {
				size[cur_frame] = 0; // or size[0] = bytes_in_buffer - 1; both OK
				for (cur_mag = 0; cur_mag < mag; cur_mag++) {
					size[cur_frame] = size[cur_frame]  | (data[mag_ptr] << (cur_mag*8) );
					mag_ptr++;
				}
				offset[cur_frame+1] = offset[cur_frame] + size[cur_frame];
				if (cur_frame == 0)
					tframesize[cur_frame] = size[cur_frame];
				else
					tframesize[cur_frame] = tframesize[cur_frame - 1] + size[cur_frame];
				total_datasize += size[cur_frame];
			}
        } else {
			printf("frame num 1");
			frame_number = 1;
			offset[0] = 0;
			size[0] = dsize; // or size[0] = bytes_in_buffer - 1; both OK
			total_datasize += dsize;
			tframesize[0] = dsize;
		}
		if (total_datasize > dsize) {
			printf("DATA overflow : 0x%X --> 0x%X\n", total_datasize, dsize);
			return -3;
		}
        
         if (frame_number >= 1) {
		
			for (cur_frame = 0; cur_frame < frame_number; cur_frame++) {
			
				int framesize = size[cur_frame];
				int offsetsize = offset[cur_frame];
				

				framesize += 4;
				
				fdata[0] = (framesize >> 24) & 0xff;
				fdata[1] = (framesize >> 16) & 0xff;
				fdata[2] = (framesize >> 8) & 0xff;
				fdata[3] = (framesize >> 0) & 0xff;
				fdata[4] = ((framesize >> 24) & 0xff) ^0xff;
				fdata[5] = ((framesize >> 16) & 0xff) ^0xff;
				fdata[6] = ((framesize >> 8) & 0xff) ^0xff;
				fdata[7] = ((framesize >> 0) & 0xff) ^0xff;
				fdata[8] = 0;
				fdata[9] = 0;
				fdata[10] = 0;
				fdata[11] = 1;
				fdata[12] = 'A';
				fdata[13] = 'M';
				fdata[14] = 'L';
				fdata[15] = 'V';
				//ret = write(handle, fdata, 16);
				framesize -= 4;
				
				memcpy(dst_data+*new_ffmpeg_packet_length,fdata,16);
				memcpy(dst_data+*new_ffmpeg_packet_length+16,data+offsetsize,framesize);
				
				//Total length to report to send to codec
				*new_ffmpeg_packet_length += framesize+16;
				
			}
			
		}
}


void AmlVideoSinkElement::DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}