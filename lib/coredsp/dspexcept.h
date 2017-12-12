#ifndef __DSP_COMMON_EXCEPTION__
#define __DSP_COMMON_EXCEPTION__

#include <string>

#include "complstd.h"

#ifndef _
#define _
#endif

template class COREDSP_API _STD allocator<char>;
template class COREDSP_API _STD basic_string<char, _STD char_traits<char> , _STD allocator<char> >;

//! Класс описания исключения, используемый в библиотеке CoreDSP
/*!
	Данный класс используется для сигнализации об ошибках,
	произошедших при работе функций библиотеки
*/
class COREDSP_API DspException
{
private:
//! Функция, где было сгенерировано исключение
	std::string mFunctionName;
//! Файл, где было сгенерировано исключение
	std::string mFileName;
//! Номер CVS-ревизии файла, где было сгенерировано исключение
	std::string mRevision;
//! Сообщение об ошибке
	std::string mMessage;			

//! Полный текст, описывающий место и причину возникновения исключения
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

//! Формирование строки с текстом описания ошибки
/*!
	Формирование строки с текстом описания ошибки, вызвавшей исключение
	\return константная строка, описывающая ошибку, вызвавшую исключение
*/
	const char* ErrorMessage() const;
};

//! Макросы помогающие сгенерировать исключение без входных параметров
#define THROW_EXCEPTION( message ) throw DspException( __FUNCTION__, __FILE__, "Revision", _( message ) )

//! Макросы помогающие сгенерировать исключение с входными параметров и шаблоном "message"
#define THROW_EXCEPTION2( message, param ) throw DspException(  __FUNCTION__, __FILE__, "Revision", _( message ), param )

#endif //__DSP_COMMON_EXCEPTION__
