#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#define TRANSFOMATION_REAL_KEYS_MIN_VALUE 0.0
#define TRANSFOMATION_REAL_KEYS_MAX_VALUE 1.0

#include "Error.h"
#include "Evaluation.h"
#include "RealCoding.h"

#include <iostream>

using namespace std;


template <class TGenotype, class TFenotype>
class CTransformation
{
public:
	CTransformation(CEvaluation<TFenotype> *pcEvaluation);

	virtual ~CTransformation();

	virtual CError eConfigure(istream *psSettings) { return CError(); }

	virtual TFenotype *pcTransform(TGenotype *pcGenotype);
	virtual void vTransform(TGenotype *pcGenotype, TFenotype *pcFenotype) = 0;

	virtual TGenotype *pcInverselyTransform(TFenotype *pcFenotype);
	virtual void vInverselyTransform(TFenotype *pcFenotype, TGenotype *pcGenotype) = 0;

protected:
	virtual TFenotype *pc_create_fenotype(TGenotype *pcGenotype) = 0;
	virtual TGenotype *pc_create_genotype(TFenotype *pcFenotype) = 0;

	CEvaluation<TFenotype> *pc_evaluation;
};//class CTransformation


template <class TGenotypeFenotype>
class CUniformTransformation : public CTransformation<TGenotypeFenotype, TGenotypeFenotype>
{
public:
	CUniformTransformation(CEvaluation<TGenotypeFenotype> *pcEvaluation);

	virtual TGenotypeFenotype *pcTransform(TGenotypeFenotype *pcGenotype) { return pcGenotype; };
	virtual void vTransform(TGenotypeFenotype *pcGenotype, TGenotypeFenotype *pcFenotype) { pcFenotype = pcGenotype; };

	virtual TGenotypeFenotype *pcInverselyTransform(TGenotypeFenotype *pcFenotype) { return pcFenotype; };
	virtual void vInverselyTransform(TGenotypeFenotype *pcFenotype, TGenotypeFenotype *pcGenotype) { pcGenotype = pcFenotype; };

protected:
	virtual TGenotypeFenotype *pc_create_fenotype(TGenotypeFenotype *pcGenotype) { return nullptr; };
	virtual TGenotypeFenotype *pc_create_genotype(TGenotypeFenotype *pcGenotype) { return nullptr; };
};//class CUniformTransformation


namespace TransformationUtils
{
	template <class TGenotype, class TFenotype>
	CTransformation<TGenotype, TFenotype> * pcGetTransformation(CEvaluation<TFenotype> *pcEvaluation, istream *psSettings, CError *pcError);
}//namespace TransformationUtils

#endif//TRANSFORMATION_H