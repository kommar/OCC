#ifndef GENERATION_H
#define GENERATION_H

#define GENERATION_ARGUMENT_TYPE "generation_type"
#define GENERATION_ARGUMENT_TYPE_REAL_RANDOM "real_random"

#include "Error.h"

#include <cstdint>
#include <istream>
#include <vector>

using namespace std;


enum EGenerationType
{
	GENERATION_REAL_RANDOM = 0
};//enum EGenerationType


template <class TGenotype>
class CGeneration
{
public:
	static uint32_t iERROR_PARENT_CGeneration;

	CGeneration();

	virtual ~CGeneration();

	virtual CError eConfigure(istream *psSettings) { return CError(iERROR_PARENT_CGeneration); };

	virtual TGenotype *pcGenerate() = 0;
	virtual TGenotype *pcGenerateEmpty() = 0;

	virtual void vUpdateBestGenotypes(uint8_t iCount, TGenotype **ppcBestGenotypes);

protected:
	void v_clear_best_genotypes();

	vector<TGenotype*> v_best_genotypes;
};//class CGeneration

#endif//GENERATION_H