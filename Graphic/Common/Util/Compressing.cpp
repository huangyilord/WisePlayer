#include "Compressing.h"
#include "DWordCompressor.h"

namespace Util
{
	Compressor::Compressor()
	{
	}
	Compressor::~Compressor()
	{
	}

	CompressorManager CompressorManager::s_Instance;

	CompressorManager::CompressorManager()
	{
		m_mapCompressors.insert( std::pair<WORD, Compressor*>( Compressor_DWord, &DWordCompressor::GetInstance() ) );
	}

	CompressorManager::~CompressorManager()
	{
		m_mapCompressors.clear();
	}
		
	Compressor* CompressorManager::_GetCompressor( WORD wTypeID )
	{
		std::map<WORD, Compressor*>::iterator iter = m_mapCompressors.find( wTypeID );
		if ( iter != m_mapCompressors.end() )
		{
			return iter->second;
		}
		return NULL;
	}
}