
#include "stdafx.h"
#include "File.h"
#include "IDocumentReader.h"
#include "IDocumentWriter.h"

OPEN_NAMESPACE(Elf);

ResultCode IDocumentReader::ERR_SUBSECTION_NOT_FOUND = Result("subsection was not found");

CLOSE_NAMESPACE(Elf);