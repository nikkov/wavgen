#ifndef _INT24_H_
#define _INT24_H_


class Int24
{
	private:
		unsigned char bytes[3];
	public:
		Int24()
		{
			bytes[0] = 0;
			bytes[1] = 0;
			bytes[2] = 0;
		}
		Int24(unsigned char* data )
		{
			memcpy(bytes, data, 3);			
		}
		Int24(int number )
		{
			memcpy(bytes, (const void*)&number, 3);						
		}
		Int24& operator=(int number )
		{
			memcpy(bytes, (const void*)&number, 3);	
			return *this;
		}
		Int24& operator=(unsigned char* data)
		{
			memcpy(bytes, data, 3);			
			return *this;
		}
		Int24& operator=(float data)
		{
			int temp = (int)data;
			memcpy(bytes, (const void*)&temp, 3);			
			return *this;
		}
		Int24& operator=(double data)
		{
			int temp = (int)data;
			memcpy(bytes, (const void*)&temp, 3);			
			return *this;
		}
		Int24& operator=(short data)
		{
			int temp = data;
			memcpy(bytes, (const void*)&temp, 3);			
			return *this;
		}
		Int24& operator+=(Int24 data)
		{
			int temp1 = data;
			int temp2 = (int)*this;
			temp1 += temp2;
			memcpy(bytes, (const void*)&temp1, 3);			
			return *this;
		}
		double operator*(const Int24& first)
		{
			return (double)(int)first * (int)(*this);
		}
		operator int() const
		{
			int number;
			if ( bytes[2] & 0x80 ) // Is this a negative?  Then we need to sign extend.
			{
				number = (0xff << 24) | (bytes[2] << 16) | (bytes[1] << 8) | (bytes[0] << 0);
			}
			else
			{
				number = (bytes[2] << 16) | (bytes[1] << 8) | (bytes[0] << 0);
			}						
			return number;
		}		
		operator float() const
		{
			int iresult = (int)*this;
			return (float)iresult;
		}		
		operator short() const
		{
			int iresult = (int)*this;
			return (short)iresult;
		}		
};


double GetMaxValueForFormat(portaudio::SampleDataFormat format);

#endif //_INT24_H_
