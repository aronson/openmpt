/*
 * StreamEncoder.h
 * ---------------
 * Purpose: Exporting streamed music files.
 * Notes  : none
 * Authors: Joern Heusipp
 *          OpenMPT Devs
 * The OpenMPT source code is released under the BSD license. Read LICENSE for more details.
 */

#pragma once

#include "openmpt/all/BuildSettings.hpp"

#include "openmpt/soundbase/SampleFormat.hpp"
#include "../common/Endianness.h"
#include "../soundlib/Tagging.h"

#include <iosfwd>
#include <string>
#include <vector>


OPENMPT_NAMESPACE_BEGIN



inline constexpr int opus_bitrates [] = {
	8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 192, 224, 256, 320, 384, 448,      510
};
inline constexpr int vorbis_bitrates [] = {
	           32,     48,     64, 80, 96, 112, 128,      160, 192, 224, 256, 320,           500
};
inline constexpr int layer3_bitrates [] = {
	8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 192, 224, 256, 320
};
inline constexpr int mpeg1layer3_bitrates [] = {
	           32, 40, 48, 56, 64, 80, 96, 112, 128,      160, 192, 224, 256, 320
};
inline constexpr uint32 opus_samplerates [] = {
	48000,
	24000,           16000,
	12000,            8000
};
inline constexpr uint32 opus_all_samplerates [] = {
	48000,  44100,  32000,
	24000,  22050,  16000,
	12000,  11025,   8000
};
inline constexpr uint32 vorbis_samplerates [] = {
	 48000,  44100,  32000,
	 24000,  22050,  16000,
	 12000,  11025,   8000
};
inline constexpr uint32 layer3_samplerates [] = {
	 48000,  44100,  32000,
	 24000,  22050,  16000
};
inline constexpr uint32 mpeg1layer3_samplerates [] = {
	 48000,  44100,  32000
};


namespace Encoder
{

	enum Mode
	{
		ModeCBR      = 1<<0,
		ModeABR      = 1<<1,
		ModeVBR      = 1<<2,
		ModeQuality  = 1<<3,
		ModeLossless = 1<<4,
		ModeInvalid  = 0
	};

	struct Format
	{
		enum class Encoding
		{
			Float = 1,
			Integer = 2,
			Alaw = 3,
			ulaw = 4,
			Unsigned = 5,
		};
		Encoding encoding;
		uint8 bits;
		mpt::endian endian;
		bool operator==(const Format &other) const
		{
			return encoding == other.encoding && bits == other.bits && endian == other.endian;
		}	
		bool operator!=(const Format& other) const
		{
			return encoding != other.encoding || bits != other.bits || endian != other.endian;
		}
		int32 AsInt() const
		{
			return (static_cast<int32>(endian == mpt::endian::little) << 16) | (static_cast<int32>(encoding) << 8) | static_cast<int32>(bits);
		}
		static Format FromInt(int32 val)
		{
			Encoder::Format f;
			f.bits = val & 0xff;
			f.encoding = static_cast<Encoder::Format::Encoding>((val >> 8) & 0xff);
			f.endian = ((val >> 16) & 0xff) ? mpt::endian::little : mpt::endian::big;
			return f;
		}
		SampleFormat GetSampleFormat() const
		{
			SampleFormat result = SampleFormat::Invalid;
			switch(encoding)
			{
			case Encoding::Float:
				switch(bits)
				{
				case 32:
					result = SampleFormat::Float32;
					break;
				case 64:
					result = SampleFormat::Float64;
					break;
				}
				break;
			case Encoding::Integer:
				switch(bits)
				{
				case 8:
					result = SampleFormat::Int8;
					break;
				case 16:
					result = SampleFormat::Int16;
					break;
				case 24:
					result = SampleFormat::Int24;
					break;
				case 32:
					result = SampleFormat::Int32;
					break;
				}
				break;
			case Encoding::Alaw:
				switch (bits)
				{
				case 16:
					result = SampleFormat::Int16;
					break;
				}
				break;
			case Encoding::ulaw:
				switch (bits)
				{
				case 16:
					result = SampleFormat::Int16;
					break;
				}
				break;
			case Encoding::Unsigned:
				switch (bits)
				{
				case 8:
					result = SampleFormat::Unsigned8;
					break;
				}
				break;
			}
			return result;
		}
	};

	struct Traits
	{
		
		mpt::PathString fileExtension;
		mpt::ustring fileShortDescription;
		mpt::ustring encoderSettingsName;

		mpt::ustring fileDescription;

		bool canTags = false;
		std::vector<mpt::ustring> genres;
		int modesWithFixedGenres = 0;
		
		bool canCues = false;

		int maxChannels = 0;
		std::vector<uint32> samplerates;
		
		int modes = Encoder::ModeInvalid;
		std::vector<int> bitrates;
		std::vector<Format> formats;

		uint32 defaultSamplerate = 48000;
		uint16 defaultChannels = 2;

		Encoder::Mode defaultMode = Encoder::ModeInvalid;
		int defaultBitrate = 0;
		float defaultQuality = 0.0f;
		Format defaultFormat = { Encoder::Format::Encoding::Float, 32, mpt::endian::little };
		int defaultDitherType = 1;
	};

	struct StreamSettings
	{
		int32 FLACCompressionLevel = 5; // 8
		uint32 AUPaddingAlignHint = 4096;
		uint32 MP3ID3v2MinPadding = 1024;
		uint32 MP3ID3v2PaddingAlignHint = 4096;
		bool MP3ID3v2WriteReplayGainTXXX = true;
		int32 MP3LameQuality = 3; // 0
		bool MP3LameID3v2UseLame = false;
		bool MP3LameCalculateReplayGain = true;
		bool MP3LameCalculatePeakSample = true;
		int32 OpusComplexity = -1; // 10
	};

	struct Settings
	{

		bool Cues;
		bool Tags;

		uint32 Samplerate;
		uint16 Channels;

		Encoder::Mode Mode;
		int Bitrate;
		float Quality;
		Encoder::Format Format;
		int Dither;

		StreamSettings Details;

	};

} // namespace Encoder


class IAudioStreamEncoder
{
protected:
	IAudioStreamEncoder() { }
public:
	virtual ~IAudioStreamEncoder() = default;
public:
	virtual SampleFormat GetSampleFormat() const = 0;
	virtual void WriteInterleaved(std::size_t frameCount, const double *interleaved) = 0;
	virtual void WriteInterleaved(std::size_t frameCount, const float *interleaved) = 0;
	virtual void WriteInterleaved(std::size_t frameCount, const int32 *interleaved) = 0;
	virtual void WriteInterleaved(std::size_t frameCount, const int24 *interleaved) = 0;
	virtual void WriteInterleaved(std::size_t frameCount, const int16 *interleaved) = 0;
	virtual void WriteInterleaved(std::size_t frameCount, const int8 *interleaved) = 0;
	virtual void WriteInterleaved(std::size_t frameCount, const uint8 *interleaved) = 0;
	virtual void WriteCues(const std::vector<uint64> &cues) = 0; // optional
	virtual void WriteFinalize() = 0;
};



std::pair<bool, std::size_t> WriteInterleavedLE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const double *interleaved);
std::pair<bool, std::size_t> WriteInterleavedLE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const float *interleaved);
std::pair<bool, std::size_t> WriteInterleavedLE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const int32 *interleaved);
std::pair<bool, std::size_t> WriteInterleavedLE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const int24 *interleaved);
std::pair<bool, std::size_t> WriteInterleavedLE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const int16 *interleaved);
std::pair<bool, std::size_t> WriteInterleavedLE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const int8 *interleaved);
std::pair<bool, std::size_t> WriteInterleavedLE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const uint8 *interleaved);

std::pair<bool, std::size_t> WriteInterleavedBE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const double *interleaved);
std::pair<bool, std::size_t> WriteInterleavedBE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const float *interleaved);
std::pair<bool, std::size_t> WriteInterleavedBE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const int32 *interleaved);
std::pair<bool, std::size_t> WriteInterleavedBE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const int24 *interleaved);
std::pair<bool, std::size_t> WriteInterleavedBE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const int16 *interleaved);
std::pair<bool, std::size_t> WriteInterleavedBE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const int8 *interleaved);
std::pair<bool, std::size_t> WriteInterleavedBE(std::ostream &f, uint16 channels, Encoder::Format format, std::size_t frameCount, const uint8 *interleaved);



class StreamWriterBase
	: public IAudioStreamEncoder
{
protected:	
	std::ostream &f;
	std::streampos fStart;
	std::vector<char> buf;
public:
	StreamWriterBase(std::ostream &stream);
	virtual ~StreamWriterBase();
public:
	SampleFormat GetSampleFormat() const override;
	void WriteInterleaved(std::size_t frameCount, const double *interleaved) override;
	void WriteInterleaved(std::size_t frameCount, const float *interleaved) override;
	void WriteInterleaved(std::size_t frameCount, const int32 *interleaved) override;
	void WriteInterleaved(std::size_t frameCount, const int24 *interleaved) override;
	void WriteInterleaved(std::size_t frameCount, const int16 *interleaved) override;
	void WriteInterleaved(std::size_t frameCount, const int8 *interleaved) override;
	void WriteInterleaved(std::size_t frameCount, const uint8 *interleaved) override;
	void WriteCues(const std::vector<uint64> &cues) override;
	void WriteFinalize() override;
protected:
	void WriteBuffer();
};


class EncoderFactoryBase
{
private:
	Encoder::Traits m_Traits;
protected:
	EncoderFactoryBase() { }
	virtual ~EncoderFactoryBase() = default;
	void SetTraits(const Encoder::Traits &traits);
public:
	virtual std::unique_ptr<IAudioStreamEncoder> ConstructStreamEncoder(std::ostream &file, const Encoder::Settings &settings, const FileTags &tags) const = 0;
	const Encoder::Traits &GetTraits() const
	{
		return m_Traits;
	}
	virtual bool IsBitrateSupported(int samplerate, int channels, int bitrate) const;
	virtual mpt::ustring DescribeQuality(float quality) const;
	virtual mpt::ustring DescribeBitrateVBR(int bitrate) const;
	virtual mpt::ustring DescribeBitrateABR(int bitrate) const;
	virtual mpt::ustring DescribeBitrateCBR(int bitrate) const;
	virtual bool IsAvailable() const = 0;
};


OPENMPT_NAMESPACE_END
