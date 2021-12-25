#include "ArchiveTreePatcher.h"

const char* archiveTreePatcherAppendData =
{
	"<Node>"
	"  <Name>ReactionPlateActionCommonCore</Name>"
	"  <Archive>ReactionPlateActionCommonCore</Archive>"
	"  <Order>0</Order>"
	"  <DefAppend>ReactionPlateActionCommonCore</DefAppend>"
	"  <Node>"
	"    <Name>ReactionPlateActionCommon</Name>"
	"    <Archive>ReactionPlateActionCommon</Archive>"
	"    <Order>0</Order>"
	"  </Node>"
	"</Node>"
	
	"<Node>"
	"  <Name>ReactionPlateActionCommon</Name>"
	"  <Archive>ReactionPlateActionCommon</Archive>"
	"  <Order>0</Order>"
	"  <DefAppend>ReactionPlateActionCommon</DefAppend>"
	"  <Node>"
	"    <Name>ActionCommon</Name>"
	"    <Archive>ActionCommon</Archive>"
	"    <Order>0</Order>"
	"  </Node>"
	"</Node>"
	
	"<Node>"
	"  <Name>ReactionPlateSystemCommonCore</Name>"
	"  <Archive>ReactionPlateSystemCommonCore</Archive>"
	"  <Order>0</Order>"
	"  <DefAppend>ReactionPlateSystemCommonCore</DefAppend>"
	"  <Node>"
	"    <Name>ReactionPlateSystemCommon</Name>"
	"    <Archive>ReactionPlateSystemCommon</Archive>"
	"    <Order>0</Order>"
	"  </Node>"
	"</Node>"
	
	"<Node>"
	"  <Name>ReactionPlateSystemCommon</Name>"
	"  <Archive>ReactionPlateSystemCommon</Archive>"
	"  <Order>0</Order>"
	"  <DefAppend>ReactionPlateSystemCommon</DefAppend>"
	"  <Node>"
	"    <Name>SystemCommon</Name>"
	"    <Archive>SystemCommon</Archive>"
	"    <Order>0</Order>"
	"  </Node>"
	"</Node>"
};

const char* archiveTreePatcherLanguageAppendData
{
	"<Archive>ReactionPlateSystemCommon</Archive>"
};

boost::shared_ptr<hh::db::CRawData>* __fastcall archiveTreePatcherGetRawDataImpl(hh::db::CDatabase* This, void* Edx,
    boost::shared_ptr<hh::db::CRawData>& rawData, const hh::base::CSharedString& name, const bool flag)
{
    This->GetRawData(rawData, name, flag);

	const bool isOther = name == "OtherArchiveTree.xml";
	const bool isLanguage = name == "LanguageTree.xml";
	
    if ((!isOther && !isLanguage) || !rawData || !rawData->m_spData)
        return &rawData;

	const char* const appendData = isLanguage ? archiveTreePatcherLanguageAppendData : archiveTreePatcherAppendData;
	const size_t appendDataSize = strlen(appendData);

	const size_t newSize = rawData->m_DataSize + appendDataSize;
	const boost::shared_ptr<char[]> buffer = boost::make_shared<char[]>(newSize);
	memcpy(buffer.get(), rawData->m_spData.get(), rawData->m_DataSize);

	char* insertionPos = strstr(buffer.get(), isLanguage ? "</Language>" : "</ArchiveTree>");

	memmove(insertionPos + appendDataSize, insertionPos, rawData->m_DataSize - (size_t)(insertionPos - buffer.get()));
	memcpy(insertionPos, appendData, appendDataSize);

	rawData = boost::make_shared<hh::db::CRawData>();
	rawData->m_Flags = hh::db::eDatabaseDataFlags_IsMadeAll;
	rawData->m_spData = buffer;
	rawData->m_DataSize = newSize;

    return &rawData;
}

void ArchiveTreePatcher::applyPatches()
{
    WRITE_CALL(0xD4C4C1, archiveTreePatcherGetRawDataImpl);
}

