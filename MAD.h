#pragma once

using namespace System;

namespace MAD {
	/// <summary>
	/// Identifies a possible decoding error.
	/// </summary>
	public enum class Error {
		None = 0x0000,
		BufferLength = 0x0001,
		BufferData = 0x0002,
		Memory = 0x0031,
		LostSync = 0x0101,
		BadLayer = 0x0102,
		BadBitRate = 0x0103,
		BadSampleRate = 0x0104,
		BadEmphasis = 0x0105,
		BadCRC = 0x0201,
		BadBitAlloc = 0x0211,
		BadScaleFactor = 0x0221,
		BadMode = 0x0222,
		BadFrameLength = 0x0231,
		BadBigValues = 0x0232,
		BadBlockType = 0x0233,
		BadSCFSI = 0x0234,
		BadData = 0x0235,
		BadAudioLength = 0x0236,
		BadHuffmanTable = 0x0237,
		BadHuffmanData = 0x0238,
		BadStero = 0x0239
	};

	/// <summary>
	/// Contains state information for an MP3 decoder based on libmad.
	/// </summary>
	public ref class Decoder {
	public:
		/// <summary>
		/// Initializes the state of the decoder.
		/// </summary>
		void Initialize() {
			this->_Stream = new mad_stream();
			this->_Frame = new mad_frame();
			this->_Synth = new mad_synth();
			mad_stream_init(this->_Stream);
			mad_frame_init(this->_Frame);
			mad_synth_init(this->_Synth);
		}

		/// <summary>
		/// Cleans up resources used by the decoder.
		/// </summary>
		void Terminate() {
			mad_stream_finish(this->_Stream);	
			mad_frame_finish(this->_Frame);
			mad_synth_finish(this->_Synth);
		}

		/// <summary>
		/// Sets the input buffer of the decoder from a data pointer. The data should remain static and fixed during decoding.
		/// </summary>
		void SetInput(void* Buffer, int Length) {
			mad_stream_buffer(this->_Stream, (const unsigned char*)Buffer, Length);
		}

		/// <summary>
		/// The amount of samples in a decoded frame.
		/// </summary>
		literal int FrameSampleCount = 1152;

		/// <summary>
		/// Gets the location of the current frame in the input buffer.
		/// </summary>
		property void* CurrentFrame {
			void* get() {
				return (void*)this->_Stream->this_frame;
			}
		}

		/// <summary>
		/// Gets the location of the next frame in the input buffer.
		/// </summary>
		property void* NextFrame {
			void* get() {
				return (void*)this->_Stream->next_frame;
			}
		}

		/// <summary>
		/// Gets the sample rate of the sample being decoded. This is updated when SynthFrame is called.
		/// </summary>
		property int SampleRate {
			int get() {
				return this->_Synth->pcm.samplerate;
			}
		}

		/// <summary>
		/// Gets the amount of channels in the sample being decoded. This is updated when SynthFrame is called.
		/// </summary>
		property int Channels {
			int get() {
				return this->_Synth->pcm.channels;
			}
		}

		/// <summary>
		/// Gets the location of the PCM output for the last synthesized frame.
		/// </summary>
		property void* Output {
			void* get() {
				return (void*)this->_Synth->pcm.samples;
			}
		}

		/// <summary>
		/// Gets or sets the current error code for the decoder.
		/// </summary>
		property Error Error {
			enum class Error get() {
				return static_cast<enum class Error>(this->_Stream->error);
			}
			void set(enum class Error value) {
				this->_Stream->error = static_cast<mad_error>(value);
			}
		}

		/// <summary>
		/// Gets wether the current error is recoverable.
		/// </summary>
		property bool ErrorRecoverable {
			bool get() {
				return MAD_RECOVERABLE(this->_Stream->error);
			}
		}

		/// <summary>
		/// Gets the message that corresponds to the current error for the decoder.
		/// </summary>
		property String^ ErrorMessage {
			String^ get() {
				return gcnew String(mad_stream_errorstr(this->_Stream));
			}
		}

		/// <summary>
		/// Decodes the next frame from the input sample. Returns false if an error occured.
		/// </summary>
		bool DecodeFrame() {
			return mad_frame_decode(this->_Frame, this->_Stream) != -1;
		}

		/// <summary>
		/// Synthesizes the next frame of the decoded sample. This should be called after DecodeFrame.
		/// </summary>
		void SynthFrame() {
			mad_synth_frame(this->_Synth, this->_Frame);
		}

	private:
		mad_stream* _Stream;
		mad_frame* _Frame;
		mad_synth* _Synth;
	};
}