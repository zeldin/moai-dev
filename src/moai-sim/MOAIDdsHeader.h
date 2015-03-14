#ifndef MOAIDDSHEADER_H
#define MOAIDDSHEADER_H

#include <endian.h>
#include <byteswap.h>

//================================================================//
// MOAIDdsHeader
//================================================================//
class MOAIDdsHeader {
public:

	class u32le {
	public:
		operator u32 () const { return getValue(); }
		const u32 operator=(const u32 v) { setValue(v); }
	private:
		u32 v;
#if __BYTE_ORDER == __LITTLE_ENDIAN
		u32 getValue () const { return v; }
		const u32 setValue (const u32 v_)
		{ v = v_; return v_; }
#else
		u32 getValue () const { return __bswap_32(v); }
		const u32 setValue (const u32 v_)
		{ v = __bswap_32(v_); return v_; }
#endif
	};


	static const u32 HEADER_SIZE		= 128;
	static const u32 DDS_FILE_MAGIC		= 0x20534444; // 'D' 'D' 'S' ' '
	static const u32 DDSD_MIPMAPCOUNT	= 0x00020000;
	static const u32 DDPF_FOURCC            = 0x00000004;
	static const u32 D3DFMT_DXT1            = 0x31545844; // 'D' 'X' 'T' '1'
	static const u32 D3DFMT_DXT3            = 0x33545844; // 'D' 'X' 'T' '3'
	static const u32 D3DFMT_DXT5            = 0x35545844; // 'D' 'X' 'T' '5'


	u32le    dwMagic;
	u32le    dwSize;
	u32le    dwFlags;
	u32le    dwHeight;
	u32le    dwWidth;
	u32le    dwPitchOrLinearSize;
	u32le    dwDepth;
	u32le    dwMipMapCount;
	u32le    dwReserved1[ 11 ];

	//  DDPIXELFORMAT
	struct {
		u32le    dwSize;
		u32le    dwFlags;
		u32le    dwFourCC;
		u32le    dwRGBBitCount;
		u32le    dwRBitMask;
		u32le    dwGBitMask;
		u32le    dwBBitMask;
		u32le    dwAlphaBitMask;
	} sPixelFormat;

	//  DDCAPS2
	struct {
		u32le    dwCaps1;
		u32le    dwCaps2;
		u32le    dwDDSX;
		u32le    dwReserved;
	} sCaps;
	u32le    dwReserved2;

	////----------------------------------------------------------------//
	static void* GetFileData ( void* data, size_t size ) {

		if ( data && ( size >= HEADER_SIZE )) {
			return ( void* )(( size_t )data + HEADER_SIZE );
		}
		return 0;
	}

	//----------------------------------------------------------------//
	inline size_t GetTotalSize ();

	//----------------------------------------------------------------//
	bool IsValid () {
		return this->dwMagic == DDS_FILE_MAGIC;
	}

	//----------------------------------------------------------------//
	u32 GetFourCC () {
		return (this->sPixelFormat.dwFlags & DDPF_FOURCC)?
			this->sPixelFormat.dwFourCC : 0;
	}

	//----------------------------------------------------------------//
	u32 GetMipMapCount () {
		return (this->dwFlags & DDSD_MIPMAPCOUNT)?
			this->dwMipMapCount : 1;
	}

	//----------------------------------------------------------------//
	void Load ( ZLStream& stream ) {

		assert ( HEADER_SIZE <= sizeof ( MOAIDdsHeader ));

		this->dwMagic = 0;
		stream.PeekBytes ( this, HEADER_SIZE );
	}

	//----------------------------------------------------------------//
	static MOAIDdsHeader* GetHeader ( const void* data, size_t size ) {

		if ( data && ( size >= HEADER_SIZE )) {
			MOAIDdsHeader* header = ( MOAIDdsHeader* )data;
			if ( header->dwMagic == DDS_FILE_MAGIC ) {
				return header;
			}
		}
		return 0;
	}

	//----------------------------------------------------------------//
	MOAIDdsHeader () {
		this->dwMagic = 0;
	}
};

size_t MOAIDdsHeader::GetTotalSize () {

	u32 bytesPerBlock = 0, fourCC = GetFourCC();
	if (fourCC == D3DFMT_DXT5 || fourCC == D3DFMT_DXT3)
		bytesPerBlock = 16;
	else if (fourCC == D3DFMT_DXT1)
		bytesPerBlock = 8;
	u32 size = HEADER_SIZE;
	if (bytesPerBlock) {
		// DXT*
		u32 w = dwWidth;
		u32 h = dwHeight;
		for (u32 m = GetMipMapCount(); m > 0; --m) {
			size += ((w+3) >> 2) * ((h+3) >> 2) * bytesPerBlock;
			w = (w+1) >> 1;
			h = (h+1) >> 1;
		}
	}
	return size;
}


#endif
