#include "EnumToString.h"

BEGIN_ENUM(ParsingResult)
{
	DECL_ENUM_ELEMENT(CLPR_OK) 
	DECL_ENUM_ELEMENT(CLPR_COMMAND_UNKNOWN_COMMAND) 
	DECL_ENUM_ELEMENT(CLPR_COMMAND_UNKNOWN_OPTION)
	DECL_ENUM_ELEMENT(CLPR_COMMAND_OBLIGATORY_OPTION_NOT_SET)
	DECL_ENUM_ELEMENT(CLPR_OPTION_INVALID_VALUE)
	DECL_ENUM_ELEMENT(CLPR_GLOB_OBLIGATORY_OPTION_NOT_SET)
	DECL_ENUM_ELEMENT(CLPR_GLOB_EMPTY_LINE_AND_DEFAULT_COMMAND_INVALID_OR_NOT_SET)
	//DECL_ENUM_ELEMENT_STR(Tuesday, "Tuesday string") //will render "Tuesday string"
	//DECL_ENUM_ELEMENT(Wednesday) //will render "Wednesday"
	//DECL_ENUM_ELEMENT_VAL_STR(Thursday, 500, "Thursday string") // will render "Thursday string" and the enum will have 500 as value
																	/* ... and so on */
}
END_ENUM(ParsingResult)