#include <stdio.h>
#include <stdarg.h>
#include "dspexcept.h"


const char* DspException::ErrorMessage() const
{
	BuildErrorMessage();
	return mErrMessage.c_str();
}

//	(((((((( OBJECT INTERNAL METHODS ))))))))

void DspException::BuildErrorMessage() const
{
	if (! mErrMessage.empty() )
		return;	
	// Don't build it if already done

	mErrMessage = "*** CmpException";
	mErrMessage += " inside ";
	mErrMessage += mFileName;
	mErrMessage += " ";
	mErrMessage += mRevision;
	mErrMessage += " ";
	mErrMessage += mFunctionName;

	mErrMessage += " ***\n\n";
	if (! mMessage.empty())
	{
		mErrMessage += "Message : ";
		mErrMessage += mMessage;
		mErrMessage += "\n\n";
	}
}

void DspException::BlindCopy( const DspException& copied )
{
	mFunctionName = copied.mFunctionName;
	mFileName = copied.mFileName;
	mRevision = copied.mRevision;
	mMessage = copied.mMessage;
}

void DspException::SetOrigin( const char* functionName, const char* fileName , const char* revision )
{
	if ( functionName == NULL )	mFunctionName.erase();
	else mFunctionName = functionName;
	if ( fileName == NULL )	mFileName.erase();
	else mFileName = fileName;
	if ( revision == NULL )	mRevision.erase();
	else mRevision = revision;
}

void DspException::SetMessage( const char* message )
{
	if ( message == NULL )
		mMessage.erase();
	else
		mMessage = message;
}

DspException::DspException()
{
}

DspException::~DspException()
{
}

DspException::DspException( const DspException& copied )
{
	BlindCopy( copied );
}

DspException& DspException::operator=( const DspException& copied )
{
	//	The copy constructor is VERY important for a class used
	//	as a C++ exception, especially with Borland BCC32 5.5
	mFunctionName.erase();
	mFileName.erase();
	mRevision.erase();
	mMessage.erase();
	BlindCopy(copied);
	return *this;
}

DspException::DspException( const char* functionName, const char* fileName , const char* revision, const char* message, ... )
{
	SetOrigin( functionName, fileName , revision );
	if ( message != NULL )
	{
		char buffer[1024];
		va_list argptr;
		va_start(argptr, message);
#if defined (_MSC_VER)
		_vsnprintf(buffer, sizeof(buffer)-1, message, argptr);
#else
		vsnprintf(buffer, sizeof(buffer)-1, message, argptr);
#endif
		buffer[sizeof(buffer)-1] = 0;
		va_end(argptr);
		SetMessage(buffer);
	}
}

