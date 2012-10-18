// ImageHelpers.cpp

#include "stdafx.h"
#include "ImageHelpers.h"


namespace ImageHelpers
{

	namespace
	{
		#pragma pack(push, 1)
    
		// TGA file header structure. This *must* be byte aligned.
		struct TgaHeader
		{
			BYTE idLength;
			BYTE colormapType;
			BYTE imageType;
			WORD firstEntryIndex;
			WORD colormapLength;
			BYTE colormapEntrySize;
			WORD xOrigin;
			WORD yOrigin;
			WORD width;
			WORD height;
			BYTE pixelDepth;
			BYTE imageDescriptor;
		};

		// TGA file footer structure. This *must* be byte aligned.
		struct TgaFooter
		{
			LONG extensionAreaOffet;
			LONG developerDirectoryOffset;
			BYTE signature[18];
		};

		#pragma pack(pop)
	}

	void GdipGetPNGEncoderClsid(CLSID* pClsid)
	{
		UINT  num = 0;		// number of image encoders
		UINT  size = 0;		// size of the image encoder array in bytes

		GetImageEncodersSize(&num, &size);
		ENSURE(size!=0);

		CByteBuffer buffer( size );

		ImageCodecInfo* pImageCodecInfo = buffer.GetDataAs<ImageCodecInfo>();
		GetImageEncoders(num, size, pImageCodecInfo);

		for( UINT j=0; j<num; ++j )
		{
			const PCTSTR format = L"image/png";
			if( wcscmp(pImageCodecInfo[j].MimeType, format)==0 )
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				return;  // Success
			}    
		}
		ENSURE(false); // Failure
	}

	bool GdipTargaBufferAsPNG(CReadStream& reader, CByteBuffer& png_buffer)
	{
		TgaHeader header = {0};

		// Read in the TGA file header.
		ENSURE( reader.Read(&header) );
		
		// Skip over the TGA file's ID field.
		if( header.idLength > 0 )
			reader.Skip(header.idLength);

		// Check for compatible color depth.
		// midi - only 24 till now
		if( header.pixelDepth!=24 )// if( !(header.pixelDepth== 32 || header.pixelDepth==24 || header.pixelDepth==8) )
			return false;
		
		// Only support uncompressed true color and grayscale images.
		if( !(header.imageType == 0x02 || header.imageType == 0x01) )
			return false;

		// Read the TGA file into a temporary buffer.
		DWORD dwPitch = header.width * (header.pixelDepth / 8);
		DWORD dwBufferSize = dwPitch * header.height;
		CByteBuffer tga_buffer(dwBufferSize);

		// Load the pixel data from the TGA file. Flip image if it's not top down.
	    if( (header.imageDescriptor & 0x20) == 0x20 )
		{
			// TGA image is stored top down in file.
			if( !reader.Read(tga_buffer, dwBufferSize) )
				return false;
 		} else {
			// TGA image is stored bottom up in file. Need to flip it.
			BYTE* pRow = 0;

			for( int i=0; i<header.height; i++ )
			{
				pRow = tga_buffer[(header.height - 1 - i) * dwPitch];
				if( !reader.Read(pRow, dwPitch) )
					return false;
			}
		}

	// Convert to a PNG
		Gdiplus::Bitmap bitmap(header.width, header.height, dwPitch, PixelFormat24bppRGB, tga_buffer);

		// PNG Encoder
		CLSID encoderClsid;
		ImageHelpers::GdipGetPNGEncoderClsid(&encoderClsid);

		// Stream support
		IStream* stream;
		HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &stream); ENSURE(hr==S_OK);
		ULARGE_INTEGER ui = {0};
		LARGE_INTEGER li = {0};
		STATSTG st;

		// Save the image to the stream
		hr = bitmap.Save(stream, &encoderClsid, NULL); ENSURE(hr==S_OK);

		// Save the stream to a buffer
		hr = stream->Stat(&st, STATFLAG_NONAME); ENSURE(hr==S_OK);
		hr = stream->Seek(li, STREAM_SEEK_SET, nullptr); ENSURE(hr==S_OK);
		png_buffer.Allocate(st.cbSize.LowPart);
		hr = stream->Read(png_buffer, st.cbSize.LowPart, nullptr); ENSURE(hr==S_OK);

		return true;
	}
}
