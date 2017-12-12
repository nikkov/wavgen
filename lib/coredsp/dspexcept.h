#ifndef __DSP_COMMON_EXCEPTION__
#define __DSP_COMMON_EXCEPTION__

#include <string>

#include "complstd.h"

#ifndef _
#define _
#endif

template class COREDSP_API _STD allocator<char>;
template class COREDSP_API _STD basic_string<char, _STD char_traits<char> , _STD allocator<char> >;

//! ����� �������� ����������, ������������ � ���������� CoreDSP
/*!
	������ ����� ������������ ��� ������������ �� �������,
	������������ ��� ������ ������� ����������
*/
class COREDSP_API DspException
{
private:
//! �������, ��� ���� ������������� ����������
	std::string mFunctionName;
//! ����, ��� ���� ������������� ����������
	std::string mFileName;
//! ����� CVS-������� �����, ��� ���� ������������� ����������
	std::string mRevision;
//! ��������� �� ������
	std::string mMessage;			

//! ������ �����, ����������� ����� � ������� ������������� ����������
	mutable std::string mErrMessage;

	void BuildErrorMessage() const;
	void BlindCopy( const DspException& );
	void SetOrigin( const char* functionName, const char* fileName , const char* revision );
	void SetMessage( const char* message );

public:
	DspException();
	DspException(const DspException&);
	DspException& operator=(const DspException&);
	DspException( const char* functionName, const char* fileName , const char* revision, const char* message = 0, ...);
	virtual ~DspException();

public:

//! ������������ ������ � ������� �������� ������
/*!
	������������ ������ � ������� �������� ������, ��������� ����������
	\return ����������� ������, ����������� ������, ��������� ����������
*/
	const char* ErrorMessage() const;
};

//! ������� ���������� ������������� ���������� ��� ������� ����������
#define THROW_EXCEPTION( message ) throw DspException( __FUNCTION__, __FILE__, "Revision", _( message ) )

//! ������� ���������� ������������� ���������� � �������� ���������� � �������� "message"
#define THROW_EXCEPTION2( message, param ) throw DspException(  __FUNCTION__, __FILE__, "Revision", _( message ), param )

#endif //__DSP_COMMON_EXCEPTION__
